/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
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

#ifndef WNS_EVENTS_MULTIPLETIMEOUT_HPP
#define WNS_EVENTS_MULTIPLETIMEOUT_HPP

#include <WNS/events/scheduler/IEvent.hpp>

#include <WNS/simulator/ISimulator.hpp>
#include <WNS/events/scheduler/Interface.hpp>

#include <WNS/Assure.hpp>

#include <map>

namespace wns { namespace events {

    /**
     * @brief Mixin to support classes that need a simple timeout mechanism
     * for each instance of a set of object instances of type T.
     *
     * To make use of this class, simply derive from it and overload the
     * onTimeout(const T& t) method.
     */
    template <typename T>
    class MultipleTimeout
    {
        typedef MultipleTimeout<T> MultipleTimeoutConcretion;
        typedef std::map<T, wns::events::scheduler::IEventPtr> EventMap;

        friend class TimeoutEvent;

        class TimeoutEvent
        {
        public:
            TimeoutEvent(MultipleTimeoutConcretion* _dest, const T& _t) :
                dest(_dest),
                t(_t)
            {
            } // TimeoutEvent

            virtual void operator()()
            {
                assure(dest->hasTimeoutSet(t), "Invalid MultipleTimeout event.");
                typename EventMap::iterator evIt = dest->events.find(t);
                assure(evIt != dest->events.end(), "Queued event does not exist in MultipleTimeout event list.");
                dest->events.erase(evIt);
                dest->onTimeout(t);
            } // execute


            virtual
            ~TimeoutEvent()
            {
            }

        private:
            MultipleTimeoutConcretion* dest;
            T t;

            friend class MultipleTimeout<T>;
        };

    public:
        MultipleTimeout() :
            events()
        {
        } // MultipleTimeout

        virtual
        ~MultipleTimeout()
        {
            cancelAllTimeouts();
        } // ~MultipleTimeout

        /**
         * @brief Set the timer for object instance t.
         *
         * Set the timer to a relative time in seconds. When the time has
         * elapsed, the method onTimeout(T t) gets called. The method onTimeout
         * has to be implemented by the deriver.
         * <p>
         * At any time there can be only one valid timer per object
         * instance t. Hence, if the timer for object instance t has been
         * set already an exception will be raised.
         */
        void
        setTimeout(const T& t, double delay)
        {
            assure(!hasTimeoutSet(t), "A timer for this object instance has been set already.");
            TimeoutEvent toEvent = TimeoutEvent(this, t);
            scheduler::IEventPtr ev = wns::simulator::getEventScheduler()->scheduleDelay(toEvent, delay);
            events.insert(std::make_pair(t, ev));
        } // setTimeout

        /**
         * @brief Set the timer for object instance t.
         *
         * Set the timer to a relative time in seconds. When the time has
         * elapsed, the method onTimeout(T t) gets called. The method onTimeout
         * has to be implemented by the deriver.
         * <p>
         * At any time there can be only one valid timer per object
         * instance t. If the timer for object instance t has been set
         * already, it will be canceled silently. Use this function only if you
         * are sure that you need this behaivior otherwise use the function
         * setTimeout(...) to avoid probable error concealment.
         */
        void
        setNewTimeout(const T& t, double delay)
        {
            if (hasTimeoutSet(t))
                cancelTimeout(t);

                setTimeout(t, delay);
        } // setNewTimeout

        /**
         * @brief Is a timer set for object instance t?
         */
        bool
        hasTimeoutSet(const T& t) const
        {
            return events.find(t) != events.end();
        } // hasTimeoutSet

        /**
         * @brief How many timers for objects of type T are set?
         */
        size_t
        numberOfTimeoutsSet() const
        {
            return events.size();
        } // numberOfTimeoutsSet

        /**
         * @brief Cancel the timer for object instance t.
         *
         * Cancel a previously set timer for object instance t.
         */
        void
        cancelTimeout(const T& t)
        {
            assure(hasTimeoutSet(t), "No timer has been set for this object instance.");

            typename EventMap::iterator evIt = events.find(t);
            assure(evIt != events.end(), "Event does not exist in MultipleTimeout event list.");
            wns::simulator::getEventScheduler()->cancelEvent(evIt->second);
            events.erase(evIt);
        } // cancelTimeout

        /**
         * @brief Cancel all timers set for objects of type T.
         */
        void
        cancelAllTimeouts()
        {
            while(!events.empty())
            {
                wns::simulator::getEventScheduler()->cancelEvent(events.begin()->second);
                events.erase(events.begin());
            }
        } // cancelAllTimeouts

        /**
         * @brief Your callback. Implement this.
         *
         * The deriver is forced to implement this method. It gets called
         * with object instance t as parameter whose timer has fired.
         */
        virtual void
        onTimeout(const T& t) = 0;

    private:
        EventMap events;
    };

} // events
} // wns

#endif // WNS_EVENTS_MULTIPLETIMEOUT_HPP


