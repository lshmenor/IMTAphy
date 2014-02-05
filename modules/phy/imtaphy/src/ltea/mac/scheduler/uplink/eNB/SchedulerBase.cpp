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

#include <IMTAPHY/ltea/mac/scheduler/uplink/eNB/SchedulerBase.hpp>
#include <algorithm>
#include <DLL/UpperConvergence.hpp>
#include <IMTAPHY/Channel.hpp>
#include <IMTAPHY/ltea/mac/scheduler/uplink/UEScheduler.hpp>
#include <iostream>
#include <iomanip>
#include <IMTAPHY/linkManagement/LinkManager.hpp>
#include <boost/algorithm/string.hpp>

using namespace ltea::mac::scheduler::uplink::enb;

SchedulerBase::SchedulerBase(wns::ldk::fun::FUN* fuNet, const wns::pyconfig::View& config) :
    wns::ldk::CommandTypeSpecifier<ltea::mac::DownlinkControlInformation>(fuNet),
    wns::ldk::HasReceptor<>(),
    wns::ldk::HasConnector<>(),
    wns::ldk::HasDeliverer<>(),
    fun(fuNet),
    layer(dynamic_cast<ltea::Layer2*>(fun->getLayer())),
    logger(config.get<wns::pyconfig::View>("logger")),
    txService(NULL),
    channelStatusManager(
        imtaphy::receivers::feedback::UplinkChannelStatusManagerInterface::getCSM()),
    mcsLookup(&(ltea::mac::TheMCSLookup::Instance())),
    channel(imtaphy::TheIMTAChannel::getInstance()),
    spectrum(channel->getSpectrum()),
    scheduleForTTI(0),
    currentTTI(0),
    harqPeriodLength(8),
    alpha(config.get<double>("alpha")),
    P0(wns::Power::from_dBm(config.get<double>("P0dBmPerPRB"))),
    pucchSize(config.get<unsigned int>("pucchSize")),
    prachPeriod(config.get<unsigned int>("prachPeriod")),
    srsPeriod(config.get<unsigned int> ("srsPeriod")),
    Ks(config.get<double>("Ks")),
    pathlossEstimationMethod(config.get<std::string>("pathlossEstimationMethod")),
    weightingFactor(config.get<double>("weightingFactor")),
    pyConfig(config)
{
    assure((weightingFactor > 0.0) && (weightingFactor <= 1.0), "weightingFactor must be between 0 and one");
    assure((alpha > 0) && (alpha <= 1), "Pathloss compensation factor \alpha must be between 0 and 1");
    assure((P0.get_dBm() >= -126) && (P0.get_dBm() <= 23), "PUSCH Power control base level should be between -126 dBm and +23 dBm");
    assure((spectrum->getNumberOfPRBs(imtaphy::Uplink) == 0) || pucchSize < spectrum->getNumberOfPRBs(imtaphy::Uplink), "either disable UL or provide enough PRBs to accomodate PUCCH");
    //assure((prachPeriod >= 2) && (prachPeriod <= 20), "PRACH period should be 2..20ms"); // reserving PRBs for PRACH without actually having PRACH can be very annoying 
    
    // initialize the users/PRB availability lookup for the 
    // periodlenght (usually 8 TTIs because that is the HARQ RT time)
    for (unsigned int i = 0; i < harqPeriodLength; i++)
    {
        // TODO: we do not want to have a global power restriction here
        usersPRBManager.push_back(new ltea::mac::scheduler::UsersPRBManager(spectrum->getNumberOfPRBs(imtaphy::Uplink), wns::Power::from_mW(42)));
    }
    
    assure(fun, "No valid FUN pointer");
    assure(layer, "Could not get Layer2 pointer");


    // create resource management module and pass its config
    wns::pyconfig::View resourceManagementConfig= config.get("resourceManager");
    resourceManager = wns::StaticFactory<wns::PyConfigViewCreator<ltea::mac::scheduler::ResourceManagerInterface> >::creator(resourceManagementConfig.get<std::string>("nameInFactory"))->create(resourceManagementConfig);

    boost::to_upper(pathlossEstimationMethod);
    
    
}

