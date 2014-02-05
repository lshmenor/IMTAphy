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

#ifndef WNS_PERIODICTIMEOUT_HPP
#define WNS_PERIODICTIMEOUT_HPP

#include <WNS/Assure.hpp>
#include <WNS/simulator/Time.hpp>
#include <WNS/events/scheduler/IEvent.hpp>

namespace wns { namespace events {

    /**
     * @brief Mixin to support classes that need a periodic timeout mechanism.
     *
     * To make use of this class, simply derive from it and overload the
     * periodically() method.
     *
     */
    class PeriodicTimeout
    {
        class PeriodicTimeoutFunctor
        {
        public:
            PeriodicTimeoutFunctor(PeriodicTimeout* _dest, wns::simulator::Time _period);

            virtual void
            operator()();

            virtual wns::simulator::Time
            getPeriod() const;

            virtual void
            print(std::ostream& aStreamRef = std::cout) const;

            virtual ~PeriodicTimeoutFunctor()
            {
            }

            PeriodicTimeoutFunctor(const PeriodicTimeoutFunctor& other):
                period_(other.period_),
                dest_(other.dest_)
            {
            }
        private:
            wns::simulator::Time period_;
            PeriodicTimeout* dest_;

            friend class PeriodicTimeout;
        };

    public:
        /**
         * @brief Constructor
         */
        explicit
        PeriodicTimeout();

        /**
         * @brief Destructor
         */
        virtual
        ~PeriodicTimeout();

        /**
         * @brief Special copy constructor
         */
        PeriodicTimeout(const PeriodicTimeout& other);

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
        startPeriodicTimeout(wns::simulator::Time _period, wns::simulator::Time delay = 0.0);

        /**
         * @brief Is a timer set?
         *
         */
        bool
        hasPeriodicTimeoutSet() const;

        /**
         * @brief Cancel the timer.
         *
         * Cancel a previously set timer. Silently ignore, whether the timer has
         * not been set.
         */
        void
        cancelPeriodicTimeout();

        /**
         * @brief Your callback. Implement this!
         *
         * The deriver is forced to implement this method. It gets called periodically,
         * whenever the timer fires.
         */
        virtual void
        periodically() = 0;

    private:
        wns::simulator::Time period_;
        scheduler::IEventPtr periodicEv_;

        const PeriodicTimeout& operator=(const PeriodicTimeout& other);
    }; // PeriodicTimeout

} // events
} // wns
#endif // NOT defined WNS_PERIODICTIMEOUT_HPP


