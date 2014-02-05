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

#ifndef WNS_EVENTS_CANTIMEOUT_HPP
#define WNS_EVENTS_CANTIMEOUT_HPP

#include <WNS/events/scheduler/IEvent.hpp>


namespace wns { namespace events {

    namespace scheduler 
    {
        class Interface;
    }

    /**
     * @brief Mixin to support classes that need a simple timeout mechanism.
     *
     * To make use of this class, simply derive from it and overload the
     * onTimeout method.
     */
    class CanTimeout
    {
        /**
         * @brief Used internally by CanTimeout
         */
        class TimeoutEvent
        {
        public:
            /**
             * @brief Initialized with target to call onTimeout for
             */
            explicit
            TimeoutEvent(CanTimeout* _target);

            /**
             * @brief Destructor
             */
            virtual
            ~TimeoutEvent();

            /**
             * @brief EventScheduler calls this when processing this
             */
            virtual void
            operator()();

            virtual void
            print(std::ostream& aStreamRef = std::cout) const;

        private:
            /**
             * @brief Target to call onTimeout for
             */
            CanTimeout* target;
        };

    public:
        /**
         * @brief Start with no timer set
         */
        CanTimeout();

        /**
         * @brief Cancels timer if one is active
         */
        virtual
        ~CanTimeout();

        /**
         * @name Operational Methods
         */
        //{@
        /**
         * @brief Set the timer.
         *
         * Set the timer to a relative time in seconds. When the time has
         * elapsed, the method onTimeout gets called. The method onTimeout
         * has to be implemented by the deriver.
         * <p>
         * At any time there is only one valid timer. Hence, if the timer has
         * been set already an exception will be raised.
         */
        void
        setTimeout(double delay);

        /**
         * @brief Set the timer.
         *
         * Set the timer to a relative time in seconds. When the time has
         * elapsed, the method onTimeout gets called. The method onTimeout
         * has to be implemented by the deriver.
         * <p>
         * At any time there is only one valid timer. If the timer has been set
         * already, it will be canceled silently. Use this function only if you
         * are sure that you need this behaivior otherwise use the function
         * setTimeout(...) to avoid probable error concealment.
         */
        void
        setNewTimeout(double delay);

        /**
         * @brief Is a timer set?
         */
        bool
        hasTimeoutSet() const;

        /**
         * @brief Cancel the timer.
         *
         * Cancel a previously set timer. If no timer has been set
         * previously, an exeption will be raised.
         */
        void
        cancelTimeout();
        //@}

        /**
         * @brief Your callback. Implement this.
         *
         * The deriver is forced to implement this method. It gets called,
         * whenever the timer fires.
         */
        virtual void
        onTimeout() = 0;

    private:
        /**
         * @brief The current active Event
         *
         * NULL if no timer is active
         */
        scheduler::IEventPtr event;

        /**
         * @brief Have scheduler at hand
         */
        wns::events::scheduler::Interface* scheduler;
    }; // CanTimeout

} // events
} // wns

#endif // NOT defined WNS_EVENTS_CANTIMEOUT_HPP

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
