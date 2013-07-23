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

#include <IMTAPHY/ltea/mac/linkAdaptation/uplink/LinkAdaptationBase.hpp>
#include <IMTAPHY/link2System/MMIBeffectiveSINR.hpp>
#include <IMTAPHY/ltea/mac/ModulationAndCodingSchemes.hpp>
#include <IMTAPHY/ltea/mac/scheduler/uplink/eNB/SchedulerBase.hpp>
#include <algorithm>

using namespace ltea::mac::la::uplink;


LinkAdaptationResult 
LinkAdaptationBase::performLinkAdaptation(wns::node::Interface* userID, 
                                        unsigned int spatialTransportBlock,
                                        const std::vector<unsigned int>& prbs,
                                        unsigned int tti, 
                                        unsigned int numLayersForTB, 
                                        bool hasSRS,
                                        double Ks)
{
    assure(prbs.size(), "Cannot do Link Adaptation on empty input");
    
    LinkAdaptationResult result;
    
    imtaphy::receivers::feedback::LteRel10UplinkChannelStatusPtr channelStatus = channelStatusManager->getChannelState(userID, tti);
    
    assure(channelStatus->numPRBs >= prbs.size(), "System feedback has less PRBs than scheduling request");
    assure(channelStatus->rank == 1, "Currently, only rank 1 transmission supported in uplink");
    assure(spatialTransportBlock == 0, "Currently, only rank 1 transmission supported in uplink");
    
    assure(sinrEstimates.find(userID) != sinrEstimates.end(), "No SINR estimates for the requested user available");
    
    // The channel status SINRs are generated assuming SRS transmission over the whole system bandwidth
    // The actual transmission bandwidth is most likely going to be less, so a higher transmit power 
    // and thus a higher SINR could be afforded. Also, the \DeltaTF will add Tx power depending on the 
    // modulation and coding scheme. However, the per PRB transmit power is still limited by the UE's 
    // total available tx power.
    
    wns::Power txPowerPerPRB = powerControl->getOpenLoopPerPRBPower(userID, prbs.size(), wns::Ratio::from_factor(1.0));
    wns::Power srsPower = powerControl->getSRSPerPRBPower(userID);
    
    wns::Ratio sinrPowerGain = txPowerPerPRB / srsPower;
    
    
    wns::Ratio maxDeltaTFdB = powerControl->getOpenLoopPerPRBPowerHeadroom(userID, prbs.size());
    
    MESSAGE_SINGLE(NORMAL, logger, "Power Control for " << prbs.size() << " transmit PRBs yields a " << sinrPowerGain << " power gain compared to SRS transmission power. Available for \\Delta_TF: " << maxDeltaTFdB << " dB");
    
    std::vector<wns::Ratio> sinrs(prbs.size());
    for (unsigned int f = 0; f < prbs.size(); f++)
    {
        assure(sinrEstimates[userID].find(prbs[f]) != sinrEstimates[userID].end(), "No SINR for this PRB for this user");
        sinrs[f] = sinrEstimates[userID][prbs[f]] + sinrPowerGain;
        
        // TODO: rename or somthing?
        result.prbTracingDict[prbs[f]]["EstInTTI"] = channelStatus->estimatedInTTI[prbs[f]];
    }
        
                   
   wns::Ratio effSINR = mmseFDE.getEffectiveSINR(sinrs, imtaphy::l2s::QPSK()); // the MMSE FDE ignores the modulation
   
   effSINR -= globalThreshold;
   
   MESSAGE_SINGLE(NORMAL, logger, "LinkAdaptation for user " << userID->getName() << "(est. eff. SINR - threshold = " << effSINR << ") layersForPRB=" << numLayersForTB << " hasSRS=" << hasSRS);

   unsigned int mcsIndex = mcsLookup->getSuitableUplinkMCSindexForEffSINR(effSINR,
                                                             prbs.size(),
                                                             numLayersForTB,
                                                             tti,
                                                             hasSRS,
                                                             maxDeltaTFdB,
                                                             Ks
                                                             );       
   
   if (prbs.size() == 1)
   {
       // in case we only have 1 PRB, we want to use at least MCS 2 to 
       // be able to actually transmit payload (and not only headers)
       mcsIndex = std::max(2u, mcsIndex);
   }
   
                                                               
   MESSAGE_SINGLE(NORMAL, logger, "Post MMSE Frequency Domain Equalization SINR - threshold= " << effSINR << " yields MCS=" << mcsIndex);
                                               
   result.estimatedSINR = effSINR;
   result.mcsIndex = mcsIndex;
   result.modulation = mcsLookup->getModulation(mcsIndex);
   result.codeRate = mcsLookup->getUplinkCodeRate(mcsIndex, prbs.size(), numLayersForTB, tti, hasSRS);
   
   return result;
}
