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

#ifndef WNS_SCHEDULER_STRATEGY_STATICPRIORITY_SUBSTRATEGYINTERFACE_HPP
#define WNS_SCHEDULER_STRATEGY_STATICPRIORITY_SUBSTRATEGYINTERFACE_HPP

#include <WNS/scheduler/strategy/Strategy.hpp>
#include <WNS/scheduler/SchedulerTypes.hpp>
#include <WNS/scheduler/SchedulingMap.hpp>
#include <WNS/PyConfigViewCreator.hpp>
//#include <WNS/scheduler/MapInfoProviderInterface.hpp>
#include <WNS/simulator/Time.hpp>
#include <WNS/StaticFactory.hpp>

namespace wns { namespace scheduler {
        namespace queue {
            class QueueInterface;
        }
        class RegistryProxyInterface;

        namespace strategy { namespace staticpriority {
                class SchedulingMap;
            }}
        namespace harq { class HARQInterface;}
    }}// forward declarations

namespace wns { namespace scheduler { namespace strategy { namespace staticpriority {

                class SubStrategyInterface
                {
                public:
                    virtual ~SubStrategyInterface(){};

                    /** @brief this method is called at the very beginning
                        Do not overload. At least call this base class method first. */
                    virtual void setColleagues(wns::scheduler::strategy::Strategy* _strategy,
                                               wns::scheduler::queue::QueueInterface* _queue,
                                               wns::scheduler::RegistryProxyInterface* _registry,
                                               wns::scheduler::harq::HARQInterface*) = 0;

                    /** @brief this method is used to initialize datastructures.
                        Please overload/implement this for your derived class */
                    virtual void
                    initialize() = 0;

                    /** @brief The main work is done here.
                        Called by StaticPriority master scheduler.
                        @param SchedulerState contains all the input parameters and current state of the scheduler.
                        @param SchedulingMap contains the (incomplete) result so far and will be updated by this method.
                    */
                    virtual MapInfoCollectionPtr
                    doStartSubScheduling(SchedulerStatePtr schedulerState,
                                         wns::scheduler::SchedulingMapPtr schedulingMap) = 0;

                    virtual bool
                    usesHARQ() = 0;
                };

                typedef wns::PyConfigViewCreator<SubStrategyInterface, SubStrategyInterface> SubStrategyCreator;
                typedef wns::StaticFactory<SubStrategyCreator> SubStrategyFactory;
            }}}}
#endif

