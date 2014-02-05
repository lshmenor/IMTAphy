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

#ifndef WNS_SIMULATOR_CPUTIMEEXHAUSTEDHANDLER_HPP
#define WNS_SIMULATOR_CPUTIMEEXHAUSTEDHANDLER_HPP

#include <WNS/simulator/SignalHandlerCallback.hpp>
#include <WNS/events/scheduler/INotification.hpp>
#include <WNS/Observer.hpp>

#include <csignal>

namespace wns { namespace events { namespace scheduler {
    class Interface;
}}}

namespace wns { namespace simulator {

    /**
     * @brief Performs a graceful shutdown
     *
     * @author Marc Schinnenburg <marc@schinnenburg.com>
     *
     * Sends wns::events::scheduler::Interface::stop(). This signal is sent by
     * SGE (Sun Grid Engine) if the soft limit of any resorce is reached.
     */
    class CPUTimeExhaustedHandler :
        public SignalHandlerCallback,
        public Observer<wns::events::scheduler::INotification>,
        public wns::events::scheduler::IgnoreAllNotifications
    {
    public:
        CPUTimeExhaustedHandler(
            wns::events::scheduler::Interface* eventScheduler,
            int signum);

    private:
        /**
         * @brief Called by wns::simulator::SignalHandler
         */
        virtual void
        doCall();

        /**
         * @brief Called from wns::events::scheduler::Interface
         *
         * Checks after each event if a graceful shutdown needs to be performed.
         */
        virtual void
        doOnProcessOneEvent();

        /**
         * @brief The event scheduler we're observing for onNextEvent
         * notifications
         */
        wns::events::scheduler::Interface* eventScheduler_;

        /**
         * @brief The signal number this handler listens to
         */
        int signum_;

        /**
         * @brief The signalmask we need to block and unblock the signal
         */
        sigset_t signalMask_;
    };

} // simulator
} // wns

#endif // NOT defined WNS_SIMULATOR_CPUTIMEEXHAUSTEDHANDLER_HPP
