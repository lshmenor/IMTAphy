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

#ifndef SPATIALCHANNELMODELINTERFACE_HPP
#define SPATIALCHANNELMODELINTERFACE_HPP

#include <WNS/PowerRatio.hpp>
#include <WNS/pyconfig/View.hpp>
#include <IMTAPHY/lsParams/LargeScaleParameters.hpp>
#include <IMTAPHY/detail/LinearAlgebra.hpp>
#include <boost/shared_ptr.hpp>
#include <IMTAPHY/Spectrum.hpp>

namespace imtaphy { 
    class StationPhy;
    class Channel;
    class LinkManager;
    class Link;
    
    namespace scm {
        
        struct ChannelLayout
        {
            unsigned int K; // links
            unsigned int N; // clusters (paths)
            unsigned int U; // max. number of receive antennas
            unsigned int S; // max. number of transmit antennas
            unsigned int F[2]; // number of frequency bins in UL/DL
        };

//        typedef boost::multi_array_ref<std::complex<double>, 4> ComplexDouble4DArray;

        template <typename PRECISION>
        class SpatialChannelModelInterface
        {
        public:
            SpatialChannelModelInterface(Channel* channel, wns::pyconfig::View config) {};
            
            /**
             * @brief To be called by TheChannel after all stations have registered, use for initialization 
             */
            virtual void onWorldCreated(LinkManager* linkManager, lsparams::LSmap* lsParams, bool keepIntermediates) = 0;
            
            virtual scm::ChannelLayout getChannelLayout() = 0;
            
            virtual std::complex<PRECISION> getCurrentCIR(unsigned int k, // SCM link ID
                                                        imtaphy::Direction d, // Uplink or Downlink   
                                                        unsigned int u, // Rx antenna ID
                                                        unsigned int s, // Tx antenna ID
                                                        int n  // cluster/path ID
                ) = 0;

            virtual std::complex<PRECISION> getCurrentCTF( unsigned int k, // SCM link ID
                                                           imtaphy::Direction d, // Uplink or Downlink
                                                           unsigned int u, // Rx antenna ID
                                                           unsigned int s, // Tx antenna ID
                                                           unsigned int f  // frequency bin (PRB) index
                ) = 0;

            virtual boost::shared_ptr<imtaphy::detail::MKLMatrix<std::complex<PRECISION> > >
            getChannelMatrix(imtaphy::Link*,
                             imtaphy::Direction d, // Uplink or Downlink
                             unsigned int f  // frequency bin (PRB) index
                            ) = 0;
            
            virtual void evolve(double t) = 0;
        };
        
    }}
#endif
