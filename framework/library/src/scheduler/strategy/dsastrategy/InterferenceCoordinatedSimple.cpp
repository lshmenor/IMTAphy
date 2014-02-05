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

#include <WNS/scheduler/strategy/dsastrategy/InterferenceCoordinatedSimple.hpp>
#include <WNS/scheduler/strategy/dsastrategy/DSAStrategyInterface.hpp>
#include <WNS/scheduler/SchedulerTypes.hpp>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace wns::scheduler;
using namespace wns::scheduler::strategy;
using namespace wns::scheduler::strategy::dsastrategy;

STATIC_FACTORY_REGISTER_WITH_CREATOR(InterferenceCoordinatedSimple,
				     DSAStrategyInterface,
				     "InterferenceCoordinatedSimple",
				     wns::PyConfigViewCreator);

InterferenceCoordinatedSimple::InterferenceCoordinatedSimple(const wns::pyconfig::View& config)
  : DSAStrategy(config)
{
  //useRandomChannelAtBeginning = config.get<bool>("useRandomChannelAtBeginning");
}

InterferenceCoordinatedSimple::~InterferenceCoordinatedSimple()
{
}

// call this before each timeSlot/frame
void
InterferenceCoordinatedSimple::initialize(SchedulerStatePtr schedulerState,
			SchedulingMapPtr schedulingMap)
{
	DSAStrategy::initialize(schedulerState,schedulingMap); // must always initialize base class too
	// just a SmartPtr:
	//ChannelQualitiesOfAllUsersPtr
	assure(schedulerState->currentState!=RevolvingStatePtr(),"currentState must be valid");
	// set member:
	channelQualitiesOfAllUsers =
	  schedulerState->currentState->channelQualitiesOfAllUsers;
	// useful for interference/neighbor cell coordinated strategies:
	// typedef std::list<wns::node::Interface*> NodeList;
	// typedef wns::node::Interface* wns::scheduler::UserID
	// TODO:
	// 1) put this into RegistryProxyInterface
	// 2) make filter in RegistryProxy for NodeType BS
	// 3) find a way to get direct neighbors only (adjacent, geometrically)
	// 4) Find a way to get neighbor scheduler/strategy/state
	// 5) Let neighbors communicate about their decisions (using a Singleton?)
}

DSAResult
InterferenceCoordinatedSimple::getSubChannelWithDSA(RequestForResource& request,
				  SchedulerStatePtr schedulerState,
				  SchedulingMapPtr schedulingMap)
{
	DSAResult dsaResult;
	/* TODO */
	assure(false,"InterferenceCoordinatedSimple: not yet implemented");
	return dsaResult; // empty with subChannel=DSAsubChannelNotFound
}

