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

#ifndef WNS_EVENTS_PERIODICREALTIMEOUT_HPP
#define WNS_EVENTS_PERIODICREALTIMEOUT_HPP

#include <WNS/events/scheduler/ICommand.hpp>

#include <pthread.h>

namespace wns { namespace events {

    /**
     * @brief Mixin to support classes that need a periodic timeout mechanism.
     *
     * To make use of this class, simply derive from it and overload the
     * periodically() method.
     *
     * Times are given in realtime. If you are looking for periodic simulation time
     * events, look at PeriodicTimeout instead.
     *
     */
    class PeriodicRealTimeout
    {
        /**
         * @brief Command to be queued for real time execution
         */
        class PeriodicRealTimeoutCommand
        {
        public:
            explicit
            PeriodicRealTimeoutCommand(PeriodicRealTimeout* _dest) :
                dest(_dest)
            {
            }

            void
            operator()()
            {
                dest->periodically();
            }

        private:
            PeriodicRealTimeout* dest;
        }; // PeriodicRealTimeoutCommand

    public:
        explicit
        PeriodicRealTimeout();

        virtual
        ~PeriodicRealTimeout();

        /**
         * @brief Start the periodic timer.
         *
         * Starts the timer with the given period in seconds. The first timeout is delayed
         * by the given value (default 0). When the period has
         * elapsed, the method periodically() is called. The method periodically()
         * has to be implemented by the deriver.
         * <p>
         * If the timer has been set before, it will be silently cancelled.
         * At any time there is only one valid timer.
         */
        void
        startPeriodicTimeout(double _period, double _delay = 0.0);

        /**
         * @brief Is a timer set?
         *
         */
        bool
        hasPeriodicRealTimeoutSet();

        /**
         * @brief Cancel the timer.
         *
         * Cancel a previously set timer. Silently ignore, whether the timer has
         * not been set.
         */
        void
        cancelPeriodicRealTimeout();

        /**
         * @brief Your callback. Implement this!
         *
         * The deriver is forced to implement this method. It gets called periodically,
         * whenever the timer fires.
         */
        virtual void
        periodically() = 0;

    private:
        pthread_cond_t dataRead;
        pthread_mutex_t mutex;
        double period;
        double delay;


        bool running;
        pthread_t thread;
        wns::events::scheduler::ICommandPtr currentCommand;

        static void* worker(void* _arg);
    }; // PeriodicRealTimeOut

} // events
} // wns


#endif // NOT defined WNS_EVENTS_PERIODICREALTIMEOUT_HPP


