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

#ifndef WNS_RNG_RNGEN_HPP
#define WNS_RNG_RNGEN_HPP

#include <boost/random.hpp>

namespace wns { namespace rng {

    // This typedef may be reaplced with a wrapper that allows to choose
    // different RNGs at configuration time (run time)
    typedef boost::mt19937 RNGen;

    template <typename DISTRIBUTION, typename ENGINE = RNGen*>
    class VariateGenerator :
        public boost::variate_generator<ENGINE, DISTRIBUTION>
    {
    public:
        VariateGenerator(ENGINE engine, DISTRIBUTION distribution) :
            boost::variate_generator<ENGINE, DISTRIBUTION>(engine, distribution)
        {
        }
    };

} // rng
} // wns

#endif  // WNS_RNG_RNGEN_HPP

