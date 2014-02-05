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

#include <WNS/scheduler/grouper/TreeBasedGrouper.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/StaticFactory.hpp>
#include <WNS/scheduler/SchedulerTypes.hpp>

#include <list>
#include <math.h>
#include <algorithm>

using namespace wns::scheduler;
using namespace wns::scheduler::grouper;

TreeBasedGrouper::TreeBasedGrouper(const wns::pyconfig::View& config)
	: SpatialGrouper(config)
{
}


std::map<UserID, wns::CandI>
TreeBasedGrouper::getCandIsForGroup(const UserSet group, ModeType mode)
{
    std::map<UserID, wns::CandI> candis;
	candis.clear();

	switch(mode) {
	case tx:
	{
		if(beamforming){

            std::map<wns::node::Interface*, wns::Power> userNoiseIInterMap;
			userNoiseIInterMap.clear();

			for (UserSet::const_iterator iter = group.begin();
			     iter != group.end(); ++iter) {
				UserID user = *iter;

				userNoiseIInterMap[user.getNode()] = 
                    colleagues.registry->estimateTxSINRAt(user).interference;
			}

            candis = convertMap(friends.ofdmaProvider->calculateCandIsTx(userNoiseIInterMap, x_friendliness, txPower));
		}
		else{
			assure(group.size() == 1, "We don't do beamforming, so only one-user groups are supported");
			UserID user = *group.begin();
            wns::scheduler::ChannelQualityOnOneSubChannel cqi = 
                colleagues.registry->estimateTxSINRAt(user);
			candis[user] = wns::CandI(cqi.carrier, cqi.interference);

		}
		MESSAGE_BEGIN(VERBOSE, logger, m, colleagues.registry->getNameForUser(colleagues.registry->getMyUserID()));

		for (std::map<UserID, wns::CandI>::const_iterator iter = candis.begin();
		     iter != candis.end(); ++iter)
			m << " Selecting TX grouping for "
              << colleagues.registry->getNameForUser(iter->first)
			  << "\nAssumed TxPower: " << txPower
			  << "\nEstimated Carrier: " <<  iter->second.C
			  << "\nEstimated Interference: " << iter->second.I << "\n"
			  << "\nEstimated SDMA intra-cell interference: " << iter->second.sdma.iIntra << "\n";
		MESSAGE_END();
		break;
	}
	case rx:
	{
		if (beamforming){
            std::vector<wns::node::Interface*> combination;
			combination.clear();
			wns::Power meanBsInterference = wns::Power::from_mW(0.0);

			for (UserSet::const_iterator user = group.begin();
			     user != group.end(); ++user) {
                combination.push_back(user->getNode());
				meanBsInterference += colleagues.registry->estimateRxSINROf(*user).interference;
			}
			assure(combination.size() == group.size(), "we estimate SINRs for a different set of users than we were asked to do");
			meanBsInterference /= static_cast<unsigned long int>(combination.size());

            candis = convertMap(friends.ofdmaProvider->calculateCandIsRx(combination, meanBsInterference));
		}
		else{ // no beamforming

			assure(group.size() == 1, "We don't do beamforming, so only one-user groups are supported");
			UserID user = *group.begin();
            wns::scheduler::ChannelQualityOnOneSubChannel cqi = 
                colleagues.registry->estimateRxSINROf(user);
            candis[user] = wns::CandI(cqi.carrier, cqi.interference);
		}

		MESSAGE_BEGIN(VERBOSE, logger, m, colleagues.registry->getNameForUser(colleagues.registry->getMyUserID()));
		for (std::map<UserID, wns::CandI>::const_iterator iter = candis.begin();
		     iter != candis.end(); ++iter)
			m << " Selecting RX grouping for "
			  << colleagues.registry->getNameForUser(iter->first)
			  << "\nEstimated Carrier: " <<  iter->second.C
			  << "\nEstimated Interference: " << iter->second.I << "\n";
		MESSAGE_END();

		break;
	}
	default:
		assure(0, "Wrong mode, can  either be RX or TX");
	}

	return candis;
}


