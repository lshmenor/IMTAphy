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

#include <IMTAPHY/spatialChannel/m2135/RayAngles.hpp>

#include <WNS/evaluation/statistics/moments.hpp>

#include <IMTAPHY/Channel.hpp>
#include <IMTAPHY/StationPhy.hpp>
#include <IMTAPHY/ChannelModuleCreator.hpp>
#include <IMTAPHY/spatialChannel/m2135/M2135.hpp>
#include <iostream>
#include <algorithm>
#include <boost/multi_array.hpp>
#include <IMTAPHY/Link.hpp>
#include <math.h>

using namespace imtaphy::scm::m2135;

RayAngles::RayAngles(imtaphy::LinkVector _links, const int _MaxClusters, const int _NumRays, imtaphy::Channel* _channel):
    links(_links), 
    MaxClusters(_MaxClusters), 
    NumRays(_NumRays), 
    K(links.size()),
    channel(_channel),
    rngForShuffle(*wns::simulator::getRNG(), uni)
{
    randomVectors_XAoD.clear(); 
    randomVectors_YAoD.clear(); 
    randomVectors_XAoA.clear(); 
    randomVectors_YAoA.clear();

    aoaVector.set_size(links.size() * MaxClusters * NumRays);
    aodVector.set_size(links.size() * MaxClusters * NumRays);

    aoaVector = NAN;
    aodVector = NAN;
    
    
    aoas = new RayAngles3DArray(aoaVector._data(), boost::extents[K][MaxClusters][NumRays]);
    aods = new RayAngles3DArray(aodVector._data(), boost::extents[K][MaxClusters][NumRays]);

}

RayAngles::~RayAngles()
{
    delete aoas;
    delete aods;
}

void RayAngles::initforTest(std::vector<itpp::mat>& rand_LoS, std::vector<itpp::mat>& rand_NLoS, itpp::vec& t_Bs, itpp::vec& t_Ms)
{
    int i_LoS = 0, i_NLoS=0;

    for (unsigned int k = 0; k < K ; k++)
    {
        if(links[k]->getPropagation() == imtaphy::Link::LoS)
        {
            randomVectors_XAoD.push_back(rand_LoS[0].get_row(i_LoS));
            randomVectors_YAoD.push_back(rand_LoS[1].get_row(i_LoS));
            randomVectors_XAoA.push_back(rand_LoS[2].get_row(i_LoS));
            randomVectors_YAoA.push_back(rand_LoS[3].get_row(i_LoS));
            i_LoS++;
        }
        else
        {
            randomVectors_XAoD.push_back(rand_NLoS[0].get_row(i_NLoS));
            randomVectors_YAoD.push_back(rand_NLoS[1].get_row(i_NLoS));
            randomVectors_XAoA.push_back(rand_NLoS[2].get_row(i_NLoS));
            randomVectors_YAoA.push_back(rand_NLoS[3].get_row(i_NLoS));
            i_NLoS++;
        }
    }
    ThetaBs = t_Bs; ThetaMs = t_Ms;
}

