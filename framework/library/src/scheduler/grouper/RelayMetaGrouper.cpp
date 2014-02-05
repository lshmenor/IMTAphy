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

#include "RelayMetaGrouper.hpp"
#include <WNS/scheduler/SchedulerTypes.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/StaticFactory.hpp>

using namespace wns::scheduler;
using namespace wns::scheduler::grouper;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	RelayMetaGrouper,
	GroupingProviderInterface,
	"RelayMetaGrouper",
	wns::PyConfigViewCreator);


RelayMetaGrouper::RelayMetaGrouper(const wns::pyconfig::View& config) :
	SpatialGrouper(config),
	internalStrategy(NULL)
{
	// Create internal strategy from Static Factory
	std::string grouperName = config.get<std::string>("internalStrategy.nameInGrouperFactory");
	SpatialGrouperCreator* grouperCreator = wns::scheduler::grouper::SpatialGrouperFactory::creator(grouperName);
	internalStrategy = grouperCreator->create(config.get<wns::pyconfig::View>("internalStrategy"));
	assure(internalStrategy, "Internal Strategy creation failed");
}

RelayMetaGrouper::~RelayMetaGrouper()
{
	delete internalStrategy;
}

Grouping
RelayMetaGrouper::getTxGrouping(const UserSet activeUsers, int maxBeams)
{
	// sort users into Relays and Terminals
	UserSet relayStations;
	UserSet userTerminals;
	for ( UserSet::const_iterator iter = activeUsers.begin();
		  iter != activeUsers.end(); ++iter)
	{
	  if ( colleagues.registry->getStationType(*iter) == wns::service::dll::StationTypes::FRS() )
			relayStations.insert(*iter);
		else
			userTerminals.insert(*iter);
	}

	// Group them separately
	Grouping relayGroups = internalStrategy->getTxGrouping(relayStations, maxBeams);
	Grouping userGroups  = internalStrategy->getTxGrouping(userTerminals, maxBeams);

	// Join the two Groupings
	Grouping result = joinGroupings(relayGroups, userGroups);
	return result;
}

Grouping
RelayMetaGrouper::getRxGrouping(const UserSet activeUsers, int maxBeams)
{
	// sort users into Relays and Terminals
	UserSet relayStations;
	UserSet userTerminals;
	for ( UserSet::const_iterator iter = activeUsers.begin();
		  iter != activeUsers.end(); ++iter)
	{
	  if ( colleagues.registry->getStationType(*iter) == wns::service::dll::StationTypes::FRS() )
			relayStations.insert(*iter);
		else
			userTerminals.insert(*iter);
	}

	// Group them separately
	Grouping relayGroups = internalStrategy->getRxGrouping(relayStations, maxBeams);
	Grouping userGroups  = internalStrategy->getRxGrouping(userTerminals, maxBeams);

	// Join the two Groupings
	Grouping result = joinGroupings(relayGroups, userGroups);
	return result;
}

void
RelayMetaGrouper::setColleagues(RegistryProxyInterface* _registry)
{
	colleagues.registry = _registry;
	internalStrategy->setColleagues(_registry);
}

void
RelayMetaGrouper::setFriends(wns::service::phy::ofdma::BFInterface* _ofdmaProvider)
{
	internalStrategy->setFriends(_ofdmaProvider);
}

Grouping
RelayMetaGrouper::joinGroupings(const Grouping& a, const Grouping& b) const
{
	Grouping result;
	result.patterns = a.patterns;
	result.patterns.insert(b.patterns.begin(), b.patterns.end());

	result.groups = a.groups;
	result.groups.insert(result.groups.end(), b.groups.begin(), b.groups.end());

	std::size_t numGroupsInA = a.groups.size();

	result.userGroupNumber = a.userGroupNumber;

	for (std::map<UserID,int>::const_iterator iter = b.userGroupNumber.begin();
		iter != b.userGroupNumber.end();
		++iter)
	{
		// add offset on all groupNumbers from b
		result.userGroupNumber[iter->first] = iter->second + numGroupsInA;
	}

	return result;
}