void 
SchedulerBase::onFUNCreated()
{
    wns::ldk::FunctionalUnit::onFUNCreated();

    ltea::Layer2* layer = dynamic_cast<ltea::Layer2*>(getFUN()->getLayer());
    assure(layer, "Expecting an ltea::Layer2*");
    
    txService = layer->getTxService();
       
    this->startObserving(&(imtaphy::TheIMTAChannel::Instance()));

    dciReader = fun->getCommandReader(this->getName());
    assure(dciReader, "Could not get my command reader!?");
    
    myStation = dynamic_cast<imtaphy::StationPhy*>(txService);
    numRxAntennas = myStation->getAntenna()->getNumberOfElements();

    // create link adaptation module and pass its config
    wns::pyconfig::View linkAdaptationConfig = pyConfig.get("linkAdaptation");
    linkAdaptation = ltea::mac::la::uplink::LinkAdaptationFactory::creator(linkAdaptationConfig.get<std::string>("nameInFactory"))->create(myStation, linkAdaptationConfig);

    linkAdaptation->setPowerControlInterface(this);
    
    myStation->getNode()->addService("UplinkGrants", this);
}





void 
SchedulerBase::applyPUCCHandPRACHrestrictions(ltea::mac::scheduler::UsersPRBManager* usersPRBManager)
{
    // Don't do anything for the moment: will be added later and might be moved to ResourceManagement module
    assure((pucchSize % 2) == 0, "PUCCH size (number of PRBs deducted from system bandwidth) should be an even number");
    
    unsigned int totalNumPRBs = spectrum->getNumberOfPRBs(imtaphy::Uplink);
    assure(totalNumPRBs >= 6, "6 PRBs is the minimum LTE bandwidth");
    
    // the dedicated PUCCH is transmitted a the edges of the system bandwidth
    for (unsigned int i = 0; i < pucchSize / 2; i++)
    {
        usersPRBManager->markPRBused(i);
        usersPRBManager->markPRBused(totalNumPRBs - i - 1);
    }
    
    // periodically, 6 PRBs are reserved for the Random Access channel to allow initial acces for the UEs
    if ((scheduleForTTI % prachPeriod) == 0)
    {
        unsigned int centerPRB = totalNumPRBs / 2;
        for (unsigned int i = 0; i < 6; i++)
        {
            usersPRBManager->markPRBused(centerPRB - 3 + i);
        }
    }
}


