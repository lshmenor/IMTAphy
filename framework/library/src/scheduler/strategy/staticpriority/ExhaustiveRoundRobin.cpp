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

#include <WNS/scheduler/strategy/staticpriority/ExhaustiveRoundRobin.hpp>
#include <WNS/scheduler/SchedulerTypes.hpp>

#include <vector>
#include <map>
#include <algorithm>

using namespace std;
using namespace wns::scheduler;
using namespace wns::scheduler::strategy;
using namespace wns::scheduler::strategy::staticpriority;

STATIC_FACTORY_REGISTER_WITH_CREATOR(ExhaustiveRoundRobin,
                                     SubStrategyInterface,
                                     "ExhaustiveRoundRobin",
                                     wns::PyConfigViewCreator);

ExhaustiveRoundRobin::ExhaustiveRoundRobin(const wns::pyconfig::View& config)
    : RoundRobin(config)
{
    blockSize=INT_MAX; // ExhaustiveRoundRobin=RoundRobin with infinite blockSize
    MESSAGE_SINGLE(NORMAL, logger, "ExhaustiveRoundRobin(): constructed");
}

ExhaustiveRoundRobin::~ExhaustiveRoundRobin()
{
}

void
ExhaustiveRoundRobin::initialize()
{
    MESSAGE_SINGLE(NORMAL, logger, "ExhaustiveRoundRobin(): initialized");
    RoundRobin::initialize();
}

MapInfoCollectionPtr
ExhaustiveRoundRobin::doStartSubScheduling(SchedulerStatePtr schedulerState,
                                           wns::scheduler::SchedulingMapPtr schedulingMap)
{
    return RoundRobin::doStartSubScheduling(schedulerState,schedulingMap);
} // doStartSubScheduling
