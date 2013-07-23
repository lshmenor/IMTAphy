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

#ifndef IMTAPHY_RECEIVERS_MMSE_HPP
#define IMTAPHY_RECEIVERS_MMSE_HPP

#include <WNS/pyconfig/View.hpp>
#include <IMTAPHY/receivers/NoiseAndInterferenceCovarianceBase.hpp>
#include <IMTAPHY/receivers/LinearReceiver.hpp>

namespace imtaphy {

    namespace receivers {

        namespace tests {
            class MMSEReceiverTest;
        }
        
        class MMSEBase :
            public LinearReceiver
        {
        public:
            MMSEBase(StationPhy* station, const wns::pyconfig::View& pyConfigView);
            
            virtual void channelInitialized(Channel* channel);
            
            friend class imtaphy::receivers::tests::MMSEReceiverTest;
                                          
        protected:
            virtual void computeFilter(imtaphy::detail::ComplexFloatMatrix& W, imtaphy::detail::ComplexFloatMatrix& precodedH, InterferencePowerMap &interferers, unsigned int m);
                        
            imtaphy::detail::ComplexFloatMatrixPtr ServingPrecodedChannelCovariance;
            imtaphy::detail::ComplexFloatMatrixPtr inverse; // this is for MMSE only
            
            imtaphy::receivers::NoiseAndInterferenceCovarianceBase* noiseAndIcovariance;
            
        private:

        };
    }}

#endif //
