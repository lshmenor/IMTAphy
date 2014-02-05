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

#ifndef WNS_PROBE_BUS_TEXTPROBEBUS_HPP
#define WNS_PROBE_BUS_TEXTPROBEBUS_HPP

#include <WNS/probe/bus/ProbeBus.hpp>
#include <WNS/pyconfig/View.hpp>

#include <fstream>

namespace wns { namespace probe { namespace bus {

    /**
     * @brief ProbeBus derivate that wraps a ProbeText StatEval
     * @ingroup probebusses
     */
    class TextProbeBus:
        public wns::probe::bus::ProbeBus
    {
    public:
        TextProbeBus(const wns::pyconfig::View& pyco);

        virtual ~TextProbeBus();

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

        void
        printText(std::ostream& theStream);

        void
        putText(std::string message);

    private:
		/** @brief Key to listen for */
		std::string key_;
		/** @brief path for the output files */
        std::string outputPath_;
		/** @brief basename to construct the output file names */
        std::string outfileBase_;
		/** @brief Wether to write the file header*/
        bool writeHeader_;

        bool isJSON_;

        bool prependSimTimeFlag_;

        int simTimePrecision_;

        int simTimeWidth_;

        std::string name_;

        std::string description_;

        std::list<std::string> messages_;

        int skipInterval_;

        int numCalls_;

        std::string jsonEntrySeparator_;
    };
} // bus
} // probe
} // wns

#endif //WNS_PROBE_BUS_TEXTPROBEBUS_HPP
