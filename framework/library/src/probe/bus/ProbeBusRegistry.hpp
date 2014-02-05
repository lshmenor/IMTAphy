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

#ifndef WNS_PROBE_BUS_PROBEBUSREGISTRY_HPP
#define WNS_PROBE_BUS_PROBEBUSREGISTRY_HPP

#include <WNS/probe/bus/ProbeBus.hpp>

#include <WNS/container/Registry.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>

namespace wns { namespace probe { namespace bus {

    /**
     * @brief The ProbeBusRegistry resolves a ProbeBus name to a PassThroughProbeBus
     * which may then be used as the entry point for measurements to the ProbeBus system.
     *
     * @author Daniel Bueltmann <me@daniel-bueltmann.de>
     */
    class ProbeBusRegistry :
        public IOutputStreamable
    {
    public:

        typedef wns::container::Registry<std::string, ProbeBus*, wns::container::registry::DeleteOnErase> ProbeBusRegistryContainer;

        typedef std::list<ProbeBus*> CreatedProbeBussesContainer;

        /**
         * @brief Constructs the registry and uses the default MasterLogger.
         */
        ProbeBusRegistry(const wns::pyconfig::View&);

        /**
         * @brief Constructs the registry and uses a cusom masterLogger.
         */
        ProbeBusRegistry(const wns::pyconfig::View&, wns::logger::Master*);

        /**
         * @brief Empty (Does NOT delete any ProbeBusses)
         */
        virtual
        ~ProbeBusRegistry();

        /**
         * @brief Reset the registry. Primarily needed for testig.
         */
        void
        reset();

        /**
         * @brief Retrieves an instance of the PassThroughProbeBus. 
         *
         * If the given name is asked for the first time the ProbeBusRegistry
         * creates a PassThroughProbeBus. Subsequent requests for the
         * same MasterProbeBus then always get the same instance. In this way
         * measurement sources and measurement sinks are decoupled.
         */
        ProbeBus*
        getMeasurementSource(const std::string&);

        /**
         * @brief All known ProbeBusses are triggered to write their results to
         * persistent storage.
         */
        void
        forwardOutput();

        /**
         * @brief Reads the own list of ProbeBus trees and connects these recursively
         * to the ProbeBusses in this registry
         */
        void
        startup();

        /**
         * @brief Reads the configured measurement sources then spawns and connects
         * all observing ProbeBusses recursively to the MasteProbeBus for that source.
         */
        void
        spawnProbeBusses(const wns::pyconfig::View& probeBusTrees);

    private:

        /**
         * @brief Used to dump the contents of ProbeBusRegistry.
         */
        std::string
        doToString() const;

        /**
         * @brief Reads the configured measurement sources then spawns and connects
         * all observing ProbeBusses recursively to the subject for that source.
         */
        void
        spawnObservers(ProbeBus* subject, const wns::pyconfig::View& config);

        wns::pyconfig::View pyco_;

        ProbeBusRegistryContainer registry_;

        wns::logger::Logger logger_;

        CreatedProbeBussesContainer createdProbeBusses_;
    };
} // bus
} // probe
} // wns

#endif //WNS_PROBE_BUS_PROBEBUSREGISTRY_HPP
