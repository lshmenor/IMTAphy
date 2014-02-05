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

#ifndef WNS_SCHEDULER_STRATEGY_STATICPRIORITY_EXHROUNDROBIN_HPP
#define WNS_SCHEDULER_STRATEGY_STATICPRIORITY_EXHROUNDROBIN_HPP

#include <WNS/scheduler/strategy/staticpriority/RoundRobin.hpp>
#include <WNS/scheduler/strategy/staticpriority/SubStrategy.hpp>
#include <WNS/scheduler/strategy/Strategy.hpp>
#include <WNS/scheduler/SchedulingMap.hpp>
#include <WNS/scheduler/queue/QueueInterface.hpp>
#include <WNS/scheduler/RegistryProxyInterface.hpp>
#include <WNS/StaticFactory.hpp>
#include <limits.h>

namespace wns { namespace scheduler { namespace strategy { namespace staticpriority {

                /**
                 * @brief Exhaustive Round Robin subscheduler.
                 *
                 */

                class ExhaustiveRoundRobin
                        : public wns::scheduler::strategy::staticpriority::RoundRobin
                {
                public:
                    ExhaustiveRoundRobin(const wns::pyconfig::View& config);

                    ~ExhaustiveRoundRobin();

                    virtual void
                    initialize();

                    virtual MapInfoCollectionPtr
                    doStartSubScheduling(SchedulerStatePtr schedulerState,
                                         wns::scheduler::SchedulingMapPtr schedulingMap);

                private:
                    /** @brief keep state of RR pointer */
                    //wns::scheduler::ConnectionID lastScheduledConnection;
                    /** @brief Number of packets to schedule of the same cid before proceeding to the next one.
                        (PyConfig parameter) */
                    //int blockSize;
                };
            }}}}
#endif
