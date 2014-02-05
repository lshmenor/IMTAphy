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

#include <WNS/scheduler/grouper/AllPossibleGroupsGrouper.hpp>
#include <WNS/scheduler/SchedulerTypes.hpp>

#include <WNS/service/phy/ofdma/Pattern.hpp>

using namespace wns::scheduler;
using namespace wns::scheduler::grouper;


void // I have to overload this to initialize the probe when I have the registry
AllPossibleGroupsGrouper::setColleagues(RegistryProxyInterface* _registry)
{
    //GroupingProviderInterface::setColleagues(_traffic, _registry);
    SpatialGrouper::setColleagues(_registry);
}

std::map<UserID, wns::CandI>
AllPossibleGroupsGrouper::getCandIs(std::vector<UserID> allUsers,
                                    std::bitset<MAX_STATIONS> bitset, ModeType mode)
{
    std::map<UserID, wns::CandI> candis;
    candis.clear();

    unsigned int noOfStations = allUsers.size();

    switch(mode)
    {
        case tx:
        {
            if (beamforming)
            {

                std::map<wns::node::Interface*, wns::Power> userNoiseIInterMap;
                userNoiseIInterMap.clear();

                for (unsigned int k = 0; k < noOfStations; ++k)
                if (bitset.test(k))
                {
                    userNoiseIInterMap[allUsers[k].getNode()] = 
                    colleagues.registry->estimateTxSINRAt(allUsers[k]).interference;
                }
            candis = convertMap(friends.ofdmaProvider->calculateCandIsTx(userNoiseIInterMap, x_friendliness, txPower));
            }   
            else
            { 
                // no beamforming
                assure(noOfStations == 1, "We don't do beamforming, so only one-user groups are supported");
                UserID user = allUsers[0];
                wns::scheduler::ChannelQualityOnOneSubChannel cqi = 
                colleagues.registry->estimateTxSINRAt(user);
                candis[user] = wns::CandI(cqi.carrier, cqi.interference);
            }

            break;
        }
        case rx:
        {
            if (beamforming)
            {
                std::vector<wns::node::Interface*> combination;
                combination.clear();

                for (unsigned int k = 0; k < noOfStations; ++k)
                if (bitset.test(k))
                {
                    combination.push_back(allUsers[k].getNode());
                }
                candis = convertMap(friends.ofdmaProvider->calculateCandIsRx(combination,
                colleagues.registry->estimateRxSINROf(allUsers[0]).interference));
                //use estimated interference of user 0 for all other
                //terminals as well, maybe average over all entries?
                //see TreeBasedGrouper
            }
            else
            {
            // no beamforming
            assure(noOfStations == 1, "We don't do beamforming, so only one-user groups are supported");
            UserID user = allUsers[0];
            wns::scheduler::ChannelQualityOnOneSubChannel cqi = 
            colleagues.registry->estimateRxSINROf(user);
            candis[user] = wns::CandI(cqi.carrier, cqi.interference);
            }

            break;
        }
        default:
            assure(0, "Wrong mode, can  either be RX or TX");
    }
    return candis;
}

float
AllPossibleGroupsGrouper::getTPperGroupTrivialGrouping(int noOfStations)
{
    // what would be the throughput / #users when every user is served on its own,
    // i.e. not grouped

    // we simply iterate over allPossibleGroups and extract all servedStations
    // bitsets that have exactly one bit - the ith - set. The matching bitsets
    // cannot be accessed directly because they are not necessarily at position
    // 2^i because invalid groups (too many beams, no service etc) got deleted

    unsigned int j = 0;
    float totalTP = 0.0;

    for (int i = 0; i < noOfStations; ++i)
    {
        while (!(allPossibleGroups[j].servedStations.test(i) &&
                (allPossibleGroups[j].servedStations.count() == 1)))
		{
			++j;
			if (j == allPossibleGroups.size())
			{
				// make sure that j does not run out of bounds if the group with
				// the single user is not found.
				MESSAGE_SINGLE(NORMAL, logger, "AllPossibleGroupsGrouper: In this user set there are users that can't be served at all, not even alone");
				j = 0;
				break; // stop this while loop - we won't find him anyway
				       // because groups with unserved users got removed before
			}
		}
		totalTP += allPossibleGroups[j].throughPut;
	}
	return totalTP / float(noOfStations);
}

