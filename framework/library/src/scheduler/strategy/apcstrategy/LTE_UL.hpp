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

#ifndef WNS_SCHEDULER_STRATEGY_APCSTRATEGY_LTEUL_HPP
#define WNS_SCHEDULER_STRATEGY_APCSTRATEGY_LTEUL_HPP

#include <WNS/scheduler/strategy/apcstrategy/APCStrategy.hpp>

namespace wns { namespace scheduler { namespace strategy { namespace apcstrategy {

    /** @brief LTE_UL complient uplink power control.
    See 3GPP Technical Specification 36.213 Chapter 5.1 for details */
    class LTE_UL :
        public APCStrategy
    {
    public:
        LTE_UL(const wns::pyconfig::View& config);
        ~LTE_UL();

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
    private:
        wns::Power pNull_;
        double alpha_;
        wns::Ratio sinrMargin_;
        int minimumPhyMode_;
    };

}}}} // namespace wns::scheduler::apcstrategy
#endif // WNS_SCHEDULER_STRATEGY_APCSTRATEGY_LTEULE_HPP



