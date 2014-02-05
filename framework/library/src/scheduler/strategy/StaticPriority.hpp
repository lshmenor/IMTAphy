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

#ifndef WNS_SCHEDULER_STRATEGY_STATICPRIORITY_HPP
#define WNS_SCHEDULER_STRATEGY_STATICPRIORITY_HPP

#include <WNS/scheduler/strategy/staticpriority/SubStrategyInterface.hpp>
#include <WNS/scheduler/strategy/Strategy.hpp>
#include <WNS/scheduler/strategy/SchedulerState.hpp>
#include <WNS/scheduler/SchedulingMap.hpp>
#include <WNS/scheduler/queue/QueueInterface.hpp>
#include <WNS/scheduler/RegistryProxyInterface.hpp>
#include <WNS/scheduler/MapInfoEntry.hpp>
#include <WNS/service/phy/phymode/PhyModeInterface.hpp>
#include <WNS/ldk/Compound.hpp>
#include <WNS/StaticFactory.hpp>
#include <list>
#include <vector>

namespace wns { namespace scheduler {
	class SchedulingMap;
}} // forward declaration

namespace wns { namespace scheduler { namespace strategy {

	class StaticPriority
	  : public Strategy,
	    public DLAspect,
	    public ULAspect,
	    public SlaveAspect // can do every purpose
	{
	public:
		StaticPriority(const wns::pyconfig::View& config);

		~StaticPriority();

		virtual float
		getResourceUsage() const;

		/** @brief indicates that strategy is of 'new' type (>=2009) */
		virtual bool isNewStrategy() const { return true; }

	private:
		virtual void
		onColleaguesKnown();

		virtual StrategyResult
		doStartScheduling(SchedulerStatePtr schedulerState,
				  SchedulingMapPtr schedulingMap);

		/** @brief numberOfPriorities to iterate over (registry knows) */
		int numberOfPriorities;
		/** @brief for each priority there is a subStrategy */
		typedef std::vector<wns::scheduler::strategy::staticpriority::SubStrategyInterface*> SubStrategies;
		/** @brief for each priority there is a subStrategy */
		SubStrategies subStrategies;
		/** @brief collects performance result e.g. for probe output */
		float resourceUsage;
	};

} // namespace strategy
} // namespace scheduler
} // namespace wns
#endif

