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

#include <WNS/scheduler/grouper/DoAGrouper.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/StaticFactory.hpp>
#include <WNS/CandI.hpp>

#include <list>
#include <math.h>
#include <algorithm>

using namespace wns::scheduler;
using namespace wns::scheduler::grouper;

DoATreeBasedGrouper::DoATreeBasedGrouper(const wns::pyconfig::View& config)
	: TreeBasedGrouper(config),
	  strategy(config.get<int>("strategy"))
{}

float
DoATreeBasedGrouper::getNormalizedDoA(UserID user)
{
	float angle = this->friends.ofdmaProvider->estimateDoA(user.getNode());

	if (angle < 0.0)
		angle += 2*M_PI;
	return angle;
}


float
DoATreeBasedGrouper::groupingCost(UserSet group, UserSet newGroup,
							   ModeType /* mode */) {
	float total = 0.0;
	std::vector<float> costs;
	costs.clear();

	for (UserSet::const_iterator iter = newGroup.begin();
		 iter != newGroup.end(); ++iter) {
		float cost = groupingCostForAUser(group, *iter);
		costs.push_back(cost);
		total += cost;
	}

	switch (strategy) {
	case 0: {
		// returns the average cost
		return total / float(newGroup.size());
		break;
	}
	case 1: {
		// returns the maximum cost a group member experiences
		int maxIndex = max_element(costs.begin(), costs.end()) - costs.begin();
		return costs[maxIndex];
		break;
	}
	default:{
		assure(0,"Wrong strategy");
	}
	}
	assure(0, "Control structure meltdown");
	return 0.0; // not reached
}



Grouping
DoATreeBasedGrouper::treeAlgorithm(const UserSet activeUsers, unsigned int maxBeams, ModeType mode)
{
	std::vector<TreeLevel> treeLevels;
	std::map<float, std::pair<unsigned int, unsigned int> > costs;

	// init the first level
	TreeLevel firstLevel;
	for (UserSet::const_iterator iter = activeUsers.begin();
		 iter != activeUsers.end(); ++iter) {
		UserSet oneUserGroup;

		oneUserGroup.clear();
		oneUserGroup.insert(*iter);

		std::map<UserID, wns::CandI> candis =
			getCandIsForGroup(oneUserGroup, mode);

		assure(candis.find(*iter) != candis.end(), "Where is my user gone?");

        // make sure we don't include users that can't even get a suitable SINR
        // when served alone
		wns::Ratio sinr(candis[*iter].C / candis[*iter].I);
		//if (colleagues.registry->getPhyModeForSIR( candis[*iter].C / candis[*iter].I ).second > 0.0001)
		//if (phymode->getRate() > 0.0001)
		if (colleagues.phyModeMapper->sinrIsAboveLimit(sinr))
		{
			individualCandIs[*iter] = candis[*iter];
			firstLevel.push_back(oneUserGroup);
		} else {
			// FIXME(fds)...
//			LOG_INFO("discarding user ", colleagues.registry->getNameForUser(*iter), " because even alone its SINR is too bad for transmission");
		}
	}
	int maxTreeHeight = firstLevel.size();
	treeLevels.push_back(firstLevel);

	// then build the next higher levels by joining the two best-joinable groups
	// from the previous level
	for (int i = 0; i < maxTreeHeight; ++i)
	{
		costs.clear();
		// get all possible combinations of two groups, but don't calculate same
		// group twice
		for (unsigned int g1 = 0; g1 < treeLevels[i].size() - 1; ++g1)
			for (unsigned int g2 = g1+1; g2 < treeLevels[i].size(); ++g2)
				if (treeLevels[i][g1].size() + treeLevels[i][g2].size() <= maxBeams)
					// cost function is overwritten by specialized TreeGrouper variant
					costs[groupingCost(treeLevels[i][g1], treeLevels[i][g2], mode)] =
						std::make_pair<unsigned int, unsigned int>(g1,g2);
		//		assure(costs.size(), "Not on upmost level but all groups too big
		// to join");
		if (costs.size() == 0) // could not join two groups, so we are done
			break;

		std::pair<unsigned int, unsigned int> bestCombi = min_element(costs.begin(), costs.end())->second;

		// build the next level
		TreeLevel nextLevel;

		// first join the the two best joinable groups and then save it
		UserSet newGroup = treeLevels[i][bestCombi.first];
		newGroup.insert(treeLevels[i][bestCombi.second].begin(),
						treeLevels[i][bestCombi.second].end());
		nextLevel.push_back(newGroup);

		// then add the others
		for(unsigned int g = 0; g < treeLevels[i].size(); ++g)
			if ((g != bestCombi.first) && (g != bestCombi.second))
				nextLevel.push_back(treeLevels[i][g]);

		treeLevels.push_back(nextLevel);
		// continue with the just constructed next level
	}

	// get the TP for the base case (no SDMA) and calculate the average TP per
	// slot. This is the reference value for our grouping gain
	float trivialTP = getTPfromTreeLevelByMode(treeLevels[0], mode);
	float normalizedTrivialTP = trivialTP / float(treeLevels[0].size());

	std::vector<float> sdmaGain;
	sdmaGain.clear();
	sdmaGain.push_back(1.0); // the  first level (index 0) is the reference => gain = 1.0

	// now calculate the gain (avg TP per group in grouping on each tree level
	// divided by the avg TP per one-user-group in the base case
	for (unsigned int i = 1; i < treeLevels.size(); ++i) {
		sdmaGain.push_back((getTPfromTreeLevelByMode(treeLevels[i], mode) / treeLevels[i].size()) / normalizedTrivialTP);
		MESSAGE_SINGLE(NORMAL, logger, "Level " << i << " has gain of " << sdmaGain[i]);
	}

	int maxIndex = max_element(sdmaGain.begin(), sdmaGain.end()) - sdmaGain.begin();

	MESSAGE_SINGLE(NORMAL, logger, "Selecting Level " << maxIndex << " resulting in a gain of " << sdmaGain[maxIndex]);

	groupingGainProbeBus->put(sdmaGain[maxIndex]);

	return convertTreeLevelToGrouping(treeLevels[maxIndex], mode);
}





