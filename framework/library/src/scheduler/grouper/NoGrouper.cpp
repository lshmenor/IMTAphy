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

#include <WNS/scheduler/grouper/NoGrouper.hpp>
#include <WNS/scheduler/SchedulerTypes.hpp>

using namespace wns::scheduler;
using namespace wns::scheduler::grouper;

STATIC_FACTORY_REGISTER_WITH_CREATOR(NoGrouper, GroupingProviderInterface, "NoGrouper", wns::PyConfigViewCreator);

NoGrouper::NoGrouper(const wns::pyconfig::View&) :
	registry(NULL)
{}

NoGrouper::~NoGrouper()
{}

Grouping
NoGrouper::getTxGrouping(const UserSet activeUsers, int)
{
	Grouping grouping;

	int groupNumber = 0;
	for (UserSet::iterator iter = activeUsers.begin();
		 iter != activeUsers.end();
		 ++iter) {
		
		assure(iter->isValid(), "No valid user");
        wns::scheduler::ChannelQualityOnOneSubChannel cqi = 
            registry->estimateTxSINRAt(*iter);
		wns::CandI sinrEstimation(cqi.carrier, cqi.interference); 
		Group group;
		group[*iter] = sinrEstimation;

		grouping.patterns[*iter] = wns::service::phy::ofdma::PatternPtr();
		grouping.groups.push_back(group);
		grouping.userGroupNumber[*iter] = groupNumber++;
	}
	return grouping;
}

Grouping
NoGrouper::getRxGrouping(const UserSet activeUsers, int)
{
	Grouping grouping;

	int groupNumber = 0;
	for (UserSet::iterator iter = activeUsers.begin();
		 iter != activeUsers.end();
		 ++iter) {

		assure(iter->isValid(), "No valid user");
        wns::scheduler::ChannelQualityOnOneSubChannel cqi = 
            registry->estimateRxSINROf(*iter);
		wns::CandI sinrEstimation(cqi.carrier, cqi.interference); 
		Group group;
		group[*iter] = sinrEstimation;

		grouping.patterns[*iter] = wns::service::phy::ofdma::PatternPtr();
		grouping.groups.push_back(group);
		grouping.userGroupNumber[*iter] = groupNumber++;
	}
	return grouping;
}

void
NoGrouper::setColleagues(RegistryProxyInterface* _registry)
{
	registry = _registry;
}


