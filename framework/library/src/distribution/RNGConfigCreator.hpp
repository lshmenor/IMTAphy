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

#ifndef WNS_DISTRIBUTION_RNGCONFIGCREATOR_HPP
#define WNS_DISTRIBUTION_RNGCONFIGCREATOR_HPP

#include <WNS/StaticFactory.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/rng/RNGen.hpp>

namespace wns { namespace distribution {
    /**
     * @brief Creator implementation to be used with StaticFactory.
     *
     * Useful for constructors with a specific RNG and pyconfig::View parameter.
     *
     */
    template <typename T, typename KIND = T>
    struct RNGConfigCreator :
        public RNGConfigCreator<KIND, KIND>
    {
        virtual KIND* create(wns::rng::RNGen* rng, const wns::pyconfig::View& config)
        {
            return new T(rng, config);
        }
    };

    template <typename KIND>
    struct RNGConfigCreator<KIND, KIND> //:
    // public wns::Creator<KIND, KIND>
    {
    public:
        virtual KIND* create(wns::rng::RNGen*, const wns::pyconfig::View&) = 0;

        virtual ~RNGConfigCreator()
        {
        }
    };

}
}


#endif // NOT defined WNS_DISTRIBUTION_RNGCONFIGCREATOR_HPP

