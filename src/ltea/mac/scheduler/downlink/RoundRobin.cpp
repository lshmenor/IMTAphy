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

#include <IMTAPHY/ltea/mac/scheduler/downlink/RoundRobin.hpp>
#include <limits>

STATIC_FACTORY_REGISTER_WITH_CREATOR(
     ltea::mac::scheduler::downlink::RoundRobin,
     wns::ldk::FunctionalUnit,
     "ltea.mac.scheduler.downlink.RoundRobin",
     wns::ldk::FUNConfigCreator);

     

using namespace ltea::mac::scheduler::downlink;

RoundRobin::RoundRobin(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config) :
    SchedulerBase(fun, config),
    codebook(&(wns::SingletonHolder<imtaphy::receivers::LteRel8Codebook<float> >::Instance())),
    prbsPerUser(config.get<int>("prbsPerUser")),
    useFixedAllocation(false)
{
    // a positive value makes the scheduler allocate only that number of PRBs to a user so that 
    // multiple users per TTI might get scheduled
    // 0 means allocate all to one (different) user each TTI
    
    if (prbsPerUser == 0)
        prbsPerUser = std::numeric_limits<int>::max();
    
    // negative means to fit all users into the TTI (if possible) and keep that allocation fixed afterwards
    if (prbsPerUser < 0)
        useFixedAllocation = true;
}


void 
RoundRobin::initScheduler()
{
   pdcchLength = 3;
   provideRel10DMRS = false;
   numRel10CSIrsSets = 0;
}