void RayAngles::generateAoAandAoDs(itpp::mat& clusterPowers, const itpp::mat *sigmas)
{
    if ((randomVectors_XAoD.empty()) && (randomVectors_YAoD.empty()) 
        && (randomVectors_XAoA.empty()) && (randomVectors_YAoA.empty()))
        // this means that initforTest was not called and hence we must
        // initialize all the required vectors
    {
        for (unsigned int k = 0; k < K ; k++)
        {
            unsigned int nClusters = FixParSingleton::Instance()(links[k]->getScenario(), links[k]->getPropagation())->NumClusters;
            randomVectors_XAoD.push_back(itpp::randu(nClusters));
            randomVectors_YAoD.push_back(itpp::randn(nClusters));
            randomVectors_XAoA.push_back(itpp::randu(nClusters));
            randomVectors_YAoA.push_back(itpp::randn(nClusters));
        }
        ThetaBs.set_size(K); ThetaMs.set_size(K);
        for (unsigned int k = 0; k < K; k++)
        {
            // everythig in here is in degrees but channel returns radians
            ThetaBs[k] = (180.0 / itpp::pi) * channel->getAzimuth(links[k]->getBS()->getPosition(), links[k]->getWrappedMSposition());
            ThetaMs[k] = (180.0 / itpp::pi) * channel->getAzimuth(links[k]->getWrappedMSposition(), links[k]->getBS()->getPosition());
        }

    }
        


    itpp::mat aoasPrime = itpp::zeros(K, MaxClusters);
    itpp::mat aodsPrime = itpp::zeros(K, MaxClusters);
    itpp::mat aoasPerCluster = itpp::zeros(K, MaxClusters);
    itpp::mat aodsPerCluster = itpp::zeros(K, MaxClusters);
        

    itpp::vec K_factors_dB;
    K_factors_dB.set_size(K);
    K_factors_dB = 10.0 * (itpp::log10(itpp::abs(sigmas->get_col(RiceanK))));


        
    for (unsigned int k = 0; k < K ; k++)
    {
        unsigned int nClusters = FixParSingleton::Instance()(links[k]->getScenario(), links[k]->getPropagation())->NumClusters;
                
        double C = 0.0; // Scaling factor C
        switch (nClusters)
        {
        case 4  : C = 0.779; break;
        case 5  : C = 0.860; break;
        case 8  : C = 1.018; break;
        case 10 : C = 1.090; break;
        case 11 : C = 1.123; break;
        case 12 : C = 1.146; break;
        case 14 : C = 1.190; break;
            // Handle the case for InH
        case 15 : (links[k]->getScenario() != imtaphy::Link::InH) ? (C = 1.211):(C = 1.434); break;
        case 16 : C = 1.226; break;
            // Handle the case for InH
        case 19 : (links[k]->getScenario() != imtaphy::Link::InH) ? (C = 1.273):(C = 1.501); break;
        case 20 : C = 1.289; break;
        default: assure(0, "invalid number of clusters");
        }
                
        // Scale C for the LoS case
        // Note: The Matlab implementation provided by WINNER-II project
        // scales C twice. A small mistake in the Matlab code!
        if ( links[k]->getPropagation() == imtaphy::Link::LoS )
        {
            if ( links[k]->getScenario() == imtaphy::Link::InH )
                C = C * (.9275 + 0.0439 * K_factors_dB(k) - 0.0071 * pow(K_factors_dB(k),2) + 0.0002 * pow(K_factors_dB(k),3));
            else
            {
                // MATLAB_ERROR 1: Matlab does this scaling twice instead of once as suggested by the document
                C = C * (1.1035 - 0.028 * K_factors_dB(k) - 0.002 * pow(K_factors_dB(k),2) + 0.0001 * pow(K_factors_dB(k),3));
            }
        }               
        // Take only the non-NAN values for computations
        itpp::vec clusterPowersCurrentLink;   // Its size will be nClusters
        clusterPowersCurrentLink.set_size(nClusters);
                
        for (unsigned int n = 0; n < nClusters; n++)
            clusterPowersCurrentLink.set(n, clusterPowers.get(k,n));
                
        double maxClusterPower;
        maxClusterPower = itpp::max(clusterPowersCurrentLink);
                
                
        // For AoDs
        itpp::vec X_aoDs = itpp::sgn(-1 + 2 * randomVectors_XAoD[k]);        // X is uniform RV from discrete set [-1 +1] and Y is Gaussian
        itpp::vec Y_aoDs = ((sigmas->get(k, ASDeparture))/7.) * randomVectors_YAoD[k];   // as per pp. 35 ITU-R M.2135 document
                
        // For AoAs
        itpp::vec X_aoAs = itpp::sgn(-1 + 2 * randomVectors_XAoA[k]);
        itpp::vec Y_aoAs = ((sigmas->get(k, ASArrival))/7.) * randomVectors_YAoA[k];
                
        for (int n = 0; n < clusterPowersCurrentLink.size(); n++)
        {
            if(links[k]->getScenario() == imtaphy::Link::InH)  // For InH
            {
                aodsPrime(k,n) = -(sigmas->get(k, ASDeparture) * (log(clusterPowersCurrentLink(n) / (maxClusterPower)))) / C;
                aoasPrime(k,n) = -(sigmas->get(k, ASArrival) * (log(clusterPowersCurrentLink(n) / (maxClusterPower)))) / C;
            }
            else // For other scenarios
            {
                aodsPrime(k,n) = 2.0 * (sigmas->get(k, ASDeparture) / 1.4) * sqrt(-log(clusterPowersCurrentLink(n)/(maxClusterPower))) / C;
                aoasPrime(k,n) = 2.0 * (sigmas->get(k, ASArrival) / 1.4) * sqrt(-log(clusterPowersCurrentLink(n)/(maxClusterPower))) / C;
            }
            //std::cout << "aodsPrime: " << aodsPrime << std::endl;
            if( links[k]->getPropagation() == imtaphy::Link::LoS )
            {
                // MATLAB ERROR 2: to get the contribution of first cluster, matlab uses aodsPrime.get(0,0) instead of using aodsPrime.get(k,0)
                // In this way matlab adds the contribution of first cluster of first link to all links
                double aod = (aodsPrime.get(k,n) * X_aoDs(n) + Y_aoDs(n) - (aodsPrime.get(k,0) * X_aoDs(0) + Y_aoDs(0) - ThetaBs(k)));
                aodsPerCluster.set(k,n,aod);

                double aoa = (aoasPrime.get(k,n) * X_aoAs(n) + Y_aoAs(n) - (aoasPrime.get(k,0) * X_aoAs(0) + Y_aoAs(0) - ThetaMs(k)));
                aoasPerCluster.set(k,n,aoa);
            }
            else
            {
                double aod = aodsPrime.get(k,n) * X_aoDs(n) + Y_aoDs(n) + ThetaBs(k);
                aodsPerCluster.set(k,n,aod);

                // MATLAB ERROR 3: Matlab add ThetaBs in aoa and aod both. ThetaBs should be added to aod, ThetaMs to aoa.
                double aoa = aoasPrime.get(k,n) * X_aoAs(n) + Y_aoAs(n) + ThetaMs(k);
                aoasPerCluster.set(k,n,aoa);
            }
        }
    } // end loop over all links
        
    // The offsets (+/- alternatively for each sub-path)
    itpp::vec alpha = "0.0447 -0.0447 0.1413 -0.1413 0.2492 -0.2492 0.3715 -0.3715 0.5129 -0.512 0.6797 -0.6797 0.8844 -0.8844 1.1481 -1.1481 1.5195 -1.5195 2.1551 -2.1551";
        

    for (unsigned int k = 0; k < K ; k++)
    {
        unsigned int nClusters = FixParSingleton::Instance()(links[k]->getScenario(), links[k]->getPropagation())->NumClusters;
        for (unsigned int n = 0; n < nClusters; n++)
        {
            for (int m = 0; m < NumRays; m++)
            {
                (*aods)[k][n][m] = static_cast<RAYPRECISION>(wrapAngleDistribution(aodsPerCluster.get(k,n) + (FixParSingleton::Instance()(links[k]->getScenario(), links[k]->getPropagation())->Cluster_ASD)*alpha(m)));
                (*aoas)[k][n][m] = static_cast<RAYPRECISION>(wrapAngleDistribution(aoasPerCluster.get(k,n) + (FixParSingleton::Instance()(links[k]->getScenario(), links[k]->getPropagation())->Cluster_ASA)*alpha(m)));
            } // end loop over all rays
                        
        } // end loop over all clusters
    } // end loop over all links


    // convert to radians
    aoaVector *= itpp::pi / 180.0;
    aodVector *= itpp::pi / 180.0;

    
    // clean up the random vectors
    randomVectors_XAoA.clear();
    randomVectors_XAoD.clear();
    randomVectors_YAoA.clear();
    randomVectors_YAoD.clear();
}

