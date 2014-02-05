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

#ifndef IMTAPHY_DETAIL_HASHRNG_HPP
#define IMTAPHY_DETAIL_HASHRNG_HPP

#include <WNS/Position.hpp>
#include <boost/random.hpp>


namespace imtaphy { namespace detail {

        /**
         * @brief Helper that can be used as pseudo-random number generator for
         * boost distributions. This may only be used to draw one random number
         * from the distribution. DO NOT USE as a random number generator!!!
         *
         * @author Daniel Bueltmann <openwns@doender.de>
         */
        class HashRNG
        {
        public:
            HashRNG(unsigned int initialSeed,
                    wns::Position bs,
                    wns::Position ms,
                    bool correlateBS,
                    bool correlateUT);

            static const bool has_fixed_range = true;

            double
            operator()();

            double
            min()
            {
                return 0.0;
            }

            double
            max()
            {
                return 1.0;
            }

            typedef double result_type;
            
        private:
            template<typename T>
            void combine( unsigned int& hash, T t)
            {
                unsigned int* it= (unsigned int*)(&t);

                assure(sizeof(T) % sizeof(unsigned int) == 0, "Incompatible hash types in HashRNG::combineDouble");

                int count = sizeof(T) / sizeof(unsigned int);

                for (int ii=0; ii < count; ++ii)
                {
                    // DJB Hash function
                    hash = ((hash << 5) + hash) + *it;
                    it++;
                }
            }

            boost::mt19937 rng;
            boost::uniform_real<> uni;
            boost::variate_generator<boost::mt19937&, boost::uniform_real<> > dis;

            unsigned int myHash;
        };

    } // detail
} // imtaphy

#endif 
