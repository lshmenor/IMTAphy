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

#ifndef LTEA_MAC_LINKADAPTATION_UPLINK_LINKADAPTATIONINTERFACE
#define LTEA_MAC_LINKADAPTATION_UPLINK_LINKADAPTATIONINTERFACE

#include <WNS/pyconfig/View.hpp>
#include <WNS/node/Interface.hpp>
#include <WNS/PowerRatio.hpp>
#include <WNS/logger/Logger.hpp>

#include <IMTAPHY/interface/TransmissionStatus.hpp>

#include <IMTAPHY/receivers/feedback/LteRel10UplinkChannelStatusManager.hpp>

#include <IMTAPHY/ltea/mac/ModulationAndCodingSchemes.hpp>
#include <IMTAPHY/ltea/mac/harq/HARQ.hpp>
#include <IMTAPHY/ltea/mac/DCI.hpp>
#include <IMTAPHY/StationPhy.hpp>
#include <map>


namespace ltea { namespace mac { 
        
        namespace scheduler { namespace uplink { namespace enb {
            class PowerControlInterface;
        }}}
    
    
        namespace la { namespace uplink {
    
       
        
        typedef std::map<imtaphy::interface::PRB, wns::Ratio> PRBpowerOffsetMap;
    
        struct LinkAdaptationResult
        {
            unsigned int mcsIndex;
            double codeRate;
            imtaphy::l2s::ModulationScheme modulation;
            wns::Ratio estimatedSINR;
            wns::Power txPowerPerPRB;
            PRBSchedulingTracingDictMap prbTracingDict;
        };
    
        class LinkAdaptationInterface 
        {
        public:
            LinkAdaptationInterface(imtaphy::StationPhy* station, const wns::pyconfig::View& pyConfigView) {}
            
            virtual void setChannelStatusManager(imtaphy::receivers::feedback::UplinkChannelStatusManagerInterface* channelStatusManager) = 0;
            virtual void setHARQ(ltea::mac::harq::HARQ* harq) = 0;
            virtual void setPowerControlInterface(ltea::mac::scheduler::uplink::enb::PowerControlInterface* powerControl) = 0;
            virtual void updateAssociatedUsers(std::vector<wns::node::Interface*> allUsers) = 0;
            
            virtual void updateChannelStatus(unsigned int tti) = 0;
            virtual void updateBLERStatistics(unsigned int tti) = 0;
             
            virtual LinkAdaptationResult performLinkAdaptation(wns::node::Interface* userID, 
                                                               unsigned int spatialTransportBlock, 
                                                               const std::vector<unsigned int>& prbs,
                                                               unsigned int tti,
                                                               unsigned int numLayersForTB, 
                                                               bool hasSRS,
                                                               double Ks
                                                               ) = 0;                     
        };
        
        typedef imtaphy::StationModuleCreator<LinkAdaptationInterface> LinkAdaptationCreator;
        typedef wns::StaticFactory<LinkAdaptationCreator> LinkAdaptationFactory;
        

        
}}}}
#endif
