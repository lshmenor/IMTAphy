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

#ifndef WNS_EVENTS_SCHEDULER_IEVENT_HPP
#define WNS_EVENTS_SCHEDULER_IEVENT_HPP

#include <WNS/RefCountable.hpp>
#include <WNS/simulator/Time.hpp>
#include <WNS/SmartPtr.hpp>
#include <WNS/events/scheduler/Callable.hpp>
#include <WNS/PythonicOutput.hpp>

namespace wns { namespace events { namespace scheduler {

    /**
     * @brief Base class for all scheduler events.
     * @ingroup group_main_classes
     *
     * The IEvent is the base class for all event types that are
     * used in all WNS scheduler.
     */
    class IEvent :
    virtual public wns::RefCountable,
        virtual public wns::PythonicOutput
    {
    public:
        /**
         * @brief If an event can be canceled this exception should be thrown by
         * the event scheduler
         *
         * @author Marc Schinnenburg <marc@schinnenburg.com>
         *
         * An event may not be canceled if it is
         *
         *  - currently running
         *  - already canceled
         *  - already executed
         *  - not submitted to the scheduler at all (which should not happen)
         */
        class CancelException :
            wns::Exception
        {
        public:
            CancelException(const std::string& s);
            ~CancelException() throw() {}
        };

        virtual
        ~IEvent();

        virtual wns::simulator::Time
        getScheduled() const = 0;

        virtual wns::simulator::Time
        getIssued() const = 0;

        /**
         * @brief Prefer this over wns::events::scheduler::Interface::cancelEvent()
         *
         * This is likely to perform a little better, because we
         * a) do not need to check if the event is NULL and
         * b) we save a dynamic cast
         * c) can call the specialized method of scheduler directly
         */
        virtual void
        cancel() = 0;

        /**
         * @brief Return true, if the Event is not submitted to the queue so
         * far (initial state)
         */
        virtual bool
        isNotSubmitted() const = 0;

        /**
         * @brief Return true if the Event is queued (waiting for execution)
         */
        virtual bool
        isQueued() const = 0;

        /**
         * @brief Return true if the Event is currently being executed
         */
        virtual bool
        isRunning() const = 0;

        /**
         * @brief Return true if the Event was executed (no longer queued, no
         * longer running)
         */
        virtual bool
        isFinished() const = 0;

        /**
         * @brief Return true if the Event was canceled
         */
        virtual bool
        isCanceled() const = 0;

    protected:
        virtual std::string
        doToString() const;
    };

    typedef wns::SmartPtr<IEvent> IEventPtr;

}
}
}
#endif // NOT defined WNS_EVENTS_SCHEDULER_IEVENT_HPP
