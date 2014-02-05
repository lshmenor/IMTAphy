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

#include <WNS/scheduler/strategy/dsastrategy/Random.hpp>
#include <WNS/scheduler/strategy/dsastrategy/DSAStrategy.hpp>
#include <WNS/scheduler/strategy/dsastrategy/DSAStrategyInterface.hpp>
#include <WNS/scheduler/SchedulerTypes.hpp>
#include <vector>
#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <time.h>

using namespace wns::scheduler;
using namespace wns::scheduler::strategy;
using namespace wns::scheduler::strategy::dsastrategy;

STATIC_FACTORY_REGISTER_WITH_CREATOR(Random,
                                     DSAStrategyInterface,
                                     "Random",
                                     wns::PyConfigViewCreator);

Random::Random(const wns::pyconfig::View& config)
    : DSAStrategy(config),
      randomDist(NULL)
{
    randomDist = new wns::distribution::StandardUniform();
}

Random::~Random()
{
    delete randomDist;
}

// call this before each timeSlot/frame
void
Random::initialize(SchedulerStatePtr schedulerState,
                         SchedulingMapPtr schedulingMap)
{
    DSAStrategy::initialize(schedulerState,schedulingMap); // must always initialize base class too
}

DSAResult
Random::getSubChannelWithDSA(RequestForResource& request,
                                   SchedulerStatePtr schedulerState,
                                   SchedulingMapPtr schedulingMap)
{
    DSAResult dsaResult;
    
    int maxSubChannel = schedulerState->currentState->strategyInput->getFChannels();
    int numberOfTimeSlots = schedulerState->currentState->strategyInput->getNumberOfTimeSlots();
    int maxSpatialLayers = schedulerState->currentState->strategyInput->getMaxSpatialLayers();

    MESSAGE_SINGLE(NORMAL, logger, "getSubChannelWithDSA(" << request.toString()<<")");

    int numberOfEssays = 0;

    DSAResult resource;
    std::vector<DSAResult> freeResources;
    for(int i = 0; i < maxSubChannel; i++)
    {
        for(int j = 0; j < numberOfTimeSlots; j++)
        {
            for(int k = 0; k < maxSpatialLayers; k++)
            {
                if (channelIsUsable(i, j, k, request, schedulerState, schedulingMap))
                {
                    resource.subChannel = i;
                    resource.timeSlot = j;
                    resource.spatialLayer = k;
                    freeResources.push_back(resource);
                }
            }
        }
    }
    MESSAGE_SINGLE(NORMAL, logger, "getSubChannelWithDSA(" << request.toString() << ") found " 
        << freeResources.size() << " potential resources");
                
    if (freeResources.size() == 0) 
    {
        MESSAGE_SINGLE(NORMAL, logger, "getSubChannelWithDSA(): no free subchannel");
        return dsaResult;
    }
    else                 
    {
        int rnd = int((*randomDist)() * freeResources.size()); 
        resource = freeResources[rnd];

        MESSAGE_SINGLE(NORMAL, logger, "getSubChannelWithDSA(): Drawn random resource: " << rnd
            << " subChannel=" << resource.subChannel << "." << resource.timeSlot << "." 
            << resource.spatialLayer);
        return resource;
    }
} // getSubChannelWithDSA
