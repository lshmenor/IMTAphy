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

#include <WNS/scheduler/strategy/dsastrategy/BestEffSINR.hpp>
#include <WNS/scheduler/strategy/dsastrategy/DSAStrategy.hpp>

using namespace wns::scheduler;
using namespace wns::scheduler::strategy;
using namespace wns::scheduler::strategy::dsastrategy;

STATIC_FACTORY_REGISTER_WITH_CREATOR(BestEffSINR,
                                     DSAStrategyInterface,
                                     "BestEffSINR",
                                     wns::PyConfigViewCreator);

BestEffSINR::BestEffSINR(const wns::pyconfig::View& config)
    : DSAStrategy(config)
{
}

BestEffSINR::~BestEffSINR()
{
}

// call this before each timeSlot/frame
void
BestEffSINR::initialize(SchedulerStatePtr schedulerState,
                         SchedulingMapPtr schedulingMap)
{
    DSAStrategy::initialize(schedulerState,schedulingMap); 
}

DSAResult
BestEffSINR::getSubChannelWithDSA(RequestForResource& request,
                                   SchedulerStatePtr schedulerState,
                                   SchedulingMapPtr schedulingMap)
{
    DSAResult dsaResult;

    int subChannel = 0;
    int timeSlot = 0;
    int spatialLayer = 0;
    int maxSubChannel = schedulerState->currentState->strategyInput->getFChannels();
    int numberOfTimeSlots = schedulerState->currentState->strategyInput->getNumberOfTimeSlots();
    int maxSpatialLayers = schedulerState->currentState->strategyInput->getMaxSpatialLayers();

    std::set<DSAResult> usable;

    bool found  = false;
    bool giveUp = false;
    while(!giveUp) {
        if (channelIsUsable(subChannel, timeSlot, spatialLayer, request, schedulerState, schedulingMap))
        { 
            dsaResult.subChannel = subChannel;
            dsaResult.timeSlot = timeSlot;
            dsaResult.spatialLayer = spatialLayer;
            usable.insert(dsaResult);
        }
        if(++spatialLayer >= maxSpatialLayers)
        { // all spatialLayers full; take next timeSlot
            spatialLayer = 0;
            if (++timeSlot >= numberOfTimeSlots)
            { // all timeSlots full; take next subChannel
                timeSlot = 0;
                if (++subChannel >= maxSubChannel)
                { // wraparound
                    subChannel = 0;
                }
            }
        }
        if (subChannel == 0 && timeSlot == 0)
        { // one complete round already done
            giveUp = true; 
            break;
        }
    } // while
    if (usable.size() == 0) 
    {
        MESSAGE_SINGLE(NORMAL, logger, "getSubChannelWithDSA(): no free subchannel");
        return dsaResult; 
    } 
    else 
    {
        MESSAGE_SINGLE(NORMAL, logger, "getSubChannelWithDSA(): (subChannel.timeSlot.spatialLayer) = ("
            << timeSlot<< "." << subChannel << "." << spatialLayer<<")");

        assure(usable.size() > 0, "No usable SCs.");

        dsaResult = DSAResult();

        wns::simulator::Time now = wns::simulator::getEventScheduler()->getTime();
        int slot = now / schedulingMap->getSlotLength() + 1;

        /* TODO: This is hardcoded. Should match numFrames in scheduler. */
        slot %= 20;

        wns::Ratio best =  wns::Ratio::from_factor(1E-20);
        std::set<DSAResult>::iterator it;
        for(it = usable.begin(); it != usable.end(); it++)
        {
            wns::Ratio effSINR;
            std::set<unsigned int> sc;
            sc.insert(it->subChannel);
            if(colleagues.registry->getDL())
                effSINR = colleagues.registry->getEffectiveDownlinkSINR(
                    request.user, sc, slot, wns::Power::from_mW(1), true);
            else
                effSINR = colleagues.registry->getEffectiveUplinkSINR(
                    request.user, sc, slot, wns::Power::from_mW(1));
    
            if(effSINR > best)
            {
                dsaResult = *it;
                best = effSINR;
            }
        }

        assure(dsaResult.subChannel != DSAsubChannelNotFound, "Could not find best SC.");
        return dsaResult;
    }
} // getSubChannelWithDSA
