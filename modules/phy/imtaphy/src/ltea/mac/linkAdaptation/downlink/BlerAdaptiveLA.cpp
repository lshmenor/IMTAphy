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

#include <IMTAPHY/ltea/mac/linkAdaptation/downlink/BlerAdaptiveLA.hpp>
#include <WNS/PyConfigViewCreator.hpp>
#include <IMTAPHY/link2System/MMIBeffectiveSINR.hpp>
#include <IMTAPHY/ltea/mac/ModulationAndCodingSchemes.hpp>

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    ltea::mac::la::downlink::BLERAdaptiveLinkAdaptation,
    ltea::mac::la::downlink::LinkAdaptationInterface,
    "ltea.dll.linkAdaptation.downlink.BLERAdaptive",
    wns::PyConfigViewCreator);

using namespace ltea::mac::la::downlink;


BLERAdaptiveLinkAdaptation::BLERAdaptiveLinkAdaptation(const wns::pyconfig::View& pyConfigView) : 
    SINRThresholdLinkAdaptation(pyConfigView),
    updateInterval(pyConfigView.get<unsigned int>("updateInterval"))
{
    assure((updateInterval > 0) && (updateInterval < 10000), "Update interval should be (0, 10000)");
    
    // positive means substracting from SINR -> be more conservative
    double offsetDelta = pyConfigView.get<double>("offsetDelta");
    double targetBLER = pyConfigView.get<double>("targetBLER");
    assure((targetBLER > 0.0) && (targetBLER < 1.0), "Target block error rate (BLER) must be between (0, 1)");
    
    nackOffset = offsetDelta;
    ackOffset = -1.0 * offsetDelta * targetBLER / (1.0 - targetBLER);

}


void 
BLERAdaptiveLinkAdaptation::updateBLERStatistics(unsigned int tti)
{
    if ((tti % updateInterval) == 0)
    {
        for (NodeRatioMap::iterator iter = thresholdMap.begin(); iter != thresholdMap.end(); iter++)
        {
            wns::node::Interface* user = iter->first;
            
            if (harq->knowsUser(user))
            {
                unsigned int currentACK = harq->getACKcount(user);
                unsigned int currentNACK = harq->getNACKcount(user);

                // do not update again until we are sure to have seen the consequences
                // and ignore what happened in between
                if (currentACK + currentNACK > ackCounter[user] + nackCounter[user])
                {
                    if (skipUntil[user] > tti)
                    {
                        ackCounter[user] = currentACK;
                        nackCounter[user] = currentNACK;

                        return;
                    }
                    else
                    {
                        skipUntil[user] = tti + 4;
                    }
                }
                
                double offset = static_cast<double>(currentACK - ackCounter[user]) * ackOffset + static_cast<double>(currentNACK - nackCounter[user]) * nackOffset;

                double schedulingRatio = static_cast<double>(currentACK + currentNACK) / static_cast<double>(tti);

                if (schedulingRatio > 0)
                {
                    offset /= schedulingRatio;        
                }
                
                double old = iter->second.get_dB();
                
                iter->second =  wns::Ratio::from_dB(old + offset);
                
                ackCounter[user] = currentACK;
                nackCounter[user] = currentNACK;
            }
        }
    }
}

void ltea::mac::la::downlink::BLERAdaptiveLinkAdaptation::updateAssociatedUsers(std::vector< wns::node::Interface* > allUsers)
{
    ltea::mac::la::downlink::SINRThresholdLinkAdaptation::updateAssociatedUsers(allUsers);

    for (unsigned int i = 0; i < allUsers.size(); i++)
    {
        ackCounter[allUsers[i]] = 0;
        nackCounter[allUsers[i]] = 0;
        skipUntil[allUsers[i]] = 0;
    }

}

