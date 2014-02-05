/*******************************************************************************
* This file is part of IMTAphy
* _____________________________________________________________________________
*
* Copyright (C) 2012
* Institute for Communication Networks (LKN)
* Associate Institute for Signal Processing (MSV)
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
/* added by Andreas Dotzler - TUM - MSV - dotzler@tum.de -                   */
#include <IMTAPHY/ltea/mac/scheduler/downlink/ZFScheduler.hpp>
#include <boost/graph/graph_concepts.hpp>
#include <IMTAPHY/linkManagement/LinkManager.hpp>
#include <IMTAPHY/Channel.hpp>
#include <IMTAPHY/antenna/LinearAntennaArray.hpp>
#include <fstream>

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    ltea::mac::scheduler::downlink::ZFScheduler,
    wns::ldk::FunctionalUnit,
    "ltea.mac.scheduler.downlink.ZFScheduler",
    wns::ldk::FUNConfigCreator);


using namespace ltea::mac::scheduler::downlink;

ZFScheduler::ZFScheduler(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config) :
    SchedulerBase(fun, config),
    codebook(&(wns::SingletonHolder<imtaphy::receivers::LteRel8Codebook<float> >::Instance())),
    alpha(config.get<double>("throughputSmoothing")),
    blerModel(imtaphy::l2s::TheLTEBlockErrorModel::getInstance())
{
    assure((alpha > 0.0) && (alpha <= 1.0), "Exponential smothing factor must be 0 < alpha <= 1 where smaller values lead to longer averaging memory");
    
    wns::probe::bus::ContextProviderCollection localcpc(layer->getNode()->getContextProviderCollection());
    groupSizeContextCollector = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(localcpc, "groupSize"));

}


void 
ZFScheduler::initScheduler()
{
    pdcchLength = 3;
    // this should be set by the base class:
    //    provideRel10DMRS = true; // we compute arbitrary ZF precoders so we have to provide precoded demodulation reference symbols
    numRel10CSIrsSets = 0;

    assure(syncHARQ == false, "ZFScheduler only supports asynchronous/adaptive HARQ operation, i.e., with HARQ retransmissions controlled by the ZF scheduler");

    // Init the throughput history for exponential averaging
    for (unsigned int i = 0; i < allUsers.size(); i++)
    {
        throughputHistory[allUsers[i]] = 1.0;
    }


    codebook = imtaphy::receivers::TheLteRel8CodebookFloat::getInstance();
    
    numPRBs = channel->getSpectrum()->getNumberOfPRBs(imtaphy::Downlink);
    
    // check what kind of feedback manager we have
    imtaphy::receivers::feedback::PU2RCFeedbackManager* testPU2RCptr = dynamic_cast<imtaphy::receivers::feedback::PU2RCFeedbackManager*>(feedbackManager);
    if (testPU2RCptr)
    {
        feedbackMode = PU2RC;
    }
    else
    {
        feedbackMode = Rank1;
    }
    
    assure(myStation->getAntenna()->getNumberOfElements() == 4, "Only works for 4 Tx antennas");
}

