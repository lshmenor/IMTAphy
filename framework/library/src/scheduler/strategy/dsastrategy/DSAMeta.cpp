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

#include <WNS/scheduler/strategy/dsastrategy/DSAMeta.hpp>
#include <WNS/scheduler/strategy/dsastrategy/DSAStrategy.hpp>
#include <WNS/scheduler/strategy/dsastrategy/DSAStrategyInterface.hpp>
#include <WNS/scheduler/SchedulerTypes.hpp>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace wns::scheduler;
using namespace wns::scheduler::strategy;
using namespace wns::scheduler::strategy::dsastrategy;

STATIC_FACTORY_REGISTER_WITH_CREATOR(DSAMeta,
                                     DSAStrategyInterface,
                                     "DSAMeta",
                                     wns::PyConfigViewCreator);

DSAMeta::DSAMeta(const wns::pyconfig::View& config)
    : DSAStrategy(config)
{
}

DSAMeta::~DSAMeta()
{
}

// call this before each timeSlot/frame
void
DSAMeta::initialize(SchedulerStatePtr schedulerState,
                     SchedulingMapPtr schedulingMap)
{    
    DSAStrategy::initialize(schedulerState,schedulingMap); // must always initialize base class too
}

DSAResult
DSAMeta::getSubChannelWithDSA(RequestForResource& request,
                               SchedulerStatePtr schedulerState,
                               SchedulingMapPtr schedulingMap)
{
    DSAResult dsaResult;
    //simTimeType requestedCompoundDuration = getCompoundDuration(request);
    //MESSAGE_SINGLE(NORMAL, logger, "getSubChannelWithDSA("<<request.toString()<<"): d="<<requestedCompoundDuration<<"s");
    int subChannel = 0;
    int timeSlot = 0;
    int spatialLayer = 0;
    int maxSubChannel = schedulerState->currentState->strategyInput->getFChannels();
    int numberOfTimeSlots = schedulerState->currentState->strategyInput->getNumberOfTimeSlots();
    int maxSpatialLayers = schedulerState->currentState->strategyInput->getMaxSpatialLayers();
    

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
        dsaResult.subChannel = 0;
        dsaResult.timeSlot = 0;
        dsaResult.spatialLayer = 0;
	
        return dsaResult;
    }
    
    for (spatialLayer = 0; spatialLayer < maxSpatialLayers; spatialLayer++)
    {
      for (timeSlot = 0; timeSlot < numberOfTimeSlots; timeSlot++)
      {
	for (subChannel = 0; subChannel < maxSubChannel; subChannel++)
	{
	  if (channelIsUsable(subChannel, timeSlot, spatialLayer, request, schedulerState, schedulingMap))
	  {	    
	    MESSAGE_SINGLE(NORMAL, logger, "getSubChannelWithDSA(): subChannel="<<subChannel<<"."<<timeSlot<<"."<<spatialLayer);
	    dsaResult.subChannel = subChannel;
	    dsaResult.timeSlot = timeSlot;
	    dsaResult.spatialLayer = spatialLayer;
	    return dsaResult;
	  }else
	  {
	  //std::cout<<request.user<<":"<<" No subChannel"<<std::endl;
	  
	  
	  
	  
	  }
	}
      }
    }
    
    MESSAGE_SINGLE(NORMAL, logger, "getSubChannelWithDSA(): no free subchannel");
    return dsaResult; 
}
