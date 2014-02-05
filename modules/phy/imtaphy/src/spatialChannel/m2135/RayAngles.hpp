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

#ifndef SPATIALCHANNEL_RAYANGLES_HPP
#define SPATIALCHANNEL_RAYANGLES_HPP

#include <IMTAPHY/spatialChannel/SpatialChannelModelInterface.hpp>
#include <IMTAPHY/Channel.hpp>
#include <boost/random.hpp>
//#include <IMTAPHY/spatialChannel/m2135/M2135.hpp>

// this could als be templated if necessary
// double or float
#define RAYPRECISION float


namespace imtaphy { namespace scm { namespace m2135 {
        
            typedef boost::multi_array_ref<RAYPRECISION, 3> RayAngles3DArray;
            
            class RayAngles
            {
                
            public:
                RayAngles(imtaphy::LinkVector _links, const int _MaxClusters, const int _NumRays, imtaphy::Channel* _channel);
                virtual ~RayAngles();
                
                void initforTest(std::vector<itpp::mat>& rand_LoS, std::vector<itpp::mat>& rand_NLoS, itpp::vec& t_Bs, itpp::vec& t_Ms);
                
                /**
                 * @brief Map an angle from [-inf +inf] to [-180 +180]
                 */
                double wrapAngleDistribution(double theta);
                
                /**
                 * @brief Calculate Angles of Arrival and Departure
                 */
                void generateAoAandAoDs(itpp::mat& clusterPowers, const itpp::mat *sigmas);
                                        
                /**
                 * @brief Does the random coupling of AoDs and AoAs in a path/cluster
                 */
                void createSubclustersAndPermutateAngles(std::vector<int> &strongestIndices, std::vector<int> &secondStrongestIndices, imtaphy::LinkVector links);

                
                RayAngles3DArray* getAoAs() {return aoas;}
                RayAngles3DArray* getAoDs() {return aods;}
                                        
                        
            private:
                imtaphy::LinkVector links;
                int MaxClusters, NumRays;
                std::vector<itpp::vec> randomVectors_XAoD;
                std::vector<itpp::vec> randomVectors_YAoD;
                std::vector<itpp::vec> randomVectors_XAoA;
                std::vector<itpp::vec> randomVectors_YAoA;
                itpp::vec ThetaBs, ThetaMs;
                unsigned int K;
                imtaphy::Channel* channel;
                
                itpp::Vec<RAYPRECISION> aoaVector;
                itpp::Vec<RAYPRECISION> aodVector;
                
                RayAngles3DArray* aoas;
                RayAngles3DArray* aods;
                
                // to allow std::random_shuffle to use openWNS' rng (boost::mt19937)
                boost::uniform_int<> uni;
                boost::variate_generator<boost::mt19937&, boost::uniform_int<> > rngForShuffle;

            };
        }}}
#endif