void 
ZFScheduler::doScheduling()
{   
    ltea::mac::scheduler::UserSet retransmissionUsers = harq->getUsersWithRetransmissions();
    
    typedef std::map<wns::node::Interface*, unsigned int, imtaphy::detail::WnsNodeInterfacePtrCompare> UserIntMap;
    typedef std::map<wns::node::Interface*, wns::ldk::CompoundPtr, imtaphy::detail::WnsNodeInterfacePtrCompare> UserCompoundMap;

    UserIntMap retransmissionSizes;
    UserCompoundMap retransmissions;
    
    for (ltea::mac::scheduler::UserSet::const_iterator iter = retransmissionUsers.begin(); iter != retransmissionUsers.end(); iter++)
    {
        wns::node::Interface* user = *iter;
        
        unsigned int processId = harq->getProcessWithNextRetransmissions(user);
        assure(harq->hasRetransmission(user, processId, 0), "Users is supposed to have retransmission but not for the first TB");
        
        wns::ldk::CompoundPtr retransmission = harq->getRetransmission(user, processId, 0);
        
        assure(dciReader->commandIsActivated(retransmission->getCommandPool()), 
                    "Retransmissions are expected to already have their DCI information stored from the first transmission attempt");

        ltea::mac::DownlinkControlInformation* dci = dciReader->readCommand<ltea::mac::DownlinkControlInformation>(retransmission->getCommandPool());
        
        dci->local.prbPowerPrecoders;
        unsigned int numPRBs = dci->local.prbPowerPrecoders.size();
        
        retransmissionSizes[user] = numPRBs;
        retransmissions[user] = retransmission;
    }
        
    
    ltea::mac::scheduler::UserSet allUsers(retransmissionUsers.begin(), retransmissionUsers.end());
    ltea::mac::scheduler::UserSet newTransmissionUsers;

    ltea::mac::scheduler::UserSet activeUsers = usersPRBManager.getActiveUsers(); //ltea::mac::scheduler::UsersPRBManager
    for (ltea::mac::scheduler::UserSet::const_iterator iter = activeUsers.begin(); iter != activeUsers.end(); iter++)
    {
        if (retransmissionUsers.find(*iter) == retransmissionUsers.end())
        {
            // not a retransmission user
            allUsers.insert(*iter);
            newTransmissionUsers.insert(*iter);
        }
    }

    updateZFFeedback(allUsers);
    
    std::multimap<double, ZFGroup> strategies;
    for (unsigned int prb = 0; prb < numPRBs; prb++)
    {
        ZFGroup strat = computeZFforPRB(newTransmissionUsers,prb);
        strategies.insert(std::make_pair<double,ZFGroup>(strat.getMetric(), strat));        
    }
    

    // Find the strongest prbs for data transmission, use the rest for retransmission
    std::vector<unsigned int> new_transmission_prbs;


    std::map<wns::node::Interface*,SchedulingResult> allocations;

    
    std::map<wns::node::Interface*,SchedulingResult> re_transmission_allocations;
    UserIntMap remainingRetransmissionSizes;
    while (1)
    {
        re_transmission_allocations.clear();
        
        remainingRetransmissionSizes = retransmissionSizes;
    
        for (std::multimap<double, ZFGroup>::const_iterator iter = strategies.begin() ; iter != strategies.end(); iter++) 
        {
            // use the "worst" performing prbs for retransmissions
            if (remainingRetransmissionSizes.size() > 0)            
            {   
//                 std::cout << "Allocating Retransmissions\n";
//                 std::cout << "I have " << remainingRetransmissionSizes.size() << " Retransmission Users\n";

                ltea::mac::scheduler::UserSet retransmissionUsersToGroup;
                
                //update the metric
                for(UserIntMap::const_iterator u_iter = remainingRetransmissionSizes.begin(); u_iter != remainingRetransmissionSizes.end(); ++u_iter)
                {
                    retransmissionUsersToGroup.insert(u_iter->first);
                }
                
                ZFGroup retransmitGroup = computeZFforPRB(retransmissionUsersToGroup, iter->second.getPRB());

                std::vector<wns::node::Interface*> selectedForRetransmission = retransmitGroup.getUsers();
                
                for(std::vector<wns::node::Interface*>::const_iterator iter_retrans = selectedForRetransmission.begin(); iter_retrans != selectedForRetransmission.end(); ++iter_retrans)
                {
                    remainingRetransmissionSizes[*iter_retrans]--;
                    if (remainingRetransmissionSizes[*iter_retrans] == 0)
                    {
                        remainingRetransmissionSizes.erase(*iter_retrans);
//                         std::cout << "I have completely allocated user " << iter_retrans->second->getName() << "\n";
                    }
                    
                    // this we overwrite multiple times, but who cares
                    re_transmission_allocations[*iter_retrans].scheduledUser = *iter_retrans;
                    re_transmission_allocations[*iter_retrans].rank = 1;
                    re_transmission_allocations[*iter_retrans].prbAndTracingInfo =  ltea::mac::PRBSchedulingTracingDictMap();
                    
                    
                    imtaphy::interface::PowerAndPrecoding pap;
        
                    // Equally devide power among the users
                    pap.power = txPowerdBmPerPRB / static_cast<float>(retransmitGroup.getUsers().size());
                    pap.precoding = retransmitGroup.getPrecoder(*iter_retrans);
                    re_transmission_allocations[*iter_retrans].prbPowerPrecodingMap[iter->second.getPRB()] = pap;
                    re_transmission_allocations[*iter_retrans].prbPowerOffsetForLA[iter->second.getPRB()] = retransmitGroup.getSINROffset(*iter_retrans);

                }
            }
            // the rest is for new transmissions
            else
            {        
                if (newTransmissionUsers.size() == 0){
//                     std::cout << "I dont have any new transmissions" << std::endl;
                    break;
                }
//                 std::cout << "Allocating New Transmissions" << std::endl;
                new_transmission_prbs.push_back(iter->second.getPRB());
                computeSchedulingResult(iter->second, &allocations);      
            }
        } 
        if ( remainingRetransmissionSizes.size()==0 )
        {
            break;
        }
        else{
            retransmissionSizes.erase(retransmissionSizes.begin());
        }        
    }
    
    assure(remainingRetransmissionSizes.size()==0, "All retransmissions should be included. If not we dont have a plan :-)");
    
    // register retransmissions
    for (std::map<wns::node::Interface*,SchedulingResult>::const_iterator iter = re_transmission_allocations.begin(); iter != re_transmission_allocations.end(); iter++)        
    {
        wns::node::Interface* user = iter->first; 
        SchedulingResult result = iter->second;
        ltea::mac::DownlinkControlInformation* dci = dciReader->readCommand<ltea::mac::DownlinkControlInformation>(retransmissions[user]->getCommandPool());
        dci->local.prbPowerPrecoders = result.prbPowerPrecodingMap;
        txService->registerTransmission(user,
                                        std::vector<wns::ldk::CompoundPtr>(1, retransmissions[user]),
                                        1, // number of layers
                                        dci->local.prbPowerPrecoders
                                        );
        harq->retransmissionStarted(user, dci->peer.processID, 0);
//         std::cout << "Register Retransmissions\n";
        assure(retransmissionSizes[user] == result.prbPowerPrecodingMap.size(), "retransmission size is not correct");
    } 
    
    // register new transmissions
    for (std::map<wns::node::Interface*,SchedulingResult>::const_iterator iter = allocations.begin(); iter != allocations.end(); iter++)        
    {
        scheduledUsers.push_back(iter->second);
    } 
    
    //when scheduling is done, let the base class perform the Link Adaptation and register the transmissions
    // actually, we overload this
    doLinkAdapationAndRegisterTransmissions();
    
    /*Update throughput history*/
    for (ltea::mac::scheduler::UserSet::const_iterator iter = allUsers.begin(); iter != allUsers.end(); iter++)
    {
        throughputHistory[*iter] = (1.0 - alpha) * throughputHistory[*iter] + alpha * throughputThisTTI[*iter];
        
        MESSAGE_SINGLE(NORMAL, logger, "Updating user " << (*iter)->getName() << "'s throughput by current throughput of "
                                    << throughputThisTTI[*iter] << " to epx. average of " << throughputHistory[*iter] << "\n");
    }
    


}

