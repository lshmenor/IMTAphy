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

#ifndef WNS_PROBE_BUS_TEST_PROBEBUSSTUB_HPP
#define WNS_PROBE_BUS_TEST_PROBEBUSSTUB_HPP

#include <WNS/probe/bus/ProbeBus.hpp>

namespace wns { namespace probe { namespace bus { namespace tests {

    /**
     * @brief ProbeBusStub. ProbeBus for testing
     * @author Daniel Bültmann <me@daniel-bueltmann.de>
     */
    class ProbeBusStub:
        public wns::probe::bus::ProbeBus
    {
    public:
        ProbeBusStub();

        virtual ~ProbeBusStub();

        virtual void
        onMeasurement(const wns::simulator::Time&,
                      const double&,
                      const wns::probe::bus::IContext&);

        virtual void
        output();

        virtual bool
        accepts(const wns::simulator::Time&, const IContext&);

        virtual void
        setFilter(std::string, long int);

        int receivedCounter;

        std::vector<double> receivedTimestamps;

        std::vector<double> receivedValues;

        std::string providerName;

        long int filter;

        std::string lastContext;

    };
} // tests
} // bus
} // probe
} // wns

#endif //WNS_PROBE_TEST_PROBEBUSSTUB_HPP