double 
RayAngles::wrapAngleDistribution(double theta)
{
    theta = theta - 360. * floor(theta/360.0);
    return (theta - 360. * floor(theta/180));
}

void
RayAngles::createSubclustersAndPermutateAngles(std::vector<int> &strongestIndices, std::vector<int> &secondStrongestIndices, imtaphy::LinkVector links)
{

    typedef boost::multi_array_ref<double, 3> Double3DArray;

    unsigned int K=links.size();
    for (unsigned int k = 0;  k < K; k++)
    {
        int nClusters = FixParSingleton::Instance()(links[k]->getScenario(), links[k]->getPropagation())->NumClusters;

        // permutate aods
        for (int n = 0; n < nClusters; n++)
        {
            // don't permutate the strongest clusters, we have to do this
            // manually below
            if ((n == strongestIndices[k]) || (n == secondStrongestIndices[k]))
                continue;

            // take each (k, n) column of the 3D matrix and shuffle the angles
            // in there
            RayAngles3DArray::array_view<1>::type column = (*aods)[boost::indices[k][n][RayAngles3DArray::index_range(0, NumRays)]];

            std::random_shuffle(column.begin(), column.end(), rngForShuffle);
        }

        // now create the subclusters and immediately permutate the aod
        // subclusters
        // the strongest and second-strongest are given by the
        // index1 and index2 parameters, their subclusters will go to:
        // strongest cluster, sub-cluster no 2: 21, rays 9, 10, 11, 12, 17, 18
        //                                                            no 3: 22,
        // rays 13, 14, 15, 16
        // second-strongest, sub-cluster  no 2: 23, rays 9, 10, 11, 12, 17, 18
        //                                no 3: 24, rays 13, 14, 15, 16
        // definition

        // set clusters 21 and 23 (20 and 22 in the array)
        int subCluster2[6] = {8, 9, 10, 11, 16, 17}; // array starts at 0
        std::vector<int> permutatedRays2(subCluster2, subCluster2 + 6);

        std::random_shuffle(permutatedRays2.begin(), permutatedRays2.end(), rngForShuffle);

        // set the 6 rays of cluster 21 (subcluster 2)
        // to a permutated selection of 6 rays (for aods) or just copy (for
        // aoas)
        // from the strongest clusters
        for (int m = 0; m < 6; m++)
        {
            (*aods)[k][20][subCluster2[m]] = (*aods)[k][strongestIndices[k]][permutatedRays2[m]];
            (*aoas)[k][20][subCluster2[m]] = (*aoas)[k][strongestIndices[k]][subCluster2[m]];
        }

        std::random_shuffle(permutatedRays2.begin(), permutatedRays2.end(), rngForShuffle);
        // set the 6 rays of cluster 23 (subcluster 2)
        // to a permutated selection of 6 rays (for aods) or just copy (for
        // aoas)
        // from the second-strongest clusters
        for (int m = 0; m < 6; m++)
        {
            (*aods)[k][22][subCluster2[m]] = (*aods)[k][secondStrongestIndices[k]][permutatedRays2[m]];
            (*aoas)[k][22][subCluster2[m]] = (*aoas)[k][secondStrongestIndices[k]][subCluster2[m]];
        }

// set clusters 22 and 24 (21 and 23 in the array)
        int subCluster3[4] = {12, 13, 14, 15};
        std::vector<int> permutatedRays3(subCluster3, subCluster3 + 4);

        std::random_shuffle(permutatedRays3.begin(), permutatedRays3.end(), rngForShuffle);

        // set the 6 rays of cluster 22 (subcluster 3)
        // to a permutated selection of 4 rays (for aods) or just copy (for
        // aoas)
        // from the strongest clusters
        for (int m = 0; m < 4; m++)
        {
            (*aods)[k][21][subCluster3[m]] = (*aods)[k][strongestIndices[k]][permutatedRays3[m]];
            (*aoas)[k][21][subCluster3[m]] = (*aoas)[k][strongestIndices[k]][subCluster3[m]];
        }

        std::random_shuffle(permutatedRays3.begin(), permutatedRays3.end(), rngForShuffle);
        // set the 4 rays of cluster 24 (subcluster 3)
        // to a permutated selection of 4 rays (for aods) or just copy (for aoa)
        // from the second-strongest clusters
        for (int m = 0; m < 4; m++)
        {
            (*aods)[k][23][subCluster3[m]] = (*aods)[k][secondStrongestIndices[k]][permutatedRays3[m]];
            (*aoas)[k][23][subCluster3[m]] = (*aoas)[k][secondStrongestIndices[k]][subCluster3[m]];
        }


        // set sub-clusters 1 at the original places of the strongest clusters
        int subCluster1[10] = {0, 1, 2, 3, 4, 5, 6, 7, 18, 19}; // array starts
                                                                // at 0
        int notSubCluster1[10] = {8, 9, 10, 11, 12, 13, 14, 15, 16, 17};

        std::vector<int> permutatedRays1(subCluster1, subCluster1 + 10);

        std::random_shuffle(permutatedRays1.begin(), permutatedRays1.end(), rngForShuffle);

        // set the 10 rays of subcluster 1 at the original places to a
        // permutated selection of 10 rays
        // from the strongest clusters
        std::vector<double> temp;
        temp.resize(10);

        for (int m = 0; m < 10; m++)
            temp[m] = (*aods)[k][strongestIndices[k]][subCluster1[m]];

        // clean all other rays from strongest cluster
        // for both aoa and aod
        for (int m = 0; m < 10; m++)
        {
            (*aods)[k][strongestIndices[k]][notSubCluster1[m]] = NAN;
            (*aoas)[k][strongestIndices[k]][notSubCluster1[m]] = NAN;
        }
        // now put in the 10 rays to the permutated positions
        for (int m = 0; m < 10; m++)
            (*aods)[k][strongestIndices[k]][permutatedRays1[m]] = temp[m];


        std::random_shuffle(permutatedRays1.begin(), permutatedRays1.end(), rngForShuffle);
        // set the 10 rays of subcluster 1 of the second stronges cluster to a
        // permutated selection of 10 rays
        // from the second-strongest clusters
        for (int m = 0; m < 10; m++)
            temp[m] = (*aods)[k][secondStrongestIndices[k]][subCluster1[m]];

        // clean all other rays from second strongest cluster
        // do this also for the aoas
        for (int m = 0; m < 10; m++)
        {
            (*aods)[k][secondStrongestIndices[k]][notSubCluster1[m]] = NAN;
            (*aoas)[k][secondStrongestIndices[k]][notSubCluster1[m]] = NAN;
        }

        for (int m = 0; m < 10; m++)
            (*aods)[k][secondStrongestIndices[k]][permutatedRays1[m]] = temp[m];

    }
}
