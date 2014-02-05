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

#ifndef IMTAPHY_RECEIVERS_CHANNELESTIMATION_IANDNCOVARIANCEBASED_HPP
#define IMTAPHY_RECEIVERS_CHANNELESTIMATION_IANDNCOVARIANCEBASED_HPP

#include <IMTAPHY/receivers/channelEstimation/channel/ChannelEstimationInterface.hpp>

namespace imtaphy {

    namespace receivers {
        
        namespace channelEstimation { namespace channel {
        
        class IandNCovarianceBasedGaussianError :
             public ChannelEstimationInterface
        {
        public:
            IandNCovarianceBasedGaussianError(LinearReceiver* receiver);
            
            IandNCovarianceBasedGaussianError(LinearReceiver* receiver, const wns::pyconfig::View& pyConfigView);

            detail::ComplexFloatMatrixPtr estimateChannel(interface::PRB prb, Link* link, wns::Power txPower);
            
        protected:
            unsigned int numRxAntennas;
            imtaphy::Direction direction;
            imtaphy::receivers::LinearReceiver* receiver;
            wns::Ratio gainOverIandN;
            bool coloredEstimationError;
        };

    }}}}

#endif //