std::vector<AllPossibleGroupsGrouper::Beams>
AllPossibleGroupsGrouper::calculateAllPossibleGroups(std::vector<UserID> allUsers, unsigned int maxBeams,
										   ModeType mode)
{
	assure(allUsers.size() <= MAX_STATIONS, "That many stations are not supported, compile with bigger value for MAX_STATIONS");

	std::map<UserID, wns::CandI> candis;
	std::vector<Beams> beams;
	beams.clear();

	// Let allUsers.size() be the number of users for which all possible beam
	// combinations of up to maxBeams are to be calculated. Then the algorithm
	// creates all 2^n possible combinations of users to be served in parallel
	// and checks whether there are only maxBeams users served at once.

	for (unsigned int bits = 1; bits < (unsigned int)(1 << allUsers.size()); ++bits)
	{
		bool userWithoutService = false;
		std::bitset<MAX_STATIONS> bitset(bits);
		if (bitset.count() > maxBeams)
			continue;

		// The SINR calculation is different for RX- or TX-Mode
		candis = getCandIs(allUsers, bitset, mode);

		// convert SINRS to Throughput
		float throughPut = 0.0;
		for (std::map<UserID, wns::CandI>::const_iterator iter = candis.begin();
		     iter != candis.end(); ++iter) {
			wns::Ratio sinr(iter->second.C/iter->second.I);
			//if (colleagues.registry->getPhyModeForSIR( iter->second.C/iter->second.I ).second < 0.001)
			if (colleagues.phyModeMapper->sinrIsAboveLimit(sinr)) {
				double tp = colleagues.phyModeMapper->getBestPhyMode(sinr)->getDataRate();
				throughPut += tp;
				//throughPut += colleagues.registry->getPhyModeForSIR( iter->second.C/iter->second.I ).second;
			} else	{ // user with zero data rate, skip this beam
				userWithoutService = true;
				break;
			}
		}
		Beams currentBeam;
		currentBeam.servedStations = bitset;
		currentBeam.throughPut = throughPut;

		// No empty beams or beams with user without service may be
		// saved. This is guaranteed because bits=1..n thus eliminating the
		// empty beam and because once a user without service is discovered, the
		// userWithoutService flag is set in checked below:
		if (!userWithoutService)
			beams.push_back(currentBeam);

	}
	return beams;
}



