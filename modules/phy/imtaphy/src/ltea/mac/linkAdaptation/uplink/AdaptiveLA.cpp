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

#include <IMTAPHY/ltea/mac/linkAdaptation/uplink/AdaptiveLA.hpp>
#include <WNS/PyConfigViewCreator.hpp>
#include <IMTAPHY/Channel.hpp>
#include <IMTAPHY/Spectrum.hpp>

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    ltea::mac::la::uplink::AdaptiveLinkAdaptation,
    ltea::mac::la::uplink::LinkAdaptationInterface,
    "ltea.dll.linkAdaptation.uplink.Adaptive",
    imtaphy::StationModuleCreator);

using namespace ltea::mac::la::uplink;

AdaptiveLinkAdaptation::AdaptiveLinkAdaptation(imtaphy::StationPhy* station, const wns::pyconfig::View& pyConfigView) : 
    LinkAdaptationBase(station, pyConfigView),
    fastCrossingWeight(pyConfigView.get<double>("fastCrossingWeight")),
    crossingThreshold(pyConfigView.get<double>("crossingThreshold")),
    longTimeWeight(pyConfigView.get<double>("longTimeWeight"))
{
    globalThreshold = wns::Ratio::from_dB(pyConfigView.get<double>("threshold_dB"));

    setChannelStatusManager(
        imtaphy::receivers::feedback::UplinkChannelStatusManagerInterface::getCSM());
    
    wns::probe::bus::ContextProviderCollection localcpc(myStation->getNode()->getContextProviderCollection());
    variationsContextCollector =  wns::probe::bus::ContextCollectorPtr(
                                new wns::probe::bus::ContextCollector(localcpc, "uplinkChannelVariations"));
}



void 
AdaptiveLinkAdaptation::updateAssociatedUsers(std::vector< wns::node::Interface* > allUsers_)
{
    
    allUsers = allUsers_;
    numPRBs = imtaphy::TheIMTAChannel::getInstance()->getSpectrum()->getNumberOfPRBs(imtaphy::Uplink);
    
    // TODO: do not reset if already existing
    // TODO: delete users that are no longer associated
    
    for (unsigned int i = 0; i < allUsers.size(); i++)
    {
        for (unsigned int f = 0; f < numPRBs; f++)
        {
            sinrEstimates[allUsers[i]][f] = wns::Ratio::from_dB(0);
            aboveLongTime[allUsers[i]][f] = false;
            crossings[allUsers[i]][f] = 0;
        }
    }
}


void 
AdaptiveLinkAdaptation::updateBLERStatistics(unsigned int tti)
{
    // we don't take this into account here
}

void ltea::mac::la::uplink::AdaptiveLinkAdaptation::updateChannelStatus(unsigned int tti)
{
    // we don't take this into account here
    for (unsigned int u = 0; u < allUsers.size(); u++)
    {
        wns::node::Interface* user = allUsers[u];
        imtaphy::receivers::feedback::LteRel10UplinkChannelStatusPtr status = channelStatusManager->getChannelState(user, tti);
 
        assure(status->numPRBs == sinrEstimates[user].size(), "Wrong number of PRBs");
        
        for (unsigned int f = 0; f < status->numPRBs; f++)
        {
            MESSAGE_SINGLE(NORMAL, logger, "Updating SINR estimate of user " << user->getName() << " on PRB " << f << " old estimate: " << sinrEstimates[user][f]);
            
            if (longTimeAvg[user].find(f) != longTimeAvg[user].end())
            {
                if ((status->sinrsTb1[f] > longTimeAvg[user][f]) && (!aboveLongTime[user][f]))
                {
                    // crossing long time avg. SINR in pos. direction
                    crossings[user][f]++;
                }
                
                if (status->sinrsTb1[f] > longTimeAvg[user][f])
                    aboveLongTime[user][f] = true;
                else
                    aboveLongTime[user][f] = false;

                if (variationsContextCollector->hasObservers())
                {
                    variationsContextCollector->put(double(tti) / double(crossings[user][f]) , boost::make_tuple("MSID", user->getNodeID(),
                                                                                            "PRB", f));
                }
                
                longTimeAvg[user][f] = wns::Ratio::from_dB(longTimeWeight * status->sinrsTb1[f].get_dB() + (1.0 - longTimeWeight) * longTimeAvg[user][f].get_dB()); 
            }
            else
            {
                if (tti > 15)
                    longTimeAvg[user][f] = status->sinrsTb1[f];
            }

            double x = double(tti) / double(crossings[user][f]);
            double weighting;
            
            if (x > crossingThreshold)
            {
                weighting = 1.0;
            }
            else
            {
                weighting = fastCrossingWeight;
            }
                
            sinrEstimates[user][f] = wns::Ratio::from_dB(weighting * status->sinrsTb1[f].get_dB() + (1.0 - weighting) * sinrEstimates[user][f].get_dB()); 

            
            MESSAGE_SINGLE(NORMAL, logger, "New estimate for user " << user->getName() << " on PRB " << f <<": " << sinrEstimates[user][f]);

        }
    }
}
