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

#ifndef WNS_PROBE_BUS_STATEVALPROBEBUS_HPP
#define WNS_PROBE_BUS_STATEVALPROBEBUS_HPP

#include <WNS/probe/bus/ProbeBus.hpp>
#include <WNS/evaluation/statistics/stateval.hpp>
#include <WNS/pyconfig/View.hpp>

class StatEval;

namespace wns { namespace probe { namespace bus {

    /**
     * @brief Wrap StatEval Objects and attach them to a ProbeBus.
     *
     * The StatEvalProbeBus accepts values for a single ProbeID.
     * Measurements that are received for the ProbID are put to the StatEval
     * object. output triggers writing of the probe.
     *
     * @author Daniel Bültmann <dbn@comnets.rwth-aachen.de>
     * @ingroup probebusses
     */
    class StatEvalProbeBus :
        public wns::probe::bus::ProbeBus
    {
    public:
        StatEvalProbeBus(const wns::pyconfig::View&);

        virtual ~StatEvalProbeBus();

        virtual void
        onMeasurement(const wns::simulator::Time&,
                      const double&,
                      const IContext&);

        virtual bool
        accepts(const wns::simulator::Time&, const IContext&);

        virtual void
        output();

    private:

        std::string outputPath;

        std::string filename;

        bool appendFlag;

        bool firstWrite;

        wns::evaluation::statistics::StatEvalInterface* statEval;
    };
} // bus
} // probe
} // wns
#endif // WNS_PROBE_BUS_STATEVALPROBEBUS_HPP
