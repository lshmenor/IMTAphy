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

#ifndef LTEA_MAC_LINKADAPTATION_DOWNLINK_SINRTHRESHOLDLA
#define LTEA_MAC_LINKADAPTATION_DOWNLINK_SINRTHRESHOLDLA

#include <IMTAPHY/ltea/mac/linkAdaptation/downlink/LinkAdaptationInterface.hpp>
#include <IMTAPHY/link2System/EffectiveSINRModelInterface.hpp>
#include <IMTAPHY/link2System/LteCQIs.hpp>
#include <IMTAPHY/detail/NodePtrCompare.hpp>

namespace ltea { namespace mac { namespace la { namespace downlink {
    
        class SINRThresholdLinkAdaptation :
        public LinkAdaptationBase
        {
        public:
            SINRThresholdLinkAdaptation(const wns::pyconfig::View& pyConfigView);
            
            virtual void updateFeedback(unsigned int tti);
            virtual void updateBLERStatistics(unsigned int tti);
            virtual void updateAssociatedUsers(std::vector<wns::node::Interface*> allUsers);
            
            virtual LinkAdaptationResult performLinkAdaptation(wns::node::Interface* userID, 
                                                               unsigned int spatialLayer, 
                                                               PRBpowerOffsetMap& prbMap, 
                                                               unsigned int tti,
                                                               unsigned int numLayersForTB, 
                                                               unsigned int pdcchLength,
                                                               bool provideRel10DMRS,
                                                               unsigned int numRel10CSIrsSets
                                                               );  
            typedef std::map<wns::node::Interface*, wns::Ratio, imtaphy::detail::WnsNodeInterfacePtrCompare> NodeRatioMap;
        protected:
            wns::Ratio globalThreshold;
            NodeRatioMap thresholdMap;
            imtaphy::l2s::EffectiveSINRModelInterface* effectiveSINR;
        };
}}}}
#endif
