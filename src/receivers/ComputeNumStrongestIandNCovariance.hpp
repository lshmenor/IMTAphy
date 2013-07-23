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

#ifndef IMTAPHY_RECEIVERS_DIAGONALNOISEANDINTERFERENCECOVARIANCE_HPP
#define IMTAPHY_RECEIVERS_DIAGONALNOISEANDINTERFERENCECOVARIANCE_HPP

#include <IMTAPHY/receivers/NoiseAndInterferenceCovarianceBase.hpp>
#include <WNS/Singleton.hpp>
#include <IMTAPHY/detail/LinearAlgebra.hpp>
#include <IMTAPHY/Transmission.hpp>

namespace imtaphy {

    namespace receivers {
        class ComputeNumStrongestIandNCovariance :
             public NoiseAndInterferenceCovarianceBase
        {
        public:
            ComputeNumStrongestIandNCovariance(unsigned int _numRxAntennas,
                                               int numToConsider):
                   NoiseAndInterferenceCovarianceBase(_numRxAntennas)

            {
                // if negative consider all
                if (numToConsider < 0)
                    numStrongestToConsider = std::numeric_limits<int>::max();
                else
                    numStrongestToConsider = numToConsider;
            };
            
            imtaphy::detail::ComplexFloatMatrixPtr computeNoiseAndInterferenceCovariance(InterferencePowerMap &interferers, const imtaphy::detail::ComplexFloatMatrixPtr noiseOnlyMatrix, int excludeInterferer);
            
        protected:
            unsigned int numStrongestToConsider;
        private:
                
        };

    }}

#endif //
