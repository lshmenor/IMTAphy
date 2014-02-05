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

#ifndef WNS_SIMULATOR_SIMULATOR_HPP
#define WNS_SIMULATOR_SIMULATOR_HPP

#include <WNS/simulator/ISimulator.hpp>
#include <WNS/pyconfig/View.hpp>

#include <memory>

namespace wns { namespace simulator {

    /**
     * @brief Provides access to simulation wide event scheduler, random number
     * generator, ...
     * @author Marc Schinnenburg <marc@schinnenburg.com>
     *
     * There is an indispensable need to access one global instance of a certain
     * type of variable within a simulator. Well known examples are the
     * EventScheduler and the RandomNumberGenerator. Others, like a data base to
     * store measured values can be easily thought of. Typically the Singleton
     * pattern is applied to this problem (see "Gamma, Design Patterns" for more
     * information). This approach has considerable drawbacks related to
     * testing. For each test a fresh and clean environment (consisting of
     * wns::events::scheduler::Scheduler, wns::RandomNumberGenerator, ...) is
     * needed. With the approach of one Singleton for each of these
     * variables, you need to reset each global variable (which is error prone,
     * if you forgets one). Instead all the necessary things for a simulation
     * environment a gathered in this class, which is realized itself as
     * Singleton ...
     */
    class Simulator :
        public ISimulator
    {
    public:
        /**
         * @brief PyConfig Constructor
         */
        explicit
        Simulator(const pyconfig::View& configuration);

        /**
         * @brief For sake of completeness
         */
        virtual
        ~Simulator();

    protected:
        /**
         * @brief helper to setup Random Number Generator
         */
        virtual void
        configureRNG(const pyconfig::View& rngConfiguration);

    private:
        /**
         * @brief Copy constructor is forbidden
         */
        Simulator(const Simulator&);

        /**
         * @brief NVI forward
         */
        virtual wns::events::scheduler::Interface*
        doGetEventScheduler() const;

        /**
         * @brief NVI forward
         */
        virtual wns::logger::Master*
        doGetMasterLogger() const;

        /**
         * @brief NVI forward
         */
        virtual wns::rng::RNGen*
        doGetRNG() const;

        /**
         * @brief NVI forward
         */
        virtual Registry*
        doGetRegistry() const;

        /**
         * @brief NVI forward
         */
        virtual wns::probe::bus::ProbeBusRegistry*
        doGetProbeBusRegistry() const;

        /**
         * @brief NVI forward
         */
        virtual ResetSignal*
        doGetResetSignal() const;

        /**
         * @brief NVI forward
         */
        virtual ShutdownSignal*
        doGetShutdownSignal() const;

        /**
         * @brief NVI forward
         */
        virtual wns::pyconfig::View
        doGetConfiguration() const;

        /**
         * @brief NVI forward
         */
        virtual void
        doReset();

        /**
         * @brief helper to setup EventScheduler
         */
        void
        configureEventScheduler(const pyconfig::View& eventSchedulerConfiguration);

        /**
         * @brief helper to setup Master Logger
         */
        void
        configureMasterLogger(const pyconfig::View& masterLoggerConfiguration);

        /**
         * @brief helper to setup ProbeBusRegistry
         */
        void
        configureProbeBusRegistry(const pyconfig::View& pbrConfiguration);

        /**
         * @brief Gloabal Configuration
         */
        wns::pyconfig::View configuration_;

        /**
         * @brief EventScheduler instance
         */
        std::auto_ptr<wns::events::scheduler::Interface> eventScheduler_;

        /**
         * @brief Master logger instance
         */
        std::auto_ptr<wns::logger::Master> masterLogger_;

        /**
         * @brief Random number generator instance
         */
        std::auto_ptr<wns::rng::RNGen> rng_;

        /**
         * @brief GlobalUntypedRegistry instance
         */
        std::auto_ptr<Registry> registry_;

        /**
         * @brief ProbeBusRegistry instance
         */
        std::auto_ptr<wns::probe::bus::ProbeBusRegistry> probeBusRegistry_;

        /**
         * @brief Container with reset signals
         */
        std::auto_ptr<ResetSignal> resetSignal_;

        /**
         * @brief Container with shutdown signals
         */
        std::auto_ptr<ShutdownSignal> shutdownSignal_;
    };

} // namespace simulator
} // namesapce wns


#endif // NOT defined WNS_SIMULATOR_SIMULATOR_HPP
