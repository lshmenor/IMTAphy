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

#ifndef WNS_PROBE_BUS_TIMESERIESEVAL_HPP
#define WNS_PROBE_BUS_TIMESERIESEVAL_HPP

#include <WNS/probe/bus/ProbeBus.hpp>

namespace wns { namespace probe { namespace bus {

    /**
     * @brief How to format the output of numbers
     */
    enum formatType
    {
		formatFixed, formatScientific
    };

    /**
     * @brief Writes time series of measurements received on a ProbeBus
     */
    class TimeSeriesProbeBus:
        public ProbeBus
    {

        struct LogEntry
        {
            double value;
            wns::simulator::Time time;
            std::vector<std::string> context;
        };

    public:
        TimeSeriesProbeBus(const wns::pyconfig::View&);
        
        virtual ~TimeSeriesProbeBus();

        virtual bool
        accepts(const wns::simulator::Time&, const IContext&);

        virtual void
        onMeasurement(const wns::simulator::Time&,
                      const double&,
                      const IContext&);

        virtual void
        output();

    private:
        /**
         * @brief Container for the logged entries not yet written to persistant storage
         */
        std::list<LogEntry>     logQueue;

        /**
         * @brief Where to write the file
         */
        std::string outputPath;

        /**
         * @brief The filename for persistant storage
         */
        std::string filename;

        /**
         * @brief Flag that shows if it is the first time we write to persistant storage
         */
        bool firstWrite;


        int timePrecision;

        int valuePrecision;

        formatType format;

        /**
         * @brief Name
         */
        std::string name;

        /**
         * @brief Description
         */
        std::string desc;

        /**
         * @brief Comment prefix to be used in output files
         */
        std::string prefix;

        /**
         * @brief Keys of all context values that should be printed
         */
        std::vector<std::string> contextKeys;

    };

} // bus
} // probe
} // wns

#endif // WNS_PROBE_BUS_TIMESERIESEVAL_HPP
