/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
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

#ifndef WNS_SCHEDULER_STRATEGY_STATICPRIORITY_HARQUPLINKRETRANSMISSION_HPP
#define WNS_SCHEDULER_STRATEGY_STATICPRIORITY_HARQUPLINKRETRANSMISSION_HPP

#include <WNS/scheduler/strategy/staticpriority/SubStrategy.hpp>

namespace wns { namespace scheduler { namespace strategy { namespace staticpriority {

class HARQUplinkRetransmission:
    public SubStrategy
{
public:
    HARQUplinkRetransmission(const wns::pyconfig::View& config);

    virtual ~HARQUplinkRetransmission();

    virtual void
    initialize();

    virtual MapInfoCollectionPtr
    doStartSubScheduling(SchedulerStatePtr schedulerState, wns::scheduler::SchedulingMapPtr schedulingMap);

private:
    std::vector<int>
    getUsableSubChannelsIDs(wns::scheduler::UserID user, const wns::scheduler::SchedulingMapPtr& schedulingMap);

    wns::distribution::StandardUniform randomDist;
    wns::logger::Logger logger_;
};

} // staticpriority
} // strategy
} // scheduler
} // wns

#endif // WNS_SCHEDULER_STRATEGY_STATICPRIORITY_HARQUPLINKRETRANSMISSION_HPP
