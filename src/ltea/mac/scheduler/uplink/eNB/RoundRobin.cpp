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

#include <IMTAPHY/ltea/mac/scheduler/uplink/eNB/RoundRobin.hpp>
#include <algorithm>

STATIC_FACTORY_REGISTER_WITH_CREATOR(
     ltea::mac::scheduler::uplink::enb::RoundRobin,
     wns::ldk::FunctionalUnit,
     "ltea.mac.scheduler.uplink.RoundRobin",
     wns::ldk::FUNConfigCreator);

     

using namespace ltea::mac::scheduler::uplink::enb;

RoundRobin::RoundRobin(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config) :
    SchedulerBase(fun, config),
    currentUser(0),
    threegppCalibration(config.get<bool>("threegppCalibration")),
    rngForShuffle(*wns::simulator::getRNG(), uni),
    threeGPPConstellationFixed(false)
{
}


void 
RoundRobin::initScheduler()
{
    MESSAGE_SINGLE(NORMAL, logger, "init in UL eNB RR scheduler");
}


void 
RoundRobin::doScheduling()
{
    // we do the round robin based on all users in the cell and filter for active users later
    if ((allUsers.size() > 0) && (currentUsersPRBManager->getActiveUsers().size() > 0))
    {
        unsigned int Mh, Ml, Nh, Nl;
        if (threegppCalibration)
        {
            if (!threeGPPConstellationFixed)
            {
                // if we have not yet determined the fixed schedule for calibration, do it now
                
                // 3GPP TR 36.814:
                // Frequency Domain Multiplexing non-channel dependent, share available bandwidth between users connected to the cell, 
                // all users get resources in every uplink subframe. With M users and Nrb PRBs available, 
                // Mh =mod(Nrb,M) users get floor(Nrb/M)+1 PRBs whereas Ml=M-Mh users get floor(Nrb/M) PRBs
                unsigned int numActiveUsers = currentUsersPRBManager->getActiveUsers().size();    
                Mh = currentUsersPRBManager->getNumPRBsAvailable() % numActiveUsers;
                Ml = numActiveUsers - Mh;
                Nh = currentUsersPRBManager->getNumPRBsAvailable() / numActiveUsers + 1; // this is an implicit floor()
                Nl = currentUsersPRBManager->getNumPRBsAvailable() / numActiveUsers;

                assure(Mh*Nh + Ml*Nl == currentUsersPRBManager->getNumPRBsAvailable(), "Does not add up...");

    #ifndef WNS_NDEBUG            
                if (Nl == 0)
                    MESSAGE_SINGLE(NORMAL, logger, "WARNING: Not enough PRBs so that each user could get one. Choose 50 PRBs and 10 users for calibration. But even for these settings it can happen for some PRACH/HARQ combinations");
                //std::cout <<  getFUN()->getLayer()->getNodeName() <<  " numActiveUsers " << numActiveUsers << " PRBs available: " << currentUsersPRBManager->getNumPRBsAvailable() << " MH=" << Mh << " Ml=" << Ml << " Nh=" << Nh << " Nl=" << Nl << "\n";
    #endif
            }
            else // we just use the previous scheduling result
            {
                for (UserSchedulingResultMap::const_iterator iter = threeGPPConstellation.begin(); iter != threeGPPConstellation.end(); iter++)
                {
                    if (! currentUsersPRBManager->isActive(iter->first))
                    {   // if user is not available (i.e., doing a retransmission)
                        continue;
                    }
                    else
                    {
                        scheduledUsers.push_back(iter->second);
                        for (imtaphy::interface::PrbPowerPrecodingMap::const_iterator iter2 = iter->second.prbPowerPrecoders.begin();
                             iter2 != iter->second.prbPowerPrecoders.end(); iter2++)
                        {
                            currentUsersPRBManager->markPRBused(iter2->first);
                        }
                        currentUsersPRBManager->removeActiveUser(iter->first);
                    }
                }
                return; // and we are done
            }
        }
        unsigned int count3GPPusers = 0;

        // randomly permutate the order of users to be scheduled so that
        // not always the same inteference constellations and frequency asisgnments result
        std::random_shuffle(allUsers.begin(), allUsers.end(), rngForShuffle);

        
        for (unsigned int i = 0; i < allUsers.size(); i++)
        {
            if (currentUsersPRBManager->getNumPRBsAvailable() == 0)
                break;
            
            currentUser = (currentUser + 1) % allUsers.size();
            wns::node::Interface* user = allUsers[currentUser];

            if (!currentUsersPRBManager->isActive(user))
                continue;
            
            imtaphy::interface::PRBVector prbs = currentUsersPRBManager->getPRBsAvailable(user);
            if (prbs.size() == 0)
                continue; // can't schedule this user anywhere
            
            unsigned int rank = 1;
                
            ltea::mac::scheduler::uplink::SchedulingResult allocation;
            allocation.scheduledUser = user;
            allocation.prbPowerPrecoders.clear();
            allocation.rank = rank;
            allocation.useDeltaMCS = true;
            allocation.closedLoopPowerControlDelta = wns::Ratio::from_factor(1.0); // no closed-loop power control

            unsigned int prbsThisUser;
            if (threegppCalibration)
            {
                if (count3GPPusers < Ml)
                {
                    prbsThisUser = Nl;
                }
                else
                {
                    prbsThisUser = Nh;
                }
                count3GPPusers++;
                
                assure(prbsThisUser <= prbs.size(), "Somehow a user has less PRBs available during calibration than I want to assign");
            }
            else
            {
                prbsThisUser = prbs.size();
            }
            
            // make sure the UE only transmits with one antenna by choosing an appropriate precoding matrix
            unsigned int numTxAntennasUE = node2LinkMap[user]->getMS()->getAntenna()->getNumberOfElements();
            for (unsigned int prb = 0; prb < prbsThisUser; prb++)
            {
                currentUsersPRBManager->markPRBused(prbs[prb]);
                
                imtaphy::interface::PowerAndPrecoding powerAndPrecoding;
                powerAndPrecoding.precoding = antennaSelection.getPrecodingVector(numTxAntennasUE, 0);
                powerAndPrecoding.power = wns::Power::from_mW(0); // will be overwritten by power control in scheduler base
                allocation.prbPowerPrecoders[prbs[prb]] =  powerAndPrecoding;
            }
            
            if (allocation.prbPowerPrecoders.size() > 0)
            {            
                scheduledUsers.push_back(allocation);

                if (threegppCalibration && !threeGPPConstellationFixed)
                    threeGPPConstellation[user] = allocation;
                
                MESSAGE_SINGLE(NORMAL, logger, "Have scheduled " << allocation.prbPowerPrecoders.size() << " PRBs to user " << user->getName());
                
                currentUsersPRBManager->removeActiveUser(user);
            }
        }
        
        threeGPPConstellationFixed = true;
    } // if active users available
}


