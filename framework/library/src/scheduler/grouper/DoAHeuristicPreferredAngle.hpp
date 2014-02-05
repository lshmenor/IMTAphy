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

#ifndef WNS_SCHEDULER_DOAHEURISTIPREFERREDANGLE_HPP
#define WNS_SCHEDULER_DOAHEURISTIPREFERREDANGLE_HPP

#include "DoAGrouper.hpp"

namespace wns { namespace scheduler { namespace grouper {
	class DoAHeuristicPreferredAngle :
		public DoATreeBasedGrouper
	{
	public:
		// inherit everything from TreeBasedGrouper except for cost functions
		DoAHeuristicPreferredAngle(const wns::pyconfig::View& config);
		~DoAHeuristicPreferredAngle() {};
	private:
		float groupingCostForAUser(UserSet group, UserID newUser);

		float minAngle;
		float weight;
		int strategy;


	};


}}} // namespace wns::scheduler::grouper

#endif // WNS_SCHEDULER_DOAHEURISTICPREFERREDANGLE_HPP