void ltea::mac::scheduler::uplink::enb::SchedulerBase::doLinkAdapationAndPowerControlAndInformUEs()
{
    for (std::list<SchedulingResult>::iterator iter = scheduledUsers.begin(); iter != scheduledUsers.end(); iter++)
    {
        wns::node::Interface* user = iter->scheduledUser;

        assure(iter->rank == 1, "Currently, only rank 1 uplink transmission supported");
        
        unsigned int numPRBs = iter->prbPowerPrecoders.size();
        std::vector<unsigned int> prbs(numPRBs);
        
        unsigned int i = 0;
        for (imtaphy::interface::PrbPowerPrecodingMap::const_iterator prbIter = iter->prbPowerPrecoders.begin(); 
             prbIter != iter->prbPowerPrecoders.end(); i++, prbIter++)
        {
            prbs[i] = prbIter->first;
        }


        bool hasSRS = ((scheduleForTTI + 1 % srsPeriod) == 0); // 1 is just some offset
        ltea::mac::la::uplink::LinkAdaptationResult laResult;
        laResult = linkAdaptation->performLinkAdaptation(iter->scheduledUser,
                                                         0, // spatial transport block, currently only rank1
                                                         prbs,
                                                         currentTTI,
                                                         1,
                                                         hasSRS,
                                                         Ks
                                                        );
                                                        
                                                        
        SchedulingGrant grant;
        grant.scheduledForTTI = scheduleForTTI;
        grant.rank = iter->rank;
        
        wns::Power equalTxPower = getOpenLoopPerPRBPower(user, 
                                                         iter->prbPowerPrecoders.size(),
                                                         iter->closedLoopPowerControlDelta + // we can add this here 
                                                         mcsLookup->computeDeltaTFdB(laResult.mcsIndex, laResult.codeRate, Ks));
        grant.prbPowerPrecoders = iter->prbPowerPrecoders;
        
        
        double linearAvgEstSINR = 0.0;
        int linearAvgEstSINRCounter = 0;
        imtaphy::receivers::feedback::LteRel10UplinkChannelStatusPtr status = imtaphy::receivers::feedback::UplinkChannelStatusManagerInterface::getCSM()->getChannelState(iter->scheduledUser, currentTTI);
        
        // overwrite initial power allocation (e.g. 0mW with actual transmit power)
        for (imtaphy::interface::PrbPowerPrecodingMap::iterator prbIter = grant.prbPowerPrecoders.begin(); prbIter != grant.prbPowerPrecoders.end(); prbIter++)
        {
            linearAvgEstSINR += status->sinrsTb1[prbIter->first].get_factor();
            linearAvgEstSINRCounter++;
            prbIter->second.power = equalTxPower;
        }
        
        grant.estimatedLinearAvgSINR = wns::Ratio::from_factor(linearAvgEstSINR / static_cast<double>(linearAvgEstSINRCounter));
        
        
        grant.mcsIndex = laResult.mcsIndex;
        grant.codeRate = laResult.codeRate;
        // limit to 25 dB to make comparison in laMismatch more meaningful because there SINR is limited
        grant.estimatedSINR = std::min(laResult.estimatedSINR, 
                                       wns::Ratio::from_dB(25));
        
        grant.prbTracingDict = laResult.prbTracingDict;
        
        
        assure(schedulingRequests.find(user) != schedulingRequests.end(), "No scheduler pointer for scheduled uplink user");
        schedulingRequests[user].ueScheduler->deliverSchedulingGrant(scheduleForTTI, grant);
    }
        // Power control is specified in TS 36.213, Section 5.1.1
        // PLc is the downlink pathloss estimate calculated in the UE for serving cell c in dB and 
        // PLc = referenceSignalPower – higher layer filtered RSRP, where referenceSignalPower is provided 
        // by higher layers and RSRP is defined in [5] for the reference serving cell and the higher layer 
        // filter configuration is defined in [11] for the reference serving cell. The serving cell chosen 
        // as the reference serving cell and used for determining referenceSignalPower and higher layer 
        // filtered RSRP is configured by the higher layer parameter pathlossReferenceLinking.

        // Sesia p. 466: "The fractional path-loss compensation factor α can be seen as a tool to trade off the fairness of 
        // the uplink scheduling against the total cell capacity. Full path-loss compensation maximizes fairness for 
        // cell-edge UEs. However, when considering multiple cells together as a system, the use of only partial path-loss 
        // compensation can increase the total system capacity in the uplink, as less resources are spent ensuring the 
        // success of transmissions from cell-edge UEs and less inter-cell interference is caused to neighbouring cells. 
        // Path-loss compensation factors around 0.7–0.8 typically give a close-to-maximal uplink system capacity without 
        // causing significant degradation to the cell-edge data rate that can be achieved."
        // Sesia p. 467-468:
        // The MCS-dependent component for the PUSCH can be set to zero if it is not needed, for example if fast Adaptive Modulation and Coding (AMC) is used instead.
}


