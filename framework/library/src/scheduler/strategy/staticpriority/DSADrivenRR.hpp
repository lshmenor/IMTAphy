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

#ifndef WNS_SCHEDULER_STRATEGY_STATICPRIORITY_DSADRIVENRR_HPP
#define WNS_SCHEDULER_STRATEGY_STATICPRIORITY_DSADRIVENRR_HPP

#include <WNS/scheduler/strategy/staticpriority/SubStrategy.hpp>
#include <WNS/scheduler/strategy/Strategy.hpp>
#include <WNS/scheduler/SchedulingMap.hpp>
#include <WNS/scheduler/queue/QueueInterface.hpp>
#include <WNS/scheduler/RegistryProxyInterface.hpp>
#include <WNS/StaticFactory.hpp>

namespace wns { namespace scheduler { namespace strategy { namespace staticpriority {

                class DSADrivenRR
                        : public SubStrategy
                {
                public:
                    DSADrivenRR(const wns::pyconfig::View& config);

                    ~DSADrivenRR();

                    virtual void
                    initialize();

                    /** @brief gives the next cid to schedule */
                    virtual wns::scheduler::ConnectionID
                    getNextConnection(const ConnectionSet &currentConnections, ConnectionID cid) const;

                    virtual wns::scheduler::MapInfoCollectionPtr
                    doStartSubScheduling(SchedulerStatePtr schedulerState,
                                         wns::scheduler::SchedulingMapPtr schedulingMap);
                protected:
                    /** @brief keep track which connection was last served */
                    wns::scheduler::ConnectionID lastServedConnection;
                };
            }}}}
#endif