void 
ZFScheduler::updateZFFeedback(ltea::mac::scheduler::UserSet& allUsers)
    {
        
        switch (feedbackMode)
        {
            case PU2RC:
            {
                for(ltea::mac::scheduler::UserSet::const_iterator iter = allUsers.begin(); iter!= allUsers.end(); iter++)
                {
                    wns::node::Interface* thisUser = *iter;
                    pu2rcFeedback[thisUser] = dynamic_cast<imtaphy::receivers::feedback::PU2RCFeedback*>(feedbackManager->getFeedback(thisUser, scheduleForTTI).get());
                    assure(pu2rcFeedback[thisUser], "Expected PU2RC feedback");
                    throughputThisTTI[thisUser] = 0.0;
                }
                break;
            }
            case Rank1:
            {
                for(ltea::mac::scheduler::UserSet::const_iterator iter = allUsers.begin(); iter!= allUsers.end(); iter++)
                {
                    wns::node::Interface* thisUser = *iter;
                    rel8Feedback[thisUser] = feedbackManager->getFeedback(thisUser, scheduleForTTI);
                    throughputThisTTI[thisUser] = 0.0;
                }

                break;
            }
            default:
            {
                assure(0, "Unknown feedback mode");
            }
        }

        
//         std::cout << "ZFScheduler updateZFFeedback is called at TTI " << scheduleForTTI << std::endl;
    }