Grouping
AllPossibleGroupsGrouper::convertPartitionToGrouping(Partition partition, ModeType mode,
													 std::vector<UserID> allUsers)
{
	Grouping grouping;
	unsigned int noOfStations = allUsers.size();


	for (unsigned int i = 0 ; i < partition.groups.size(); ++i) {
		Group newGroup;
		Beams currentGroup = allPossibleGroups[partition.groups[i]];
        std::vector<wns::node::Interface*> usersInGroup;
		usersInGroup.clear();

		for (unsigned int j = 0; j < noOfStations; ++j)
			if (currentGroup.servedStations.test(j))
                usersInGroup.push_back(allUsers[j].getNode());

		// calculate the pattern for every user in the group
		for (unsigned int d = 0; d < usersInGroup.size(); ++d) {
            std::vector<wns::node::Interface*> undesireds;
			undesireds.clear();

			for (unsigned int u = 0; u < usersInGroup.size(); ++u)
				if (d != u)
                    undesireds.push_back(usersInGroup[u]);
			// calculate users pattern with his co-scheduled users as undesireds
			if(mode == tx){
                grouping.patterns[UserID(usersInGroup[d])] = friends.ofdmaProvider->
                    calculateAndSetBeam(usersInGroup[d], undesireds, x_friendliness);
			}
			else{ // rx case
                grouping.patterns[UserID(usersInGroup[d])] = friends.ofdmaProvider->
                    calculateAndSetBeam(usersInGroup[d], undesireds, colleagues.registry->estimateRxSINROf(UserID(usersInGroup[d])).interference);
				//see TreeBasedGrouper

			}

            assure(grouping.patterns[UserID(usersInGroup[d])] !=
				   wns::service::phy::ofdma::PatternPtr(), "Invalid pattern returned");
		}

		// calculate the SINR values for every user in the group
		newGroup = getCandIs(allUsers, currentGroup.servedStations, mode);
		// save it in grouping
		grouping.groups.push_back(newGroup);
		// and update the group look-up-table
        for (std::vector<wns::node::Interface*>::const_iterator iter = usersInGroup.begin();
			 iter != usersInGroup.end();
			 ++iter)
            grouping.userGroupNumber[UserID(*iter)] = i;


// assure that the calculated total TP was not corrupted/mixed up
#ifndef WNS_NDEBUG
		float throughPut = 0.0;
		for (Group::const_iterator iter = newGroup.begin();
		     iter != newGroup.end(); ++iter) {
			wns::Ratio sinr(iter->second.C/iter->second.I);
			if (colleagues.phyModeMapper->sinrIsAboveLimit(sinr)) {
				double tp = colleagues.phyModeMapper->getBestPhyMode(sinr)->getDataRate();
				throughPut += tp;
				//throughPut += colleagues.registry->getPhyModeForSIR( iter->second.C/iter->second.I ).second;
			} else	{ // user with zero data rate, skip this beam
				assure(0, "Users with zero data rate should have been removed before");
			}
		}
		double diff = throughPut - currentGroup.throughPut;
		assure((diff > -0.0001) && (diff < 0.0001), "Different throughputs calculated");
#endif

	}
	return grouping;
}

/**
 * @brief Return a vector of users but only include those that could get an SINR
 * that is suitable for transmission when served alone. If we can't even
 * transmit when the users is grouped alone, there is no point in trying to
 * group him with users.
 */
std::vector<UserID>
AllPossibleGroupsGrouper::getServableUserVectorFromSet(const UserSet userSet, ModeType mode) {
	std::map<UserID, wns::CandI> candis;
	std::vector<UserID> userVector;

	// convert UserSet to userVector needed for calculateAllPossibleGroups
	for (UserSet::const_iterator iter = userSet.begin();
	     iter != userSet.end(); ++iter)
	{
		switch(mode) {
		case tx:
		{
			if (beamforming){
                std::map<wns::node::Interface*, wns::Power> userNoiseIInterMap;
				userNoiseIInterMap.clear();
				userNoiseIInterMap[iter->getNode()] = colleagues.registry->estimateTxSINRAt(*iter).interference;
				candis = convertMap(friends.ofdmaProvider->calculateCandIsTx(userNoiseIInterMap, x_friendliness, txPower));
			}
			else{ // no beamforming
				assure(userSet.size() == 1, "We don't do beamforming, so only one-user groups are supported");
				UserID user = *iter;
                wns::scheduler::ChannelQualityOnOneSubChannel cqi =
                    colleagues.registry->estimateTxSINRAt(user);
				candis[user] = wns::CandI(cqi.carrier, cqi.interference);
			}
		}
		break;
		case rx:
		{
			if (beamforming){
                std::vector<wns::node::Interface*> combination;
				combination.clear();
                combination.push_back(iter->getNode());

				candis = convertMap(friends.ofdmaProvider->calculateCandIsRx(combination,
                    colleagues.registry->estimateRxSINROf(*iter).interference));
			}
			else{ // no beamforming

				assure(userSet.size() == 1, "We don't do beamforming, so only one-user groups are supported");
				UserID user = *iter;
                wns::scheduler::ChannelQualityOnOneSubChannel cqi =
                    colleagues.registry->estimateRxSINROf(user);
				candis[user] = wns::CandI(cqi.carrier, cqi.interference);
			}

		}
		break;
		default:
			assure(0, "Unknown mode");
		}
		wns::Ratio sinr(candis[*iter].C / candis[*iter].I);
		//if (colleagues.registry->getPhyModeForSIR(candis[*iter].C / candis[*iter].I).second > 0.0001)
		if (colleagues.phyModeMapper->sinrIsAboveLimit(sinr))
			userVector.push_back(*iter);
		else {
			MESSAGE_BEGIN(VERBOSE, logger, m, colleagues.registry->getNameForUser(colleagues.registry->getMyUserID()));
			m << "removing user"
			  << colleagues.registry->getNameForUser(*iter)
			  << " because SINR is not sufficient even for serving the user alone";
			MESSAGE_END();

		}
	}
	return userVector;
}


