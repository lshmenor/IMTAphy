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

#ifndef SPATIALCHANNEL_CLUSTERPOWERS_HPP
#define SPATIALCHANNEL_CLUSTERPOWERS_HPP

#include <IMTAPHY/spatialChannel/SpatialChannelModelInterface.hpp>
#include <IMTAPHY/Channel.hpp>
#include <IMTAPHY/spatialChannel/m2135/M2135.hpp>

namespace imtaphy { namespace scm { namespace m2135 {
        
            class ClusterPowers
            {

            public:
                        
                ClusterPowers(imtaphy::LinkManager* linkManager, unsigned int _maxClusters);
                        
                /**
                 * @brief Initialize random parameters randomly
                 */
                void init();
                        
                /**
                 * @brief Initialize all the random parameters to fixed matrices for testing purposes only
                 */
                void initforTest(itpp::mat& _ksi);
                        
                /**
                 * @brief Calculate cluster powers
                 */
                void computeClusterPowers(const itpp::mat& sigma, const itpp::mat* delays);
                
                void determineTwoStrongestClusters();
                
                void addPowersforSubClusters();

                std::vector<int>* getStrongestIndices() {return &strongestIndices;}    
                std::vector<int>* getSecondStrongestIndices() {return &secondStrongestIndices;}                                        

                                             
                itpp::mat* getClusterPowers() {return &clusterPowers;}
                itpp::mat* getScaledClusterPowers() {return &scaledClusterPowers;}


            private:
                itpp::mat ksi;
                imtaphy::LinkVector scmLinks;
                unsigned int maxClusters;
            
                itpp::mat clusterPowers;
                itpp::mat scaledClusterPowers;
                
                std::vector<int> strongestIndices;
                std::vector<int> secondStrongestIndices;
            };
        }}}
#endif
