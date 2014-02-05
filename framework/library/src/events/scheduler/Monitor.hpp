/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 16, D-52074 Aachen, Germany
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

#ifndef WNS_EVENTS_SCHEDULER_MONITOR_HPP
#define WNS_EVENTS_SCHEDULER_MONITOR_HPP

#include <WNS/pyconfig/View.hpp>
#include <WNS/Observer.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/events/scheduler/INotification.hpp>

namespace wns { namespace events { namespace scheduler {

    /**
     * @brief Collects some statistics about the event scheduler and logs to
     * master logger
     *
     * @author Marc Schinnenburg <marc@schinnenburg.com>
     */
    class Monitor :
        public wns::Observer<INotification>
    {
    public:
        explicit
        Monitor(const wns::pyconfig::View&);

        void
        logStatistics();

    private:
        virtual void
        doOnProcessOneEvent();

        virtual void
        doOnCancelEvent();

        virtual void
        doOnSchedule();

        virtual void
        doOnScheduleDelay();

        virtual void
        doOnScheduleNow();

        virtual void
        doOnAddEvent();

        wns::logger::Logger logger_;
        long long int processedEvents_;
        long long int canceledEvents_;
        long long int scheduledNowEvents_;
        long long int scheduledEvents_;
        long long int scheduledDelayEvents_;
    };

} // scheduler
} // events
} // wns


#endif // NOT defined WNS_EVENT_SCHEDULER_MONITOR_HPP
