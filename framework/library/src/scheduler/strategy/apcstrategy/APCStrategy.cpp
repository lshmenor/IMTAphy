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

#include <WNS/scheduler/strategy/apcstrategy/APCStrategy.hpp>
#include <WNS/scheduler/strategy/StrategyInterface.hpp>
#include <vector>
#include <string>

using namespace wns::scheduler;
using namespace wns::scheduler::strategy;
using namespace wns::scheduler::strategy::apcstrategy;


APCStrategy::APCStrategy(const wns::pyconfig::View& config)
	: logger(config.get("logger")),
	  phyModeMapper(NULL),
	  //maxSummedPowerOnAllChannels(),
	  //maxPowerPerSubChannel(),
	  //nominalPowerPerSubChannel(),
	  apcstrategyName(config.get<std::string>("nameInAPCStrategyFactory"))
{
	MESSAGE_SINGLE(NORMAL, logger,"APCStrategy="<<apcstrategyName);
}

APCStrategy::~APCStrategy()
{
}

void
APCStrategy::setColleagues(RegistryProxyInterface* _registry)
{
	colleagues.registry = _registry;
	assure(colleagues.registry!=NULL,"need colleagues.registry");
	phyModeMapper = _registry->getPhyModeMapper();
}

// called before each timeSlot/frame
void
APCStrategy::initialize(SchedulerStatePtr schedulerState,
			SchedulingMapPtr schedulingMap)
{
	assure(colleagues.registry!=NULL,"need colleagues.registry");
	assure(phyModeMapper!=NULL,"need phyModeMapper");
	assure(schedulerState->strategy!=NULL,"need strategy");
	MESSAGE_SINGLE(NORMAL, logger, "APCStrategy::initialize("<<apcstrategyName<<")");
} // initialize

void
APCStrategy::postProcess(SchedulerStatePtr schedulerState,
			 SchedulingMapPtr schedulingMap)
{
} // postProcess


/*
  Local Variables:
  mode: c++
  fill-column: 80
  c-basic-offset: 8
  c-comment-only-line-offset: 0
  c-tab-always-indent: t
  indent-tabs-mode: t
  tab-width: 8
  End:
*/
