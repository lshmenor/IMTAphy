/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2009
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 5, D-52074 Aachen, Germany
 * phone: ++49-241-80-27910,
 * fax: ++49-241-80-22242
 * email: info@openwns.org
 * www: http://www.openwns.org
 * _____________________________________________________________________________
 *
 * openWNS is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License version 2 as published by the
 * Free Software Foundation;
 *
 * openWNS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#include <WNS/scheduler/strategy/dsastrategy/Fixed.hpp>
#include <WNS/scheduler/SchedulerTypes.hpp>

using namespace wns::scheduler;
using namespace wns::scheduler::strategy;
using namespace wns::scheduler::strategy::dsastrategy;

// Used for sorting of resources
bool 
FreqFirst::operator()(DSAResult a, DSAResult b) const
{
    /*if(a.spatialLayer != b.spatialLayer)
    {
        return a.spatialLayer < b.spatialLayer;
    }
    else
    {*/
        if(a.subChannel != b.subChannel)
        {
            return a.subChannel < b.subChannel;
        }
        else
        {
            return a.timeSlot < b.timeSlot;
        }
    //}
}

bool
TimeFirst::operator()(DSAResult a, DSAResult b) const
{
    /*if(a.spatialLayer != b.spatialLayer)
    {
        return a.spatialLayer < b.spatialLayer;
    }
    else
    {*/
    if(a.timeSlot != b.timeSlot)
    {
        return a.timeSlot < b.timeSlot;
    }
    else
    {
        return a.subChannel < b.subChannel;
    }
    //}
}

STATIC_FACTORY_REGISTER_WITH_CREATOR(Fixed,
                                     DSAStrategyInterface,
                                     "Fixed",
                                     wns::PyConfigViewCreator);

Fixed::Fixed(const wns::pyconfig::View& config)
    : DSAStrategy(config),
    timeFirst_(config.get<bool>("timeFirst"))
{
}

Fixed::~Fixed()
{
}

// call this before each timeSlot/frame
void
Fixed::initialize(SchedulerStatePtr schedulerState,
                         SchedulingMapPtr schedulingMap)
{
    DSAStrategy::initialize(schedulerState,schedulingMap); // must always initialize base class too

    int maxSubChannel = schedulerState->currentState->strategyInput->getFChannels();
    int numberOfTimeSlots = schedulerState->currentState->strategyInput->getNumberOfTimeSlots();
    int maxSpatialLayers = schedulerState->currentState->strategyInput->getMaxSpatialLayers();

    int numberOfPriorities = colleagues.registry->getNumberOfPriorities();

    std::set<wns::scheduler::UserID> userIDs;
    unsigned int numberOfUsers = 0;
    unsigned int numberOfResources = maxSubChannel * numberOfTimeSlots;// * maxSpatialLayers;
    sortedResources_.clear();
    spatialLayer_.clear();
    resAmount_.clear();
    resStart_.clear();

    bool sdma = maxSpatialLayers > 1 && schedulerState->currentState->strategyInput->beamforming;
    GroupingPtr grouping = schedulerState->currentState->getGrouping();
    if (sdma && grouping != GroupingPtr()) 
    {
        //with SDMA resources are equally shared between groups
       numberOfUsers = grouping->groups.size();
       MESSAGE_SINGLE(NORMAL, logger, myUserID << " initialize: Distributing "
            << numberOfUsers << " groups on " << numberOfResources << " resources."
            << " isDL.isTx: "<<schedulerState->isDL<<"."<<schedulerState->isTx<<" ;with users "<<userIDs.size());
    } else {
        wns::scheduler::ConnectionSet conns;
        for(int prio = 0; prio < numberOfPriorities; prio++)
        {
            wns::scheduler::ConnectionSet c = 
                colleagues.registry->getConnectionsForPriority(prio);

            wns::scheduler::ConnectionSet::iterator it;
            for(it = c.begin(); it != c.end(); it++)
                conns.insert(*it);
        }
        
        wns::scheduler::ConnectionSet::iterator it;

        for(it = conns.begin(); it != conns.end(); it++)
        {
            wns::scheduler::UserID user = colleagues.registry->getUserForCID(*it);
            if(!user.isBroadcast())
            {
                    userIDs.insert(user);
                    spatialLayer_[user.getNodeID()] = 0;
            }
        }
        numberOfUsers = userIDs.size();
        MESSAGE_SINGLE(NORMAL, logger, "initialize: Distributing "
            << numberOfUsers << " users on " << numberOfResources << " resources.");
    }

    if(numberOfUsers == 0)
        return;

    for(int i = 0; i < maxSubChannel; i++)
    {
        for(int j = 0; j < numberOfTimeSlots; j++)
        {
                DSAResult res;
                res.subChannel = i;
                res.timeSlot = j;
                sortedResources_.push_back(res);
        }
    }
    if (timeFirst_)
        sortedResources_.sort(TimeFirst());
    else
        sortedResources_.sort(FreqFirst());
    int usersMore = numberOfResources % numberOfUsers;
    int usersLess = numberOfUsers - usersMore;
    int resourcesMore = int(numberOfResources / numberOfUsers) + 1;
    int resourcesLess = int(numberOfResources / numberOfUsers);

    MESSAGE_SINGLE(NORMAL, logger, "initialize: " 
        << usersLess << " users/groups get " << resourcesLess << ", "
        << usersMore << " users/groups get " << resourcesMore << " resources");

    assure(resourcesLess > 0, "Not enough resources for all users");
    assure(usersMore * resourcesMore + usersLess * resourcesLess == numberOfResources,
        "Mismatched resource distribution");

    std::set<wns::scheduler::UserID>::iterator uIt;
    std::list<DSAResult>::iterator itr;

    int plus = 0;
    //with SDMA user corresponds to group
    int user = 0;
    int counter = 0;
    uIt = userIDs.begin();

    for(itr = sortedResources_.begin(); itr != sortedResources_.end(); itr++)
    {
        if(counter == resourcesLess + plus|| itr == sortedResources_.begin())
        {
            if (sdma && grouping != GroupingPtr()) 
            {
                //user of the same group have the same resources seperated by spatialLayers
                int spatialLayer = 0;
                for (Group::iterator gIt = grouping->groups[user].begin(); gIt != grouping->groups[user].end(); ++gIt)
                {
                    wns::scheduler::UserID userID = (*gIt).first;
                    userIDs.insert(userID); //not useful
                    spatialLayer_[userID.getNodeID()] = spatialLayer;
                        ++spatialLayer;
                    resStart_[userID.getNodeID()] = itr;
                    resAmount_[userID.getNodeID()] = resourcesLess + plus;

                    MESSAGE_SINGLE(NORMAL, logger, "initialize: User " 
                        << userID.getName()<< " starts at " 
                        << itr->subChannel << "."
                        << itr->timeSlot << "."
                        << spatialLayer_[userID.getNodeID()]
                        << " and gets " << resourcesLess + plus << " resources");
                }
            } else {
            resStart_[(*uIt).getNodeID()] = itr;
            resAmount_[(*uIt).getNodeID()] = resourcesLess + plus;

            MESSAGE_SINGLE(NORMAL, logger, "initialize: User " 
                << (*uIt).getName()<< " starts at " 
                << itr->subChannel << "."
                << itr->timeSlot << "."
                << spatialLayer_[(*uIt).getNodeID()]
                << " and gets " << resourcesLess + plus << " resources");
            }
            counter = 0;
            uIt++;
            user++;
        }
        if(plus == 0 && user == usersLess)
        {
            plus++;
            counter++;
        }
    counter++;
    }
}

