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

#ifndef WNS_PROBE_BUS_PYTHONPROBEBUS_HPP
#define WNS_PROBE_BUS_PYTHONPROBEBUS_HPP

#include <WNS/probe/bus/ProbeBus.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/pyconfig/Object.hpp>

namespace wns { namespace probe { namespace bus {

    /**
     * @brief Filter Measurements by calling back a PythonObject
     *
     * Use this ProbeBus for rapid deployment if individual filter rules.
     * The wns::pyconfig::View passed to the constructor must contain a
     * callable named accepts. This callable must accept two parameters. The
     * first is the ProbeID which is mapped to a Python string and the second
     * is a Dict of Python strings to Python ints. Based on this information
     * the Python method must return wether the PythonProbeBus accepts the
     * measurement or not.
     *
     * PythonProbeBus does not do anything in its onMeasurement method. It is
     * simply a gate that determines wether measurements are forwarded to its
     * Observers or not.
     *
     * If you only want to evaluate measurements for certain IDs plug the
     * PythonProbeBus between the MasterProbeBus and a StatEvalProbeBus and
     * provide the filtering criteria from within your configuration file.
     *
     * @author Daniel Bültmann <me@daniel-bueltmann.de>
     * @todo dbn: Documentation is outdated. Rewrite.
     * @ingroup probebusses
     */
    class PythonProbeBus:
        public wns::probe::bus::ProbeBus
    {
    public:
        PythonProbeBus(const wns::pyconfig::View& pyco);

        virtual ~PythonProbeBus();

        virtual void
        onMeasurement(const wns::simulator::Time&,
                      const double&,
                      const IContext&);

        virtual bool
        accepts(const wns::simulator::Time&, const IContext&);

        virtual void
        output();

    private:

        void
        showdown(const std::string& reason) const;

        wns::pyconfig::View pyco_;

        wns::pyconfig::Object pyAcceptsMethod_;

        wns::pyconfig::Object pyOnMeasurementMethod_;

        wns::pyconfig::Object pyOutputMethod_;

        bool reportErrors_;
    };
} // bus
} // probe
} // wns

#endif //WNS_PROBE_BUS_PYTHONPROBEBUS_HPP
