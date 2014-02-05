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

#include <WNS/scheduler/grouper/OptimalGrouper.hpp>

using namespace wns::scheduler;
using namespace wns::scheduler::grouper;

STATIC_FACTORY_REGISTER_WITH_CREATOR(OptimalGrouper, GroupingProviderInterface, "OptimalGrouper", wns::PyConfigViewCreator);


OptimalGrouper::OptimalGrouper(const wns::pyconfig::View& config)
	: AllPossibleGroupsGrouper(config)
{
}

AllPossibleGroupsGrouper::Partition
OptimalGrouper::makeGrouping(int _maxBeams, unsigned int _noOfStations)
{ // wrapper for recursive function
	maxBeams = _maxBeams;
	noOfStations = _noOfStations;

	// start with the emptyGrouping and the first group
	Partition emptyPartition;

	emptyPartition.servedStations = std::bitset<MAX_STATIONS>(0);
	emptyPartition.groups.clear();
	emptyPartition.totalThroughput = 0.0;

	throughputCurrentBestGrouping= 0.0;
	throughputTrivialGrouping = getTPperGroupTrivialGrouping(noOfStations);

	makeGroupingRecursively(emptyPartition, 0);

	groupingGainProbeBus->put(throughputCurrentBestGrouping / throughputTrivialGrouping);

	return currentBestGrouping;
}

void
OptimalGrouper::makeGroupingRecursively(Partition currentGroups, int firstGroup)
{
// This function is the heart of the optimal grouper because it performs the
// exhaustive search for the optimal grouping. It needs the following member
// variables to perform its search:
//     - allPossibleGroups which contains all possible station combinations and
//       has to be computed before
//	   - noOfStations an integer that contains the total numbers of users to be grouped
//
//  makeGrouping recursively enumerates all valid partitions of the set of
//  active users. A partition is valid if every user is covered exactly once

	if (currentGroups.servedStations.count() == noOfStations)
	{   // all stations served, partitioning done
		// now check if this is a new best solution

		float TPperGroups = (float) (currentGroups.totalThroughput) / (float)(currentGroups.groups.size());

		if (TPperGroups > throughputCurrentBestGrouping) {
			// FIXME(jke):
			// LOG_INFO("New best grouping found with gain of ", TPperGroups / throughputTrivialGrouping);
			currentBestGrouping = currentGroups;
			throughputCurrentBestGrouping = TPperGroups;
		}
		return;
	}
	else {
		// i runs from beam firstGroup..maxGroups
		// This is important for speedup: the order in which the groups
		// are found is not important. Like this, only the grouping in
		// canoncial ordering is found

		// j runs over number of stations in Group i

		for (unsigned int i = firstGroup; i < allPossibleGroups.size(); ++i) {
			// do a bitwise AND of the two bitsets to check whether the set of
			// already covered stations in curentGroups and the stations in the
			// group to be considered (allPossibleGroups[i]) conflict
			if ((allPossibleGroups[i].servedStations & currentGroups.servedStations).count() == 0) {
				// all stations not yet covered

				Partition newGrouping = currentGroups;

				// set stations as covered in new grouping
				// this is simply the bitwise OR of stations already covered and
				// newly covered by allPossibleGroups[i]
				newGrouping.servedStations = (allPossibleGroups[i].servedStations | currentGroups.servedStations);

				// add group to new grouping
				newGrouping.groups.push_back(i);
				newGrouping.totalThroughput += allPossibleGroups[i].throughPut;

				makeGroupingRecursively(newGrouping, i+1);
			}
		}
		return;
	}
}



