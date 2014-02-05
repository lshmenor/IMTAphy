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

#ifndef WNS_SCHEDULER_GROUPER_TESTS_GROUPERSTUB_HPP
#define WNS_SCHEDULER_GROUPER_TESTS_GROUPERSTUB_HPP

#include <WNS/scheduler/grouper/SpatialGrouper.hpp>

namespace wns { namespace scheduler { namespace grouper { namespace tests {


class GrouperStub
    : public GroupingProviderInterface 
{
public:
    ~GrouperStub()
    {
    };
    Grouping getTxGrouping(UserSet activeUsers, int maxBeams);
    Grouping getRxGrouping(UserSet activeUsers, int maxBeams);

    void setColleagues(RegistryProxyInterface*  /* registry */) {};
    void setFriends(wns::service::phy::ofdma::BFInterface*  /* ofdmaProvider */
                    /*, InteferenceManager* ifmanager */) {};

    // Stub control functions

    /**
     * @brief addGroup allows to specify the groups the grouper should return to
     * the strategy. Of course, only the activeUsers as provided to the
     * getTxGrouping / getRxGrouping are returned as group members. Further
     * pre-defined group members are omitted and further members of active Users
     * are returned in individual groups.
     */
    void addGroup(Group group);

    /**
     * @brief clearGroups resets all pre-defined groups. 
     */
    void clearGroups();

private:
    UserSet userLog;
    std::list<Group> groupList;
};
 
} // tests
} // grouper
} // scheduler
} // wns


#endif // WNS_SCHEDULER_GROUPER_TESTS_GROUPERSTUB_HPP


