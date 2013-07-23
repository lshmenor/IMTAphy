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

#include <IMTAPHY/receivers/channelEstimation/channel/IandNCovarianceBased.hpp>
#include <IMTAPHY/receivers/LinearReceiver.hpp>
#include <IMTAPHY/receivers/Interferer.hpp>

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    imtaphy::receivers::channelEstimation::channel::IandNCovarianceBasedGaussianError,
    imtaphy::receivers::channelEstimation::channel::ChannelEstimationInterface,
    "imtaphy.receiver.channelEstimation.IandNCovarianceBasedGaussianError",
    imtaphy::receivers::ReceiverModuleCreator);

using namespace imtaphy::receivers::channelEstimation::channel;

IandNCovarianceBasedGaussianError::IandNCovarianceBasedGaussianError(LinearReceiver* receiver_) :
        ChannelEstimationInterface(receiver),
        numRxAntennas(receiver_->getNumRxAntennas()),
        direction(receiver_->getDirection()),
        receiver(receiver_),
        gainOverIandN(wns::Ratio::from_dB(0)),
        coloredEstimationError(false)
{
}

IandNCovarianceBasedGaussianError::IandNCovarianceBasedGaussianError(LinearReceiver* receiver_, const wns::pyconfig::View& pyConfigView) :
        ChannelEstimationInterface(receiver, pyConfigView),
        numRxAntennas(receiver_->getNumRxAntennas()),
        direction(receiver_->getDirection()),
        receiver(receiver_),
        gainOverIandN(wns::Ratio::from_dB(pyConfigView.get<double>("gainOverIandN"))),
        coloredEstimationError(pyConfigView.get<bool>("colored"))
{
}

imtaphy::detail::ComplexFloatMatrixPtr 
IandNCovarianceBasedGaussianError::estimateChannel(imtaphy::interface::PRB prb, imtaphy::Link* link, wns::Power txPower)
{
    // copy the perfect channel matrix
    imtaphy::detail::ComplexFloatMatrixPtr noisyChannel(new imtaphy::detail::ComplexFloatMatrix(*(link->getChannelMatrix(direction, prb))));
    imtaphy::detail::ComplexFloatMatrix gaussian(numRxAntennas, noisyChannel->getColumns());
    
    // scale with tx power:
    imtaphy::detail::scaleMatrixA(*noisyChannel, static_cast<float>(sqrt(txPower.get_mW())));
    
//     std::cout << "Using perfect channel with norm^2=" << imtaphy::detail::matrixNormSquared(*noisyChannel) << " perfectChannel=";
//     imtaphy::detail::displayMatrix(*noisyChannel);

    // 2nd parameter is "average per matrix element power", matrix will be scaled by sqrt() of that value
    // For the random gaussian matrix gaussian (A), we should get E{A*A^H} = I with A having NumRx-by-NumTx dimensions
    // so we scale by the number of tx antennas (columns of the channel matrix) and the desired gain over IandN
    float perElementPower = 1.0 / (static_cast<float>(noisyChannel->getColumns()) *  gainOverIandN.get_factor());
    imtaphy::detail::fillWhiteGaussianNoise(gaussian, perElementPower); 

    imtaphy::StationPhy* source;
    if (direction == imtaphy::Downlink)
        source = link->getBS();
    else
        source = link->getMS();

    // we have to get the interferers to this link. we can assume that everything that does 
    // not come from the transmitter in question is interference
    imtaphy::receivers::InterferersCollectionPtr interferers = 
            receiver->getInterferersOnPRB(prb, imtaphy::receivers::LinearReceiver::ExcludeNode, 
                                            imtaphy::TransmissionPtr(static_cast<Transmission*>(NULL)),
                                            source);
    
    imtaphy::detail::ComplexFloatMatrixPtr covariance = receiver->getPerfectInterferenceAndNoiseCovariance(prb, interferers);

//     std::cout << "Trace of covariance is " << imtaphy::detail::trace(covariance).real() << " covariance=";
//     imtaphy::detail::displayMatrix(covariance);
    
    if (coloredEstimationError)
    {
        // determine lower Cholesky factorization of the covariance matrix (i.e. for A, A=L*L^H)
        // this works also for diagonal matrices, e.g. if only the diagonal of the Noise+I Covariance would be known (noise only case)
        // note: only works on square, symmetric and positive definite matrices which will always be the case here
        
        imtaphy::detail::ComplexFloatMatrix choleskyFactor(covariance->getRows(), covariance->getColumns());
        imtaphy::detail::performCholeskyFactorization(*covariance, choleskyFactor);

        imtaphy::detail::ComplexFloatMatrix temp(numRxAntennas, noisyChannel->getColumns());
        imtaphy::detail::matrixMultiplyCequalsAB(temp, choleskyFactor, gaussian);

//         std::cout << "Adding error matrix with norm^2=" << imtaphy::detail::matrixNormSquared<>(temp) << ". coloredGaussian=";
//         imtaphy::detail::displayMatrix(temp);
        
        imtaphy::detail::matrixAddAtoB(temp, *noisyChannel);
        
    }
    else 
    {   // no colored error, simply scale by I+N power
    
        std::complex<float> covarianceTrace = imtaphy::detail::trace(*covariance);
        
        // the gaussian matrix has already scaled down by numTxAntennas and the configurable gainOverIandN
        // now scale by total I+N power scaled by numRxAntennas
        imtaphy::detail::scaleMatrixA(gaussian, static_cast<float>(sqrt(covarianceTrace.real() / static_cast<float>(noisyChannel->getRows()))));
        
//         std::cout << "Adding error matrix with norm^2=" << imtaphy::detail::matrixNormSquared(gaussian) << ". whiteGaussian=";
//         imtaphy::detail::displayMatrix(gaussian);
        
        imtaphy::detail::matrixAddAtoB(gaussian, *noisyChannel);
    }

    
//     std::cout << "Returning noisy channel with norm^2=" << imtaphy::detail::matrixNormSquared(*noisyChannel) << " noisyChannel=";
//     imtaphy::detail::displayMatrix(*noisyChannel);

    return noisyChannel;
}

