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

#include <IMTAPHY/receivers/channelEstimation/covariance/GaussianErrorIandNCovariance.hpp>
#include <IMTAPHY/receivers/LinearReceiver.hpp>
#include <IMTAPHY/receivers/Interferer.hpp>

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    imtaphy::receivers::channelEstimation::covariance::GaussianErrorInterferenceAndNoiseCovariance,
    imtaphy::receivers::channelEstimation::covariance::NoiseAndInterferenceCovarianceInterface,
    "imtaphy.receiver.covarianceEstimation.GaussianError",
    imtaphy::receivers::ReceiverModuleCreator);

using namespace imtaphy::receivers::channelEstimation::covariance;

GaussianErrorInterferenceAndNoiseCovariance::GaussianErrorInterferenceAndNoiseCovariance(LinearReceiver* receiver_) :
        NoiseAndInterferenceCovarianceInterface(receiver),
        numRxAntennas(receiver_->getNumRxAntennas()),
        direction(receiver_->getDirection()),
        relativeErrorPower(wns::Ratio::from_dB(0.0)), // note that this is a lot!
        receiver(receiver_)
{
}

GaussianErrorInterferenceAndNoiseCovariance::GaussianErrorInterferenceAndNoiseCovariance(LinearReceiver* receiver_, const wns::pyconfig::View& pyConfigView) :
        NoiseAndInterferenceCovarianceInterface(receiver, pyConfigView),
        numRxAntennas(receiver_->getNumRxAntennas()),
        direction(receiver_->getDirection()),
        relativeErrorPower(wns::Ratio::from_dB(pyConfigView.get<double>("relativeError"))),
        receiver(receiver_)
{
}

imtaphy::detail::ComplexFloatMatrixPtr
GaussianErrorInterferenceAndNoiseCovariance::computeNoiseAndInterferenceCovariance(imtaphy::receivers::InterferersCollectionPtr interferers, const imtaphy::detail::ComplexFloatMatrixPtr noiseOnlyMatrix, unsigned int prb)
{
    // copy the perfect IandNoiseCovariance matrix
    imtaphy::detail::ComplexFloatMatrixPtr IandNoiseCovariance(new imtaphy::detail::ComplexFloatMatrix(*(receiver->getPerfectInterferenceAndNoiseCovariance(prb, interferers))));
    
//     std::cout << "Norm before=" << imtaphy::detail::matrixNormSquared(*IandNoiseCovariance)<< "\n";
//     imtaphy::detail::displayMatrix(*IandNoiseCovariance);
    
    
    addGaussianError(IandNoiseCovariance, relativeErrorPower);

//     std::cout << "Norm after=" << imtaphy::detail::matrixNormSquared(*IandNoiseCovariance)<< "\n";
//     imtaphy::detail::displayMatrix(*IandNoiseCovariance);

    
    return IandNoiseCovariance;
}

void 
GaussianErrorInterferenceAndNoiseCovariance::addGaussianError(imtaphy::detail::ComplexFloatMatrixPtr matrix, wns::Ratio relativeErrorPower)
{
    assure(matrix->getColumns() == matrix->getRows(), "Must be square.");
    
    std::complex<float> trace = imtaphy::detail::trace(*matrix);
    
    imtaphy::detail::ComplexFloatMatrix noise(matrix->getRows(), matrix->getColumns());
    
    // trace is the total power of I+N
    // the relativeErrorPower should indicate the ratio between (expected) noise power and total I+N power
    
    float iAndNpower = trace.real();
    float noisePower = iAndNpower * relativeErrorPower.get_factor();

    // after we add the noise*noise^H to the perfect covariance, we scale everything so that the 
    // expected I+N power remains unchanged
    float scaling = iAndNpower / (iAndNpower + noisePower);
    
    imtaphy::detail::fillWhiteGaussianNoise(noise, noisePower / static_cast<float>(matrix->getRows() * matrix->getColumns()));
    
    imtaphy::detail::matrixMultiplyCequalsAlphaSquareTimesAAhermitianPlusC(*matrix, // C
                                                                           noise,  // A
                                                                           static_cast<float>(1.0)     // alpha
                                                                          );
    // now scale down the sum of covariane + noise*noise^H
    imtaphy::detail::scaleMatrixA(*matrix, scaling);
    
}
