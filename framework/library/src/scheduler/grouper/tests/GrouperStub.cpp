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

#include <WNS/scheduler/grouper/tests/GrouperStub.hpp>

using namespace wns::scheduler;
using namespace wns::scheduler::grouper::tests;


Grouping
GrouperStub::getTxGrouping(UserSet activeUsers, int /* maxBeams */)
{
    Grouping grouping;
    int groupCount = 0;


    // first add pre-defined groups but include only those members that were
    // provided by activeUsers

    for (std::list<Group>::const_iterator iter = groupList.begin();
        iter != groupList.end(); ++iter)
    {
        Group newGroup;
        newGroup.clear();

        ///\todo Who assures that not more than maxBeams users get added to the group?
        for (Group::const_iterator iter2 = iter->begin();
            iter2 != iter->end(); ++iter2)
        {
            UserID user = iter2->first;
            wns::CandI candi = iter2->second;

            if (activeUsers.find(user) != activeUsers.end())
            newGroup[user] = candi;
            activeUsers.erase(user);

            // add user to grouping's global pattern and group number DBs
            grouping.userGroupNumber[user] = groupCount;
            grouping.patterns[user] = wns::service::phy::ofdma::PatternPtr();
        }

        grouping.groups.push_back(newGroup);
        groupCount++;
    }

    // now include all remaining members from activeUsers in individual groups

    while (activeUsers.size())
    {
        // remove one user from activeUsers set
        UserID user = *activeUsers.begin();
        activeUsers.erase(user);

        ///\todo FIXME which dB value should user get?
        Group newGroup;
        wns::CandI userCandI = wns::CandI();
        // C and I resulting to C/I of 42 dB
        userCandI.C = wns::Power::from_dBm(-38);
        userCandI.I = wns::Power::from_dBm(-80);
        newGroup[user] = userCandI;


        // add user to grouping's global pattern and group number DBs
        grouping.userGroupNumber[user] = groupCount;
        grouping.patterns[user] = wns::service::phy::ofdma::PatternPtr();

        // save newGroup to grouping
        grouping.groups.push_back(newGroup);
        groupCount++;
    }

    return grouping;
}

Grouping
GrouperStub::getRxGrouping(UserSet activeUsers, int maxBeams)
{
    // for testing purposes, we don't distinguish between Rx and Tx
    return getTxGrouping(activeUsers, maxBeams);
}

void
GrouperStub::addGroup(Group group)
{
    // check if one of the users was already assigned to another group
    for (Group::const_iterator iter = group.begin();
        iter != group.end(); ++iter)
    {
        assure(userLog.find(iter->first) == userLog.end(),
              "User already assigned to different group");
        userLog.insert(iter->first);
    }

    // save the group
    groupList.push_back(group);
}

void
GrouperStub::clearGroups()
{
    groupList.clear();
    userLog.clear();
}