ZFGroup
ZFScheduler::computeZFforPRB(ltea::mac::scheduler::UserSet newTransmissionUsers, unsigned int prb)
{
    std::multimap<double, wns::node::Interface*> metric;
    std::map<wns::node::Interface*, double> SINR;
    ltea::mac::scheduler::UserSet remainingActiveUsers;
    
    ZFGroup best(prb, numTxAntennas);
    ZFGroup current(prb, numTxAntennas);
    float bestMetric = 0;
    
    if (newTransmissionUsers.size() > 0)        
    {
        for (ltea::mac::scheduler::UserSet::const_iterator user_iter = newTransmissionUsers.begin(); user_iter != newTransmissionUsers.end(); ++user_iter)
        {
            wns::node::Interface* thisUser = *user_iter;
            SINR[thisUser] = getEstimatedSINR(thisUser, prb).get_factor(); 
        }
        
        remainingActiveUsers = newTransmissionUsers;
        std::set<unsigned int> occupiedCDIs;
        
        for (unsigned int n_users = 1; n_users <= numTxAntennas; ++n_users)
        {  
            unsigned int newUserIndex = n_users - 1;
            wns::node::Interface* selectedUser = NULL;
            
            for (ltea::mac::scheduler::UserSet::const_iterator userIter = remainingActiveUsers.begin(); userIter != remainingActiveUsers.end(); userIter++)
            {
                wns::node::Interface* newUser = *userIter;
                
                assure(occupiedCDIs.size() == n_users -1, "Not enough CDIs saved");          
                            
                if (occupiedCDIs.find(getChannelVectorId(newUser, prb)) == occupiedCDIs.end())
                {    
                    imtaphy::detail::ComplexFloatMatrixPtr newUsersChannel = getChannelVector(newUser, prb);
                    // add this user to position newUserIndex
                    current.setUser(newUser, newUserIndex, newUsersChannel);


                    if (current.getRank() == n_users)
                    {
                        std::vector<wns::node::Interface*> currentUsers = current.getUsers();
                        float currentSumMetric = 0;
                        
                        for (unsigned int u = 0; u < currentUsers.size(); u++)
                        {
                            wns::node::Interface* member = currentUsers[u];
                            
                            float precoderNorm = current.getPrecoderNorm(member);
                            float precoderNormSquared = precoderNorm * precoderNorm;
                            
                            double est_sinr = static_cast<float>(numTxAntennas) / static_cast<float>(n_users) * SINR[member] / precoderNormSquared; 
                            current.setSINROffset(member, getSINRCorrection(member, prb) + wns::Ratio::from_factor( static_cast<float>(numTxAntennas) / (static_cast<float>(n_users) * precoderNormSquared)));

                            currentSumMetric += log2(1.0 + std::min(est_sinr, wns::Ratio::from_dB(20).get_factor())) / throughputHistory[member];
                        }

                        if (currentSumMetric > bestMetric)
                        {
                            bestMetric = currentSumMetric;
                            best = current;
                            selectedUser = newUser;
                            current.setMetric(currentSumMetric);
                        }
                        
                    }
                }
                else
                {
        //                    std::cout << "Cannot add user " << newUser->getName() << " because it does not have an available CDI\n";
                }
            } // end of loop over users
            if (!selectedUser)
            {
                break; // no more users added, stop here
            }
            else
            {
        //                std::cout << "Finally selected user " << selectedUser->getName() << " on level " << n_users << "\n";
                
                // extend from best config
                current = best;
                
                remainingActiveUsers.erase(selectedUser);
                occupiedCDIs.insert(getChannelVectorId(selectedUser, prb));
            }                
        } // loop over group size (n_users)

    std::vector<wns::node::Interface*> finalGroup = best.getUsers();
        //std::cout << "Selected group with " << finalGroup.size() << " users and sum metric = " << bestMetric <<"\n";
        for (unsigned int i = 0; i < finalGroup.size(); i++)
        {
    //            std::cout << "User at position " << i << " is " << finalGroup[i]->getName() << " with SINR offset " << best.getSINROffset(finalGroup[i]) << "\n";
            
        }
        groupSizeContextCollector->put(finalGroup.size());
        
    } // if new transmission users exist    

    return best;
}

