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

#ifndef WNS_SCHEDULER_STRATEGY_APCSTRATEGY_USEMAXTXPOWER_HPP
#define WNS_SCHEDULER_STRATEGY_APCSTRATEGY_USEMAXTXPOWER_HPP

#include <WNS/scheduler/strategy/apcstrategy/APCStrategy.hpp>
#include <vector>

namespace wns { namespace scheduler { namespace strategy { namespace apcstrategy {

	/** @brief Use Maximum TxPower strategy.
	    Give maximum power to each subchannel.
	    Adjust power down, in case the sum exceeds the total power limit. */
	class UseMaxTxPower :
		public APCStrategy
	{
	public:
		UseMaxTxPower(const wns::pyconfig::View& config);
		~UseMaxTxPower();

		/** @brief call this before each timeSlot/frame.
		    Important to reset starting values. */
		virtual void initialize(SchedulerStatePtr schedulerState,
					SchedulingMapPtr schedulingMap);

		/** @brief determine txPower, PhyMode and SINR for request.
		    This is only a suggestion. The allocation/fixation is done outside. */
		virtual APCResult
		doStartAPC(RequestForResource& request,
			   SchedulerStatePtr schedulerState,
			   SchedulingMapPtr schedulingMap);

		/** @brief After all resource scheduling is done, this method is invoked.
		    It changes PowerPerSubchannel so that we don't exceed the total max power. */
		virtual void postProcess(SchedulerStatePtr schedulerState,
					 SchedulingMapPtr schedulingMap);

		bool requiresCQI() const { return false; };
	};


}}}} // namespace wns::scheduler::apcstrategy
#endif // WNS_SCHEDULER_STRATEGY_APCSTRATEGY_USEMAXTXPOWER_HPP

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
