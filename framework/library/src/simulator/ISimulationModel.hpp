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

#ifndef WNS_SIMULATOR_ISIMULATIONMODEL_HPP
#define WNS_SIMULATOR_ISIMULATIONMODEL_HPP

#include <WNS/StaticFactory.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/PyConfigViewCreator.hpp>

namespace wns { namespace simulator {

    /**
     * @author Marc Schinnenburg <marc@schinnenburg.com>
     * @ingroup group_main_classes
     */
    class ISimulationModel
    {
    public:
        /**
         * @brief No interface without virtual d'tor
         */
        virtual
        ~ISimulationModel();

        /**
         * @brief Called after the simulation framework (event scheduler) is
         * setup
         */
        void
        startup();

        /**
         * @brief Called after the simulation has finished
         */
        void
        shutdown();

    private:
        /**
         * @brief NVI
         */
        virtual void
        doStartup() = 0;

        /**
         * @brief NVI
         */
        virtual void
        doShutdown() = 0;
    };

    typedef wns::PyConfigViewCreator<ISimulationModel> ISimulationModelCreator;
    typedef wns::StaticFactory<ISimulationModelCreator> ISimulationModelFactory;
}
}


#endif // NOT defined WNS_SIMULATOR_ISIMULATIONMODEL_HPP