void
ZFScheduler::computeSchedulingResult(ZFGroup best, std::map<wns::node::Interface*,SchedulingResult>* col_allocation)
{
    
    std::vector<wns::node::Interface*> groupMembers = best.getUsers();

    
#ifndef WNS_NDEBUG 
    std::vector<std::vector<wns::Ratio> > patterns(groupMembers.size(), std::vector<wns::Ratio>(360));
#endif

    
    for (unsigned int userId = 0; userId < groupMembers.size(); userId++)
    {
        wns::node::Interface* thisUser = groupMembers[userId];

        
        (*col_allocation)[thisUser].scheduledUser = thisUser;
        (*col_allocation)[thisUser].rank = 1;
        (*col_allocation)[thisUser].prbAndTracingInfo = ltea::mac::PRBSchedulingTracingDictMap();

        imtaphy::interface::PowerAndPrecoding pap;
        
        // Equally devide power among the users
        pap.power = txPowerdBmPerPRB / static_cast<float>(groupMembers.size());
        pap.precoding = best.getPrecoder(thisUser);

        //imtaphy::detail::displayMatrix(*pap.precoding);
        (*col_allocation)[thisUser].prbPowerPrecodingMap[best.getPRB()] = pap;
        (*col_allocation)[thisUser].prbPowerOffsetForLA[best.getPRB()] = best.getSINROffset(thisUser);
        
        
#ifndef WNS_NDEBUG    
        imtaphy::antenna::LinearAntennaArray* antennaArray = dynamic_cast<imtaphy::antenna::LinearAntennaArray*>(myStation->getAntenna());
        assure(antennaArray, "No antenna array");
        
        antennaArray->computeAntennaPattern(*pap.precoding, patterns[userId], channel->getSpectrum()->getSystemCenterFrequencyWavelenghtMeters(imtaphy::Downlink));
#endif
    }
    
#ifndef WNS_NDEBUG
    // don't write too much output: only BSs 1-3 and only every 10 TTIs
    // and only on PRB 1
    if ((myStation->getNode()->getNodeID() <= 3) && ((scheduleForTTI % 10) == 0) && (best.getPRB() == 1))
    {
        std::ofstream patternFile;
        std::stringstream ss;
        ss << wns::simulator::getConfiguration().get<std::string>("outputDir") << "/pattern_" << myStation->getNode()->getName() << "_TTI" << scheduleForTTI << ".dat";
        patternFile.open(ss.str().c_str());

        for (unsigned int i = 0; i < 360; i++)
        {
            patternFile << i;
            for (unsigned int userId = 0; userId < groupMembers.size(); userId++)
            {
                patternFile << "\t" << patterns[userId][i].get_factor() / static_cast<float>(groupMembers.size());
            }
            patternFile << "\n";
        }
        patternFile.close();
    }
#endif
    
}
    
        
void 
ZFScheduler::doLinkAdapationAndRegisterTransmissions()
{
    for (std::list<SchedulingResult>::iterator iter = scheduledUsers.begin(); iter != scheduledUsers.end(); iter++)
    {
        std::vector<wns::ldk::CompoundPtr> transportBlocks; 
        transportBlocks.clear();
        
        unsigned int numPRBs = iter->prbPowerPrecodingMap.size();
        
        double user_rate = 0.0;
        
        assure(iter->rank == 1, "Only suports rank-1 transmission");
        
        rankContextCollector->put(iter->rank);

        ltea::mac::la::downlink::LinkAdaptationResult laResult0;
        laResult0 = linkAdaptation->performLinkAdaptation(iter->scheduledUser,
                                                        0,                   // this is the first "spatial" transport block 
                                                        iter->prbPowerOffsetForLA,      //
                                                        scheduleForTTI,      //
                                                        1,// number of layers for this TB  
                                                        pdcchLength,
                                                        provideRel10DMRS,
                                                        numRel10CSIrsSets
                                                        );
        
    
        unsigned int tb0Size = mcsLookup->getSize(laResult0.mcsIndex, numPRBs, 1);
        wns::ldk::CompoundPtr compound0 = queue->getHeadOfLinePDUSegment(iter->scheduledUser, tb0Size);                                                                    

        MESSAGE_SINGLE(NORMAL, logger, "On the physical layer a transport block of " << tb0Size << " bits (" << numPRBs << " PRBs, MCS " << laResult0.mcsIndex << ") has been allocated for user "
                                        << iter->scheduledUser->getName() << ", payload size is " 
                                        << compound0->getLengthInBits() << " bits. Still queued: " 
                                        << queue->numBitsForUser(iter->scheduledUser) << " bits." );
            
        user_rate += compound0->getLengthInBits();
        
#ifndef WNS_NDEBUG
        if (tb0Size > compound0->getLengthInBits() + compound0->getPCI()->getSize())
            MESSAGE_SINGLE(NORMAL, logger, "WARNING: Reservation on PHY (and thus maybe number of occupied resource blocks) is larger than needed: " 
                                            << tb0Size << " > " << compound0->getLengthInBits() << " + " << compound0->getPCI()->getSize() << " bits.");
#endif                                                                     

            
        tbSizeContextCollector->put(tb0Size, boost::make_tuple("Stream", 0));

        ltea::mac::DownlinkControlInformation* dci0 = activateCommand( compound0->getCommandPool() );
        dci0->peer.assignedToLayers = std::vector<unsigned int>(1,1);
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
        
        
        double linearAvgEstSINR = 0.0;
        int linearAvgEstSINRCounter = 0;
        
        for (imtaphy::interface::PrbPowerPrecodingMap::const_iterator prbIter =  iter->prbPowerPrecodingMap.begin(); prbIter != iter->prbPowerPrecodingMap.end(); prbIter++)
        {
            linearAvgEstSINR += (getExactSINR(iter->scheduledUser, prbIter->first) + iter->prbPowerOffsetForLA[prbIter->first]).get_factor();
            linearAvgEstSINRCounter++;
        }

        assure(linearAvgEstSINRCounter, "No PRB?");
        dci0->magic.estimatedLinearAvgSINR = wns::Ratio::from_factor(linearAvgEstSINR / static_cast<double>(linearAvgEstSINRCounter));
        ////////////
        
        transportBlocks.push_back(compound0);


        
        harq->storeScheduledTransportBlocks(iter->scheduledUser, transportBlocks);
        txService->registerTransmission(iter->scheduledUser,
                                        transportBlocks,
                                        iter->rank, // number of layers
                                        iter->prbPowerPrecodingMap);
        
        throughputThisTTI[iter->scheduledUser] += user_rate;
    }
    
    
    // unblock the queues
    getReceptor()->wakeup();
}

