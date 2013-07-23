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

#include <IMTAPHY/ltea/mac/linkAdaptation/uplink/SINRThresholdLA.hpp>
#include <WNS/PyConfigViewCreator.hpp>


STATIC_FACTORY_REGISTER_WITH_CREATOR(
    ltea::mac::la::uplink::SINRThresholdLinkAdaptation,
    ltea::mac::la::uplink::LinkAdaptationInterface,
    "ltea.dll.linkAdaptation.uplink.SINRThreshold",
    imtaphy::StationModuleCreator);

using namespace ltea::mac::la::uplink;

SINRThresholdLinkAdaptation::SINRThresholdLinkAdaptation(imtaphy::StationPhy* station, const wns::pyconfig::View& pyConfigView) : 
    LinkAdaptationBase(station, pyConfigView)
{
    globalThreshold = wns::Ratio::from_dB(pyConfigView.get<double>("threshold_dB"));
    setChannelStatusManager(imtaphy::receivers::feedback::UplinkChannelStatusManagerInterface::getCSM());
}



void 
SINRThresholdLinkAdaptation::updateAssociatedUsers(std::vector< wns::node::Interface* > allUsers)
{
    for (unsigned int i = 0; i < allUsers.size(); i++)
    {
        sinrEstimates[allUsers[i]] = PRBsinrMap();
    }
}


void 
SINRThresholdLinkAdaptation::updateBLERStatistics(unsigned int tti)
{
    // we don't take this into account here
}

void ltea::mac::la::uplink::SINRThresholdLinkAdaptation::updateChannelStatus(unsigned int tti)
{
    // we don't take this into account here
    for (NodePRBsinrMap::iterator userIter = sinrEstimates.begin(); userIter != sinrEstimates.end(); userIter++)
    {
        imtaphy::receivers::feedback::LteRel10UplinkChannelStatusPtr status = channelStatusManager->getChannelState(userIter->first, tti);
        
        for (unsigned int f = 0; f < status->numPRBs; f++)
        {
            userIter->second[f] = status->sinrsTb1[f]; 
        }
    }
}
