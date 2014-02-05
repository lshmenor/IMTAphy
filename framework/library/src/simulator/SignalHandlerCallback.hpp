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

#ifndef WNS_SIMULATOR_SIGNALHANDLERCALLBACK_HPP
#define WNS_SIMULATOR_SIGNALHANDLERCALLBACK_HPP

#include <csignal>

namespace wns { namespace simulator {

    /**
     * @brief Base class for SignalHandler callbacks
     *
     * @author Marc Schinnenburg <marc@schinnenburg.com>
     *
     * Although you can register anything callable (with operator()()), you can
     * use this class for your convience, when wrtiting such a call back. It
     * implemnents two methods and leaves one method for customization up to
     * you.
     */
    class SignalHandlerCallback
    {
    public:
        /**
         * @brief Default c'tor - sets wasCalled_ to 0
         */
        SignalHandlerCallback();

        /**
         * @brief Virtual d'tor
         */
        virtual
        ~SignalHandlerCallback();

        /**
         * @brief Called by signal handler
         *
         * If this method was called, wasCalled_ will be set to 1. Afterwards
         * doCall() is called;
         */
        void
        operator()();

    protected:
        sig_atomic_t wasCalled_;

    private:
        virtual void
        doCall() = 0;

    };

} // namespace simulator
} // namespace wns

#endif // NOT defined WNS_SIMULATOR_SIGNALHANDLERCALLBACK_HPP

