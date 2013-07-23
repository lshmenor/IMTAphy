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

#include <IMTAPHY/receivers/channelEstimation/covariance/PerfectIandNCovariance.hpp>
#include <IMTAPHY/receivers/LinearReceiver.hpp>
#include <IMTAPHY/receivers/Interferer.hpp>

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    imtaphy::receivers::channelEstimation::covariance::PerfectInterferenceAndNoiseCovariance,
    imtaphy::receivers::channelEstimation::covariance::NoiseAndInterferenceCovarianceInterface,
    "imtaphy.receiver.covarianceEstimation.Perfect",
    imtaphy::receivers::ReceiverModuleCreator);

using namespace imtaphy::receivers::channelEstimation::covariance;

PerfectInterferenceAndNoiseCovariance::PerfectInterferenceAndNoiseCovariance(LinearReceiver* receiver) :
        NoiseAndInterferenceCovarianceInterface(receiver),
        numRxAntennas(receiver->getNumRxAntennas()),
        direction(receiver->getDirection())
{
}

PerfectInterferenceAndNoiseCovariance::PerfectInterferenceAndNoiseCovariance(LinearReceiver* receiver, const wns::pyconfig::View& pyConfigView) :
        NoiseAndInterferenceCovarianceInterface(receiver, pyConfigView),
        numRxAntennas(receiver->getNumRxAntennas()),
    direction(receiver->getDirection())
{
}

imtaphy::detail::ComplexFloatMatrixPtr
PerfectInterferenceAndNoiseCovariance::computeNoiseAndInterferenceCovariance(imtaphy::receivers::InterferersCollectionPtr interferers, const imtaphy::detail::ComplexFloatMatrixPtr noiseOnlyMatrix, unsigned int prb)
{
    imtaphy::detail::ComplexFloatMatrixPtr IandNoiseCovariance(new imtaphy::detail::ComplexFloatMatrix(*noiseOnlyMatrix));
    

    for (imtaphy::receivers::InterferersSet::const_iterator iter = interferers->getInterferersSet().begin(); iter != interferers->getInterferersSet().end(); iter++)
    {
        const imtaphy::receivers::Interferer& interferer = *iter;
        
        unsigned int mI = interferer.interferingTransmission->getNumLayers();
        
        std::complex<float>* rawChannel = interferer.interferingLink->getRawChannelMatrix(direction, prb);
        imtaphy::detail::ComplexFloatMatrixPtr precoding = interferer.interferingTransmission->getPrecodingMatrix(prb);
        
        assure(precoding != imtaphy::detail::ComplexFloatMatrixPtr(), "Invalid precoding matrix");
        assure(precoding->getRows() == interferer.interferingTransmission->getNumTxAntennas(), "Inconsistent info about Tx antennas");
        assure(precoding->getColumns() == mI, "Inconsistent info about num layers");
        
        imtaphy::detail::ComplexFloatMatrixPtr interferersEffectiveChannel = imtaphy::detail::ComplexFloatMatrixPtr(new imtaphy::detail::ComplexFloatMatrix(numRxAntennas, mI));
        
        // precodedChannel = Hi * precoding with Hi being u-by-s and precoding bying s-by-mI
        imtaphy::detail::matrixMultiplyCequalsAB(*interferersEffectiveChannel, 
                                                 rawChannel,
                                                 *precoding);
        
        float interferingTxPower = interferer.interferingTransmission->getTxPower(prb).get_mW();
        
        // sum = (alpha*A) * (alpha*A)^H + sum
        imtaphy::detail::matrixMultiplyCequalsAlphaSquareTimesAAhermitianPlusC(*IandNoiseCovariance, // increment this
                                                                               *interferersEffectiveChannel, // add the alpha^2 * AA^H
                                                                               interferingTxPower // alpha^2
                                                                              );
   
    }
    
    return IandNoiseCovariance;
}