void 
SchedulerBase::onNewTTI(unsigned int ttiNumber)
{
    // maybe move this to a better place but be careful to assure that all mobiles are already
    // associated to the base station
    if (ttiNumber == 1)
    {
        allUsers =  txService->getAssociatedNodes();
        
        std::vector<imtaphy::StationPhy*> associatedUserStations;
        imtaphy::LinkMap allLinks = channel->getLinkManager()->getServedLinksForBaseStation(myStation);
        node2LinkMap.clear();
        for (imtaphy::LinkMap::const_iterator iter = allLinks.begin(); iter != allLinks.end(); iter++)
        {
            node2LinkMap[iter->first->getNode()] = iter->second;
            associatedUserStations.push_back(iter->first);
        }

        channelStatusManager->registerBS(myStation, associatedUserStations, channel);

        resourceManager->initResourceManager(channel, myStation);
        resourceManager->setUplinkStatusManager(channelStatusManager);
        
        linkAdaptation->updateAssociatedUsers(allUsers);
        
        initScheduler(); // really move this somewhere else. onFunCreated is too early, btw
    }

    // without PRBs, we don't need to do anything here (used to disable uplink scheduling)
    if (spectrum->getNumberOfPRBs(imtaphy::Uplink) == 0)
        return;
    
    currentTTI = ttiNumber;
    scheduleForTTI = ttiNumber + 4;
    MESSAGE_SINGLE(NORMAL, logger, "Accessing entry " << scheduleForTTI % harqPeriodLength << " of ringbuffer");

    linkAdaptation->updateChannelStatus(currentTTI);
    linkAdaptation->updateBLERStatistics(currentTTI);
    
    currentUsersPRBManager = usersPRBManager[scheduleForTTI % harqPeriodLength];
    
    scheduledUsers.clear();
    
    applyPUCCHandPRACHrestrictions(currentUsersPRBManager);
    resourceManager->determineResourceRestrictions(*currentUsersPRBManager, imtaphy::Uplink);
    
    MESSAGE_BEGIN(NORMAL, logger, m, "Starting Uplink Scheduling in TTI " << ttiNumber << " for TTI " << scheduleForTTI << ". The following PRBs are available (+) before the start of scheduling:\n");
           for (unsigned int prb = 0; prb < spectrum->getNumberOfPRBs(imtaphy::Uplink); prb++)
           {
               m << std::setw(3) << prb;
           }
           m << "\n";
           for (unsigned int prb = 0; prb < spectrum->getNumberOfPRBs(imtaphy::Uplink); prb++)
           {
               m << " ";
               if (currentUsersPRBManager->prbAvailable(prb))
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

    doScheduling();

    MESSAGE_BEGIN(NORMAL, logger, m, "After uplink scheduling in TTI " << ttiNumber << " the following PRBs are still available (+):\n");
           for (unsigned int prb = 0; prb < spectrum->getNumberOfPRBs(imtaphy::Uplink); prb++)
           {
               m << std::setw(3) << prb;
           }
           m << "\n";
           for (unsigned int prb = 0; prb < spectrum->getNumberOfPRBs(imtaphy::Uplink); prb++)
           {
               m << " ";
               if (currentUsersPRBManager->prbAvailable(prb))
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
    
    doLinkAdapationAndPowerControlAndInformUEs();
    
    
    scheduledUsers.clear();
    
    // add all active users and PRBs back to that TTI in the ringbuffer
    currentUsersPRBManager->reset();
    
    for (unsigned int i = 0; i < allUsers.size(); i++)
    {
        if (schedulingRequests.find(allUsers[i]) != schedulingRequests.end())
        {
            currentUsersPRBManager->addActiveUser(allUsers[i]);
        }
    }
}

void 
SchedulerBase::registerHARQRetransmission(wns::node::Interface* user, imtaphy::interface::PRBVector& prbs, unsigned int failedDuringTTI)
{
    MESSAGE_SINGLE(NORMAL, logger, "Expecting retransmission of failed transmission from user " << user->getName() << " in TTI " << failedDuringTTI + harqPeriodLength);
    
    MESSAGE_SINGLE(NORMAL, logger, "Accessing entry " << failedDuringTTI % harqPeriodLength << " of ringbuffer");
    
    for (unsigned int i = 0; i < prbs.size(); i++)
    {
        if (!usersPRBManager[failedDuringTTI % harqPeriodLength]->prbAvailable(prbs[i]))
        {
            for (int n = 0; n < 8; n++)
            {
                std::cout << "displaying availability for failedDuringTTI+" << n << "\n";
                imtaphy::interface::PRBVector prbsAvailable = usersPRBManager[(failedDuringTTI + n) % harqPeriodLength]->getPRBsAvailable();
                for (unsigned int j = 0; j < prbsAvailable.size(); j++)
                    std::cout << "PRB " << prbsAvailable[j] << " available\n";
                
            }
        }
        // we are going to have a retransmission in failedDuringTTI + harqPeriodLength
        assure(usersPRBManager[failedDuringTTI % harqPeriodLength]->prbAvailable(prbs[i]), "Wanted to reserve PRB for retransmission but was already used otherwise!?");

        
        usersPRBManager[failedDuringTTI % harqPeriodLength]->markPRBused(prbs[i]);
    }
    
    usersPRBManager[failedDuringTTI % harqPeriodLength]->removeActiveUser(user);
}

void 
SchedulerBase::schedulingRequest(ltea::mac::scheduler::uplink::SchedulingRequest& request)
{
    // currently no real handling of scheduling requests
    // just add the user to the set of active users
    
    for (unsigned int i = 0; i < usersPRBManager.size(); i++)
    {
        usersPRBManager[i]->addActiveUser(request.requestingUser);
    }
    
    schedulingRequests[request.requestingUser] = request;
    
    // SRS power control
    channelStatusManager->setReferencePerPRBTxPowerForUser(request.requestingUser,
                                                           getSRSPerPRBPower(request.requestingUser));
}

wns::Ratio
SchedulerBase::getPathlossForPowerControl(imtaphy::Link* link)
{
    if (pathlossEstimationMethod == "RSRP")
    {
        MESSAGE_SINGLE(NORMAL, logger, "Using RSRP-based pathloss value of " << channel->getLinkManager()->getRSRPReferenceTxPower() / link->getRSRP()
                                        << " for uplink power control. WBL=" << link->getWidebandLoss());
        return channel->getLinkManager()->getRSRPReferenceTxPower() / link->getRSRP();
    }
    if (pathlossEstimationMethod == "WBL")
    {
        MESSAGE_SINGLE(NORMAL, logger, "Using WBL-based pathloss value of " << link->getWidebandLoss() 
                                        << " for uplink power control. RSRP=" << channel->getLinkManager()->getRSRPReferenceTxPower() / link->getRSRP());

        return link->getWidebandLoss(); 
    }
    if (pathlossEstimationMethod == "EXACT")
    {
        double gain = 0.0;
        unsigned int counter = 0;
        for (unsigned int f = 0; f < channel->getSpectrum()->getNumberOfPRBs(imtaphy::Uplink); f++)
        {
            imtaphy::detail::ComplexFloatMatrixPtr H = link->getChannelMatrix(imtaphy::Uplink, f);

            if (H->getRows() > 1)
            {
                counter++;
                gain += norm((*H)[0][0]) +  norm((*H)[1][0]);
            }
            else
            {
                gain += norm((*H)[0][0]);
                counter++;
            }
        }
        
        gain = static_cast<double>(counter) / gain;
        
        MESSAGE_SINGLE(NORMAL, logger, "Using exact channel attenuation value of " << wns::Ratio::from_factor(gain) 
                                << " for uplink power control. RSRP=" << channel->getLinkManager()->getRSRPReferenceTxPower() / link->getRSRP());

        
        return wns::Ratio::from_factor(gain);
    }
    if (pathlossEstimationMethod == "AVERAGED")
    {
        double gain = 0.0;
        unsigned int counter = 0;
        for (unsigned int f = 0; f < channel->getSpectrum()->getNumberOfPRBs(imtaphy::Downlink); f++)
        {
            imtaphy::detail::ComplexFloatMatrixPtr H = link->getChannelMatrix(imtaphy::Downlink, f);

            if (H->getColumns() > 1)
            {
                counter++;
                gain += norm((*H)[0][0]) +  norm((*H)[0][1]);
            }
            else
            {
                gain += norm((*H)[0][0]);
                counter++;
            }
        }
        
        gain = static_cast<double>(counter) / gain;
        
        if (gainMap.find(link) == gainMap.end())
        {   // first entry
            gainMap[link] = gain;
        }
        else
        {
            gainMap[link] = weightingFactor * gain + (1.0 - weightingFactor) * gainMap[link];
        }
     
        MESSAGE_SINGLE(NORMAL, logger, "Using averaged downlink channel attenuation value of " << wns::Ratio::from_factor(gainMap[link] )
                                        << " based on current value of " << wns::Ratio::from_factor(gain) << " with weightingFactor=" << weightingFactor
                                        << " for uplink power control. RSRP=" << channel->getLinkManager()->getRSRPReferenceTxPower() / link->getRSRP());

     
        return wns::Ratio::from_factor(gainMap[link]);
    }
    
    assure(0, "Should have returned before");
}

wns::Power
SchedulerBase::getOpenLoopPerPRBPower(wns::node::Interface* user, unsigned int numPRBs, wns::Ratio delta)
{
    assure(schedulingRequests.find(user) != schedulingRequests.end(), "User unknown");
    
    wns::Power ueAvailablePower =  schedulingRequests[user].totalAvailableTxPower;

    // for calibration purposes,it seems that when using RSRP too many fast fading influences are captured in the RSRP
    // so that doing the power control based on that value, not enough variance in the resulting SINR remains
    //wns::Ratio downlinkPathloss = node2LinkMap[user]->getWidebandLoss(); 

    wns::Ratio downlinkPathloss = getPathlossForPowerControl(node2LinkMap[user]); 

    
    double totalTxPower_mW = std::min(ueAvailablePower.get_mW(),
                                      P0.get_mW() *  static_cast<double>(numPRBs) * wns::Ratio::from_dB(alpha * downlinkPathloss.get_dB() + delta.get_dB()).get_factor());  

    return wns::Power::from_mW(totalTxPower_mW / static_cast<double>(numPRBs));
}



wns::Power
SchedulerBase::getSRSPerPRBPower(wns::node::Interface* user)
{
    assure(schedulingRequests.find(user) != schedulingRequests.end(), "User unknown");
    
    wns::Power ueAvailablePower =  schedulingRequests[user].totalAvailableTxPower;
    unsigned int numPRBs = spectrum->getNumberOfPRBs(imtaphy::Uplink);
    
    wns::Ratio downlinkPathloss = getPathlossForPowerControl(node2LinkMap[user]); 

    
    double totalTxPower_mW = std::min(ueAvailablePower.get_mW(),
                                      P0.get_mW() *  static_cast<double>(numPRBs) * wns::Ratio::from_dB(alpha * downlinkPathloss.get_dB()).get_factor());  

    return wns::Power::from_mW(totalTxPower_mW / static_cast<double>(numPRBs));
}


wns::Ratio
SchedulerBase::getOpenLoopPerPRBPowerHeadroom(wns::node::Interface* user, unsigned int numPRBs)
{
    assure(schedulingRequests.find(user) != schedulingRequests.end(), "User unknown");
    
    wns::Power ueAvailablePower =  schedulingRequests[user].totalAvailableTxPower;
    wns::Ratio downlinkPathloss = getPathlossForPowerControl(node2LinkMap[user]); 
    
    double totalTxPower_mW = std::min(ueAvailablePower.get_mW(),
                                      P0.get_mW() *  static_cast<double>(numPRBs) * wns::Ratio::from_dB(alpha * downlinkPathloss.get_dB()).get_factor());  

    double headroom = ueAvailablePower.get_mW() / totalTxPower_mW;
    
    assure(headroom >= 1.0, "total tx power should never be bigger than available power");
    
    return wns::Ratio::from_factor(headroom);
}