/**
 * @brief Calculates an optimal grouping from a set of users. Such a grouping is
 * a partition of the set of activeUsers into groups (subsets) of users so that
 * every active users is served in exactly one group.
 * The grouping is optimal with respect to the metric introduced in my (Jan
 * Ellenbeck) diploma thesis.
 *
 * TX case
*/
Grouping
AllPossibleGroupsGrouper::getTxGrouping(const UserSet activeUsers, int maxBeams ) {
	assure(dynamic_cast<wns::service::phy::ofdma::DataTransmission*>(friends.ofdmaProvider),
		   "Grouper needs an OFDMA Provider");
	assure(colleagues.registry, "AllPossibleGroupsGrouper needs a registry");

	std::vector<UserID> userVector = getServableUserVectorFromSet(activeUsers, tx);
	allPossibleGroups = calculateAllPossibleGroups(userVector, maxBeams, tx);

	MESSAGE_BEGIN(VERBOSE, logger, m, colleagues.registry->getNameForUser(colleagues.registry->getMyUserID()));
	m << "AllPossibleGroupsGrouper: calculated all possible groups, that's "
	  << allPossibleGroups.size()
	  << " groups";
	MESSAGE_END();

	// now that I have all the possible groups that can build the partition
	Partition bestGrouping = makeGrouping(maxBeams, userVector.size());

	// now the currentBestGrouping is set to the best grouping
	// from that I have to construct the Grouping data structure that I have to
	// return

	return convertPartitionToGrouping(bestGrouping, tx, userVector);
}

/**
 * @brief Calculates an optimal grouping from a set of users. Such a grouping is
 * a partition of the set of activeUsers into groups (subsets) of users so that
 * every active users is served in exactly one group.
 * The grouping is optimal with respect to the metric introduced in my (Jan
 * Ellenbeck) diploma thesis.
 *
 * RX case
*/
Grouping
AllPossibleGroupsGrouper::getRxGrouping(const UserSet activeUsers , int maxBeams) {
	assure(dynamic_cast<wns::service::phy::ofdma::DataTransmission*>(friends.ofdmaProvider),
		   "Grouper needs an OFDMA Provider");
	assure(colleagues.registry, "AllPossibleGroupsGrouper needs a registry");

	std::vector<UserID> userVector = getServableUserVectorFromSet(activeUsers, rx);
	allPossibleGroups = calculateAllPossibleGroups(userVector, maxBeams, rx);

	MESSAGE_BEGIN(VERBOSE, logger, m, colleagues.registry->getNameForUser(colleagues.registry->getMyUserID()));
	m << "AllPossibleGroupsGrouper: calculated all possible groups, that's "
	  << allPossibleGroups.size()
	  << " groups";
	MESSAGE_END();

	// now that I have all the possible groups that can build the partition
	Partition bestGrouping = makeGrouping(maxBeams,  userVector.size());

	// now the currentBestGrouping is set to the optimal grouping
	// from that I have to construct the Grouping data structure that I have to
	// return

	return convertPartitionToGrouping(bestGrouping, rx, userVector);
}



