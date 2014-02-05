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

#ifndef IMTAPHY_SCM_PHASES
#define IMTAPHY_SCM_PHASES


// For opt versions disable range checking in the multi_array wrapper
// WNS_NDEBUG means: assures are disabled
#ifdef  WNS_NDEBUG
#define BOOST_DISABLE_ASSERTS
#endif
#include <boost/multi_array.hpp>


#include <itpp/itbase.h>

namespace imtaphy { namespace scm { namespace m2135 {

            typedef boost::multi_array_ref<double, 1> Double1DArray;
            typedef boost::multi_array_ref<double, 3> Double3DArray;

            class Phases {
            private:
                unsigned int K;
                unsigned int N; 
                unsigned int M;
                itpp::vec vvVector; // K x N x M
                itpp::vec vhVector; // K x N x M
                itpp::vec hvVector; // K x N x M
                itpp::vec hhVector; // K x N x M
                itpp::vec LOSvvVector; // K
                itpp::vec LOShhVector; // K
                
                
                
        
            public:
                Double3DArray vv;
                Double3DArray vh;
                Double3DArray hv;
                Double3DArray hh;
                Double1DArray LOSvv;
                Double1DArray LOShh;
                
                Phases(unsigned int K_, unsigned int N_, unsigned int M_) :
                    K(K_),
                    N(N_),
                    M(M_),
                    vvVector(K * N * M),
                    vhVector(K * N * M),
                    hvVector(K * N * M),
                    hhVector(K * N * M),
                    LOSvvVector(K),
                    LOShhVector(K),             
                    vv(vvVector._data(), boost::extents[K][N][M]),
                    vh(vhVector._data(), boost::extents[K][N][M]),
                    hv(hvVector._data(), boost::extents[K][N][M]),
                    hh(hhVector._data(), boost::extents[K][N][M]),
                    LOSvv(LOSvvVector._data(), boost::extents[K]),
                    LOShh(LOShhVector._data(), boost::extents[K])
                
                {

                }
                
                void initPhasesUniformlyRandomly(double lower, double upper)
                {
                    vvVector = lower + (upper - lower) * itpp::randu(K * N * M);
                    vhVector = lower + (upper - lower) * itpp::randu(K * N * M);
                    hvVector = lower + (upper - lower) * itpp::randu(K * N * M);
                    hhVector = lower + (upper - lower) * itpp::randu(K * N * M);
                    LOSvvVector = lower + (upper - lower) * itpp::randu(K);
                    LOShhVector = lower + (upper - lower) * itpp::randu(K);
                }

        
            };

        }}}

#endif
