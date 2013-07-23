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

#include <IMTAPHY/ltea/mac/scheduler/ResourceManagerInterface.hpp>
#include <iostream>

using namespace ltea::mac::scheduler;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    ltea::mac::scheduler::ResourceManagerBase,
    ltea::mac::scheduler::ResourceManagerInterface,
    "ltea.dll.schedulers.resourceManager.ResourceManagerBase",
    wns::PyConfigViewCreator);
    
    
ResourceManagerBase::ResourceManagerBase(const wns::pyconfig::View& pyConfigView) :
    ResourceManagerInterface(pyConfigView),
    logger(pyConfigView.get("logger")),
    feedbackManager(NULL),
    uplinkChannelStatus(NULL)
{
};
    
void 
ResourceManagerBase::determineResourceRestrictions(ltea::mac::scheduler::UsersPRBManager& usersPRBManager, imtaphy::Direction direction)
{
    MESSAGE_SINGLE(NORMAL, logger, "Not applying any resource restrictions.");
}

void 
ResourceManagerBase::setDownlinkFeedbackManager(imtaphy::receivers::feedback::DownlinkFeedbackManagerInterface* feedbackManager_)
{
    assure(feedbackManager_, "Need feedback manager pointer");
    feedbackManager = feedbackManager_;
}


void 
ResourceManagerBase::setUplinkStatusManager(imtaphy::receivers::feedback::UplinkChannelStatusManagerInterface* uplinkChannelStatus_)
{
    assure(uplinkChannelStatus_, "Need uplink channel status pointer");
    uplinkChannelStatus = uplinkChannelStatus_;
}

void
ResourceManagerBase::initResourceManager(imtaphy::Channel* channel_, imtaphy::StationPhy* station_)
{
    assure(channel_, "Need channel pointer");
    assure(station_, "Need station pointer");

    channel = channel_;
    myStation = station_;
}
    