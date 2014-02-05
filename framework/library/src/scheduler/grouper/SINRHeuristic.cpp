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

#include <WNS/scheduler/grouper/SINRHeuristic.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/StaticFactory.hpp>

using namespace wns::scheduler;
using namespace wns::scheduler::grouper;

STATIC_FACTORY_REGISTER_WITH_CREATOR(SINRHeuristic, GroupingProviderInterface, "SINRHeuristic", wns::PyConfigViewCreator);


SINRHeuristic::SINRHeuristic(const wns::pyconfig::View& config)
	: TreeBasedGrouper(config)
{
}

float
SINRHeuristic::groupingUtility(UserSet group, UserSet newGroup, ModeType mode) {
	// merge the two groups
	group.insert(newGroup.begin(), newGroup.end());

	std::map<UserID, wns::CandI> candis =
		getCandIsForGroup(group, mode);

	double sum = 0.0;

	for (UserSet::iterator iter = group.begin();
		 iter != group.end(); ++iter)
	{
		assure(candis.find(*iter) != candis.end(), "Where is my user gone?");
		wns::Ratio sinr(candis[*iter].C / candis[*iter].I);
		//double tp = colleagues.registry->getPhyModeForSIR(candis[*iter].C / candis[*iter].I).second;
		if (colleagues.phyModeMapper->sinrIsAboveLimit(sinr)) {
			double tp = colleagues.phyModeMapper->getBestPhyMode(sinr)->getDataRate();
			sum += tp;
		} else
			return 0.0; // don't allow user without service
	}
	return sum;
}



float
SINRHeuristic::getTPfromTreeLevel(TreeLevel level, std::map<UserSet, float> &groupTP)
{
	float tpLevel = 0.0;
	for (TreeLevel::const_iterator iter = level.begin();
		 iter != level.end(); ++iter)
		if (groupTP[*iter] < 0.0001)
			return 0.0;
		else
			tpLevel += groupTP[*iter];
	return tpLevel;
}


Grouping
SINRHeuristic::treeAlgorithm(const UserSet activeUsers, unsigned int maxBeams, ModeType mode)
{
	std::vector<TreeLevel> treeLevels;
	std::map<float, std::pair<unsigned int, unsigned int> > combinedTP;
	std::map<UserSet, float> groupTP; // database that stores TP for every group

	std::vector<float> sdmaGain;
	sdmaGain.clear();

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
		//float tp = colleagues.registry->getPhyModeForSIR(candis[*iter].C / candis[*iter].I).second;
		//if (phymode->getRate() > 0.0001)
		if (colleagues.phyModeMapper->sinrIsAboveLimit(sinr))
		{
			double tp = colleagues.phyModeMapper->getBestPhyMode(sinr)->getDataRate();
			groupTP[oneUserGroup] = tp;
			individualCandIs[*iter] = candis[*iter];
			firstLevel.push_back(oneUserGroup);
		} else { // do nothing
			MESSAGE_BEGIN(NORMAL, logger, m, colleagues.registry->getNameForUser(colleagues.registry->getMyUserID()));
			m << ": discarding user "
			  << colleagues.registry->getNameForUser(*iter)
			  << " because even alone its SINR is too bad for transmission. SINR=" << sinr;
			MESSAGE_END();
		}
	}
	treeLevels.push_back(firstLevel);
	unsigned int maxTreeHeight = firstLevel.size();

	// get the TP for the base case (no SDMA) and calculate the average TP per
	// slot. This is the reference value for our grouping gain
	float trivialTP = getTPfromTreeLevel(treeLevels[0], groupTP);
	float normalizedTrivialTP = trivialTP / float(treeLevels[0].size());
	sdmaGain.push_back(1.0); // the  first level (index 0) is the reference => gain = 1.0


	// then build the next higher levels by joining the two best-joinable groups
	// from the previous level
	for (unsigned int i = 0; i < maxTreeHeight; ++i)
	{
		combinedTP.clear();
		// get all possible combinations of two groups, but don't calculate same
		// group twice
		for (unsigned int g1 = 0; g1 < treeLevels[i].size() - 1; ++g1)
			for (unsigned int g2 = g1+1; g2 < treeLevels[i].size(); ++g2)
				if (treeLevels[i][g1].size() + treeLevels[i][g2].size() <= maxBeams)
					// cost function is overwritten by specialized TreeGrouper variant
					combinedTP[groupingUtility(treeLevels[i][g1], treeLevels[i][g2], mode)] =
						std::make_pair<unsigned int, unsigned int>(g1,g2);
		//		assure(costs.size(), "Not on upmost level but all groups too big
		// to join");
		if (combinedTP.size() == 0) // could not join two groups, so we are done
			break;

		std::pair<float, std::pair<unsigned int, unsigned int> > maxEntry = *(max_element(combinedTP.begin(), combinedTP.end()));
		std::pair<unsigned int, unsigned int> bestCombi = maxEntry.second;

		// build the next level
		TreeLevel nextLevel;

		// first join the the two best joinable groups and then save it
		UserSet newGroup = treeLevels[i][bestCombi.first];
		newGroup.insert(treeLevels[i][bestCombi.second].begin(),
						treeLevels[i][bestCombi.second].end());
		nextLevel.push_back(newGroup);

		// the TP for the new group was delivered by the utility function
		groupTP[newGroup] = maxEntry.first;

		// then add the others
		for (unsigned int g = 0; g < treeLevels[i].size(); ++g)
			if ((g != bestCombi.first) && (g != bestCombi.second))
				nextLevel.push_back(treeLevels[i][g]);

		// calculate the SDMA gain for next level
		sdmaGain.push_back((getTPfromTreeLevel(nextLevel, groupTP) / nextLevel.size()) / normalizedTrivialTP);
		treeLevels.push_back(nextLevel);
		// continue with the just constructed next level
	}

	// now see who got the best sdmaGain and convert it to a grouping before returning

	int maxIndex = max_element(sdmaGain.begin(), sdmaGain.end()) - sdmaGain.begin();

	MESSAGE_SINGLE(NORMAL, logger, "Selecting Level " << maxIndex << " resulting in a gain of " << sdmaGain[maxIndex]);

	groupingGainProbeBus->put(sdmaGain[maxIndex]);

	return convertTreeLevelToGrouping(treeLevels[maxIndex], mode);
}






