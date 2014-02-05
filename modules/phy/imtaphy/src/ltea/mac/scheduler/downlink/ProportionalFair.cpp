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

#include <IMTAPHY/ltea/mac/scheduler/downlink/ProportionalFair.hpp>

STATIC_FACTORY_REGISTER_WITH_CREATOR(
     ltea::mac::scheduler::downlink::ProportionalFair,
     wns::ldk::FunctionalUnit,
     "ltea.mac.scheduler.downlink.ProportionalFair",
     wns::ldk::FUNConfigCreator);


using namespace ltea::mac::scheduler::downlink;

ProportionalFair::ProportionalFair(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config) :
    SchedulerBase(fun, config),
    currentUser(0),
    codebook(&(wns::SingletonHolder<imtaphy::receivers::LteRel8Codebook<float> >::Instance())),
    alpha(config.get<double>("throughputSmoothing")),
    historyExponent(config.get<double>("historyExponent"))
{
    assure((alpha > 0.0) && (alpha <= 1.0), "Exponential smothing factor must be 0 < alpha <= 1 where smaller values lead to longer averaging memory");
}


void 
ProportionalFair::initScheduler()
{
   pdcchLength = 3;
   provideRel10DMRS = false;
   numRel10CSIrsSets = 0;

   assure(syncHARQ, "ProportionalFair currently only supports synchronous HARQ operation, i.e., with HARQ retransmissions performed by SchedulerBase");
   
   // Init the throughput history for exponential averaging
   for (unsigned int i = 0; i < allUsers.size(); i++)
   {
       throughputHistory[allUsers[i]] = 1.0;
   }
}


void 
ProportionalFair::doScheduling()
{
    PerUserPrbPowerPrecoderMap resourceMap;
    PerUserPowerOffsetMap powerOffsetMap;
    typedef std::map<wns::node::Interface*, ltea::mac::PRBSchedulingTracingDictMap> PerUserPRBtracingDictMap;
    PerUserPRBtracingDictMap userTracing;
    
    
    ltea::mac::scheduler::UserSet activeUsers = usersPRBManager.getActiveUsers();
    std::map<wns::node::Interface*, imtaphy::receivers::feedback::LteRel8DownlinkFeedbackPtr, imtaphy::detail::WnsNodeInterfacePtrCompare> feedback;
    
    std::map<wns::node::Interface*, double, imtaphy::detail::WnsNodeInterfacePtrCompare> throughputThisTTI;
    std::map<wns::node::Interface*, Bit> availableBitsInQueue;

    // init counter for scheduled throughput during this TTI needed for updating the history at the end
    // also, we get the feedback to avoid calling over and over
    for (UserSet::const_iterator iter = activeUsers.begin(); iter != activeUsers.end(); iter++)
    {
        throughputThisTTI[*iter] = 0.0;
        availableBitsInQueue[*iter] = queue->numBitsForUser(*iter);
        MESSAGE_SINGLE(NORMAL, logger, "According to our queue, user " << (*iter)->getName() << " has " << availableBitsInQueue[*iter] << " bits in the queue.");
        feedback[*iter] = feedbackManager->getFeedback(*iter, scheduleForTTI);
    }

    imtaphy::interface::PRBVector allPrbs = usersPRBManager.getPRBsAvailable();
    

    for (unsigned int i = 0; i < allPrbs.size(); i++)
    {
        unsigned int prb = allPrbs[i];
        
        ltea::mac::scheduler::UserSet usersThisPRB = usersPRBManager.getActiveUsers(prb);
        
        if (usersThisPRB.size() == 0)
            continue;
        
        std::multimap<double, wns::node::Interface*> ranking;
        for (UserSet::const_iterator iter = usersThisPRB.begin(); iter != usersThisPRB.end(); iter++)
        {
            wns::node::Interface* user = *iter;
            
            double expectedSpectralEfficiency = estimateExpectedThroughput(feedback[user], prb);
            
            double metric = expectedSpectralEfficiency / pow(throughputHistory[user], historyExponent);
            ranking.insert(std::make_pair<double, wns::node::Interface*>(metric,user));
            
            MESSAGE_SINGLE(NORMAL, logger, "Metric for user " << user->getName() << " is: " << metric << " = " << expectedSpectralEfficiency 
                                            << " / " << throughputHistory[user] << "^" << historyExponent);
        }
        
        // ranking is a std::map that is inheritely sorted from smallest metric to largest; 
        // "rbegin" return the reverse iterator pointing to the element with the highest metric
        wns::node::Interface* selectedUser =  ranking.rbegin()->second;

        MESSAGE_SINGLE(NORMAL, logger, "Selected user " << selectedUser->getName() 
                                    << " based on metric=" << ranking.rbegin()->first
                                    << " compared to worst metric " << ranking.begin()->first << " on PRB" << prb);
        
        
        // The LTE Rel8 codebook
        imtaphy::interface::PowerAndPrecoding powerAndPrecoding;
        powerAndPrecoding.power = usersPRBManager.getAvailablePower(selectedUser, prb);
        powerAndPrecoding.precoding = codebook->getPrecodingMatrix(numTxAntennas,
                                                                      feedback[selectedUser]->rank,
                                                                      feedback[selectedUser]->pmi[prb]);
        resourceMap[selectedUser][prb] = powerAndPrecoding;
        
        powerOffsetMap[selectedUser][prb] = wns::Ratio::from_factor(1.0); // no offset, we honor the feedback's choice
        throughputThisTTI[selectedUser] += estimateExpectedThroughput(feedback[ranking.rbegin()->second], prb);
        
        // provide info for tracing
        userTracing[selectedUser][prb]["PMI"] = feedback[selectedUser]->pmi[prb];
        userTracing[selectedUser][prb]["RI"] = feedback[selectedUser]->rank;
        userTracing[selectedUser][prb]["PFmetric"] = ranking.rbegin()->first;

        ltea::mac::la::downlink::LinkAdaptationResult provisionalLa 
            = linkAdaptation->performLinkAdaptation(selectedUser, 0, powerOffsetMap[selectedUser], 
                                                    scheduleForTTI, feedback[selectedUser]->rank, pdcchLength, provideRel10DMRS, numRel10CSIrsSets);
        Bit currentTBsize = mcsLookup->getSize(provisionalLa.mcsIndex, powerOffsetMap[selectedUser].size(), feedback[selectedUser]->rank);
        
        if (currentTBsize >= availableBitsInQueue[selectedUser])
        {
            MESSAGE_SINGLE(NORMAL, logger, "User " << selectedUser->getName() << " removed from scheduling because current allocation is enough to schedule all queued bits");
            usersPRBManager.removeActiveUser(selectedUser);
        }
        else
        {
            MESSAGE_SINGLE(NORMAL, logger, "Still not enough resources (currently " << currentTBsize << " bits)for user " 
                                            << selectedUser->getName() << " (needs " << availableBitsInQueue[selectedUser] << " bits) continue scheduling");
        }
    } // end of loop over all PRBs
    
    for (PerUserPrbPowerPrecoderMap::const_iterator iter = resourceMap.begin(); iter != resourceMap.end(); iter++)
    {
        SchedulingResult allocation;
        allocation.scheduledUser = iter->first;
        allocation.rank = feedback[allocation.scheduledUser]->rank;
        allocation.prbPowerPrecodingMap = iter->second;
        allocation.prbPowerOffsetForLA = powerOffsetMap[allocation.scheduledUser];
        allocation.prbAndTracingInfo = userTracing[allocation.scheduledUser];
        
        scheduledUsers.push_back(allocation);
    }

    // Update throughput history. Active users that weren't scheduled have a throughputThisTTI==0
    // and the others the sum of the spectral efficiencies corresponding to their CQIs
    // As long as the transport block can actually be filled with enough data, this should be a 
    // sufficiently accurate (i.e. proportional) approximation of the throughput they will actuall get
    for (ltea::mac::scheduler::UserSet::const_iterator iter = activeUsers.begin(); iter != activeUsers.end(); iter++)
    {
        throughputHistory[*iter] = (1.0 - alpha) * throughputHistory[*iter] + alpha * throughputThisTTI[*iter];
        
        MESSAGE_SINGLE(NORMAL, logger, "Updating user " << (*iter)->getName() << "'s throughput by current throughput of "
                                       << throughputThisTTI[*iter] << " to epx. average of " << throughputHistory[*iter] << "\n");
    }
    
    // when scheduling is done, let the base class perform the Link Adaptation and register the transmissions
    doLinkAdapationAndRegisterTransmissions();
}

