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

#ifndef WNS_SCHEDULER_STRATEGY_APCSTRATEGY_APCSTRATEGY_HPP
#define WNS_SCHEDULER_STRATEGY_APCSTRATEGY_APCSTRATEGY_HPP

#include <WNS/scheduler/strategy/apcstrategy/APCStrategyInterface.hpp>
#include <WNS/scheduler/strategy/SchedulerState.hpp>
#include <WNS/scheduler/SchedulerTypes.hpp>
#include <WNS/scheduler/RegistryProxyInterface.hpp>
#include <WNS/service/phy/phymode/PhyModeInterface.hpp>
#include <WNS/logger/Logger.hpp>
#include <vector>
#include <string>

namespace wns { namespace scheduler { namespace strategy { namespace apcstrategy {
	/** @brief provides methods for "Adaptive Power Control" (APC).
	    Used by scheduler strategies for during doAdaptiveResourceScheduling()
	 */
	class APCStrategy :
	    virtual public APCStrategyInterface
	{
	public:
		APCStrategy(const wns::pyconfig::View& config);
		virtual ~APCStrategy();

		/** @brief called once at the beginning */
		virtual void setColleagues(RegistryProxyInterface* _registry);

		/** @brief call this before each timeSlot/frame.
		    Important to reset starting values. */
		virtual void initialize(SchedulerStatePtr schedulerState,
					SchedulingMapPtr schedulingMap);

		/** @brief After all resource scheduling is done,
		    this method is always invoked.
		    It can change the PhyModes and PowerPerSubchannel (=> CompoundDurations)
		    so that we maybe can save more power.
		    Default implementation is empty. Overload in derived classes if necessary. */
		virtual void postProcess(SchedulerStatePtr schedulerState,
					 SchedulingMapPtr schedulingMap);

	protected:
		wns::logger::Logger logger;

		/** @brief the phyModeMapper can calculate PhyMode from SINR and back */
		wns::service::phy::phymode::PhyModeMapperInterface* phyModeMapper;
		struct Colleagues {
			Colleagues() {registry=NULL;};
			RegistryProxyInterface* registry;
		} colleagues;

		/** @brief These parameters maxSummedPowerOnAllChannels, maxPowerPerSubChannel, nominalPowerPerSubChannel
		    are assumed constant for this scheduler.
		    This is good and ok if all users are the same (default of all systems),
		    but if they are different (future), APC must call schedulerState->strategy->getPowerCapabilities(request.user)
		 */
		/** @brief strategy name of derived class */
		std::string apcstrategyName;
	};

}}}} // namespace wns::scheduler::strategy::apcstrategy
#endif // WNS_SCHEDULER_APCSTRATEGY_APCSTRATEGY_HPP

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
