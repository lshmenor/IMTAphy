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

#ifndef WNS_SCHEDULER_STRATEGY_STATICPRIORITY_SUBSTRATEGY_HPP
#define WNS_SCHEDULER_STRATEGY_STATICPRIORITY_SUBSTRATEGY_HPP

#include <WNS/scheduler/queue/QueueInterface.hpp>
#include <WNS/scheduler/RegistryProxyInterface.hpp>
#include <WNS/scheduler/SchedulingMap.hpp>
#include <WNS/scheduler/strategy/StrategyInterface.hpp>
#include <WNS/scheduler/strategy/Strategy.hpp>
#include <WNS/scheduler/strategy/staticpriority/SubStrategyInterface.hpp>
#include <WNS/scheduler/harq/HARQInterface.hpp>

#include <WNS/logger/Logger.hpp>

namespace wns { namespace scheduler { namespace strategy { namespace staticpriority {

	class SubStrategy :
		virtual public wns::scheduler::strategy::staticpriority::SubStrategyInterface
	{
	public:
		virtual ~SubStrategy(){};

		SubStrategy();
		/** @brief this is the constructor which the subStrategies must implement: */
		SubStrategy(const wns::pyconfig::View& config);

		/** @brief this method is called at the very beginning
		    Do not overload. At least call this base class method first. */
		virtual void
		setColleagues(wns::scheduler::strategy::Strategy* _strategy,
			      wns::scheduler::queue::QueueInterface* _queue,
			      wns::scheduler::RegistryProxyInterface* _registry,
                      wns::scheduler::harq::HARQInterface* _harq);

		/** @brief schedule PDUs of this cid on any available subchannel. Limited by blockSize PDUs. */
		virtual bool
		scheduleCid(SchedulerStatePtr schedulerState,
			    wns::scheduler::SchedulingMapPtr schedulingMap,
			    const wns::scheduler::ConnectionID cid,
			    int& pduCounter, // modified
			    const int blockSize,
			    MapInfoCollectionPtr mapInfoCollection // result
			    );

        virtual bool
        usesHARQ();

	protected:
		wns::logger::Logger logger;
		struct Colleagues {
            Colleagues() {strategy=NULL;queue=NULL;registry=NULL;harq=NULL;};
			wns::scheduler::strategy::Strategy* strategy;
			wns::scheduler::queue::QueueInterface* queue;
			wns::scheduler::RegistryProxyInterface* registry;
            wns::scheduler::harq::HARQInterface* harq;
		} colleagues;
		/** @brief true if the queue allows dynamic Segmentation: */
		bool useDynamicSegmentation;
		/** @brief if the queue allows dynamic Segmentation, this is the smallest unit */
		int minimumSegmentSize;

		/**
		 * @brief Indicates whether HARQ is enabled for this priority class
		 */
		bool useHARQ;
	};


}}}} // namespace wns / scheduler / strategy / staticpriority
#endif // WNS_SCHEDULER_STRATEGY_STATICPRIORITY_SUBSTRATEGY_HPP

/*
  Local Variables:
  mode: c++
  fill-column: 80
  c-basic-offset: 8
  c-comment-only-line-offset: 0
  c-tab-always-indent: t
  indent-tabs-mode: t
  tab-width: 8
  End:
*/
