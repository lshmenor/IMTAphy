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

#ifndef WNS_SIMULATOR_UNITTESTS_HPP
#define WNS_SIMULATOR_UNITTESTS_HPP

#include <WNS/simulator/Simulator.hpp>

#include <sstream>

namespace wns { namespace simulator {

    /**
     * @brief Simulator used if testing is enabled
     * @author Marc Schinnenburg <marc@schinnenburg.com>
     *
     * This type of Simulator provides a valid reset method
     */
    class UnitTests :
        public Simulator
    {
    public:
        /**
         * @brief PyConfig Constructor
         */
        explicit
        UnitTests(const pyconfig::View& configuration);

        /**
         * @brief For sake of completeness
         */
        virtual
        ~UnitTests();

    private:
        /**
         * @brief Copy constructor is forbidden
         */
        UnitTests(const UnitTests&);

        /**
         * @brief NVI
         */
        virtual void
        doReset();

        /**
         * @brief Overridden here to store the initial state
         */
        virtual void
        configureRNG(const wns::pyconfig::View& config);

        /**
         * @brief Has its own Random number generator instance
         */
        std::stringstream initialRNGState_;
    };

} // namespace simulator
} // namesapce wns


#endif // NOT defined WNS_SIMULATOR_UNITTESTS_HPP
