/*******************************************************************************
 * This file is part of IMTAphy
 * _____________________________________________________________________________
 *
 * Copyright (C) 2012
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

#ifndef LTEA_MAC_SCHEDULER_RESOURCEMANAGERINTERFACE
#define LTEA_MAC_SCHEDULER_RESOURCEMANAGERINTERFACE

#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>
#include <IMTAPHY/ltea/mac/scheduler/UsersPRBManager.hpp>
#include <IMTAPHY/receivers/feedback/LteRel8DLFeedbackManager.hpp>
#include <IMTAPHY/receivers/feedback/LteRel10UplinkChannelStatusManager.hpp>
#include <IMTAPHY/StationPhy.hpp>

namespace ltea { namespace mac { namespace scheduler {

        class ResourceManagerInterface 
        {
        public:
            ResourceManagerInterface(const wns::pyconfig::View& pyConfigView) {}
            
            // harq and feedbackManager could be added as well
            virtual void initResourceManager(imtaphy::Channel* channel, imtaphy::StationPhy* station) = 0;
            virtual void setDownlinkFeedbackManager(imtaphy::receivers::feedback::DownlinkFeedbackManagerInterface* feedbackManager) = 0;
            virtual void setUplinkStatusManager(imtaphy::receivers::feedback::UplinkChannelStatusManagerInterface* uplinkChannelStatus) = 0;

            virtual void determineResourceRestrictions(ltea::mac::scheduler::UsersPRBManager& usersPRBManager, imtaphy::Direction direction) = 0;

        };
        
        class ResourceManagerBase :
            public ResourceManagerInterface
        {
            public:
                ResourceManagerBase(const wns::pyconfig::View& pyConfigView);
            
                virtual void initResourceManager(imtaphy::Channel* channel, imtaphy::StationPhy* station);
                virtual void setDownlinkFeedbackManager(imtaphy::receivers::feedback::DownlinkFeedbackManagerInterface* feedbackManager);
                virtual void setUplinkStatusManager(imtaphy::receivers::feedback::UplinkChannelStatusManagerInterface* uplinkChannelStatus);
                virtual void determineResourceRestrictions(ltea::mac::scheduler::UsersPRBManager& usersPRBManager, imtaphy::Direction direction);

            protected:
                wns::logger::Logger logger;
                imtaphy::Channel* channel;
                imtaphy::StationPhy* myStation;
                imtaphy::receivers::feedback::DownlinkFeedbackManagerInterface* feedbackManager;
                imtaphy::receivers::feedback::UplinkChannelStatusManagerInterface* uplinkChannelStatus;
        };   
}}}
#endif
