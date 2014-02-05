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

#ifndef WNS_SCHEDULER_STRATEGY_DSASTRATEGY_DSASTRATEGY_HPP
#define WNS_SCHEDULER_STRATEGY_DSASTRATEGY_DSASTRATEGY_HPP

#include <WNS/scheduler/strategy/dsastrategy/DSAStrategyInterface.hpp>
#include <WNS/scheduler/strategy/StrategyInterface.hpp>
#include <WNS/scheduler/strategy/SchedulerState.hpp>
#include <WNS/scheduler/SchedulerTypes.hpp>
#include <WNS/scheduler/RegistryProxyInterface.hpp>
#include <WNS/logger/Logger.hpp>
#include <string>
#include <vector>

namespace wns { namespace scheduler { namespace strategy { namespace dsastrategy {
                /** @brief provides methods for "Dynamic Subchannel Assignment" (DSA).
                    Used by scheduler strategies for during doAdaptiveResourceScheduling()
                */
                class DSAStrategy :
            virtual public DSAStrategyInterface
                {
                public:
                    DSAStrategy(const wns::pyconfig::View& config);
                    virtual ~DSAStrategy();

                    /** @brief called once at the beginning */
                    virtual void setColleagues(RegistryProxyInterface* _registry);

                    /** @brief call this before each timeSlot/frame.
                        Important to reset starting values. */
                    virtual void initialize(SchedulerStatePtr schedulerState,
                                            SchedulingMapPtr schedulingMap);

                    /** @brief if phyModePtr is specified,
                        the duration of the PDU on the subChannel can be calculated.
                        Otherwise, undefined means: assume the highest PhyMode */
                    virtual simTimeType
                    getCompoundDuration(RequestForResource& request) const;

                    /** @brief true if requested PDU can be scheduled on that subChannel */
                    int
                    getSpatialLayerForSubChannel(int subChannel,
                                         int timeSlot,
                                         RequestForResource& request,
                                         SchedulerStatePtr schedulerState,
                                         SchedulingMapPtr schedulingMap) const;

                    /** @brief true if requested PDU can be scheduled on that subChannel&timeSlot&spatialLayer */
                    virtual bool
                    channelIsUsable(int subChannel,
                                    int timeSlot,
                                    int spatialLayer,
                                    RequestForResource& request,
                                    SchedulerStatePtr schedulerState,
                                    SchedulingMapPtr schedulingMap) const;

                    /** @brief true if requested PDU can be scheduled on that resource block */
                    virtual bool
                    channelIsUsable(int subChannel,
                                    int timeSlot,
                                    RequestForResource& request,
                                    SchedulerStatePtr schedulerState,
                                    SchedulingMapPtr schedulingMap) const;

                protected:
                    wns::logger::Logger logger;
                    struct Colleagues {
                        Colleagues() {registry=NULL;};
                        RegistryProxyInterface* registry;
                    } colleagues;

                protected:
                    /** @brief may different users share a sbubchannel? Usually not. */
                    bool oneUserOnOneSubChannel;
                    /** @brief UL: true if all subchannels must be adjacent (SC-FDMA) */
                    bool adjacentSubchannelsOnUplink;
                    /** @brief data rate per subChannel assuming the highest PhyMode */
                    double highestDataRatePerSubChannel;
                    /** @brief phyModeMapper is required to get the dataRate of a PhyMode */
                    wns::service::phy::phymode::PhyModeMapperInterface* phyModeMapper;
                    /** @brief myUserID is important in channelIsUsable() */
                    wns::scheduler::UserID myUserID;
                private:
                    std::string dsastrategyName;
                };
            }}}} // namespace wns::scheduler::strategy::dsastrategy
#endif //  WNS_SCHEDULER_DSASTRATEGY_DSASTRATEGY_HPP
