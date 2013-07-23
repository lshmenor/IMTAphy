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

#ifndef SPATIALCHANNEL_DELAYS_HPP
#define SPATIALCHANNEL_DELAYS_HPP

#include <IMTAPHY/Channel.hpp>
#include <IMTAPHY/spatialChannel/m2135/FixPar.hpp>
#include <itpp/itbase.h>
#include <IMTAPHY/Link.hpp>


namespace imtaphy { namespace scm { namespace m2135 {

            class Delays
            {
            public:
                
                Delays(itpp::mat* _sigmas, imtaphy::LinkVector links, int _MaxClusters);
                void init();
                void initForTest(itpp::mat& _delaysForAllLinks, itpp::mat* _sigmas);
                void generateDelays();
                void addSubclustersToDelays(std::vector<int> &strongestIndices, 
                                            std::vector<int> &secondStrongestIndices);
                
                itpp::mat* getDelays() {return &delays;}
                itpp::mat* getScaledDelays() {return &scaledDelays;}

            private:
                itpp::mat* sigmas;
                imtaphy::LinkVector links;
                int K;
                int MaxClusters;
                
                // delays for each multipath (N) and link (K)
                itpp::mat delays;
                // Scaled delays K x N
                itpp::mat scaledDelays;
            };

        }}}

#endif