inline
double 
ProportionalFair::estimateExpectedThroughput(imtaphy::receivers::feedback::LteRel8DownlinkFeedbackPtr feedback, unsigned int prb)
{
    double expectedSpectralEfficiency = 0;
    
    switch (feedback->rank)
    {
        case 1:
        default:
            expectedSpectralEfficiency = imtaphy::l2s::CQITable[feedback->cqiTb1[prb]].efficiency;
            break;
        case 2:
            expectedSpectralEfficiency = imtaphy::l2s::CQITable[feedback->cqiTb1[prb]].efficiency +
                                         imtaphy::l2s::CQITable[feedback->cqiTb2[prb]].efficiency;
            break;
        case 3:
            expectedSpectralEfficiency = imtaphy::l2s::CQITable[feedback->cqiTb1[prb]].efficiency +
                                         imtaphy::l2s::CQITable[feedback->cqiTb2[prb]].efficiency +
                                         imtaphy::l2s::CQITable[feedback->cqiTb2[prb]].efficiency;

            break;
        case 4:
            expectedSpectralEfficiency = imtaphy::l2s::CQITable[feedback->cqiTb1[prb]].efficiency +
                                         imtaphy::l2s::CQITable[feedback->cqiTb1[prb]].efficiency +
                                         imtaphy::l2s::CQITable[feedback->cqiTb2[prb]].efficiency +
                                         imtaphy::l2s::CQITable[feedback->cqiTb2[prb]].efficiency;
            break;  
    }

    // the expectedSpectralEfficiency is a gross spectral efficiency that is 
    // directly computed from the bits per modulation symbol and the effective code
    // rate. the exact number of available REs will be estimated by the link adaptation
    // mechanism. here we use a conservative estimate. as this will also be used for 
    // determining if all available bits fit into the allocated PRBs, we have some margin 
    // for RLC overhead and link adaptation choosing a conservative MCS
    
    return expectedSpectralEfficiency * static_cast<double>(12 * 14) * 0.7;
}