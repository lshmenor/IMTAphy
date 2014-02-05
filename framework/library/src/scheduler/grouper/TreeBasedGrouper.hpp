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

#ifndef WNS_SCHEDULER_GROUPER_TREEBASEDGROUPER_HPP
#define WNS_SCHEDULER_GROUPER_TREEBASEDGROUPER_HPP

#include <WNS/scheduler/SchedulerTypes.hpp>
#include <WNS/scheduler/grouper/SpatialGrouper.hpp>
#include <WNS/StaticFactory.hpp>
#include <WNS/PyConfigViewCreator.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/CandI.hpp>
#include <vector>

namespace wns { namespace scheduler { namespace grouper {

	class TreeBasedGrouper :
		public SpatialGrouper
	{
	public:
		TreeBasedGrouper(const wns::pyconfig::View& config);
		~TreeBasedGrouper() {};

		virtual Grouping getTxGrouping(const UserSet activeUsers, int maxBeams);
		virtual Grouping getRxGrouping(const UserSet activeUsers, int maxBeams);

	protected:
		enum ModeType {
			rx = 0,
			tx
		};
		typedef std::vector<UserSet> TreeLevel;

		// pure virtual, to be implemented by the different Heuristics:
		virtual Grouping treeAlgorithm(const UserSet activeUsers, unsigned int maxBeams, ModeType mode) = 0;
		// basic functionality
		virtual float getTPfromTreeLevelByMode(TreeLevel level, ModeType mode);
		virtual Grouping convertTreeLevelToGrouping(TreeLevel level, ModeType mode);
		virtual std::map<UserID, wns::CandI> getCandIsForGroup(const UserSet group, ModeType mode);

		std::map<UserID, wns::CandI> individualCandIs;

		int MonteCarloTreeHeuristicProbe;
	};


}}} // namespace wns::scheduler::grouper
#endif // WNS_SCHEDULER_GROUPER_TREEBASEDGROUPER_HPP


