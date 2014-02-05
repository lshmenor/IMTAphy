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

#include <WNS/scheduler/strategy/apcstrategy/APCSlave.hpp>
#include <WNS/scheduler/strategy/apcstrategy/APCStrategy.hpp>
#include <WNS/scheduler/strategy/StrategyInterface.hpp>
#include <WNS/scheduler/SchedulerTypes.hpp>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace wns::scheduler;
using namespace wns::scheduler::strategy;
using namespace wns::scheduler::strategy::apcstrategy;

STATIC_FACTORY_REGISTER_WITH_CREATOR(APCSlave,
				     APCStrategyInterface,
				     "APCSlave",
				     wns::PyConfigViewCreator);

APCSlave::APCSlave(const wns::pyconfig::View& config)
	: APCStrategy(config)
{
}

APCSlave::~APCSlave()
{
}

// called before each timeSlot/frame
void
APCSlave::initialize(SchedulerStatePtr schedulerState,
			      SchedulingMapPtr schedulingMap)
{
	APCStrategy::initialize(schedulerState,schedulingMap); // must always initialize base class too
	MESSAGE_SINGLE(NORMAL, logger, "APCSlave::initialize("<<apcstrategyName<<")");
} // initialize

APCResult
APCSlave::doStartAPC(RequestForResource& request,
                     SchedulerStatePtr schedulerState,
                     SchedulingMapPtr schedulingMap)
{
	// no power control, just nominal values
	APCResult apcResult;
	assure(request.subChannel>=0,"need a valid subChannel");
	assure(request.timeSlot  >=0,"need a valid timeSlot");
	assure(request.spatialLayer      >=0,"need a valid spatialLayer");
        if (request.phyModePtr == wns::service::phy::phymode::PhyModeInterfacePtr()) 
        { // not given? Hmm.
            request.phyModePtr = schedulingMap->getPhyModeUsedInResource(request.subChannel,request.timeSlot,request.spatialLayer);
            assure(request.phyModePtr != wns::service::phy::phymode::PhyModeInterfacePtr(),
                   "APCSlave requires given phyMode");
        }
        wns::Power txPower = schedulingMap->getTxPowerUsedInResource(request.subChannel,request.timeSlot,request.spatialLayer);
        assure(schedulerState->defaultTxPower!=wns::Power(),
             "APCSlave requires defaultTxPower");
        assure(txPower!=wns::Power(),
             "APCSlave requires given TxPower");
        apcResult.txPower    = txPower; // must be given
        apcResult.phyModePtr = request.phyModePtr; // must be given
	//wns::Ratio pathloss     = request.cqiOnSubChannel.pathloss;
	//wns::Power interference = request.cqiOnSubChannel.interference;
	//apcResult.sinr = apcResult.txPower/(interference*pathloss);
	//apcResult.estimatedCandI = wns::CandI(apcResult.txPower/pathloss,interference);
	MESSAGE_SINGLE(NORMAL, logger,"doStartAPC("<<request.toString()<<"): "
		       <<"TxP="<<apcResult.txPower);
	//MESSAGE_SINGLE(NORMAL, logger,"doStartAPC("<<request.toString()<<"): "
	//	       <<"TxP="<<apcResult.txPower<<", pl="<<pathloss<<", estd: I="<<interference<<", C="<<apcResult.estimatedCandI.C);
	//MESSAGE_SINGLE(NORMAL, logger,"doStartAPC("<<request.toString()<<"): "
	//	       <<"SINR="<<apcResult.sinr<<", PhyMode="<<*(apcResult.phyModePtr));
	return apcResult;
}
