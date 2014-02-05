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

#include <WNS/scheduler/strategy/dsastrategy/BestChannel.hpp>
#include <WNS/scheduler/strategy/dsastrategy/DSAStrategyInterface.hpp>
#include <WNS/scheduler/SchedulerTypes.hpp>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace wns::scheduler;
using namespace wns::scheduler::strategy;
using namespace wns::scheduler::strategy::dsastrategy;

STATIC_FACTORY_REGISTER_WITH_CREATOR(BestChannel,
                                     DSAStrategyInterface,
                                     "BestChannel",
                                     wns::PyConfigViewCreator);

BestChannel::BestChannel(const wns::pyconfig::View& config)
    : DSAStrategy(config)
{
    //useRandomChannelAtBeginning =
    //config.get<bool>("useRandomChannelAtBeginning");
}

BestChannel::~BestChannel()
{
}

// call this before each timeSlot/frame
void
BestChannel::initialize(SchedulerStatePtr schedulerState,
                        SchedulingMapPtr schedulingMap)
{
    DSAStrategy::initialize(schedulerState,schedulingMap); // must always initialize base class too
    if (!userInfoMap.empty())
    {
        userInfoMap.clear();
    }
    // just a SmartPtr:
    //ChannelQualitiesOfAllUsersPtr
    assure(schedulerState->currentState!=RevolvingStatePtr(),"currentState must be valid");
    // set member:
    channelQualitiesOfAllUsers =
        schedulerState->currentState->channelQualitiesOfAllUsers;

}

