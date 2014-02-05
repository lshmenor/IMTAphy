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

#ifndef WNS_SCHEDULER_GROUPER_DOATREEBASEDGROUPER_HPP
#define WNS_SCHEDULER_GROUPER_DOATREEBASEDGROUPER_HPP

#include <WNS/scheduler/SchedulerTypes.hpp>
#include <WNS/scheduler/grouper/TreeBasedGrouper.hpp>
#include <WNS/StaticFactory.hpp>
#include <WNS/PyConfigViewCreator.hpp>
#include <WNS/pyconfig/View.hpp>
#include <vector>

namespace wns { namespace scheduler { namespace grouper {

	class DoATreeBasedGrouper :
		public TreeBasedGrouper
	{
	public:
		DoATreeBasedGrouper(const wns::pyconfig::View& config);
		~DoATreeBasedGrouper() {};

	protected:
		// pure virtual, to be implemented by the different Heuristics:
		virtual float groupingCostForAUser(UserSet group, UserID newUser) = 0;
		// this could be shared
		virtual float groupingCost(UserSet group, UserSet newGroup, ModeType mode);

		virtual Grouping treeAlgorithm(const UserSet activeUsers, unsigned int maxBeams, ModeType mode);
		virtual float getNormalizedDoA(UserID user);
		virtual float rad2deg(float rad) { return rad*180.0/M_PI; }

		int strategy;
	};


}}} // namespace wns::scheduler::grouper

#endif // WNS_SCHEDULER_GROUPER_DOATREEBASEDGROUPER_HPP


