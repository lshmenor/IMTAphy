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

#ifndef LTEA_MAC_LINKADAPTATION_UPLINK_SINRTHRESHOLDLA
#define LTEA_MAC_LINKADAPTATION_UPLINK_SINRTHRESHOLDLA

#include <IMTAPHY/ltea/mac/linkAdaptation/uplink/LinkAdaptationBase.hpp>

namespace ltea { namespace mac { namespace la { namespace uplink {
   
        class SINRThresholdLinkAdaptation :
        public LinkAdaptationBase
        {
        public:
            SINRThresholdLinkAdaptation(imtaphy::StationPhy* station, const wns::pyconfig::View& pyConfigView);
            
            virtual void updateChannelStatus(unsigned int tti);
            virtual void updateBLERStatistics(unsigned int tti);
            virtual void updateAssociatedUsers(std::vector<wns::node::Interface*> allUsers);
        };
}}}}
#endif
