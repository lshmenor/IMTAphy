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

#ifndef WNS_PROBE_BUS_DETAIL_IPROBEBUSNOTIFICATION_HPP
#define WNS_PROBE_BUS_DETAIL_IPROBEBUSNOTIFICATION_HPP

#include <WNS/simulator/Time.hpp>
#include <WNS/probe/bus/Context.hpp>

namespace wns { namespace probe { namespace bus { namespace detail {

    /**
     * @brief Internal Interface for Subject/Observer implementation which is
     * used as backend for the ProbeBus
     *
     * @author Daniel Bueltmann <me@daniel-bueltmann.de>
     */
    class IProbeBusNotification
    {
    public:
        virtual ~IProbeBusNotification() {};

        /**
         * @brief Send a value on this ProbeBus
         * @param probeId The ProbeID of the associated probe
         * @param aValue The value to send (this was formerly the argument to
         * probe->put)
         * @param idRegistry The set of IDs known in this scope. You may sort
         * according to these.
         * @note The Context is deleted after the send call. Do not store it!
         *
         * This method is used to forward measurements. Call this if you
         * generate a new measurement and want it to propagate through
         * the ProbeBus Hierarchy.
         */
        virtual void
        forwardMeasurement(const wns::simulator::Time&,
                           const double&,
                           const IContext&) = 0;

        /**
         * @brief Trigger writing of output.
         */
        virtual void
        forwardOutput() = 0;
    };

} 
}
}
}

#endif // WNS_PROBE_BUS_DETAIL_IPROBEBUSNOTIFICATION_HPP
