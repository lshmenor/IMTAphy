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

#include <WNS/scheduler/grouper/GreedyGrouper.hpp>
#include <algorithm>

using namespace wns::scheduler;
using namespace wns::scheduler::grouper;

STATIC_FACTORY_REGISTER_WITH_CREATOR(GreedyGrouper, GroupingProviderInterface, "GreedyGrouper", wns::PyConfigViewCreator);

GreedyGrouper::GreedyGrouper(const wns::pyconfig::View& config)
	: AllPossibleGroupsGrouper(config)
{
}


bool
GreedyGrouper::BeamCmp::operator() (const Beams& b1, const Beams& b2) const {
	return b1.throughPut > b2.throughPut;
}


AllPossibleGroupsGrouper::Partition
GreedyGrouper::makeGrouping(int /* maxBeams */, unsigned int noOfStations )
{
	// calculate this before sorting the array, as it depends on the original
	// order
	float throughputTrivialGrouping = getTPperGroupTrivialGrouping(noOfStations);

	// sort the beams
	// choose the best beam
	// choose the next beam that doesn't conflict
	// sort beam beginning with highest throughput
	sort(allPossibleGroups.begin(), allPossibleGroups.end(), BeamCmp());

	// init greedyGrouping
	Partition groupingGreedy;
	groupingGreedy.servedStations = std::bitset<MAX_STATIONS>(0);
	groupingGreedy.groups.clear();
	groupingGreedy.totalThroughput = 0.0;

    // try to consecutively add next biggest beam

	for (unsigned int i = 0; i < allPossibleGroups.size(); ++i) {
		// do a bitwise AND of the two bitsets to check whether the set of
		// already covered stations in groupingGreedy and the stations in the
		// group to be considered (allPossibleGroups[i]) conflict
		if ((allPossibleGroups[i].servedStations & groupingGreedy.servedStations).count() == 0)
		{ // can add this group
			groupingGreedy.servedStations = (allPossibleGroups[i].servedStations | groupingGreedy.servedStations);

            // add group to new grouping
			groupingGreedy.groups.push_back(i);
			groupingGreedy.totalThroughput += allPossibleGroups[i].throughPut;
		}
		if (groupingGreedy.servedStations.count() == noOfStations)
			break; // all covered, we are done
	}

	assure(groupingGreedy.servedStations.count() == noOfStations, "Greedy did not find grouping covering everything -> impossible");

	float throughputBestGrouping = (float) (groupingGreedy.totalThroughput) / (float)(groupingGreedy.groups.size());

	groupingGainProbeBus->put(throughputBestGrouping / throughputTrivialGrouping);

	MESSAGE_BEGIN(VERBOSE, logger, m, colleagues.registry->getNameForUser(colleagues.registry->getMyUserID()));
	m << " GreedyGrouper calculated new grouping with a gain of "
	  << throughputBestGrouping / throughputTrivialGrouping;
	MESSAGE_END();

	return groupingGreedy;
}




