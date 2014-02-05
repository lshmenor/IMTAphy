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

#ifndef WNS_EVENTS_SCHEDULER_REALTIME_HPP
#define WNS_EVENTS_SCHEDULER_REALTIME_HPP

#include <WNS/events/scheduler/Map.hpp>

#include <sys/time.h>

namespace wns { namespace events { namespace scheduler {
    /**
     * @brief Schedules events in real time (if possible ;))
     *
     * @author Marc Schinnenburg <marc@schinnenburg.com>
     *
     * This version of the EventScheduler will schedule events of a
     * simulation in real time (if possible). This means it delays the
     * execution of events until the simulation is in sync with the real
     * time again. This works of course only, if the simulation can run in
     * real time anyway (this scheduler can't do some magic ;))
     *
     * @note Currently the scheduler will warn if the simulation is more
     * than 10 ms behind the real time. The accuracy of the scheduler has
     * been measured to be in the range of +-10 ms. From "man nanosleep"
     *
     * The current implementation of nanosleep() is based on the normal
     * kernel timer mechanism, which has a resolution of 1/HZ s (see
     * time(7)).  Therefore, nanosleep() pauses always for at least the
     * specified time, however it can take up to 10 ms longer than specified
     * until the process becomes runnable again. For the same reason, the
     * value returned in case of a delivered signal in *rem is usually
     * rounded to the next larger multiple of 1/HZ s.
     *
     * Improvements to lower this are welcome ...
     */
    class RealTime :
        public Map
    {
    public:
        /**
         * @brief Default constructor
         */
        explicit
        RealTime();

        /**
         * @brief Destructor
         */
        virtual
        ~RealTime();

    private:
        /**
         * @brief New sim time (scheduler will wait here if needed ...)
         *
         * This method works as follows
         *
         * @verbatim
                                                              simTime
                 ----------------------------------------------------->
                 |                                     |
                 0                                     nextEvent
                                 |------ delay --------|
                                 |                           realTime
          ---...------------------------------------------------------>
          |      |               |
          epoch  schedulerStart  currentTime @endverbatim
         *
         * Scheduler will sleep for "delay" seconds, where "delay" is:
         * @verbatim
           delay = schedulerStart + nextTime - currentTime @endverbatim
         */
        virtual void
        onNewSimTime(const wns::simulator::Time& nextTime);

        /**
         * @brief Reset scheduler to initial state
         */
        virtual void
        doReset();

        /**
         * @brief Start the scheduler
         */
        virtual void
        doStart();

        /**
         * @brief Helper to convert "timeval" to "double"
         */
        static double
        timevalToDouble(const timeval* t);

        /**
         * @brief Reference point (start has been called there)
         */
        timeval timeOfSchedulerStart_;

        /**
         * @brief In sync with real time (or lagging?)
         */
        bool inSync_;
    };

} // scheduler
} // events
} // wns

#endif  // WNS_EVENTS_SCHEDULER_REALTIME_HPP
