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

#ifndef WNS_SCHEDULER_STRATEGY_DSASTRATEGY_DSAMETA_HPP
#define WNS_SCHEDULER_STRATEGY_DSASTRATEGY_DSAMETA_HPP

#include <WNS/scheduler/strategy/dsastrategy/DSAStrategy.hpp>
#include <vector>

namespace wns { namespace scheduler { namespace strategy { namespace dsastrategy {

                /** @brief DSA strategy to be used in slave scheduler (UT-RS-TX, uplink) */
                class DSAMeta :
            public DSAStrategy {
                public:
                    DSAMeta(const wns::pyconfig::View& config);

                    ~DSAMeta();

                    virtual void initialize(SchedulerStatePtr schedulerState,
                                            SchedulingMapPtr schedulingMap);

                    virtual DSAResult
                    getSubChannelWithDSA(RequestForResource& request,
                                         SchedulerStatePtr schedulerState,
                                         SchedulingMapPtr schedulingMap);

                    bool requiresCQI() const { return false; };

                private:
                    /** @brief TODO optimization */
                    //int lastUsedSubChannel;
                    //int lastUsedTimeSlot;
                    //int lastUsedBeam;
                };

            }}}} // namespace wns::scheduler::strategy::dsastrategy
#endif // WNS_SCHEDULER_STRATEGY_DSASTRATEGY_DSAMETA_HPP
