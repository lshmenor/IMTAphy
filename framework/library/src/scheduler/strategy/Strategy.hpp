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

#ifndef WNS_SCHEDULER_STRATEGY_STRATEGY_HPP
#define WNS_SCHEDULER_STRATEGY_STRATEGY_HPP

#include <WNS/scheduler/strategy/StrategyInterface.hpp>
#include <WNS/scheduler/strategy/SchedulerState.hpp>
#include <WNS/scheduler/SchedulerTypes.hpp>
#include <WNS/scheduler/queue/QueueInterface.hpp>
#include <WNS/scheduler/grouper/SpatialGrouper.hpp>
#include <WNS/scheduler/strategy/apcstrategy/APCStrategyInterface.hpp>
#include <WNS/scheduler/strategy/dsastrategy/DSAStrategyInterface.hpp>
#include <WNS/service/phy/phymode/PhyModeInterface.hpp>
#include <WNS/logger/Logger.hpp>

namespace wns { namespace scheduler { namespace harq { class HARQInterface; }}}

namespace wns { namespace scheduler { namespace strategy {

            const simTimeType slotLengthRoundingTolerance = 1e-12;

            class CapabilityAspect
            {
            public:
                virtual ~CapabilityAspect(){};
                virtual bool canHandleDL() { return false; };
                virtual bool canHandleUL() { return false; };
                virtual bool canBeSlave()  { return false; };
            };

            /** @brief let your strategy derive from this if it supports DL scheduling */
            class DLAspect
                    : virtual public CapabilityAspect
            {
            public:
                DLAspect(){};
                virtual ~DLAspect(){};
                virtual bool canHandleDL() { return true; };
            };

            /** @brief let your strategy derive from this if it supports UL master scheduling */
            class ULAspect
                    : virtual public CapabilityAspect
            {
            public:
                ULAspect(){};
                virtual ~ULAspect(){};
                virtual bool canHandleUL() { return true; };
            };

            /** @brief let your strategy derive from this if it supports UL slave scheduling */
            class SlaveAspect
                    : virtual public CapabilityAspect
            {
            public:
                SlaveAspect(){};
                virtual ~SlaveAspect(){};
                virtual bool canBeSlave()  { return true; };
            };

            /** @brief this is the base class of a scheduler strategy;
                it provides basic operations common for all strategies.
                See StaticPriority and SubStrategies for more advanced concepts */
            class Strategy
                    : virtual public StrategyInterface,
                      virtual public CapabilityAspect
            {
            public:
                Strategy(const wns::pyconfig::View& config);
                /** @brief cleanup. The derived method MUST call this base class method at the end.  */
                virtual ~Strategy();
                /** @brief Colleagues are required to communicate with the environment */
                virtual void setColleagues(queue::QueueInterface* _queue,
                                           grouper::GroupingProviderInterface* _grouper,
                                           RegistryProxyInterface* _registry,
                                           wns::scheduler::harq::HARQInterface* _harq
                    );

                /** @brief can be overloaded by derived classes to perform initialization code.
                    The derived method MUST call this base class method first. */
                virtual void onColleaguesKnown();

                virtual void setFriends(wns::service::phy::ofdma::BFInterface* _ofdmaProvider);

                /** @brief retrieve info about a peer user's power capabilities (for master UL scheduling) */
                wns::scheduler::PowerCapabilities
                getPowerCapabilities(const UserID user) const;

                /** @brief obtain percentage of resources allocated during last round.
                    Default Implementation only. */
                virtual float getResourceUsage() const;

                /** @brief performs resource scheduling including DSA, AMC and APC algorithms */
                virtual MapInfoEntryPtr
                doAdaptiveResourceScheduling(wns::scheduler::strategy::RequestForResource& request,
                                             //wns::scheduler::SchedulerStatePtr schedulerState,
                                             wns::scheduler::SchedulingMapPtr schedulingMap);

                /** @brief there are three positions for the scheduler... */
                virtual wns::scheduler::SchedulerSpotType
                getSchedulerSpotType() const;

                /** @brief indicates type of ResourceScheduler (TX or RX).
                    Only used in WinProSt. */
                virtual bool isTx() const;

                /** @brief indicates if strategy is of 'old' (before 2009) or 'new' type (>=2009). Used in assertions. */
                virtual bool isNewStrategy() const { return false; }

                /** @brief get scheduling result. For sending Maps. Called from MapHandler.
                    The caller must do colleagues.strategy->clearMap() afterwards.
                    Used by LTE. */
                MapInfoCollectionPtr getMapInfo() const;

                virtual dsastrategy::DSAStrategyInterface*
                getDSAStrategy(){return colleagues.dsafbstrategy;};

                virtual apcstrategy::APCStrategyInterface*
                getAPCStrategy(){return colleagues.apcstrategy;};

            protected:

                /** @brief The strategies need a new state. */
                virtual SchedulerStatePtr
                getNewSchedulerState();
                /** @brief The state is revolved before a new doStartScheduling() call. */
                virtual SchedulerStatePtr
                revolveSchedulerState(const StrategyInput& strategyInput);
                /** @brief Get the scheduler state here. */
                virtual SchedulerStatePtr
                getSchedulerState();

                /** @brief (for AP) determines the order of PDUs in the given full set of resources.
                    calls doStartScheduling() of derived class.
                    @param StrategyInput contains parameters set by the caller.
                    @return StrategyResult collection. All set inside.
                */
                virtual StrategyResult
                startScheduling(const StrategyInput& StrategyInput);

                /** @brief return decision of the Link Adaptation algorithm or
                 * the fixed, predefined phymode */
                virtual wns::service::phy::phymode::PhyModeInterfacePtr
                getBestPhyMode(const wns::Ratio& sinr) const;

                virtual bool groupingRequired() const;

                struct Colleagues {
                    Colleagues() {queue=NULL;grouper=NULL;registry=NULL;dsastrategy=NULL;dsafbstrategy=NULL;apcstrategy=NULL;harq=NULL;};
                    queue::QueueInterface* queue;
                    grouper::GroupingProviderInterface* grouper;
                    RegistryProxyInterface* registry;
                    wns::scheduler::harq::HARQInterface* harq;
                    dsastrategy::DSAStrategyInterface* dsastrategy;
                    dsastrategy::DSAStrategyInterface* dsafbstrategy;
                    apcstrategy::APCStrategyInterface* apcstrategy;
                } colleagues;

                struct Friends {
                    Friends() {ofdmaProvider=NULL;};
                    wns::service::phy::ofdma::BFInterface* ofdmaProvider;
                } friends;

                /** @brief Python Config View */
                wns::pyconfig::View pyConfig;
                /** @brief Logger */
                wns::logger::Logger logger;

            protected:
                /** @brief helper method to support the old scheduler strategies. */
                /** @brief return info about txPower (in slave scheduling mode) */
                wns::Power getTxPower() const;

            private:
                SchedulerStatePtr schedulerState;
            }; // class Strategy
        }}} // namespace wns::scheduler::strategy
#endif // WNS_SCHEDULER_STRATEGY_STRATEGY_HPP


