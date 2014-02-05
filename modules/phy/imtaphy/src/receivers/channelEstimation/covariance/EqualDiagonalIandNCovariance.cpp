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

#include <IMTAPHY/receivers/channelEstimation/covariance/EqualDiagonalIandNCovariance.hpp>
#include <IMTAPHY/receivers/LinearReceiver.hpp>
#include <IMTAPHY/receivers/Interferer.hpp>

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    imtaphy::receivers::channelEstimation::covariance::EqualDiagonalIandNCovariance,
    imtaphy::receivers::channelEstimation::covariance::NoiseAndInterferenceCovarianceInterface,
    "imtaphy.receiver.covarianceEstimation.EqualDiagonal",
    imtaphy::receivers::ReceiverModuleCreator);

using namespace imtaphy::receivers::channelEstimation::covariance;

EqualDiagonalIandNCovariance::EqualDiagonalIandNCovariance(LinearReceiver* receiver_) :
        NoiseAndInterferenceCovarianceInterface(receiver),
        numRxAntennas(receiver_->getNumRxAntennas()),
        direction(receiver_->getDirection()),
        receiver(receiver_)
{
}

EqualDiagonalIandNCovariance::EqualDiagonalIandNCovariance(LinearReceiver* receiver_, const wns::pyconfig::View& pyConfigView) :
        NoiseAndInterferenceCovarianceInterface(receiver, pyConfigView),
        numRxAntennas(receiver_->getNumRxAntennas()),
        direction(receiver_->getDirection()),
        receiver(receiver_)
{
}

imtaphy::detail::ComplexFloatMatrixPtr
EqualDiagonalIandNCovariance::computeNoiseAndInterferenceCovariance(imtaphy::receivers::InterferersCollectionPtr interferers, const imtaphy::detail::ComplexFloatMatrixPtr noiseOnlyMatrix, unsigned int prb)
{
    // copy the perfect IandNoiseCovariance matrix
    imtaphy::detail::ComplexFloatMatrixPtr IandNoiseCovariance(new imtaphy::detail::ComplexFloatMatrix(*(receiver->getPerfectInterferenceAndNoiseCovariance(prb, interferers))));

    std::complex<float> trace = imtaphy::detail::trace(*IandNoiseCovariance);
    
    for (unsigned int i =  0; i < IandNoiseCovariance->getRows(); i++)
        for (unsigned int j = 0; j < IandNoiseCovariance->getColumns(); j++)
        {
            if (i == j)
                (*IandNoiseCovariance)[i][i] = trace / static_cast<float>(IandNoiseCovariance->getColumns());
            else
                (*IandNoiseCovariance)[i][j] = (*IandNoiseCovariance)[j][i] = std::complex<float>(0.0, 0.0);
        }
        

    
    return IandNoiseCovariance;
}
