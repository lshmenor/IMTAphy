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

#ifndef WNS_SIMULATOR_SIGNALHANDLER_HPP
#define WNS_SIMULATOR_SIGNALHANDLER_HPP

#include <WNS/container/Registry.hpp>
#include <WNS/Singleton.hpp>
#include <WNS/NonCopyable.hpp>

#include <boost/signal.hpp>

#include <memory>
#include <string>
#include <csignal>

namespace wns { namespace simulator {

    /**
     * @brief An special signal handler for openWNS
     * @author Marc Schinnenburg <marc@schinnenburg.com>
     */
    class SignalHandler :
        private NonCopyable
    {
        friend class wns::DefaultCreation<wns::simulator::SignalHandler>;
        typedef boost::signal0<void> Handler;
        typedef wns::container::Registry<int, Handler*, wns::container::registry::DeleteOnErase> Map;
    public:
        /**
         * @brief Add a signal handler
         *
         * @pre A signal with this name may not be registered before!
         *
         */
        template <typename HANDLER>
        void
        addSignalHandler(int signum, HANDLER handler)
        {
            // block all signals until the handler is added
            sigset_t allSignals;
            sigfillset(&allSignals);
            sigprocmask(SIG_BLOCK, &allSignals, NULL);

            // will throw if there is already a handler registered with this
            // number
            // use std::auto_ptr to avoid leak if registry throws
            std::auto_ptr<Handler> localHandler(new Handler());
            map_.insert(signum, localHandler.get());
            // if everything went fine (no throw) release the pointer since the
            // registry now cares about the pointer
            localHandler.release();

            map_.find(signum)->disconnect_all_slots();
            map_.find(signum)->connect(handler);
            struct sigaction action;
            sigfillset (&action.sa_mask);
            action.sa_flags = 0;
            action.sa_handler = SignalHandler::catchSignal;
            sigaction(signum, &action, NULL);
            // Unblock all signals
            sigprocmask(SIG_UNBLOCK, &allSignals, NULL);
        }

        /**
         * @brief Remove signal according signal number, installs default
         * signal handler again.
         */
        void
        removeSignalHandler(int signum);

        /**
         * @brief Remove all registered handlers and restore default handler
         */
        void
        removeAllSignalHandlers();

    private:
        // Callable by SingletonHolder only
        /**
         * @brief Default c'tor
         */
        SignalHandler();

        /**
         * @brief Remove all registered handlers and restore default handler
         */
        virtual
        ~SignalHandler();

        /**
         * @brief Adapter - Dispatch the C-style signals to the respective
         * handler in C++
         */
        static void
        catchSignal(int signum);

        /**
         * @brief Map the signals to the signal handlers
         */
        Map map_;
    };

    /**
     * @brief Access the one and only SignalHandler via:
     * wns::simulator::GlobalSignalHandler::getInstance()
     */
    typedef wns::SingletonHolder<wns::simulator::SignalHandler, 
            wns::DefaultCreation,
            wns::AtApplicationExit> GlobalSignalHandler;

} // namespace simulator
} // namespace wns

#endif // NOT defined WNS_SIMULATOR_SIGNALHANDLER_HPP

