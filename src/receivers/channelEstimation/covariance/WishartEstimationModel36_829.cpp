/*******************************************************************************
 * This file is part of IMTAphy
 * _____________________________________________________________________________
 *
 * Copyright (C) 2012
 * Institute for Communication Networks (LKN)
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

#include <IMTAPHY/receivers/channelEstimation/covariance/WishartEstimationModel36_829.hpp>
#include <IMTAPHY/receivers/LinearReceiver.hpp>
#include <IMTAPHY/receivers/Interferer.hpp>

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    imtaphy::receivers::channelEstimation::covariance::WishartEstimationModel36_829,
    imtaphy::receivers::channelEstimation::covariance::NoiseAndInterferenceCovarianceInterface,
    "imtaphy.receiver.covarianceEstimation.WishartModel36829",
    imtaphy::receivers::ReceiverModuleCreator);

using namespace imtaphy::receivers::channelEstimation::covariance;

WishartEstimationModel36_829::WishartEstimationModel36_829(LinearReceiver* receiver_) :
        NoiseAndInterferenceCovarianceInterface(receiver),
        numRxAntennas(receiver_->getNumRxAntennas()),
        direction(receiver_->getDirection()),
        receiver(receiver_),
        numberOfSamples(16) // some default number
{
}

WishartEstimationModel36_829::WishartEstimationModel36_829(LinearReceiver* receiver_, const wns::pyconfig::View& pyConfigView) :
        NoiseAndInterferenceCovarianceInterface(receiver, pyConfigView),
        numRxAntennas(receiver_->getNumRxAntennas()),
        direction(receiver_->getDirection()),
        receiver(receiver_),
        numberOfSamples(pyConfigView.get<unsigned int>("numberOfSamples"))
{
}

imtaphy::detail::ComplexFloatMatrixPtr
WishartEstimationModel36_829::computeNoiseAndInterferenceCovariance(imtaphy::receivers::InterferersCollectionPtr interferers, const imtaphy::detail::ComplexFloatMatrixPtr noiseOnlyMatrix, unsigned int prb)
{
    imtaphy::detail::ComplexFloatMatrixPtr perfectCovariance = receiver->getPerfectInterferenceAndNoiseCovariance(prb, interferers);

    imtaphy::detail::ComplexFloatMatrixPtr IandNoiseCovariance(new imtaphy::detail::ComplexFloatMatrix(perfectCovariance->getRows(), perfectCovariance->getColumns()));
    
    imtaphy::detail::ComplexFloatMatrix choleskyFactor(perfectCovariance->getRows(), perfectCovariance->getColumns());
    imtaphy::detail::performCholeskyFactorization(*perfectCovariance, choleskyFactor);
                                    
    imtaphy::detail::ComplexFloatMatrix A(perfectCovariance->getRows(), perfectCovariance->getColumns());
    imtaphy::detail::generateLowerTriangularBartlettDecompositionMatrix(A, numberOfSamples);
    
    imtaphy::detail::ComplexFloatMatrix product(perfectCovariance->getRows(), perfectCovariance->getColumns());
    imtaphy::detail::matrixMultiplyCequalsAB(product, choleskyFactor, A);
    
    imtaphy::detail::matrixMultiplyCequalsAAhermitian(*IandNoiseCovariance, product);
    imtaphy::detail::scaleMatrixA(*IandNoiseCovariance, 1.0f / static_cast<float>(numberOfSamples));


    return IandNoiseCovariance;
}
