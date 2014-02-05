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

#ifndef WNS_PROBE_BUS_LOGGINGPROBEBUS_HPP
#define WNS_PROBE_BUS_LOGGINGPROBEBUS_HPP

#include <WNS/probe/bus/ProbeBus.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>

namespace wns { namespace probe { namespace bus {

    /**
     * @brief A logger for the ProbeBus
     *
     * Attach the LoggingProbeBus to another ProbeBus to see the Measurements
     * that pass that ProbeBus. The measurements are printed to std::out.
     * Use this for debugging purposes.
     *
     * @author Daniel Bültmann <me@daniel-bueltmann.de>
     * @ingroup probebusses
     */
    class LoggingProbeBus:
        public ProbeBus
    {
    public:
        LoggingProbeBus(const wns::pyconfig::View&);

        virtual ~LoggingProbeBus();

        virtual void
        onMeasurement(const wns::simulator::Time&,
                      const double&, const IContext&);

        virtual bool
        accepts(const wns::simulator::Time&, const IContext&);

        virtual void
        output();

    private:
        wns::logger::Logger logger_;
    };
} // bus
} // probe
} // wns
#endif //WNS_PROBE_BUS_LOGGINGPROBEBUS_HPP