void 
RoundRobin::doScheduling()
{
    ltea::mac::scheduler::UserSet users;
    std::set<wns::node::Interface*, imtaphy::detail::WnsNodeInterfacePtrCompare> usersWithRetransmissions;
    
    if (useFixedAllocation && fixedAllocation.size())
    {
        for (std::list<ltea::mac::scheduler::downlink::SchedulingResult>::iterator iter = fixedAllocation.begin(); iter != fixedAllocation.end(); iter++)
        {
            if (! usersPRBManager.isActive(iter->scheduledUser))
            {   // if user is not available (i.e., doing a retransmission)
                continue;
            }
            else
            {
                imtaphy::receivers::feedback::LteRel8DownlinkFeedbackPtr feedback = feedbackManager->getFeedback(iter->scheduledUser, scheduleForTTI);

                scheduledUsers.push_back(*iter);
                for (imtaphy::interface::PrbPowerPrecodingMap::iterator iter2 = iter->prbPowerPrecodingMap.begin();
                        iter2 != iter->prbPowerPrecodingMap.end(); iter2++)
                {
                    unsigned int prb = iter2->first;
                    iter->prbAndTracingInfo[prb]["PMI"] = feedback->pmi[prb];
                    iter->prbAndTracingInfo[prb]["RI"] = feedback->rank;
                    iter->rank = feedback->rank;    
                    
                    imtaphy::interface::PowerAndPrecoding powerAndPrecoding;
                    powerAndPrecoding.power = usersPRBManager.getAvailablePower(iter->scheduledUser, prb);
                    powerAndPrecoding.precoding = codebook->getPrecodingMatrix(numTxAntennas,
                                                                               feedback->rank, // layers
                                                                               feedback->pmi[prb]); //pmi
                    iter2->second = powerAndPrecoding; 
                    usersPRBManager.markPRBused(prb);
                }
                usersPRBManager.removeActiveUser(iter->scheduledUser);
            }
        }
        // when scheduling is done, let the base class perform the Link Adaptation and register the transmissions
        doLinkAdapationAndRegisterTransmissions();

        return;
    }
    
    unsigned int numInitialPRBs = usersPRBManager.getNumPRBsAvailable();
    
    if (!syncHARQ)
    {   // when HARQ is not handled the synchronous way by SchedulerBase, we have to trigger it
        usersWithRetransmissions = harq->getUsersWithRetransmissions();
        
        // HARQ users should be in allUsers anyway but who knows...
        for (std::set<wns::node::Interface*, imtaphy::detail::WnsNodeInterfacePtrCompare>::const_iterator iter = usersWithRetransmissions.begin();
             iter != usersWithRetransmissions.end(); iter++)
        {
            users.insert(*iter);
        }
    }
    
    for (std::vector<wns::node::Interface*>::const_iterator iter = allUsers.begin();
         iter != allUsers.end(); iter++)
    {
        users.insert(*iter);
    }
    
    
    UserList::iterator queueIter = roundRobinQueue.begin();
    while (queueIter != roundRobinQueue.end())
    {
        
        if (users.find(*queueIter) == users.end()) // not associated any more
        {
            // user in round robin queue not associated anymore -> delete from RR queue
            roundRobinQueue.erase(queueIter++);
        }
        else
        {
            // user in queue, do not need to add later
            users.erase(*queueIter);

            // leave user in queue
                queueIter++;
        }
    }
    
    // now, no non-active users remain in round robin queue and 
    // all users remaining in users have to be added to the end of the queue
    // queue Iter points to the end of the queue
    UserList::iterator queueEnd = queueIter;
    
    for (ltea::mac::scheduler::UserSet::iterator iter = users.begin(); iter != users.end(); iter++)
    {
        roundRobinQueue.insert(queueEnd, *iter);
    }
    
    // now go once from the beginnging over the list
    // all users that can be scheduled, will be scheduled and put at the end of the list
    // after we are through
    
    UserList tail;
    
    queueIter = roundRobinQueue.begin();
    while (queueIter != roundRobinQueue.end())
    {
        if  ((!usersPRBManager.isActive(*queueIter)) && (usersWithRetransmissions.find(*queueIter) == usersWithRetransmissions.end()))
        {   // user neither has new nor old data -> ignore
            queueIter++;
            continue;
        }
        else // user has either new or old data
        {
            wns::node::Interface* user = *queueIter;
            
            if (usersWithRetransmissions.find(user) != usersWithRetransmissions.end())
            {
                if (performRetransmissionsFor(user))
                {
                    tail.insert(tail.end(), user);
                    roundRobinQueue.erase(queueIter++);
                }
                else
                {   // the user needed a retransmission but I did not have resources
                    // probably has to wait until he is the first in line
                    queueIter++;
                    continue;
                }
            }
            else
            {
                // else continue with new data
                if (!usersPRBManager.isActive(user))
                {
                    queueIter++;
                    continue;
                }
                
                imtaphy::interface::PRBVector prbs = usersPRBManager.getPRBsAvailable(user);

                if (prbs.size() != 0)
                {
                    // I can schedule this user somewhere
                    
                    imtaphy::receivers::feedback::LteRel8DownlinkFeedbackPtr feedback = feedbackManager->getFeedback(user, scheduleForTTI);
                    
                    SchedulingResult allocation;
                    allocation.scheduledUser = user;
                    allocation.prbPowerPrecodingMap.clear();
                    allocation.prbPowerOffsetForLA.clear();
                    allocation.rank = feedback->rank;
                    
                    if (useFixedAllocation)
                    {
                        // determine number of PRBs for initial (and then fixed) frequency allocation
                        
                        unsigned int numUsers = users.size();
                        
                        if (scheduledUsers.size() < numInitialPRBs % numUsers)
                        {
                            prbsPerUser = numInitialPRBs / numUsers + 1;
                        }
                        else
                        {
                            prbsPerUser = numInitialPRBs / numUsers;
                        }
                        
                    }
                    

                    // assign all available PRBs up to the maximum number per user
                    unsigned int count  = 0;
                    for (unsigned int prb = 0; (prb < prbs.size()) && (count < prbsPerUser); prb++, count++)
                    {
                        usersPRBManager.markPRBused(prbs[prb]);
                        
                        allocation.prbAndTracingInfo[prbs[prb]]["PMI"] = feedback->pmi[prbs[prb]];
                        allocation.prbAndTracingInfo[prbs[prb]]["RI"] = feedback->rank;
    
                        imtaphy::interface::PowerAndPrecoding powerAndPrecoding;
                        powerAndPrecoding.power = usersPRBManager.getAvailablePower(user, prbs[prb]);
                        powerAndPrecoding.precoding = codebook->getPrecodingMatrix(numTxAntennas,
                                                                                 allocation.rank, // layers
                                                                                 feedback->pmi[prbs[prb]]); //pmi
                        allocation.prbPowerPrecodingMap[prbs[prb]] =  powerAndPrecoding;
                        allocation.prbPowerOffsetForLA[prbs[prb]] = wns::Ratio::from_factor(1.0);
                    }
                    
                    scheduledUsers.push_back(allocation);

                    usersPRBManager.removeActiveUser(user);
                    
                    // remove him from current position in queue and put him to the back
                    tail.insert(tail.end(), *queueIter);
                    roundRobinQueue.erase(queueIter++);
                }
                
                else
                { // I cannot schedule this user, try the next in line
                    queueIter++;
                }
            } // end else new data
        } // end user has either new or old data
    } // end while loop
    
    if (useFixedAllocation)
    {
        // initial scheduling done
        fixedAllocation = scheduledUsers;
        syncHARQ = true;
    }
    
    // append new tail to queue
    roundRobinQueue.insert(roundRobinQueue.end(), tail.begin(), tail.end());
    
    // when scheduling is done, let the base class perform the Link Adaptation and register the transmissions
    doLinkAdapationAndRegisterTransmissions();
}


