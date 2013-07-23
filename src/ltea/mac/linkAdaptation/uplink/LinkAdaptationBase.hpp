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

#ifndef LTEA_MAC_LINKADAPTATION_UPLINK_LINKADAPTATIONBASE
#define LTEA_MAC_LINKADAPTATION_UPLINK_LINKADAPTATIONBASE

#include <IMTAPHY/ltea/mac/linkAdaptation/uplink/LinkAdaptationInterface.hpp>
#include <IMTAPHY/link2System/MMSE-FDE.hpp>
#include <IMTAPHY/link2System/LteCQIs.hpp>

namespace ltea { namespace mac { 
        namespace la { namespace uplink {

            typedef std::map<unsigned int, wns::Ratio> PRBsinrMap;
            typedef std::map<wns::node::Interface*, PRBsinrMap> NodePRBsinrMap;


        class LinkAdaptationBase :
            public LinkAdaptationInterface
        {
            public:
                LinkAdaptationBase(imtaphy::StationPhy* station, const wns::pyconfig::View& pyConfigView) :
                    LinkAdaptationInterface(station, pyConfigView),
                    channelStatusManager(NULL),
                    harq(NULL),
                    mcsLookup(ltea::mac::TheMCSLookup::getInstance()),
                    logger(pyConfigView.get<wns::pyconfig::View>("logger")),
                    myStation(station)
                    {
                    };
                    
                virtual void setChannelStatusManager(imtaphy::receivers::feedback::UplinkChannelStatusManagerInterface* channelStatusManager_)
                {
                    channelStatusManager = channelStatusManager_;
                }
                
                virtual void setHARQ(ltea::mac::harq::HARQ* harq_)
                {
                    harq = harq_;
                }
                
                virtual void setPowerControlInterface(ltea::mac::scheduler::uplink::enb::PowerControlInterface* powerControl_)
                {
                    powerControl = powerControl_;
                }
                
                virtual LinkAdaptationResult performLinkAdaptation(wns::node::Interface* userID, 
                                                   unsigned int spatialTransportBlock,
                                                   const std::vector<unsigned int>& prbs,
                                                   unsigned int tti, 
                                                   unsigned int numLayersForTB, 
                                                   bool hasSRS,
                                                   double Ks);
            
            protected:
                imtaphy::receivers::feedback::UplinkChannelStatusManagerInterface* channelStatusManager;
                ltea::mac::harq::HARQ* harq;
                ltea::mac::ModulationAndCodingSchemes* mcsLookup;
                wns::logger::Logger logger;
                ltea::mac::scheduler::uplink::enb::PowerControlInterface* powerControl;
                imtaphy::l2s::MMSEFrequencyDomainEqualization mmseFDE;
                imtaphy::StationPhy* myStation;
            
                wns::Ratio globalThreshold;
                
                NodePRBsinrMap sinrEstimates;
        };   
}}}}
#endif
