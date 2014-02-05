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

#include <WNS/scheduler/strategy/dsastrategy/DSAStrategy.hpp>
#include <WNS/scheduler/strategy/dsastrategy/DoNotUseDSA.hpp>

using namespace wns::scheduler;
using namespace wns::scheduler::strategy;
using namespace wns::scheduler::strategy::dsastrategy;

STATIC_FACTORY_REGISTER_WITH_CREATOR(DoNotUseDSA,
				     DSAStrategyInterface,
				     "DoNotUseDSA",
				     wns::PyConfigViewCreator);

DoNotUseDSA::DoNotUseDSA(const wns::pyconfig::View& config)
  : DSAStrategy(config)
{
}

DSAResult
DoNotUseDSA::getSubChannelWithDSA(RequestForResource& /*request*/,
				  SchedulerStatePtr /*schedulerState*/,
				  SchedulingMapPtr /*schedulingMap*/)
{
	throw wns::Exception("DoNotUseDSA::getSubChannelWithDSA must never be called");
	return DSAResult();
}
