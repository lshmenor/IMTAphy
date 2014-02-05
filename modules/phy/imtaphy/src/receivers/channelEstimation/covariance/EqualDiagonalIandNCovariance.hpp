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

#ifndef IMTAPHY_RECEIVERS_EQUALDIAGONALNOISEANDINTERFERENCECOVARIANCE_HPP
#define IMTAPHY_RECEIVERS_EQUALDIAGONALNOISEANDINTERFERENCECOVARIANCE_HPP

#include <IMTAPHY/receivers/channelEstimation/covariance/NoiseAndInterferenceCovarianceBase.hpp>
#include <WNS/Singleton.hpp>
#include <IMTAPHY/detail/LinearAlgebra.hpp>
#include <IMTAPHY/Transmission.hpp>

namespace imtaphy {

    namespace receivers {
        
        class LinearReceiver;
        
        namespace channelEstimation { namespace covariance {
        
        class EqualDiagonalIandNCovariance :
             public NoiseAndInterferenceCovarianceInterface
        {
        public:
            EqualDiagonalIandNCovariance(LinearReceiver* receiver);
            
            EqualDiagonalIandNCovariance(LinearReceiver* receiver, const wns::pyconfig::View& pyConfigView);

            imtaphy::detail::ComplexFloatMatrixPtr computeNoiseAndInterferenceCovariance(InterferersCollectionPtr interferers, const imtaphy::detail::ComplexFloatMatrixPtr noiseOnlyMatrix, unsigned int prb);
            
        protected:
            unsigned int numRxAntennas;
            imtaphy::Direction direction;
            imtaphy::receivers::LinearReceiver* receiver;
        };

    }}}}

#endif //