float
TreeBasedGrouper::getTPfromTreeLevelByMode(const TreeLevel level, ModeType mode)
{
	float throughput = 0.0;

	// a tree level consists of a number of UserSets
	for (TreeLevel::const_iterator iter = level.begin();
		 iter != level.end(); ++iter)
	{
		UserSet group = *iter;
		std::map<UserID, wns::CandI> candis = getCandIsForGroup(group, mode);

		for (std::map<UserID, wns::CandI>::const_iterator iter2 = candis.begin();
		     iter2 != candis.end(); ++iter2) {
			wns::Ratio sinr(iter2->second.C / iter2->second.I);
			//float dataRate = colleagues.registry->getPhyModeForSIR( iter2->second.C / iter2->second.I ).second;
			if (colleagues.phyModeMapper->sinrIsAboveLimit(sinr)) {
				double tp = colleagues.phyModeMapper->getBestPhyMode(sinr)->getDataRate();
				throughput += tp;
			} else // user without service, we don't allow this
				return 0.0;
		}
	}

	return throughput;
}

Grouping
TreeBasedGrouper::convertTreeLevelToGrouping(TreeLevel level, ModeType mode)
{
	Grouping grouping;
	int groupCount = 0;


	// for all user groups in that level
	for (TreeLevel::const_iterator iter = level.begin();
		 iter != level.end(); ++iter) {
		UserSet users = *iter;
		groupCount++;

        std::vector<wns::node::Interface*> usersInGroup;
		usersInGroup.clear();

		// calculate the pattern for every user in the group:
		for (UserSet::const_iterator iter2 = users.begin();
			 iter2 != users.end(); ++iter2)
            usersInGroup.push_back(iter2->getNode());

		///\todo this code is duplicate in AllPossibleGroupsGrouper
		// for every user generate a pattern that has the other group members as
		// undesired interferers
		for (unsigned int d = 0; d < usersInGroup.size(); ++d) {
            std::vector<wns::node::Interface*> undesireds;
			undesireds.clear();
			for (unsigned int u = 0; u < usersInGroup.size(); ++u)
				if (d != u)
					undesireds.push_back(usersInGroup[u]);
			// calculate users pattern with his co-scheduled users as undesireds
			if(mode == tx){
                grouping.patterns[UserID(usersInGroup[d])] = friends.ofdmaProvider->
					calculateAndSetBeam(usersInGroup[d], undesireds,  x_friendliness);
			}
			else{
				// according to the estimation of C and I
				// values in getCandIsForGroup, the mean
				// interference level of the SSs is taken into
				// account
				wns::Power meanBsInterference = wns::Power::from_mW(0.0);
				for (UserSet::const_iterator user = users.begin();
				     user != users.end(); ++user) {
					meanBsInterference += colleagues.registry->estimateRxSINROf(*user).interference;
				}
				meanBsInterference /= static_cast<unsigned long int>(users.size());

                grouping.patterns[UserID(usersInGroup[d])] = friends.ofdmaProvider->
					calculateAndSetBeam(usersInGroup[d], undesireds, meanBsInterference);
// 				grouping.patterns[usersInGroup[d]] = friends.ofdmaProvider->
// 			                calculateAndSetBeam(usersInGroup[d], undesireds,
// 						    colleagues.registry->estimateRxSINROf(usersInGroup[d])->interference;
			}

            assure(grouping.patterns[UserID(usersInGroup[d])] !=
				   wns::service::phy::ofdma::PatternPtr(), "Invalid pattern returned");
		}
		// calculate the SINR values for every user in the group
		Group newGroup = getCandIsForGroup(users, mode);
		// save it in grouping
		grouping.groups.push_back(newGroup);
		// and update the group look-up-table
        for (std::vector<wns::node::Interface*>::const_iterator iter2 = usersInGroup.begin();
			 iter2 != usersInGroup.end();
			 ++iter2)
            grouping.userGroupNumber[UserID(*iter2)] = groupCount;
	} // for all user groups in level
	return grouping;
}



Grouping
TreeBasedGrouper::getTxGrouping(const UserSet activeUsers, int maxBeams) {
	assure(colleagues.registry, "Registry not set");
	assure(friends.ofdmaProvider, "OFDMAprovider not set");

	return treeAlgorithm(activeUsers, maxBeams, tx);
}

Grouping
TreeBasedGrouper:: getRxGrouping(const UserSet activeUsers, int maxBeams) {
	assure(colleagues.registry, "Registry not set");
	assure(friends.ofdmaProvider, "OFDMAprovider not set");

	return treeAlgorithm(activeUsers, maxBeams, rx);
}




