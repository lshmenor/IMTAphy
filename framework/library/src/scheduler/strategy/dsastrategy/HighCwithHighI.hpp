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

#ifndef WNS_SCHEDULER_STRATEGY_DSASTRATEGY_HIGHCWITHHIGHI_HPP
#define WNS_SCHEDULER_STRATEGY_DSASTRATEGY_HIGHCWITHHIGHI_HPP
#include <WNS/scheduler/strategy/dsastrategy/DSAStrategy.hpp>
#include <WNS/scheduler/strategy/dsastrategy/Fixed.hpp>
#include <WNS/Positionable.hpp>

namespace wns { namespace scheduler { namespace strategy { namespace dsastrategy {

    /** @brief DSA startegy equally distributing available resources between users.
        If there are n resources and m users: m1 = n mod m users get floor(n/m) + 1 resources,
        m - m1 users get floor(n/m) resources.
        The resources are then granted to a user by first increasing the subChannel number, 
        then the timeSlot, then the spatialLayer.
        TODO: Make it configurable in which order time, frequency and space domain are used
        for resource sorting.
    */
    class HighCwithHighI : public DSAStrategy
    {
    public:
        HighCwithHighI(const wns::pyconfig::View& config);

        ~HighCwithHighI();

        virtual void initialize(SchedulerStatePtr schedulerState,
                                SchedulingMapPtr schedulingMap);

        virtual DSAResult
        getSubChannelWithDSA(RequestForResource& request,
                                SchedulerStatePtr schedulerState,
                                SchedulingMapPtr schedulingMap);

        bool requiresCQI() const { return false; };

    private:

        //map<UserID, Ressourcen Vektor pro User>
		std::map<unsigned int, std::vector<DSAResult> > usersResources_;

    };

}}}} // namespace wns::scheduler::strategy::dsastrategy
#endif // WNS_SCHEDULER_DSASTRATEGY_HIGHCWITHHIGHI_HPP
