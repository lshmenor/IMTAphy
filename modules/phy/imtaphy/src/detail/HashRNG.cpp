/*******************************************************************************
 * This file is part of IMTAphy
 * _____________________________________________________________________________
 *
 * Copyright (C) 2010
 * Institute of Communication Networks (LKN)
 * Department of Electrical Engineering and Information Technology (EE & IT)
 * Technische Universitaet Muenchen
 * Arcisstr. 21
 * 80333 Muenchen - Germany
 * http://www.lkn.ei.tum.de/~jan/imtaphy/index.html
 * 
 * _____________________________________________________________________________
 *
 *   IMTAphy is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   IMTAphy is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with IMTAphy.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
// based on the original implementation:
/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 5, D-52074 Aachen, Germany
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

#include <IMTAPHY/detail/HashRNG.hpp>
#include <WNS/distribution/Uniform.hpp>

#include <boost/random.hpp>
#include <boost/functional/hash.hpp>
#include <sstream>

using namespace imtaphy::detail;

HashRNG::HashRNG(unsigned int initialSeed,
                 wns::Position bs,
                 wns::Position ms,
                 bool correlateBS, bool correlateUT):
    uni(0.0, 1.0),
    dis(rng, uni),
    myHash(5381)
{
    // First take care of the initial seed
    combine(myHash, initialSeed);


    if (correlateBS && correlateUT)
    {
        // Feed to seed, but such that swapping p1 and p2 yields the same results
        // channel is reciprocal
        double xMax = std::max(bs.getX(), ms.getX());
        double xMin = std::min(bs.getX(), ms.getX());
        double yMax = std::max(bs.getY(), ms.getY());
        double yMin = std::min(bs.getY(), ms.getY());
        double zMax = std::max(bs.getZ(), ms.getZ());
        double zMin = std::min(bs.getZ(), ms.getZ());

        combine(myHash, xMax);
        combine(myHash, xMin);
        combine(myHash, yMax);
        combine(myHash, yMin);
        combine(myHash, zMax);
        combine(myHash, zMin);
    }
    else if (correlateBS)
    {
        combine(myHash, bs.getX());
        combine(myHash, bs.getY());
        combine(myHash, bs.getZ());
    }
    else if (correlateUT)
    {
        combine(myHash, ms.getX());
        combine(myHash, ms.getY());
        combine(myHash, ms.getZ());
    }
    else
    {
        assure(false, "Unsupported case in HashRNG. Correlate the RNG either to BS, UT or to both!");
    }

    rng.seed(myHash);
}

double
HashRNG::operator()()
{
    return dis();
}
