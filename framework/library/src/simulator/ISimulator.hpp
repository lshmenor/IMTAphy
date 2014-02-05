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

#ifndef WNS_SIMULATOR_ISIMULATOR_HPP
#define WNS_SIMULATOR_ISIMULATOR_HPP

#include <WNS/pyconfig/View.hpp>
#include <WNS/container/UntypedRegistry.hpp>
#include <WNS/rng/RNGen.hpp>

#include <boost/signal.hpp>

#include <memory>

namespace wns { namespace events { namespace scheduler {
    class Interface;
}}}

namespace wns { namespace logger {
    class Master;
}}

namespace wns { namespace probe { namespace bus {
    class ProbeBusRegistry;
}}}

namespace wns { namespace simulator {

    typedef boost::signal0<void> ResetSignal;
    typedef boost::signal0<void> ShutdownSignal;
    typedef wns::container::UntypedRegistry<std::string> Registry;

    /**
     * @brief Provides access to simulation wide event scheduler, random number
     * generator, ...
     * @author Marc Schinnenburg <marc@schinnenburg.com>
     * @ingroup group_main_classes
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
     * if you forget one). Instead all the necessary things for a simulation
     * environment are gathered in this class, which is realized itself as a
     * Singleton ...
     */
    class ISimulator
    {
    public:
        /**
         * @brief For sake of completeness
         */
        virtual
        ~ISimulator();

        /**
         * @brief Access the global event scheduler
         */
        wns::events::scheduler::Interface*
        getEventScheduler() const;

        /**
         * @brief Access the global logger
         */
        wns::logger::Master*
        getMasterLogger() const;

        /**
         * @brief Access the global Random Number Generator
         */
        wns::rng::RNGen*
        getRNG() const;

        /**
         * @brief Access the global Configuration
         */
        wns::pyconfig::View
        getConfiguration() const;

        /**
         * @brief Acces the gloabal ProbeBusRegistry
         */
        wns::probe::bus::ProbeBusRegistry*
        getProbeBusRegistry() const;

        /**
         * @brief Acces a container with global variables (should only be used
         * as last resort)
         */
        Registry*
        getRegistry() const;

        /**
         * @brief Register anything that should be reset here
         */
        ResetSignal*
        getResetSignal() const;

        /**
         * @brief Register anything that should be executed on shutdown here
         */
        ShutdownSignal*
        getShutdownSignal() const;

        /**
         * @brief Reset the simulator to its initial state
         */
        void
        reset();

    private:
        /**
         * @brief NVI forward
         */
        virtual wns::events::scheduler::Interface*
        doGetEventScheduler() const = 0;

        /**
         * @brief NVI forward
         */
        virtual wns::logger::Master*
        doGetMasterLogger() const = 0;

        /**
         * @brief NVI forward
         */
        virtual wns::rng::RNGen*
        doGetRNG() const = 0;

        /**
         * @brief NVI forward
         */
        virtual wns::pyconfig::View
        doGetConfiguration() const = 0;

        /**
         * @brief NVI forward
         */
        virtual wns::probe::bus::ProbeBusRegistry*
        doGetProbeBusRegistry() const = 0;

        /**
         * @brief NVI forward
         */
        virtual Registry*
        doGetRegistry() const = 0;

        /**
         * @brief NVI forward
         */
        virtual ResetSignal*
        doGetResetSignal() const = 0;

        /**
         * @brief NVI forward
         */
        virtual ShutdownSignal*
        doGetShutdownSignal() const = 0;

        /**
         * @brief NVI forward
         */
        virtual void
        doReset() = 0;
    };


    /**
     * @brief used to access the global instance of ISimulator
     *
     * @author Marc Schinnenburg <marc@schinnenburg.com>
     */
    class Singleton
    {
    public:
        /**
         * @brief Constructor
         */
        explicit
        Singleton();

        /**
         * @brief Destructor
         */
        virtual
        ~Singleton();

        /**
         * @brief Retrieve the global instance of ISimulator
         * @pre An instance must be set
         */
        ISimulator*
        getInstance();

        /**
         * @brief Retrieve set the global instance of ISimulator
         * @pre No instance must be set
         */
        void
        setInstance(ISimulator* simulator);

        /**
         * @brief Delete instance and set to zero
         */
        void
        shutdownInstance();

    private:
        /**
         * @brief The global instance of ISimulator
         */
        std::auto_ptr<ISimulator> simulator_;
    };

    Singleton&
    getSingleton();

    /**
     * @brief Use this to access the global Simulator object
     */
    ISimulator*
    getInstance();

    /**
     * @brief Provide access to global event scheduler
     */
    wns::events::scheduler::Interface*
    getEventScheduler();

    /**
     * @brief Provide access to global logger
     */
    wns::logger::Master*
    getMasterLogger();

    /**
     * @brief Provide access to global Random Number Generator
     */
    wns::rng::RNGen*
    getRNG();

    /**
     * @brief Provide access to global variables
     */
    wns::probe::bus::ProbeBusRegistry*
    getProbeBusRegistry();

    /**
     * @brief Provide access to global variables
     */
    Registry*
    getRegistry();

    /**
     * @brief Provide access to global Configuration
     */
    wns::pyconfig::View
    getConfiguration();

    /**
     * @brief Provide access to global Resset Signal
     */
    ResetSignal*
    getResetSignal();

    /**
     * @brief Provide access to global Shutdown Signal
     */
    ShutdownSignal*
    getShutdownSignal();

} // namespace simulator
} // namesapce wns


#endif // NOT defined WNS_SIMULATOR_ISIMULATOR_HPP
