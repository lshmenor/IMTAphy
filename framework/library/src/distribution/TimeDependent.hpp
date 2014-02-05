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

#ifndef WNS_DISTRIBUTION_TIMEDEPENDENT_HPP
#define WNS_DISTRIBUTION_TIMEDEPENDENT_HPP

#include <WNS/pyconfig/View.hpp>
#include <WNS/distribution/Distribution.hpp>

#include <map>

namespace wns { namespace distribution {

    /**
     * @brief Configurable Distrubitions over time
     * @author Marc Schinnenburg <msg@comnets.rwth-aachen.de>
     *
     * This class can be configured to change it's distribution over the
     * time.
     *
     * @intern The class works as follows: It reads a list of Events from the
     * pyconfig::View. The Events will be queued into the EventContainer
     * "events". Each event holds a configuration for a distribution. At the time
     * the event will be executed, it will set a new distribution in this class.
     *
     * Here are two examples. One for configuration:
     * @include "TimeDependent::config.example"
     *
     * This will create values like this:
     * @code
     *
     *    |
     *    |
     * 47-|----|
     *    |    |
     *    |    |
     * 15-|    |                                 |---------------------
     *    |    |                                 |
     * 11-|    |---------|                       |
     *    |    |         |                       |
     * 8 -|    |         |-----------------------|
     * ___|____|_________|_______________________|_____________________
     *    |    |         |                       |                   (t)
     *         2         5                       11
     *
     * @endcode
     *
     * Usage in C++:
     * @include TimeDependent::usage.example
     */
    class TimeDependent :
        virtual public wns::distribution::Distribution
    {
        /**
         * @brief Carries the View to create new Distribution from
         */
        class DistributionEvent
        {
        public:
            DistributionEvent(TimeDependent* td, const wns::pyconfig::View& view);

            /**
             * @brief Queue new event and set new distribution function
             */
            virtual void
            operator()();

            const wns::pyconfig::View&
            getConfig()
            {
                return config_;
            }

            virtual
            ~DistributionEvent();

        private:
            /**
             * @brief New distribution will be set here
             */
            TimeDependent* target_;

            /**
             * @brief The configuration for the new distribution
             */
            wns::pyconfig::View config_;
        };

        typedef std::map<simTimeType, DistributionEvent> EventContainer;

    public:
        /**
         * @brief pyconfig::View Constructor
         */
        explicit
        TimeDependent(const wns::pyconfig::View& view);

        /**
         * @brief pyconfig::View Constructor with RNGen
         */
        explicit
        TimeDependent(wns::rng::RNGen* rng, const wns::pyconfig::View& view);

        /**
         * @brief Destructor
         */
        virtual
        ~TimeDependent();

        /**
         * @brief Draw value from current Distribution
         */
        virtual double
        operator()();

        virtual std::string
        paramString() const;

    private:
        /**
         * @brief Queue Event to create next Distribution (if events left)
         */
        void
        queueNextDistribution();

        /**
         * @brief Set new distribution according to pyconfig::View
         */
        void
        setDistribution(const wns::pyconfig::View& distConfig);

        /**
         * @brief Deletes a Distribution and sets pointer to NULL
         */
        void
        removeDistribution();

        /**
         * @brief Current distribution
         */
        Distribution* distribution_;

        void
        init();

        /**
         * @brief Stores events with new distributions
         */
        EventContainer events_;

        wns::pyconfig::View config_;

    }; // TimeDependent

} // distribution
} // wns


#endif // WNS_DISTRIBUTION_TIMEDEPENDENT_HPP


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
