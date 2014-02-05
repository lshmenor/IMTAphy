/*******************************************************************************
 * This file is part of IMTAphy
 * _____________________________________________________________________________
 *
 * Copyright (C) 2011
 * Institute of Communication Networks (LKN)
 * Department of Electrical Engineering and Information Technology (EE & IT)
 * Technische Universitaet Muenchen
 * Arcisstr. 21
 * 80333 Muenchen - Germany
 * http://www.lkn.ei.tum.de/~jan/imtaphy/index.html
 * 
 * _____________________________________________________________________________
 *
 *   IMTAphy is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   IMTAphy is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with IMTAphy.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#include <IMTAPHY/ltea/mac/scheduler/downlink/SchedulerBase.hpp>
#include <algorithm>
#include <iomanip>
#include <DLL/UpperConvergence.hpp>
#include <IMTAPHY/Channel.hpp>

using namespace ltea::mac::scheduler::downlink;

SchedulerBase::SchedulerBase(wns::ldk::fun::FUN* fuNet, const wns::pyconfig::View& config) :
    wns::ldk::CommandTypeSpecifier<ltea::mac::DownlinkControlInformation>(fuNet),
    wns::ldk::HasReceptor<>(),
    wns::ldk::HasConnector<>(),
    wns::ldk::HasDeliverer<>(),
    fun(fuNet),
    layer(dynamic_cast<ltea::Layer2*>(fun->getLayer())),
    logger(config.get<wns::pyconfig::View>("logger")),
    txService(NULL),
    feedbackManager(imtaphy::receivers::feedback::DownlinkFeedbackManagerInterface::getFeedbackManager()),
    mcsLookup(&(ltea::mac::TheMCSLookup::Instance())),
    harq(new ltea::mac::harq::HARQ(config.get("harq"))),
    txPowerdBmPerPRB(wns::Power::from_dBm(config.get<double>("txPowerdBmPerPRB"))),
    currentRetransmissionUser(0),
    channel(imtaphy::TheIMTAChannel::getInstance()),
    spectrum(channel->getSpectrum()),
    scheduleForTTI(0),
    pdcchLength(3),          // could/should be overwritten by init and or doScheduling
    provideRel10DMRS(config.get<bool>("provideRel10DMRS")), // could/should be overwritten by init and or doScheduling
    syncHARQ(config.get<bool>("syncHARQ")),
    numRel10CSIrsSets(0),     // could/should be overwritten by init and or doScheduling
    usersPRBManager(spectrum->getNumberOfPRBs(imtaphy::Downlink), wns::Power::from_dBm(config.get<double>("txPowerdBmPerPRB")))
{
    assure(fun, "No valid FUN pointer");
    assure(layer, "Could not get Layer2 pointer");
    
    wns::pyconfig::View queueConfig = config.getView("queue");
    queue = wns::StaticFactory<wns::PyConfigViewCreator<
        ltea::rlc::IQueue> >::creator(queueConfig.get<std::string>("nameInQueueFactory"))->create(queueConfig);
    queue->setFUN(fuNet);
    
    // create link adaptation module and pass its config
    wns::pyconfig::View linkAdaptationConfig = config.get("linkAdaptation");
    linkAdaptation = wns::StaticFactory<wns::PyConfigViewCreator<ltea::mac::la::downlink::LinkAdaptationInterface> >::creator(linkAdaptationConfig.get<std::string>("nameInFactory"))->create(linkAdaptationConfig);

    // create resource management module and pass its config
    wns::pyconfig::View resourceManagementConfig= config.get("resourceManager");
    resourceManager = wns::StaticFactory<wns::PyConfigViewCreator<ltea::mac::scheduler::ResourceManagerInterface> >::creator(resourceManagementConfig.get<std::string>("nameInFactory"))->create(resourceManagementConfig);
}

void 
SchedulerBase::onFUNCreated()
{
    wns::ldk::FunctionalUnit::onFUNCreated();
    
    friends.pdcpCommandReader = fun->getCommandReader("pdcp");
    
    ltea::Layer2* layer = dynamic_cast<ltea::Layer2*>(getFUN()->getLayer());
    assure(layer, "Expecting an ltea::Layer2*");
    
    txService = layer->getTxService();
       
    this->startObserving(&(imtaphy::TheIMTAChannel::Instance()));

    dciReader = fun->getCommandReader(this->getName());
    assure(dciReader, "Could not get my command reader!?");
    harq->setDCIReader(dciReader);
    
    myStation = dynamic_cast<imtaphy::StationPhy*>(txService);
    numTxAntennas = myStation->getAntenna()->getNumberOfElements();

    linkAdaptation->setHARQ(harq);
    linkAdaptation->setFeedbackManager(feedbackManager);
    
    wns::probe::bus::ContextProviderCollection localcpc(layer->getNode()->getContextProviderCollection());
    rankContextCollector = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(localcpc, "rank"));

    tbSizeContextCollector = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(localcpc, "tbSize"));
}


void
SchedulerBase::doSendData(const wns::ldk::CompoundPtr& compound)
{
    assure(doIsAccepting(compound), "Not accepting compound");
    
    queue->put(compound);
    
    MESSAGE_SINGLE(NORMAL, logger, "Accepting compound in doSendData");
    

}

void
SchedulerBase::doOnData(const wns::ldk::CompoundPtr& compound)
{
    getDeliverer()->getAcceptor(compound)->onData(compound);
}

bool
SchedulerBase::doIsAccepting(const wns::ldk::CompoundPtr& compound) const
{
    return queue->isAccepting(compound);
}
void
SchedulerBase::doWakeup()
{
    getReceptor()->wakeup();
}

void 
SchedulerBase::determineActiveUsers()
{
    for (unsigned int i = 0; i < allUsers.size(); i++)
    {
        if (queue->queueHasPDUs(allUsers[i]))
        {
            // the HARQ retransmission step does not rely on the users being active, so 
            // we can do this here. A user will only become unblocked after an ACK/NACK
            // so it is safe to already exclude him over here
            if (harq->hasFreeSenderProcess(allUsers[i]))
            {
                usersPRBManager.addActiveUser(allUsers[i]);
            }
        }
    }
}


bool 
SchedulerBase::performRetransmissionsFor(wns::node::Interface* user)
{
    std::set<wns::node::Interface*, imtaphy::detail::WnsNodeInterfacePtrCompare> usersWithRetransmissions = harq->getUsersWithRetransmissions();
    assure(usersWithRetransmissions.find(user) != usersWithRetransmissions.end(), 
           "Perform retransmissions called for user without retransmissions");
    
    MESSAGE_SINGLE(NORMAL, logger, "User " << user->getName() << " needs a retransmission, looking for exact process id now");
    
    unsigned int firstProcessId = harq->getProcessWithNextRetransmissions(user);
    unsigned int processId = firstProcessId;
    
    do
    {
        unsigned int numRetransmissions = harq->getNumberOfRetransmissions(user, processId);
        assure(numRetransmissions > 0, "A user without retransmissions should not be returned");

        std::vector<wns::ldk::CompoundPtr> tbs;
        tbs.clear();
            
        ltea::mac::DownlinkControlInformation* dci0;
        ltea::mac::DownlinkControlInformation* dci1;
        std::vector<ltea::mac::DownlinkControlInformation*> dcis;
        wns::ldk::CompoundPtr retransmission0;
        wns::ldk::CompoundPtr retransmission1;
        
        // Check if the initially as spatial==0 transmitted TB needs a retransmission
        if (harq->hasRetransmission(user, processId, 0))
        {
            MESSAGE_SINGLE(NORMAL, logger, "User " << user->getName() << " process ID " << processId << " needs a retransmission for the first transport block");
            
            retransmission0 = harq->getRetransmission(user, processId, 0);
            
            assure(dciReader->commandIsActivated(retransmission0->getCommandPool()), 
                    "Retransmissions are expected to already have their DCI information stored from the first transmission attempt");

            dci0 = dciReader->readCommand<ltea::mac::DownlinkControlInformation>(retransmission0->getCommandPool());
            assure(dci0, "Could not get DCI");

            bool retransmissionFits = true;
            for (imtaphy::interface::PrbPowerPrecodingMap::const_iterator iter = dci0->local.prbPowerPrecoders.begin();
                    iter != dci0->local.prbPowerPrecoders.end(); iter++)
            {
                // For the time being, we only do non-adaptive HARQ retransmissions
                if (!usersPRBManager.prbAvailable(iter->first))
                {
                    retransmissionFits = false;
                    break;
                }
            }
            
            if (retransmissionFits)
            {
                tbs.push_back(retransmission0);
                dcis.push_back(dci0);
            }
        }
    
        // and now for a potential second Transport Block (spatial == 1)
        if (harq->hasRetransmission(user, processId, 1))
        {
            MESSAGE_SINGLE(NORMAL, logger, "User " << user->getName() << " process ID " << processId << " needs a retransmission for the second transport block");
            
            retransmission1 = harq->getRetransmission(user, processId, 1);
            
            assure(dciReader->commandIsActivated(retransmission1->getCommandPool()), 
                    "Retransmissions are expected to already have their DCI information stored from the first transmission attempt");

            dci1 = dciReader->readCommand<ltea::mac::DownlinkControlInformation>(retransmission1->getCommandPool());
            assure(dci1, "Could not get DCI");

            bool retransmissionFits = true;
            for (imtaphy::interface::PrbPowerPrecodingMap::const_iterator iter = dci1->local.prbPowerPrecoders.begin();
                    iter != dci1->local.prbPowerPrecoders.end(); iter++)
            {
                // For the time being, we only do non-adaptive HARQ retransmissions
                if (!usersPRBManager.prbAvailable(iter->first))
                {
                    retransmissionFits = false;
                    break;
                }
            }
            
            if (retransmissionFits)
            {
                tbs.push_back(retransmission1);
                dcis.push_back(dci1);
            }
        }
        
        if (tbs.size() > 0)
        {
            assure(tbs.size() <= 2, "There can only be two spatial streams");
            assure(tbs.size() == dcis.size(), "dcis and tbs sizes must match");
                                            
            for (imtaphy::interface::PrbPowerPrecodingMap::const_iterator iter = dcis[0]->local.prbPowerPrecoders.begin();
                    iter != dcis[0]->local.prbPowerPrecoders.end(); iter++)
            {   // mark the prbs as non available for further scheduling during this TTI
                usersPRBManager.markPRBused(iter->first);
            }   
            
#ifndef WNS_NDEBUG
            if (tbs.size() > 1)
            {
                assure(dcis[0]->local.prbPowerPrecoders.size() == dcis[1]->local.prbPowerPrecoders.size(), "The two transport blocks need to be transmitted on the exact PRBs");

                imtaphy::interface::PrbPowerPrecodingMap::const_iterator iter1;
                imtaphy::interface::PrbPowerPrecodingMap::const_iterator iter2;

                for (iter1 = dcis[0]->local.prbPowerPrecoders.begin(), iter2 = dcis[1]->local.prbPowerPrecoders.begin();
                        iter1 != dcis[0]->local.prbPowerPrecoders.end(); iter1++, iter2++)
                {   
                    assure(iter1->first == iter2->first, "PRBs for both transport blocks must be identical");
                }   
                    
            }
#endif                    
            assure(dcis.size() > 0, "Have TBs but no DCI info?");
            if ((dcis[0]->local.prbPowerPrecoders.begin()->second.precoding->getColumns() > 1) &&  // the original transmission was done with more than 1 layer
                (tbs.size() == 1)) // but now we only have to retransmit one transport block / code word
            {
                // in this case, remove the layers we don't retransmit from the precoding matrix because they would cause interference otherwise
                // TODO: do we increase the power for the retransmission to account for the fact that we have to divide the power over less layers?
                // pro: if we don't increase the power, we would, e.g. actually transmit with half the availalbe power if we retransmit 2 out of 4 original layers
                // con: the receiver would need to know this to properly scale the CS-RS which are not precoded -> can it be signaled?
                
                wns::ldk::CompoundPtr tbToRetransmit = tbs[0];
                ltea::mac::DownlinkControlInformation* dciForRetransmission = dcis[0];
                std::vector<unsigned int> originalLayers =  dciForRetransmission->peer.assignedToLayers;
                
              
                // check if for some reason (e.g., because this was already a retransmission) the precoding matrix has already been reduced to the number of layers
                if (originalLayers.size() != dciForRetransmission->local.prbPowerPrecoders.begin()->second.precoding->getColumns())
                {
                    MESSAGE_SINGLE(NORMAL, logger, "Retransmitting one of original two transport blocks; extracting " 
                                                   << originalLayers.size() << " out of " << dciForRetransmission->local.prbPowerPrecoders.begin()->second.precoding->getColumns()
                                                   << " precoding matrix columns (layers)");
                    
                    for (imtaphy::interface::PrbPowerPrecodingMap::iterator precoderIter = dciForRetransmission->local.prbPowerPrecoders.begin();
                         precoderIter != dciForRetransmission->local.prbPowerPrecoders.end(); precoderIter++)
                    {
                        imtaphy::detail::ComplexFloatMatrixPtr newPrecoder(new imtaphy::detail::ComplexFloatMatrix(numTxAntennas, originalLayers.size()));
                        
                        for (unsigned int t = 0; t < numTxAntennas; t++)
                        {
                            for (unsigned int l = 0; l < originalLayers.size(); l++)
                            {
                                (*newPrecoder)[t][l] = (*(precoderIter->second.precoding))[t][originalLayers[l] - 1]; // layers are counted from 1
                            }
                        }
                        precoderIter->second.precoding = newPrecoder;
                    }
                }
                
                std::vector<unsigned int> reassignedTo(originalLayers.size());
                for (unsigned int l = 0; l < originalLayers.size(); l++)
                {
                    reassignedTo[l] = l+1;
                }
                
                dciForRetransmission->peer.assignedToLayers = reassignedTo;
            }

            txService->registerTransmission(user,
                                            tbs,
                                            dcis[0]->local.prbPowerPrecoders.begin()->second.precoding->getColumns(), // number of layers
                                            dcis[0]->local.prbPowerPrecoders
                                            );
            for (unsigned int spatial = 0; spatial < 2; spatial++)
            {
                // we will always do both retransmissions but we don't know which one it was if there is only one to retransmit
                // TODO: we should use the dci->magic.spatialID
                if (harq->hasRetransmission(user, processId, spatial))
                {
                    harq->retransmissionStarted(user, processId, spatial);
                }
            }
            
            usersPRBManager.removeActiveUser(user);
            
            return true; // don't retransmit on more than 1 HARQ process 
        }

        processId = harq->getProcessWithNextRetransmissions(user);
    } while (processId != firstProcessId);

    return false; // unable to do a retransmission
}


void 
SchedulerBase::performRetransmissions()
{
    // We perform the retransmission in a round robin way
    // This does not transform other strategies into round-robin
    // because a non-round-robin bias is created by the scheduler 
    
    if (allUsers.size() == 0)
        return;
    
    std::set<wns::node::Interface*, imtaphy::detail::WnsNodeInterfacePtrCompare> usersWithRetransmissions = harq->getUsersWithRetransmissions();
    
    if (usersWithRetransmissions.size() == 0)
        return;
    
    for (unsigned int i = 0; i < allUsers.size(); i++)
    {
        currentRetransmissionUser = (currentRetransmissionUser + 1) % allUsers.size();
        wns::node::Interface* user = allUsers[currentRetransmissionUser];
        
        
        if (usersWithRetransmissions.find(user) == usersWithRetransmissions.end())
            continue;
        
        performRetransmissionsFor(user);
    }
}


void 
SchedulerBase::doLinkAdapationAndRegisterTransmissions()
{
    // this is how feedback manager computes the CQIs
        
      // TB to layer mapping in LTE (according to Farooq, TS36.? source?)
        //         layer 1     layer 2     layer 3     layer 4
        // rank 1:   TB 1
        // rank 2:   TB 1         TB 2
        // rank 3:   TB 1         TB 2        TB 2
        // rank 4:   TB 1         TB 1        TB 2      TB 2
    for (std::list<SchedulingResult>::iterator iter = scheduledUsers.begin(); iter != scheduledUsers.end(); iter++)
    {
        std::vector<unsigned int> tb0AssignedTo;
        std::vector<unsigned int> tb1AssignedTo;
        
        std::vector<wns::ldk::CompoundPtr> transportBlocks; 
        transportBlocks.clear();
        
        unsigned int numPRBs = iter->prbPowerPrecodingMap.size();
        
        switch (iter->rank)
        {
            case 1:
                tb0AssignedTo.resize(1);
                tb0AssignedTo[0] = 1;
                
                tb1AssignedTo.clear();
                break;
            case 2:
                tb0AssignedTo.resize(1);
                tb0AssignedTo[0] = 1;
                
                tb1AssignedTo.resize(1);
                tb1AssignedTo[0] = 2;
                break;
            case 3:
                tb0AssignedTo.resize(1);
                tb0AssignedTo[0] = 1;
                
                tb1AssignedTo.resize(2);
                tb1AssignedTo[0] = 2;
                tb1AssignedTo[1] = 3;
                break;
            case 4:
                tb0AssignedTo.resize(2);
                tb0AssignedTo[0] = 1;
                tb0AssignedTo[1] = 2;

                tb1AssignedTo.resize(2);
                tb1AssignedTo[0] = 3;
                tb1AssignedTo[1] = 4;
                
                break;
            default:
                assure(0, "Rank must be 1, 2, 3, or 4");
            
        }
        
        rankContextCollector->put(iter->rank);

        ltea::mac::la::downlink::LinkAdaptationResult laResult0;
        laResult0 = linkAdaptation->performLinkAdaptation(iter->scheduledUser,
                                                          0,                   // this is the first "spatial" transport block 
                                                          iter->prbPowerOffsetForLA,      //
                                                          scheduleForTTI,      //
                                                          tb0AssignedTo.size(),// number of layers for this TB  
                                                          pdcchLength,
                                                          provideRel10DMRS,
                                                          numRel10CSIrsSets
                                                        );
        unsigned int tb0Size = mcsLookup->getSize(laResult0.mcsIndex, numPRBs, tb0AssignedTo.size());
        wns::ldk::CompoundPtr compound0 = queue->getHeadOfLinePDUSegment(iter->scheduledUser, tb0Size);                                                                    

        MESSAGE_SINGLE(NORMAL, logger, "On the physical layer a transport block of " << tb0Size << " bits (" << numPRBs << " PRBs, MCS " << laResult0.mcsIndex << ") has been allocated for user "
                                        << iter->scheduledUser->getName() << ", payload size is " 
                                        << compound0->getLengthInBits() << " bits. Still queued: " 
                                        << queue->numBitsForUser(iter->scheduledUser) << " bits." );
            
        
#ifndef WNS_NDEBUG
        if (tb0Size > compound0->getLengthInBits() + compound0->getPCI()->getSize())
            MESSAGE_SINGLE(NORMAL, logger, "WARNING: Reservation on PHY (and thus maybe number of occupied resource blocks) is larger than needed: " 
                                            << tb0Size << " > " << compound0->getLengthInBits() << " + " << compound0->getPCI()->getSize() << " bits.");
#endif                                                                     

            
        tbSizeContextCollector->put(tb0Size, boost::make_tuple("Stream", 0));

        ltea::mac::DownlinkControlInformation* dci0 = activateCommand( compound0->getCommandPool() );
        dci0->peer.assignedToLayers = tb0AssignedTo;
        dci0->peer.blockSize = tb0Size;
        dci0->peer.codeRate = laResult0.codeRate;
        dci0->peer.modulation = laResult0.modulation;
        dci0->local.prbPowerPrecoders = iter->prbPowerPrecodingMap;
        dci0->magic.mcsIndex = laResult0.mcsIndex;
        dci0->magic.estimatedLinkAdaptationSINR = laResult0.estimatedSINR;
        dci0->magic.direction = imtaphy::Downlink;
        dci0->magic.spatialID = 0;
        dci0->magic.id = iter->scheduledUser->getNodeID() * 100000 + scheduleForTTI* 10 + 0;
        dci0->magic.prbTracingDict = iter->prbAndTracingInfo;

        // join scheduling and link adaptation tracing dicts
        for (ltea::mac::PRBSchedulingTracingDictMap::iterator dictIter = dci0->magic.prbTracingDict.begin(); dictIter != dci0->magic.prbTracingDict.end(); dictIter++)
        {
            dictIter->second.insert(laResult0.prbTracingDict[dictIter->first].begin(), laResult0.prbTracingDict[dictIter->first].end());
        }
        
         //////////// currently a somewhat dirty hack
        imtaphy::receivers::feedback::LteRel8DownlinkFeedbackPtr feedback = feedbackManager->getFeedback(iter->scheduledUser, scheduleForTTI);
        
        double linearAvgEstSINR = 0.0;
        int linearAvgEstSINRCounter = 0;
        for (unsigned int l = 0; l < tb0AssignedTo.size(); l++)
        {
            unsigned int layer = tb0AssignedTo[l];
            for (imtaphy::interface::PrbPowerPrecodingMap::const_iterator prbIter =  iter->prbPowerPrecodingMap.begin(); prbIter != iter->prbPowerPrecodingMap.end(); prbIter++)
            {
                linearAvgEstSINR += feedback->magicSINRs[prbIter->first][layer - 1].get_factor();
                linearAvgEstSINRCounter++;
            }
        }
        assure(linearAvgEstSINRCounter, "No PRB?");
        dci0->magic.estimatedLinearAvgSINR = wns::Ratio::from_factor(linearAvgEstSINR / static_cast<double>(linearAvgEstSINRCounter));
        ///////////////////
        
        transportBlocks.push_back(compound0);

        if ((iter->rank > 1) && (queue->queueHasPDUs(iter->scheduledUser)))
        {
            ltea::mac::la::downlink::LinkAdaptationResult laResult1;
            laResult1 = linkAdaptation->performLinkAdaptation(iter->scheduledUser,
                                                              1,                   // this is the second "spatial" transport block 
                                                              iter->prbPowerOffsetForLA,      //
                                                              scheduleForTTI,      //
                                                              tb1AssignedTo.size(),// number of layers for this TB  
                                                              pdcchLength,
                                                              provideRel10DMRS,
                                                              numRel10CSIrsSets
                                                            );
                                                            
            unsigned int tb1Size = mcsLookup->getSize(laResult1.mcsIndex, numPRBs, tb1AssignedTo.size());
            wns::ldk::CompoundPtr compound1 = queue->getHeadOfLinePDUSegment(iter->scheduledUser, tb1Size);                                                                    

            tbSizeContextCollector->put(tb1Size, boost::make_tuple("Stream", 1));


            ltea::mac::DownlinkControlInformation* dci1 = activateCommand( compound1->getCommandPool() );
            dci1->peer.assignedToLayers = tb1AssignedTo;
            dci1->peer.blockSize = tb1Size;
            dci1->peer.codeRate = laResult1.codeRate;
            dci1->peer.modulation = laResult1.modulation;
            dci1->local.prbPowerPrecoders = iter->prbPowerPrecodingMap;
            dci1->magic.mcsIndex = laResult1.mcsIndex;
            dci1->magic.estimatedLinkAdaptationSINR = laResult1.estimatedSINR;
            dci1->magic.direction = imtaphy::Downlink;
            dci1->magic.spatialID = 1;
            dci1->magic.id = iter->scheduledUser->getNodeID() * 100000 + scheduleForTTI* 10 + 1;
            dci1->magic.prbTracingDict = iter->prbAndTracingInfo;

            // join scheduling and link adaptation tracing dicts
            for (ltea::mac::PRBSchedulingTracingDictMap::iterator dictIter = dci1->magic.prbTracingDict.begin(); dictIter != dci1->magic.prbTracingDict.end(); dictIter++)
            {
                dictIter->second.insert(laResult1.prbTracingDict[dictIter->first].begin(), laResult1.prbTracingDict[dictIter->first].end());
            }

            //////////// currently a somewhat dirty hack
            double linearAvgEstSINR = 0.0;
            int linearAvgEstSINRCounter = 0;
            for (unsigned int l = 0; l < tb1AssignedTo.size(); l++)
            {
                unsigned int layer = tb1AssignedTo[l];
                for (imtaphy::interface::PrbPowerPrecodingMap::const_iterator prbIter =  iter->prbPowerPrecodingMap.begin(); prbIter != iter->prbPowerPrecodingMap.end(); prbIter++)
                {
                    linearAvgEstSINR += feedback->magicSINRs[prbIter->first][layer - 1].get_factor();
                    linearAvgEstSINRCounter++;
                }
            }
            assure(linearAvgEstSINRCounter, "No PRB?");
            dci1->magic.estimatedLinearAvgSINR = wns::Ratio::from_factor(linearAvgEstSINR / static_cast<double>(linearAvgEstSINRCounter));
            ////////////
            transportBlocks.push_back(compound1);
        }
        
        harq->storeScheduledTransportBlocks(iter->scheduledUser, transportBlocks);
        txService->registerTransmission(iter->scheduledUser,
                                        transportBlocks,
                                        iter->rank, // number of layers
                                        iter->prbPowerPrecodingMap);
    }
    
    // unblock the queues
    getReceptor()->wakeup();
}


void 
SchedulerBase::onNewTTI(unsigned int ttiNumber)
{
    // without PRBs, we don't need to do anything here (used to disable downlink scheduling)
    if (spectrum->getNumberOfPRBs(imtaphy::Downlink) == 0)
        return;

    // maybe move this to a better place but be careful to assure that all mobiles are already
    // associated to the base station
    if (ttiNumber == 1)
    {
        allUsers =  txService->getAssociatedNodes();
        initScheduler(); // really move this somewhere else. onFunCreated is too early, btw
        linkAdaptation->updateAssociatedUsers(allUsers);
        
        // set initial power to be used for feedback computation
        for (unsigned int u = 0; u < allUsers.size(); u++)
        {
            feedbackManager->setReferencePerPRBTxPowerForUser(allUsers[u], txPowerdBmPerPRB);
        }
        
        resourceManager->initResourceManager(channel, myStation);
        resourceManager->setDownlinkFeedbackManager(feedbackManager);
    }

    usersPRBManager.reset();
    
    scheduleForTTI = ttiNumber;

    scheduledUsers.clear();
    
    // give the LinkAdaptation the chance to update before it will be used later on
    linkAdaptation->updateBLERStatistics(ttiNumber);
    linkAdaptation->updateFeedback(ttiNumber);
    
    // add the active users to usersPRBManager
    determineActiveUsers();
    
    if (syncHARQ)
        performRetransmissions();
    
    // remove retransmitted users from active users
    
    
    resourceManager->determineResourceRestrictions(usersPRBManager, imtaphy::Downlink);

    // Now set the reference power again. In an actual system, the CS-RS should be transmitted with 
    // uniform (and probably constant) power but for our semi-static resource management schemes, we want
    // to allow also inhomogeneous powers and by adapting the "CS-RS" power, this yields CQI feedback that
    // takes these power levels into account
    for (unsigned int u = 0; u < allUsers.size(); u++)
    {
        for (unsigned int prb = 0; prb < spectrum->getNumberOfPRBs(imtaphy::Downlink); prb++)
        {
            feedbackManager->setReferencePerPRBTxPowerForUser(allUsers[u], prb, 
                                                              usersPRBManager.getAvailablePower(allUsers[u], prb));
        }
    }
    
    MESSAGE_BEGIN(NORMAL, logger, m, "Starting Downlink Scheduling in TTI " << ttiNumber << ". The following PRBs are available (+) before the start of scheduling:\n");
        for (unsigned int prb = 0; prb < spectrum->getNumberOfPRBs(imtaphy::Downlink); prb++)
        {
            m << std::setw(3) << prb;
        }
        m << "\n";
        for (unsigned int prb = 0; prb < spectrum->getNumberOfPRBs(imtaphy::Downlink); prb++)
        {
            m << " ";
            if (usersPRBManager.prbAvailable(prb))
            {
                m << "+";
            }
            else
            {
                m << "-";
            }
            m << " ";
        }
    MESSAGE_END();

    // TODO: filter out users whithout free harq processes
    doScheduling();
    
    MESSAGE_BEGIN(NORMAL, logger, m, "After Downlink Scheduling in TTI " << ttiNumber << ". The following PRBs are still available (+):\n");
        for (unsigned int prb = 0; prb < spectrum->getNumberOfPRBs(imtaphy::Downlink); prb++)
        {
            m << std::setw(3) << prb;
        }
        m << "\n";
        for (unsigned int prb = 0; prb < spectrum->getNumberOfPRBs(imtaphy::Downlink); prb++)
        {
            m << " ";
            if (usersPRBManager.prbAvailable(prb))
            {
                m << "+";
            }
            else
            {
                m << "-";
            }
            m << " ";
        }
    MESSAGE_END();

}



