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

#ifndef WNS_PROBE_BUS_TABLEPROBEBUS_HPP
#define WNS_PROBE_BUS_TABLEPROBEBUS_HPP

#include <WNS/probe/bus/ProbeBus.hpp>
#include <WNS/probe/bus/detail/StatEvalTable.hpp>
#include <WNS/pyconfig/View.hpp>

namespace wns { namespace probe { namespace bus {

    /**
     * @brief ProbeBus derivate that can create Table output
     *
     * @ingroup probebusses
     */
    class TableProbeBus:
        public wns::probe::bus::ProbeBus
    {
    public:
        TableProbeBus(const wns::pyconfig::View& pyco);

        virtual ~TableProbeBus();

        //@{
        /** @name Implementation of wns::probe::bus::ProbeBus Interface */
        virtual void
        onMeasurement(const wns::simulator::Time&,
                      const double& aValue,
                      const IContext& reg);

        virtual bool
        accepts(const wns::simulator::Time&,
                const IContext& reg);

        virtual void
        output();
        //@}

    private:
        /** @brief array of Sorters, one for each dimension of the Table*/
        std::vector<detail::Sorter> sorters;
        /** @brief array of strings, describing what to evaluate, e.g. "mean",
		 * "variance", "trials", etc. */
        std::vector<std::string> evals;
        /** @brief array of strings identifying the desired output formats */
        std::vector<std::string> formats;

        /** @brief path for the output files */
        std::string outputPath;
        /** @brief basename to construct the output file names */
        std::string outfileBase;

        /** @brief The datastructure in which the actual evaluation is stored. */
        detail::StatEvalTable* t;
    };
} // bus
} // probe
} // wns

#endif //WNS_PROBE_BUS_TABLEPROBEBUS_HPP
