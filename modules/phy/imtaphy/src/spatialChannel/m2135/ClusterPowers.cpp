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

#include <IMTAPHY/spatialChannel/m2135/ClusterPowers.hpp>

#include <WNS/evaluation/statistics/moments.hpp>
#include <IMTAPHY/Link.hpp>
#include <IMTAPHY/linkManagement/LinkManager.hpp>
#include <IMTAPHY/Channel.hpp>
#include <IMTAPHY/StationPhy.hpp>
#include <IMTAPHY/ChannelModuleCreator.hpp>

#include <iostream>
#include <algorithm>

#include <math.h>

using namespace imtaphy::scm::m2135;

ClusterPowers::ClusterPowers(imtaphy::LinkManager* _linkManager, unsigned int _maxClusters):
    scmLinks(_linkManager->getSCMLinks()),
    maxClusters(_maxClusters)
{
    
#ifndef WNS_NDEBUG    
    for (imtaphy::LinkVector::const_iterator iter_links=scmLinks.begin(); iter_links!=scmLinks.end(); iter_links++) 
    {
        assure(FixParSingleton::Instance()((*iter_links)->getScenario(), (*iter_links)->getPropagation())->NumClusters <= maxClusters,
               "Link's scenario needs more clusters than expected");
    }
#endif

    clusterPowers = itpp::ones(scmLinks.size(), maxClusters)*NAN;
    scaledClusterPowers = itpp::ones(scmLinks.size(), maxClusters)*NAN;

    
    ksi = itpp::ones(scmLinks.size(), maxClusters)*NAN;
}


void ClusterPowers::init()
{
    for (imtaphy::LinkVector::const_iterator iter_links=scmLinks.begin(); iter_links!=scmLinks.end(); iter_links++)
    {
        // fill ksi with random values according to link"s fixpars
        unsigned int numClusters = (FixParSingleton::Instance()((*iter_links)->getScenario(), (*iter_links)->getPropagation()))->NumClusters;
        if(maxClusters > numClusters) // need to append some nans before copying
            ksi.set_row( (*iter_links)->getSCMlinkId(), 
                         itpp::concat(((FixParSingleton::Instance()((*iter_links)->getScenario(), (*iter_links)->getPropagation()))->LNS_ksi) * itpp::randn(numClusters), 
                                      itpp::ones(maxClusters - numClusters)*NAN) );
        else if(maxClusters == numClusters)
            ksi.set_row( (*iter_links)->getSCMlinkId(), 
                         ((FixParSingleton::Instance()((*iter_links)->getScenario(), (*iter_links)->getPropagation()))->LNS_ksi) * itpp::randn(numClusters));
        else
            assure(0, "NumClusters should not be greater than MaxClusters, problem with ClusterPowers");
    }
}

void ClusterPowers::initforTest(itpp::mat& _ksi)
{
    assure(ksi.rows()==_ksi.rows() && ksi.cols()==_ksi.cols(), "matrix ksi dimensions are not correct");
    ksi = _ksi;
}

void
ClusterPowers::computeClusterPowers(const itpp::mat& sigma, const itpp::mat* delays)
{
    for (imtaphy::LinkVector::const_iterator iter_links=scmLinks.begin(); iter_links!=scmLinks.end(); iter_links++)
    {
        unsigned int k = (*iter_links)->getSCMlinkId();
        
        double r_tau = FixParSingleton::Instance()((*iter_links)->getScenario(), (*iter_links)->getPropagation())->r_tau;
                
        double sumPower=0;
        for (unsigned int n = 0; n < FixParSingleton::Instance()((*iter_links)->getScenario(), (*iter_links)->getPropagation())->NumClusters; n++)
        {
            assure (!isnan(ksi(k,n)), "wrong values of ksi accessed");
            clusterPowers(k,n) = exp( (-(*delays)(k, n)) * (r_tau-1)/(r_tau*sigma(k,DSpread))) * pow(10.0, ((-ksi(k,n))/10.0));
            sumPower+=clusterPowers(k,n);
        }
        assure(sumPower!=0, "Power sum equals zero");
        
        // normalize by sum power 
        clusterPowers.set_row(k, (clusterPowers.get_row(k)) / sumPower);
        scaledClusterPowers.set_row(k, clusterPowers.get_row(k));
        
        // if the link is LoS, add the LoS power fraction to the LoS path (n=0)
        // and scale down all other paths accordingly (thus keeping power normalization)
        if((*iter_links)->getPropagation() == imtaphy::Link::LoS)
        {
            double P1LoS = sigma(k, RiceanK) / (sigma(k,RiceanK) + 1.0);
            scaledClusterPowers.set_row(k, scaledClusterPowers.get_row(k) / (sigma(k,RiceanK) + 1.0));
            scaledClusterPowers(k, 0) += P1LoS; 
        }
    }
}

void
ClusterPowers::determineTwoStrongestClusters()
{
    strongestIndices.resize(scmLinks.size());
    secondStrongestIndices.resize(scmLinks.size());
    
    for(unsigned int k=0; k < scmLinks.size(); k++)
    {
        unsigned int nClusters = FixParSingleton::Instance()(scmLinks[k]->getScenario(), scmLinks[k]->getPropagation())->NumClusters;
        int i_strongest=0, i_secondStrongest=0;
        double maxPower = -100000.0, secondmaxPower=-10000.0;
        for (unsigned int n=0; n<nClusters; n++)
        {
            if(clusterPowers(k,n) > maxPower)
            {
                if(n!=0)
                {
                    i_secondStrongest = i_strongest;
                    secondmaxPower = maxPower;
                }
                i_strongest = n;
                maxPower = clusterPowers(k,n);
            }
            else if (clusterPowers(k,n) > secondmaxPower)
            {
                i_secondStrongest = n;
                secondmaxPower = clusterPowers(k,n);
            }

        }
        strongestIndices[k] = i_strongest;
        secondStrongestIndices[k] = i_secondStrongest;
    }
}

void
ClusterPowers::addPowersforSubClusters()
{
    for (unsigned int k=0; k<scmLinks.size(); k++)
    {
        double strongestPower = clusterPowers(k,strongestIndices[k]);
        double secondStrongestPower= clusterPowers(k,secondStrongestIndices[k]);
        clusterPowers(k,strongestIndices[k]) = strongestPower/2;
        clusterPowers(k,20) = strongestPower*(3.0/10.0);
        clusterPowers(k,22) = strongestPower*(2.0/10.0);
        clusterPowers(k,secondStrongestIndices[k]) = secondStrongestPower/2;
        clusterPowers(k,21) = secondStrongestPower*(3.0/10.0);
        clusterPowers(k,23) = secondStrongestPower*(2.0/10.0);
    }
}