DSAResult
Fixed::getSubChannelWithDSA(RequestForResource& request,
                                   SchedulerStatePtr schedulerState,
                                   SchedulingMapPtr schedulingMap)
{
    assure(request.user.isBroadcast() || resStart_.find(request.user.getNodeID()) != resStart_.end(),
        "No resources for user " + request.user.getName());

    // Give the broadcast channel the first resource
    if(request.user.isBroadcast())
    { 
        assure(channelIsUsable(0, 
                                0,
                                0,
                                request, 
                                schedulerState, 
                                schedulingMap), "First resource not available for broadcast");
        MESSAGE_SINGLE(NORMAL, logger, "getSubChannelWithDSA(): Granting resource: 0.0.0"
            << " to " << request.toString());
        DSAResult dsaResult;
        dsaResult.subChannel = 0;
        dsaResult.timeSlot = 0;
        dsaResult.spatialLayer = 0;
        return dsaResult;
    }

    assure(resAmount_.find(request.user.getNodeID()) != resAmount_.end(),
        "Unknown resource amount for user " + request.user.getNodeID());

    int max = resAmount_[request.user.getNodeID()];
    std::list<DSAResult>::iterator it;
    int i = 0;
    bool found = false;
    for(it = resStart_[request.user.getNodeID()]; i < max && !found; it++)
    {
        i++;
        found = channelIsUsable(it->subChannel, 
                                it->timeSlot,
                                spatialLayer_[request.user.getNodeID()],
                                request, 
                                schedulerState, 
                                schedulingMap);
    }

    if(!found)
    {
        MESSAGE_SINGLE(NORMAL, logger, "getSubChannelWithDSA(): no free subchannel");
        DSAResult dsaResult;
        return dsaResult;
    }
    else
    {
        it--;
        MESSAGE_SINGLE(NORMAL, logger, "getSubChannelWithDSA(): Granting resource: "
            << it->subChannel << "." << it->timeSlot << "." 
            << spatialLayer_[request.user.getNodeID()]<< " to " << request.toString());
        DSAResult dsaResult;
        dsaResult.subChannel = it->subChannel;
        dsaResult.timeSlot = it->timeSlot;
        dsaResult.spatialLayer = spatialLayer_[request.user.getNodeID()];
        return dsaResult;
    }
} // getSubChannelWithDSA
