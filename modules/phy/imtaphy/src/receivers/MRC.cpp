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

#include <IMTAPHY/receivers/MRC.hpp>
#include <IMTAPHY/detail/LinearAlgebra.hpp>
#include <IMTAPHY/Link.hpp>
#include <IMTAPHY/Channel.hpp>
#include <IMTAPHY/receivers/feedback/LteRel8DLFeedbackManager.hpp>
#include <IMTAPHY/receivers/ComputeNumStrongestIandNCovariance.hpp>
#include <IMTAPHY/linkManagement/LinkManager.hpp>

using namespace imtaphy;
using namespace imtaphy::receivers;


STATIC_FACTORY_REGISTER_WITH_CREATOR(
    imtaphy::receivers::MRC,
    imtaphy::receivers::ReceiverInterface,
    "imtaphy.receiver.MRC",
    imtaphy::receivers::StationModuleCreator);
    
    
MRC::MRC(StationPhy* _station, const wns::pyconfig::View& pyConfigView) : 
    LinearReceiver(_station, pyConfigView),
    ServingPrecodedChannelCovariance(imtaphy::detail::ComplexFloatMatrixPtr()) // "smart" null pointers
{
    numRxAntennas = station->getAntenna()->getNumberOfElements();

    // this receiver is not suitable for spatial multiplexing
    maxRank = 1;
}


void 
MRC::channelInitialized(imtaphy::Channel* _channel)
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

    if (station->getStationType() == MOBILESTATION)
    { // only mobiles should compute feedback

        imtaphy::receivers::feedback::TheDLFeedbackManager::Instance().registerMS(station,
                                                                                station->getNode(),
                                                                                this,
                                                                                channel);

        receivingInDirection = imtaphy::Downlink;
    }
    else
    {
        receivingInDirection = imtaphy::Uplink;
    }
    
    // cache all links towards my station so that lookups from Linear Receiver will be faster later
    allMyLinks = channel->getLinkManager()->getAllLinksForStation(station);
    
    // that's for computing the SINR; -1 should mean consider all
    interferenceTreatment = new ComputeNumStrongestIandNCovariance(numRxAntennas, -1);
}



void 
MRC::computeFilter(imtaphy::detail::ComplexFloatMatrix& W, imtaphy::detail::ComplexFloatMatrix& precodedH, InterferencePowerMap &interferers, unsigned int m)
{
    // This MRC implementation is only supposed to work for the case of one stream
    // Therefore, we first check if we are receiving one stream

    assure(m == 1, "MRC is not supposed to work on multi-layer transmissions");
    assure(W.getColumns() == 1, "MRC is not supposed to work on multi-layer transmissions");
    assure(precodedH.getColumns() == 1, "MRC is not supposed to work on multi-layer transmissions");
    assure(W.getRows() == precodedH.getRows(), "Equivalent channel matrix and filter matrix must have identical number of rows");
    assure(numRxAntennas == W.getRows(), "Equivalent channel matrix and filter matrix must be identical to #Rx antennas");
    
    // for MRC, we just use the equivalent channel as the MRC filter
    memcpy(W.getLocation(), precodedH.getLocation(), sizeof(std::complex<float>) * numRxAntennas * 1);
}

//#endif // MKL