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

#include <IMTAPHY/receivers/channelEstimation/channel/ThermalNoiseBasedGaussianError.hpp>
#include <IMTAPHY/receivers/LinearReceiver.hpp>
#include <IMTAPHY/receivers/Interferer.hpp>

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    imtaphy::receivers::channelEstimation::channel::ThermalNoiseBasedGaussianError,
    imtaphy::receivers::channelEstimation::channel::ChannelEstimationInterface,
    "imtaphy.receiver.channelEstimation.ThermalNoiseBasedGaussianError",
    imtaphy::receivers::ReceiverModuleCreator);

using namespace imtaphy::receivers::channelEstimation::channel;

ThermalNoiseBasedGaussianError::ThermalNoiseBasedGaussianError(LinearReceiver* receiver_) :
        ChannelEstimationInterface(receiver),
        numRxAntennas(receiver_->getNumRxAntennas()),
        direction(receiver_->getDirection()),
        receiver(receiver_),
        noisePower_mW(receiver->getThermalNoiseIncludingNoiseFigure().get_mW())
{
}

ThermalNoiseBasedGaussianError::ThermalNoiseBasedGaussianError(LinearReceiver* receiver_, const wns::pyconfig::View& pyConfigView) :
        ChannelEstimationInterface(receiver, pyConfigView),
        numRxAntennas(receiver_->getNumRxAntennas()),
        direction(receiver_->getDirection()),
        receiver(receiver_),
        noisePower_mW(receiver->getThermalNoiseIncludingNoiseFigure().get_mW())
{
    noisePower_mW *= wns::Ratio::from_dB(pyConfigView.get<double>("errorPowerRelativeToNoise")).get_factor();
}

imtaphy::detail::ComplexFloatMatrixPtr 
ThermalNoiseBasedGaussianError::estimateChannel(imtaphy::interface::PRB prb, imtaphy::Link* link, wns::Power txPower)
{
    // copy the perfect channel matrix
    imtaphy::detail::ComplexFloatMatrixPtr noisyChannel(new imtaphy::detail::ComplexFloatMatrix(*(link->getChannelMatrix(direction, prb))));
    
    // scale with tx power:
    imtaphy::detail::scaleMatrixA(*noisyChannel, static_cast<float>(sqrt(txPower.get_mW())));
    

    // noise power is the thermal noise (including Rx noise figure) per Rx antenna branch
    // for the numRx-by-numTx channel matrix, it has to be scaled by the number of Tx antennas before
    // sqrt(noise) is applied to each element of the standard normal Gaussian matrix
    
//     std::cout << "Norm before=" << imtaphy::detail::matrixNormSquared(*noisyChannel)<< "\n";
//     imtaphy::detail::displayMatrix(*noisyChannel);
    
    imtaphy::detail::applyWhiteGaussianNoiseToA(*noisyChannel, noisePower_mW / static_cast<float>(noisyChannel->getColumns()));
    
//     std::cout << "Norm after=" << imtaphy::detail::matrixNormSquared(*noisyChannel)<< "\n";
//     imtaphy::detail::displayMatrix(*noisyChannel);
    
    return noisyChannel;
}