DSAResult
BestChannel::getSubChannelWithDSA(RequestForResource& request,
                                  SchedulerStatePtr schedulerState,
                                  SchedulingMapPtr schedulingMap)
{
    DSAResult dsaResult;
    UserID user = request.user;
    if (userInfoMap.find(user) == userInfoMap.end()) {
        userInfoMap.insert(UserInfoMap::value_type(user, UserInfo(schedulerState->currentState->strategyInput->fChannels)));
    }
    UserInfo& userInfo = userInfoMap.find(user)->second;

    int lastUsedSubChannel = userInfo.lastUsedSubChannel;
    int subChannel = lastUsedSubChannel;
    int maxSubChannel = schedulingMap->subChannels.size();
    int maxTimeSlots = schedulerState->currentState->strategyInput->getNumberOfTimeSlots();
    int lastUsedTimeSlot = userInfo.lastUsedTimeSlot;
    int timeSlot = lastUsedTimeSlot;

    assure(maxSubChannel==schedulerState->currentState->strategyInput->fChannels,"maxSubChannel="<<maxSubChannel<<"!=fChannels");
    ChannelQualitiesOnAllSubBandsPtr channelQualitiesOnAllSubBands = userInfo.channelQualitiesOnAllSubBands;
    if (channelQualitiesOnAllSubBands==ChannelQualitiesOnAllSubBandsPtr()) { // empty
        assure(channelQualitiesOfAllUsers->knowsUser(user),"channelQualitiesOfAllUsers["<<user.getName()<<"] invalid");
        // ^ or should we ask the registryProxy for CQI here?
        channelQualitiesOnAllSubBands = channelQualitiesOfAllUsers->find(user)->second;
        userInfo.channelQualitiesOnAllSubBands = channelQualitiesOnAllSubBands;
    }
    MESSAGE_SINGLE(NORMAL, logger, "getSubChannelWithDSA("<<request.toString()<<"): lastSC="<<lastUsedSubChannel);
    // does this change the schedulerState->currentState->channelQualitiesOfAllUsers ?
    // sort takes O(N*log(N)) and stable_sort takes O(N*log(N)^2).
    // so why? If linear search is faster: O(N)
    //std::stable_sort(schedulingPar.channelQualities.begin(), schedulingPar.channelQualities.end(),
    //wns::scheduler::strategy::betterChannelQuality());

    int spatialLayer=0;
    bool found  = false;
    bool giveUp = false;
    if (!schedulerState->isTx
        && adjacentSubchannelsOnUplink
        && (lastUsedSubChannel!=DSAsubChannelNotFound) // already used subChannel
        )
    { // UL SC-FDMA
        // adjacentSubchannelsOnUplink
        while(!found && !giveUp)
        {
            // try same old subChannel again:
            subChannel = lastUsedSubChannel;
            if (!userInfo.usedSubChannels[subChannel]
                && channelIsUsable(subChannel, timeSlot, request, schedulerState, schedulingMap))
            { // PDU fits in
                found=true; break;
            } else { // mark unusable
                userInfo.usedSubChannels[subChannel] = true;
            }
            // TODO: consider timeSlots !!!

            // try +/-1 +/-2 and so on
            for (int tryThisSubChannelOffset=0; tryThisSubChannelOffset<maxSubChannel/2; tryThisSubChannelOffset++)
            {
                subChannel = (lastUsedSubChannel + tryThisSubChannelOffset*userInfo.toggleOffset);
                if ((subChannel>=0) && !userInfo.usedSubChannels[subChannel]
                    && (subChannel<maxSubChannel)
                    && channelIsUsable(subChannel, timeSlot, request, schedulerState, schedulingMap))
                { // PDU fits in
                    found=true; break;
                } else { // mark unusable
                    userInfo.usedSubChannels[subChannel] = true;
                }
                subChannel = (lastUsedSubChannel - tryThisSubChannelOffset*userInfo.toggleOffset);
                if ((subChannel>=0) && !userInfo.usedSubChannels[subChannel]
                    && (subChannel<maxSubChannel)
                    && channelIsUsable(subChannel, timeSlot, request, schedulerState, schedulingMap))
                { // PDU fits in
                    found=true; break;
                } else { // mark unusable
                    userInfo.usedSubChannels[subChannel] = true;
                }
            } // for offset +/-
            // the resulting DSA may not be contiguous in the case of another small-band user nearby
        } // while
        userInfo.toggleOffset *= -1;
    } else { // free search
        BetterChannelQuality comparator; // from SchedulerTypes.hpp
        // O(N^2) operations:
        while(!found && !giveUp)
        {
            subChannel = DSAsubChannelNotFound;
            ChannelQualityOnOneSubChannel bestChannelQuality;
            // find best of the remaining subChannels:
            for (int tryThisSubChannel=0; tryThisSubChannel<maxSubChannel; tryThisSubChannel++)
            {
                for (int tryThisTimeSlot=0; tryThisTimeSlot<maxTimeSlots; tryThisTimeSlot++)
                {
                    // TODO: userInfo.usedSubChannels[tryThisSubChannel][tryThisTimeSlot]
                    if (!userInfo.usedSubChannels[tryThisSubChannel]) { // could be free (at least not checked before)
                        ChannelQualityOnOneSubChannel channelQuality
                            = (*channelQualitiesOnAllSubBands)[tryThisSubChannel];
                        if (comparator(channelQuality, bestChannelQuality)) {
                            bestChannelQuality = channelQuality;
                            subChannel = tryThisSubChannel;
                            timeSlot = tryThisTimeSlot;
                        }
                    } // if unused
                } // forall tryThisTimeSlot
            } // forall tryThisSubChannel
            if (subChannel==DSAsubChannelNotFound)
            { // one complete round already done
                giveUp=true; break;
            }
            // best subChannel found; now check if usable:
            if (channelIsUsable(subChannel, timeSlot, request, schedulerState, schedulingMap))
            { // PDU fits in
                found=true; break;
            } else { // mark unusable
                userInfo.usedSubChannels[subChannel] = true;
            }
        } // while
    } // if free search or linear (SC-FDMA)

    if (giveUp) {
        MESSAGE_SINGLE(NORMAL, logger, "getSubChannelWithDSA(): no free subchannel");
        return dsaResult; // empty with subChannel=DSAsubChannelNotFound
    } else {
        MESSAGE_SINGLE(NORMAL, logger, "getSubChannelWithDSA(): subChannel="<<subChannel);
        spatialLayer = getSpatialLayerForSubChannel(subChannel, timeSlot, request, schedulerState, schedulingMap);
        userInfo.lastUsedSubChannel = subChannel;
        dsaResult.subChannel = subChannel;
        dsaResult.spatialLayer = spatialLayer;
        return dsaResult;
    }
    return dsaResult; // empty with subChannel=DSAsubChannelNotFound
}

