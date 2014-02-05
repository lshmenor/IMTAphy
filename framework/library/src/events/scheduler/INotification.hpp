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

#ifndef WNS_EVENTS_SCHEDULER_INOTIFICATION_HPP
#define WNS_EVENTS_SCHEDULER_INOTIFICATION_HPP

#include <WNS/ObserverInterface.hpp>
#include <WNS/simulator/Time.hpp>

namespace wns { namespace events { namespace scheduler {

    /**
     * @brief Notification interface for wns::events::scheduler::Interface
     *
     * @author Marc Schinnenburg <marc@schinnenburg.com>
     */
    class INotification
    {
    public:
        virtual
        ~INotification();

        void
        onProcessOneEvent();

        void
        onCancelEvent();

        void
        onSchedule();

        void
        onScheduleDelay();

        void
        onScheduleNow();

        void
        onAddEvent();

    private:
        /**
         * @brief Called before an event is processed
         */
        virtual void
        doOnProcessOneEvent() = 0;

        /**
         * @brief Called when an event is being cancled (at this point in time
         * it is NOT canceled!)
         */
        virtual void
        doOnCancelEvent() = 0;

        /**
         * @brief Called when an event is being scheduled (at this point in time
         * it is NOT already part of the scheduler!)
         */
        virtual void
        doOnSchedule() = 0;

        /**
         * @brief Called when an event is being scheduled with a delay (at this
         * point in time it is NOT already part of the scheduler!)
         */
        virtual void
        doOnScheduleDelay() = 0;
        /**
         * @brief Called when an event is being scheduled for "now" (at this
         * point in time it is NOT already part of the scheduler!)
         */
        virtual void
        doOnScheduleNow() = 0;

        /**
         * @brief Called when an event is being added (combines the calls to
         * onSchedule, onScheduledDelay and onScheduleNow)
         */
        virtual void
        doOnAddEvent() = 0;

    };

    typedef ObserverInterface<INotification> IObserver;

    /**
     * @brief This class ignores all notifiactions
     *
     * @author Marc Schinnenburg <marc@schinnenburg.com>
     *
     * This class can be used if only one notification of INotification should
     * be observed. Then, you don't need to implement the other implementations
     */
    class IgnoreAllNotifications :
        virtual public INotification
    {
        virtual void
        doOnProcessOneEvent()
        {
        }

        virtual void
        doOnCancelEvent()
        {
        }

        virtual void
        doOnSchedule()
        {
        }

        virtual void
        doOnScheduleDelay()
        {
        }

        virtual void
        doOnScheduleNow()
        {
        }

        virtual void
        doOnAddEvent()
        {
        }
    };

} // scheduler
} // events
} // wns

#endif // NOT defined WNS_EVENTS_SCHEDULER_INOTIFICATION_HPP
