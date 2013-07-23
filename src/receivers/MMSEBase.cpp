/*******************************************************************************
 * This file is part of IMTAphy
 * _____________________________________________________________________________
 *
 * Copyright (C) 2011
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

#include <IMTAPHY/receivers/MMSEBase.hpp>
#include <IMTAPHY/detail/LinearAlgebra.hpp>
#include <IMTAPHY/Link.hpp>
#include <IMTAPHY/Channel.hpp>
#include <IMTAPHY/receivers/feedback/LteRel8DLFeedbackManager.hpp>
#include <IMTAPHY/linkManagement/LinkManager.hpp>


using namespace imtaphy;
using namespace imtaphy::receivers;

MMSEBase::MMSEBase(StationPhy* _station, const wns::pyconfig::View& pyConfigView) : 
    LinearReceiver(_station, pyConfigView),
    ServingPrecodedChannelCovariance(imtaphy::detail::ComplexFloatMatrixPtr()), // "smart" null pointers
    inverse(imtaphy::detail::ComplexFloatMatrixPtr())                           // "smart" null pointers

{
    numRxAntennas = station->getAntenna()->getNumberOfElements();
}


void 
MMSEBase::channelInitialized(imtaphy::Channel* _channel)
{
    channel = _channel;
    
    thermalNoiseInclNF = wns::Power::from_dBm(-174.0);
    thermalNoiseInclNF = thermalNoiseInclNF * wns::Ratio::from_factor(channel->getSpectrum()->getPRBbandWidthHz());
    thermalNoiseInclNF = thermalNoiseInclNF * noiseFigure;  
   
    // init noise covariance with identity
    NoiseCovariance = imtaphy::detail::ComplexFloatMatrixPtr(new imtaphy::detail::ComplexFloatMatrix(*(wns::SingletonHolder<imtaphy::detail::Identity<std::complex<float> > >::Instance().get(numRxAntennas))));

    // and now scale with thermal noise power + receiver noise figure
    imtaphy::detail::scaleMatrixA(*NoiseCovariance, static_cast<float>(thermalNoiseInclNF.get_mW()));
           
    scm = channel->getSpatialChannelModel();
   
    ServingPrecodedChannelCovariance = imtaphy::detail::ComplexFloatMatrixPtr(new imtaphy::detail::ComplexFloatMatrix(numRxAntennas, numRxAntennas)); 
    
    // for MMSE only, to hold the inverse of the covariance matrix:
    inverse = imtaphy::detail::ComplexFloatMatrixPtr(new imtaphy::detail::ComplexFloatMatrix(numRxAntennas, numRxAntennas));
    
    if (station->getStationType() == MOBILESTATION)
    { // only mobiles should compute feedback

        imtaphy::receivers::feedback::TheDLFeedbackManager::Instance().registerMS(station,
                                                                                  station->getNode(),
                                                                                  this,
                                                                                  channel);

        // That's the number of layers we can max. handle
        unsigned int numTxAntennasServingBS = channel->getLinkManager()->getServingLinkForMobileStation(station)->getBS()->getAntenna()->getNumberOfElements();
        maxRank = std::min(numRxAntennas, numTxAntennasServingBS);
        
        receivingInDirection = imtaphy::Downlink;
    }
    else // we are in a base station
    {
        receivingInDirection = imtaphy::Uplink;
    }

    // cache all links towards my station so that lookups from Linear Receiver will be faster later
    allMyLinks = channel->getLinkManager()->getAllLinksForStation(station);
}



void 
MMSEBase::computeFilter(imtaphy::detail::ComplexFloatMatrix& W, imtaphy::detail::ComplexFloatMatrix& precodedH, InterferencePowerMap &interferers, unsigned int m)
{
    // numRxAntennas is the number of Rx antennas u
    // m is the number of streams

    assure(numRxAntennas == precodedH.getRows(), "Precoded channel matrix needs as many rows as we have Rx antennas");
    assure(numRxAntennas == W.getRows(), "Filter matrix needs as many rows as we have Rx antennas");
    assure(m == precodedH.getColumns(), "Precoded channel matrix needs as many columns as we expect streams/layers");
    assure(m == W.getColumns(), "Filter matrix matrix needs as many columns as we expect streams/layers");
    
    // prepare inteference and noise covariance matrix
    imtaphy::detail::ComplexFloatMatrixPtr IandNoiseCovariance = noiseAndIcovariance->computeNoiseAndInterferenceCovariance(interferers, NoiseCovariance, -1);        

    // all involved matrices have been pre-allocated and are reused for each prb again
    // compute u-by-u covariance matrix Hs * Hs^H with Hs being the precoded channel as a u-by-m matrix
    imtaphy::detail::matrixMultiplyCequalsAAhermitian<float>(*ServingPrecodedChannelCovariance, precodedH);

    // add the noise and interference covariance matrix to it
    imtaphy::detail::matrixAddAtoB(*IandNoiseCovariance, *ServingPrecodedChannelCovariance);

    // take the inverse
    imtaphy::detail::matrixInverse(*ServingPrecodedChannelCovariance, *inverse);

    // W is u-by-m, Ainv is u-by-u and Hs is u-by-m and 
    imtaphy::detail::matrixMultiplyCequalsAB(W, *inverse, precodedH);
}