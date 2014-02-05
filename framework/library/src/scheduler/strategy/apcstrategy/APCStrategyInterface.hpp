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

#ifndef WNS_SCHEDULER_STRATEGY_APCSTRATEGY_APCSTRATEGYINTERFACE_HPP
#define WNS_SCHEDULER_STRATEGY_APCSTRATEGY_APCSTRATEGYINTERFACE_HPP

#include <WNS/StaticFactory.hpp>
#include <WNS/PyConfigViewCreator.hpp>
#include <WNS/scheduler/RegistryProxyInterface.hpp>
#include <WNS/scheduler/SchedulerTypes.hpp>
#include <WNS/scheduler/strategy/SchedulerState.hpp>
#include <vector>
#include <string>

namespace wns { namespace scheduler { namespace strategy { namespace apcstrategy {
	struct APCResult
	{
		APCResult() {};
		~APCResult() {};
		/** @brief result of Adaptive Power Control (APC) */
		wns::Power txPower;
		/** @brief phyMode for the SINR we assumed */
		wns::service::phy::phymode::PhyModeInterfacePtr phyModePtr;
		/** @brief SINR we assumed */
		wns::Ratio sinr;
		/** @brief SINR we assumed (alternative interface) */
		ChannelQualityOnOneSubChannel estimatedCandI;
	};

	/** @brief provides methods for "Adaptive Power Control" (APC).
	    Used by scheduler strategies for during doAdaptiveResourceScheduling()
	 */
	class APCStrategyInterface
	{
	public:
		virtual ~APCStrategyInterface() {};

		/** @brief called once at the beginning */
		virtual void setColleagues(RegistryProxyInterface* _registry) = 0;

		/** @brief call this before each timeSlot/frame.
		    Important to reset starting values. */
		virtual void initialize(SchedulerStatePtr schedulerState,
					SchedulingMapPtr schedulingMap) = 0;

		/** @brief determine txPower, PhyMode and SINR for request.
		    This is only a suggestion. The allocation/fixation is done outside. */
		virtual APCResult
		doStartAPC(RequestForResource& request,
			   SchedulerStatePtr schedulerState,
			   SchedulingMapPtr schedulingMap) = 0;

		/** @brief After all resource scheduling is done,
		    this method is always invoked.
		    It can change the PhyModes and PowerPerSubchannel (=> CompoundDurations)
		    so that we maybe can save more power. */
		virtual void postProcess(SchedulerStatePtr schedulerState,
					 SchedulingMapPtr schedulingMap) = 0;

		/** @brief true if the strategy must have CQI information.
		    E.g. for detailed calculations based on pathLoss */
		virtual bool requiresCQI() const = 0;
	};

	typedef wns::PyConfigViewCreator<APCStrategyInterface, APCStrategyInterface> APCStrategyCreator;
	typedef wns::StaticFactory<APCStrategyCreator> APCStrategyFactory;

}}}} // namespace wns::scheduler::strategy::apcstrategy
#endif // WNS_SCHEDULER_APCSTRATEGY_APCSTRATEGYINTERFACE_HPP

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
