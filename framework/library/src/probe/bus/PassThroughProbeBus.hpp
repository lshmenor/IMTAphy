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

#ifndef WNS_PROBE_BUS_PASSTHROUGHPROBEBUS_HPP
#define WNS_PROBE_BUS_PASSTHROUGHPROBEBUS_HPP

#include <WNS/probe/bus/ProbeBus.hpp>
#include <WNS/pyconfig/View.hpp>

namespace wns { namespace probe { namespace bus {
    /**
     * @brief The PassThroughProbeBus publishes all Measurements available.
     *
     * If you want to receive messages implement the ProbeBus Interface and
     * use the startObserving method on the PassThroughProbeBus to receive
     * measurements. You may also use existing general purpose implementations
     * already available.
     *
     * @author Daniel Bültmann <me@daniel-bueltmann.de>
     * @ingroup probebusses
     */
    class PassThroughProbeBus:
        virtual public ProbeBus
    {
    public:

        PassThroughProbeBus();

        PassThroughProbeBus(const wns::pyconfig::View&);

        virtual ~PassThroughProbeBus() {}

        virtual bool
        accepts(const wns::simulator::Time&, const IContext&);

        virtual void
        onMeasurement(const wns::simulator::Time&,
                      const double&,
                      const IContext&);

        virtual void
        output();

    };
} // bus
} // probe
} // wns

#endif // WNS_PROBE_BUS_PASSTHROUGHPROBEBUS_HPP
