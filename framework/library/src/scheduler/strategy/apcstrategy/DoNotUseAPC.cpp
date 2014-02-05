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

#include <WNS/scheduler/strategy/apcstrategy/DoNotUseAPC.hpp>
#include <WNS/scheduler/strategy/apcstrategy/APCStrategy.hpp>
#include <WNS/scheduler/strategy/StrategyInterface.hpp>
#include <WNS/scheduler/SchedulerTypes.hpp>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace wns::scheduler;
using namespace wns::scheduler::strategy;
using namespace wns::scheduler::strategy::apcstrategy;

STATIC_FACTORY_REGISTER_WITH_CREATOR(DoNotUseAPC,
				     APCStrategyInterface,
				     "DoNotUseAPC",
				     wns::PyConfigViewCreator);

DoNotUseAPC::DoNotUseAPC(const wns::pyconfig::View& config)
	: APCStrategy(config)
{
}

DoNotUseAPC::~DoNotUseAPC()
{
}

// called before each timeSlot/frame
void
DoNotUseAPC::initialize(SchedulerStatePtr schedulerState,
			SchedulingMapPtr schedulingMap)
{
	APCStrategy::initialize(schedulerState,schedulingMap); // must always initialize base class too
	MESSAGE_SINGLE(NORMAL, logger, "DoNotUseAPC::initialize("<<apcstrategyName<<")");
} // initialize

APCResult
DoNotUseAPC::doStartAPC(RequestForResource& request,
			SchedulerStatePtr schedulerState,
			SchedulingMapPtr schedulingMap)
{
	throw wns::Exception("DoNotUseAPC::doStartAPC must never be called");
	return APCResult();
}
