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

#include "DoAHeuristicPreferredAngle.hpp"

#include <WNS/pyconfig/View.hpp>
#include <WNS/StaticFactory.hpp>

#include <list>
#include <math.h>
#include <algorithm>

using namespace wns::scheduler;
using namespace wns::scheduler::grouper;


STATIC_FACTORY_REGISTER_WITH_CREATOR(DoAHeuristicPreferredAngle, GroupingProviderInterface, "DoAHeuristicPreferredAngle", wns::PyConfigViewCreator);


DoAHeuristicPreferredAngle::DoAHeuristicPreferredAngle(const wns::pyconfig::View& config)
	: DoATreeBasedGrouper(config),
	  minAngle(config.get<float>("minAngleDegree")*M_PI/180.0),
	  weight(config.get<float>("weight"))
{
}


float
DoAHeuristicPreferredAngle::groupingCostForAUser(UserSet group, UserID newUser) {
	float cost = 0.0;
	float threshold = minAngle;
//	float threshold = 20.0 / 180.0 * M_PI;
	float penalty = 1000.0;
	float min = M_PI;

	float newUserDoA = getNormalizedDoA(newUser);

// 	if (Debug)
// 		std::cout << "New user at " << rad2deg(newUserDoA) << " degrees\n";

	for (UserSet::const_iterator iter = group.begin();
		 iter != group.end(); ++iter) {
		float userDoA = getNormalizedDoA(*iter);

// 		if (Debug)
// 			std::cout << "Group user at " << rad2deg(userDoA) << " degrees\n";

		float diff = userDoA - newUserDoA;
		// normalize angles to [0.0, M_PI]
		if (diff < 0.0)
			diff = diff + 2*M_PI;
		if (diff > M_PI)
			diff = 2*M_PI - diff;

		if (fabs(diff) < min)
			min = fabs(diff);
	}

// 	if (Debug)
// 		std::cout << "Minimum " << rad2deg(min) << " degrees\n";

    // calculate cost to add user
	if (min < threshold)
		cost = penalty;
	else
	{
		// 0..threshold verboten
		// threshold .. 2.5X threshold erwuenscht mit minimalen kosten bei 2x
		// threshold. von 2.5x threshold bis 180 konstanter langsam wachsender
		// wert auf 1+mincost

		float minCost = threshold / M_PI;
		if (min < 2.5*threshold)
			cost = weight*(fabs(min-2*threshold)/threshold+minCost);
		else
			// fange bei 2.5*th mit 0.5+minCost an, dann steiger dich auf
			// 1+minCost bei M_PI
			cost = weight*((min-2.5*threshold)*(0.5/(M_PI-2.5*threshold)) +0.5+minCost );

	}

// 	if (Debug)
// 		std::cout << "Resulting in cost of " << cost << " \n\n";

	return cost;
}






