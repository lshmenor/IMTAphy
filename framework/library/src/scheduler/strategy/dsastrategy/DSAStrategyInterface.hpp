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

#ifndef WNS_SCHEDULER_DSASTRATEGY_DSASTRATEGYINTERFACE_HPP
#define WNS_SCHEDULER_DSASTRATEGY_DSASTRATEGYINTERFACE_HPP

#include <WNS/StaticFactory.hpp>
#include <WNS/PyConfigViewCreator.hpp>
#include <WNS/scheduler/RegistryProxyInterface.hpp>
#include <WNS/scheduler/SchedulerTypes.hpp>
#include <WNS/scheduler/strategy/SchedulerState.hpp>
#include <WNS/distribution/Uniform.hpp>
#include <vector>

namespace wns { namespace scheduler { namespace strategy { namespace apastrategy {
	class APAStrategyInterface;
}}}}

namespace wns { namespace scheduler { namespace strategy { namespace dsastrategy {

	const int DSAsubChannelNotFound = -1;

	struct DSAResult
	{
		DSAResult() {subChannel=DSAsubChannelNotFound; timeSlot=0; spatialLayer=0;};
		~DSAResult() {};
		/** @brief result of Dynamic Subcarrier Assignment (DSA) */
		int subChannel;
		/** @brief result of Dynamic Subcarrier Assignment (DSA) */
		int timeSlot;
		/** @brief result of Dynamic Subcarrier Assignment (DSA) */
		int spatialLayer;

        bool
        operator<(const DSAResult& other) const
        {
            if(subChannel != other.subChannel)
                return subChannel < other.subChannel;
            else if(timeSlot != other.timeSlot)
                return timeSlot < other.timeSlot;
            else    
                return spatialLayer < other.spatialLayer;
        };
	};

	/** @brief provides methods for "Dynamic Subchannel Assignment" (DSA).
	    Used by scheduler strategies for during doAdaptiveResourceScheduling()
	 */
	class DSAStrategyInterface
	{
	public:
		virtual ~DSAStrategyInterface() {};

		/** @brief called once at the beginning */
		virtual void setColleagues(RegistryProxyInterface* _registry) = 0;

		/** @brief call this before each timeSlot/frame.
		    Important to reset starting values. */
		virtual void initialize(SchedulerStatePtr schedulerState,
					SchedulingMapPtr schedulingMap) = 0;

		/** @brief determine subChannel for request.
		    This is only a suggestion. The allocation/fixation is done outside. */
		virtual DSAResult
		getSubChannelWithDSA(RequestForResource& request,
				     SchedulerStatePtr schedulerState,
				     SchedulingMapPtr schedulingMap) = 0;

		/** @brief true if the strategy must have CQI information.
		    E.g. for getBestSubChannel based on CQI */
		virtual bool requiresCQI() const = 0;
	};

	typedef wns::PyConfigViewCreator<DSAStrategyInterface, DSAStrategyInterface> DSAStrategyCreator;
	typedef wns::StaticFactory<DSAStrategyCreator> DSAStrategyFactory;

}}}} // namespace wns::scheduler::strategy::dsastrategy
#endif // WNS_SCHEDULER_DSASTRATEGY_DSASTRATEGYINTERFACE_HPP

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
