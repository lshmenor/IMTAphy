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

#ifndef LTEA_MAC_LINKADAPTATION_DOWNLINK_BLERADAPTAIVELA
#define LTEA_MAC_LINKADAPTATION_DOWNLINK_BLERADAPTAIVELA

#include <IMTAPHY/ltea/mac/linkAdaptation/downlink/SINRThresholdLA.hpp>
#include <IMTAPHY/link2System/EffectiveSINRModelInterface.hpp>
#include <IMTAPHY/link2System/LteCQIs.hpp>

namespace ltea { namespace mac { namespace la { namespace downlink {
   
        class BLERAdaptiveLinkAdaptation :
        public SINRThresholdLinkAdaptation
        {
        public:
            BLERAdaptiveLinkAdaptation(const wns::pyconfig::View& pyConfigView);
            
            void updateBLERStatistics(unsigned int tti);
            void updateAssociatedUsers(std::vector<wns::node::Interface*> allUsers);
            
        private:
            std::map<wns::node::Interface*, unsigned int> ackCounter;
            std::map<wns::node::Interface*, unsigned int> nackCounter;
            std::map<wns::node::Interface*, unsigned int> skipUntil;

            
            unsigned int updateInterval;
            double nackOffset;
            double ackOffset;
        };
}}}}
#endif
