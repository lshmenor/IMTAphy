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

#ifndef WNS_SCHEDULER_GROUPER_ALLPOSSIBLEGROUPSGROUPER_HPP
#define WNS_SCHEDULER_GROUPER_ALLPOSSIBLEGROUPSGROUPER_HPP

#include <WNS/scheduler/SchedulerTypes.hpp>
#include <WNS/scheduler/grouper/SpatialGrouper.hpp>
#include <WNS/StaticFactory.hpp>
#include <WNS/PyConfigViewCreator.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/CandI.hpp>
#include <bitset>
#include <vector>

#define MAX_STATIONS 64

namespace wns { namespace scheduler { namespace grouper {

	class AllPossibleGroupsGrouper :
		public SpatialGrouper
	{
	public:
		AllPossibleGroupsGrouper(const wns::pyconfig::View& config)
			: SpatialGrouper(config)
			{};
		~AllPossibleGroupsGrouper() {};

		virtual Grouping getTxGrouping(const UserSet activeUsers, int maxBeams);
		virtual Grouping getRxGrouping(const UserSet activeUsers, int maxBeams);

		virtual void setColleagues(RegistryProxyInterface* _registry);

	protected:
		enum ModeType {
			rx = 0,
			tx
		};

		typedef struct
		{   // datastructure to store the stations served in a beam and the
			// resulting throughput
			std::bitset<MAX_STATIONS> servedStations;
			float throughPut;
		} Beams;

		typedef struct
		{   // datastructure to store the stations served by a (preliminary)
			// grouping, the throughput achieved and the beams used.
			// The grouping is a partition of all served stations into groups
			// (the beams)
			std::bitset<MAX_STATIONS> servedStations;
			std::vector<int> groups; // store the group indices from allPossibleGroups
			float totalThroughput;
		} Partition;

		virtual Partition makeGrouping(int maxBeams, unsigned int noOfStations) = 0;
		virtual std::map<UserID, wns::CandI> getCandIs(std::vector<UserID> users, std::bitset<MAX_STATIONS> bitset, ModeType mode);
		virtual std::vector<Beams> calculateAllPossibleGroups(std::vector<UserID> allUsers, unsigned int maxBeams,  ModeType mode);
		virtual float getTPperGroupTrivialGrouping(int noOfStations);
		virtual Grouping convertPartitionToGrouping(Partition partition, ModeType mode, std::vector<UserID> allUsers);
		virtual std::vector<UserID> getServableUserVectorFromSet(const UserSet userSet, ModeType mode);


		std::vector<Beams> allPossibleGroups;

	};



}}} // namespace wns::scheduler::grouper

#endif // WNS_SCHEDULER_GROUPER_ALLPOSSIBLEGROUPSGROUPER