inline
imtaphy::detail::ComplexFloatMatrixPtr 
ZFScheduler::getChannelVector(wns::node::Interface* user, unsigned int prb)
{
    switch (feedbackMode)
    {
        case PU2RC:
        {
            return codebook->get4TxCodebookColumn(pu2rcFeedback[user]->pmi[prb], 
                                                  pu2rcFeedback[user]->columnIndicator[prb]);
            break;
        }
        case Rank1:
        {
            // avoid invalid PMI feedback e.g. during startup
            unsigned int pmi = rel8Feedback[user]->pmi[prb];
            pmi = (pmi < 16) ? pmi :  0;
            return codebook->get4TxCodebookColumn(pmi, 0);

            break;
        }
        default:
        {
            assure(0, "Unknown feedback mode");
        }
    }
}

inline
unsigned int 
ZFScheduler::getChannelVectorId(wns::node::Interface* user, unsigned int prb)
{
    switch (feedbackMode)
    {
        case PU2RC:
        {
            return codebook->getIndex(pu2rcFeedback[user]->pmi[prb], pu2rcFeedback[user]->columnIndicator[prb]);
            break;
        }
        case Rank1:
        {
            // avoid invalid PMI feedback e.g. during startup
            unsigned int pmi = rel8Feedback[user]->pmi[prb];
            pmi = (pmi < 16) ? pmi :  0;

            return pmi;
            break;
        }
        default:
        {
            assure(0, "Unknown feedback mode");
        }
    }
}

