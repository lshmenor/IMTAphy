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

#include <IMTAPHY/spatialChannel/m2135/Delays.hpp>
#include <itpp/itbase.h>

#include <IMTAPHY/StationPhy.hpp>

#include <iostream>

#include <math.h>

using namespace imtaphy::scm::m2135;


Delays::Delays(itpp::mat* _sigmas, imtaphy::LinkVector _links, int _MaxClusters): 
    sigmas(_sigmas),
    links(_links),
    K(links.size()),
    MaxClusters(_MaxClusters)
{
    delays = itpp::ones(K,MaxClusters)*NAN;
    scaledDelays = itpp::ones(K,MaxClusters)*NAN;
}

void
Delays::init()
{
        
    for (int k = 0; k < K; k++)
    {
        unsigned int nClusters = FixParSingleton::Instance()(links[k]->getScenario(), links[k]->getPropagation())->NumClusters;
                
        // Only fill the needed positions in the delays matrix
        for (unsigned int n=0; n < nClusters; n++)
            //delays(k,i) = -(fp->r_tau)*(sigmas->get(k,2))*log(itpp::randu());
            delays(k,n) = itpp::randu();
    }
}

void
Delays::initForTest(itpp::mat& _delays, itpp::mat* _sigmas)
{
    // Overwrite some stuff for test
    delays = _delays;
    sigmas = _sigmas;
}

void
Delays::generateDelays()
{
    itpp::vec tempDelays;

    for (int k = 0; k < K; k++)
    {
        unsigned int nClusters = FixParSingleton::Instance()(links[k]->getScenario(), links[k]->getPropagation())->NumClusters;
                
        assure(nClusters > 0, "nClusters must not be 0");
                
        double r_tau = FixParSingleton::Instance()(links[k]->getScenario(), links[k]->getPropagation())->r_tau;
                               
        // Compute delays
        tempDelays.set_size(nClusters);
        tempDelays.zeros();
                
        for (int n = 0; n < tempDelays.size(); n++)
            tempDelays(n) = -(r_tau) * (sigmas->get(k,DSpread)) * log(delays(k,n));
                
        // Normalize the delays and sort them
        itpp::vec delays_min;
        delays_min.set_size(tempDelays.size());
        delays_min.ones();

        tempDelays = tempDelays - (itpp::min(tempDelays)) * delays_min;
        itpp::sort(tempDelays);
                                
        // Update the delays matrices
        double K_factor_dB, D;
        for(int n = 0; n < tempDelays.size(); n++) 
        {
            delays.set(k,n, tempDelays(n));
                        
            // Scale delays if it is a LoS link
            if (links[k]->getPropagation() == imtaphy::Link::LoS) 
            {
                K_factor_dB = 10.0 * log10(fabs(sigmas->get(k,RiceanK)));
                D =  0.7705 - 0.0433 * K_factor_dB + 0.0002 * pow(K_factor_dB, 2) + 0.000017 * pow(K_factor_dB, 3);
                scaledDelays.set(k,n, tempDelays(n) / D);

            }
            else
            {
                scaledDelays.set(k,n, tempDelays(n));
            }
        }
    }
}

void
Delays::addSubclustersToDelays(std::vector<int> &strongestIndices, std::vector<int> &secondStrongestIndices)
{
    for (int k=0; k < K; k++)
    {
        scaledDelays(k,20) = scaledDelays(k, strongestIndices[k]) + 5e-9;
        scaledDelays(k,21) = scaledDelays(k, secondStrongestIndices[k]) + 5e-9;
        scaledDelays(k,22) = scaledDelays(k, strongestIndices[k]) + 10e-9;
        scaledDelays(k,23) = scaledDelays(k, secondStrongestIndices[k]) + 10e-9;
    }
}
