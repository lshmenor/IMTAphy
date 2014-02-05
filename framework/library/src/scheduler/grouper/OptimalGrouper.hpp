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

#ifndef WNS_SCHEDULER_GROUPER_OPTIMALGROUPER_HPP
#define WNS_SCHEDULER_GROUPER_OPTIMALGROUPER_HPP

#include <WNS/scheduler/grouper/AllPossibleGroupsGrouper.hpp>

namespace wns { namespace scheduler { namespace grouper {

	class OptimalGrouper :
		public AllPossibleGroupsGrouper
	{
	public:
		// inherit everything from AllPossibleGroupsGrouper except for makeGrouping
		OptimalGrouper(const wns::pyconfig::View& config);
		~OptimalGrouper() {};

	protected:
		virtual Partition makeGrouping(int maxBeams, unsigned int noOfStations);
	private:
		void makeGroupingRecursively(Partition currentGroups, int firstGroup);
		unsigned int noOfStations;

		Partition trivialGrouping;
		Partition currentBestGrouping;
		float throughputCurrentBestGrouping;
		float throughputTrivialGrouping;
		int maxBeams;

		int MonteCarloRandomProbe;
		int MonteCarloOptimalProbe;
	};



}}} // namespace wns::scheduler::grouper
#endif // WNS_SCHEDULER_GROUPER_OPTIMALGROUPER_HPP