inline
wns::Ratio 
ZFScheduler::getEstimatedSINR(wns::node::Interface* user, unsigned int prb)
{
    switch (feedbackMode)
    {
        case PU2RC:
        {
            return blerModel->getSINRthreshold(pu2rcFeedback[user]->cqiTb1[prb]);
            break;
        }
        case Rank1:
        {
            return blerModel->getSINRthreshold(rel8Feedback[user]->cqiTb1[prb]) + getSINRCorrection(user, prb);
            break;
        }
        default:
        {
            assure(0, "Unknown feedback mode");
        }
    }
}

inline
wns::Ratio 
ZFScheduler::getExactSINR(wns::node::Interface* user, unsigned int prb)
{
    switch (feedbackMode)
    {
        case PU2RC:
        {
            return pu2rcFeedback[user]->sinrMatrix[prb][pu2rcFeedback[user]->pmi[prb]][pu2rcFeedback[user]->columnIndicator[prb]];
            break;
        }
        case Rank1:
        {
            return rel8Feedback[user]->magicSINRs[prb][0];
            break;
        }
        default:
        {
            assure(0, "Unknown feedback mode");
        }
    }
}


inline
wns::Ratio 
ZFScheduler::getSINRCorrection(wns::node::Interface* user, unsigned int prb)
{
    switch (feedbackMode)
    {
        case PU2RC:
        {
            return wns::Ratio::from_factor(1.0);
            break;
        }
        case Rank1:
        {
            // TODO: Tx scaling and no array gain at receiver but extra intra interference
            // adapt the SINR to a power normalization for 4 layer transmission
            return wns::Ratio::from_factor(1.0/4.0) + wns::Ratio::from_factor(static_cast<double>(rel8Feedback[user]->rank) / 4.0);
            break;
        }
        default:
        {
            assure(0, "Unknown feedback mode");
        }
    }

}