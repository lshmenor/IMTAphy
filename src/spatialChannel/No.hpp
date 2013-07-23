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

#ifndef SPATIALCHANNELMODELINTERFACE_NO_HPP
#define SPATIALCHANNELMODELINTERFACE_NO_HPP

#include <WNS/PowerRatio.hpp>
#include <WNS/pyconfig/View.hpp>

#include <IMTAPHY/spatialChannel/SpatialChannelModelInterface.hpp>
#include <IMTAPHY/Link.hpp>
#include <IMTAPHY/detail/LinearAlgebra.hpp>

namespace imtaphy { 
    class StationPhy;
    class Channel;
    
    namespace scm {

        template <typename PRECISION>
        class No:
                public SpatialChannelModelInterface<PRECISION>
        {
        public:
            No(Channel* channel, wns::pyconfig::View config) :
                imtaphy::scm::SpatialChannelModelInterface<float>(channel, config)
            {
                emptyLayout.F[imtaphy::Uplink] = 0;
                emptyLayout.F[imtaphy::Downlink] = 0;
                emptyLayout.K = 0;
                emptyLayout.N = 0;
                emptyLayout.U = 0;
                emptyLayout.S = 0;
            }
            
            
            void onWorldCreated(LinkManager* linkManager, lsparams::LSmap* lsParams, bool keepIntermediates){}
            
            scm::ChannelLayout getChannelLayout() {return emptyLayout;}
            
            std::complex<PRECISION> getCurrentCIR( unsigned int k, // SCM link ID
                                                   imtaphy::Direction d, // Uplink or Downlink
                                                   unsigned int u, // Rx antenna ID
                                                   unsigned int s, // Tx antenna ID
                                                   int n  // cluster/path ID
                )
            {
                // only the first "cluster" has a contribution
    
                if (n == 0)
                    return std::complex<PRECISION>(1.0, 0.0);
                else
                    return std::complex<PRECISION>(0.0, 0.0);
            }

            std::complex<PRECISION> getCurrentCTF( unsigned int k, // SCM link ID
                                                   imtaphy::Direction d, // Uplink or Downlink
                                                   unsigned int u, // Rx antenna ID
                                                   unsigned int s, // Tx antenna ID
                                                   unsigned int f  // frequency bin (PRB) index
                )
            {
                return std::complex<PRECISION>(1.0, 0.0);
            }

            boost::shared_ptr<imtaphy::detail::MKLMatrix<std::complex<PRECISION> > >
            getChannelMatrix(imtaphy::Link* link,
                             imtaphy::Direction direction,        
                             unsigned int f)  // frequency bin (PRB) index
            {
                unsigned int u = link->getMS()->getAntenna()->getNumberOfElements();
                unsigned int s = link->getBS()->getAntenna()->getNumberOfElements();

                imtaphy::detail::MKLMatrix<std::complex<PRECISION> >* matrix;
                if (direction == imtaphy::Downlink)
                {
                     matrix = new imtaphy::detail::MKLMatrix<std::complex<PRECISION> >(u, s);
                }
                else // uplink
                {
                     matrix = new imtaphy::detail::MKLMatrix<std::complex<PRECISION> >(s, u);
                }
                // simply return a static ones matrix

                for (unsigned int i = 0; i < u; i++)
                    for (unsigned int j = 0; j < s; j++)
                    {
                        if (direction == imtaphy::Downlink)
                        {
                            (*matrix)[i][j] = std::complex<PRECISION>(1.0 / sqrt(link->getWidebandLoss().get_factor()), 0.0);
                        }
                        else // uplink
                        {
                            (*matrix)[j][i] = std::complex<PRECISION>(1.0 / sqrt(link->getWidebandLoss().get_factor()), 0.0);
                        }
                    }
                    
                return boost::shared_ptr<imtaphy::detail::MKLMatrix<std::complex<PRECISION> > >(matrix);
            }
            void evolve(double t) {};
            
        private:
            ChannelLayout emptyLayout;
        };
        
    }}
#endif
