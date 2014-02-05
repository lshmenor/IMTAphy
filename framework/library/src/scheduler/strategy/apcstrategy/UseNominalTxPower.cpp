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

#include <WNS/scheduler/strategy/apcstrategy/UseNominalTxPower.hpp>
#include <WNS/scheduler/strategy/apcstrategy/APCStrategy.hpp>
#include <WNS/scheduler/strategy/StrategyInterface.hpp>
#include <WNS/scheduler/SchedulerTypes.hpp>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace wns::scheduler;
using namespace wns::scheduler::strategy;
using namespace wns::scheduler::strategy::apcstrategy;

STATIC_FACTORY_REGISTER_WITH_CREATOR(UseNominalTxPower,
				     APCStrategyInterface,
				     "UseNominalTxPower",
				     wns::PyConfigViewCreator);

UseNominalTxPower::UseNominalTxPower(const wns::pyconfig::View& config)
	: APCStrategy(config)
{
}

UseNominalTxPower::~UseNominalTxPower()
{
}

// called before each timeSlot/frame
void
UseNominalTxPower::initialize(SchedulerStatePtr schedulerState,
			      SchedulingMapPtr schedulingMap)
{
	APCStrategy::initialize(schedulerState,schedulingMap); // must always initialize base class too
	MESSAGE_SINGLE(NORMAL, logger, "UseNominalTxPower::initialize("<<apcstrategyName<<")");
} // initialize

APCResult
UseNominalTxPower::doStartAPC(RequestForResource& request,
			      SchedulerStatePtr schedulerState,
			      SchedulingMapPtr schedulingMap)
{
	// no power control, just nominal values
	APCResult apcResult;

	if (schedulerState->defaultTxPower!=wns::Power())
	{ // predefined, e.g. in slave mode
		apcResult.txPower = schedulerState->defaultTxPower;
	} else {
		wns::scheduler::PowerCapabilities powerCapabilities =
                    schedulerState->strategy->getPowerCapabilities(request.user);
		apcResult.txPower = powerCapabilities.nominalPerSubband;
	}
	wns::Ratio pathloss     = request.cqiOnSubChannel.pathloss;
	wns::Power interference = request.cqiOnSubChannel.interference;
	apcResult.sinr = apcResult.txPower/(interference*pathloss);
	apcResult.estimatedCandI = 
        ChannelQualityOnOneSubChannel(pathloss, interference, apcResult.txPower/pathloss);
	MESSAGE_SINGLE(NORMAL, logger,"doStartAPC("<<request.toString()<<"): "
		       <<"TxP="<<apcResult.txPower<<", pl="<<pathloss<<", estd: I="<<interference<<", C="<<apcResult.estimatedCandI.carrier);
	if (schedulerState->defaultPhyModePtr != wns::service::phy::phymode::PhyModeInterfacePtr())
	{ // predefined, e.g. in slave mode
		apcResult.phyModePtr = schedulerState->defaultPhyModePtr;
	}
	else
	{
		apcResult.phyModePtr = phyModeMapper->getBestPhyMode(apcResult.sinr);
	}
	MESSAGE_SINGLE(NORMAL, logger,"doStartAPC("<<request.toString()<<"): "
		       <<"SINR="<<apcResult.sinr<<", PhyMode="<<*(apcResult.phyModePtr));
	request.phyModePtr = apcResult.phyModePtr; // maybe needed later
	return apcResult;
}
