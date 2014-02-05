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

#include <IMTAPHY/ltea/mac/linkAdaptation/downlink/SINRThresholdLA.hpp>
#include <WNS/PyConfigViewCreator.hpp>
#include <IMTAPHY/link2System/MMIBeffectiveSINR.hpp>
#include <IMTAPHY/ltea/mac/ModulationAndCodingSchemes.hpp>

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    ltea::mac::la::downlink::SINRThresholdLinkAdaptation,
    ltea::mac::la::downlink::LinkAdaptationInterface,
    "ltea.dll.linkAdaptation.downlink.SINRThreshold",
    wns::PyConfigViewCreator);

using namespace ltea::mac::la::downlink;

SINRThresholdLinkAdaptation::SINRThresholdLinkAdaptation(const wns::pyconfig::View& pyConfigView) : 
    LinkAdaptationBase(pyConfigView),
    globalThreshold(wns::Ratio::from_dB(pyConfigView.get<double>("threshold")))
{
    effectiveSINR = imtaphy::l2s::TheMMIBEffectiveSINRModel::getInstance();
    setFeedbackManager(imtaphy::receivers::feedback::DownlinkFeedbackManagerInterface::getFeedbackManager());
}

LinkAdaptationResult
SINRThresholdLinkAdaptation::performLinkAdaptation(wns::node::Interface* userID, 
                                                   unsigned int spatialLayer, 
                                                   PRBpowerOffsetMap& prbMap, 
                                                   unsigned int tti,
                                                   unsigned int numLayersForTB, 
                                                   unsigned int pdcchLength,
                                                   bool provideRel10DMRS,
                                                   unsigned int numRel10CSIrsSets
                                                   )
{
    assure(prbMap.size(), "Cannot do Link Adaptation on empty input");
    
    LinkAdaptationResult result;
    
    std::vector<wns::Ratio> sinrs;
    sinrs.reserve(prbMap.size());

    imtaphy::receivers::feedback::LteRel8DownlinkFeedbackPtr feedback = feedbackManager->getFeedback(userID, tti);
    
    assure(feedback->numPRBs >= prbMap.size(), "System feedback has less PRBs than scheduling request");
    std::vector<imtaphy::l2s::CQI>* cqis;
    unsigned int numSixCenterPRBs = 0;
    unsigned int sixCenterMin = feedback->numPRBs - feedback->numPRBs  / 2 - 3;
    unsigned int sixCenterMax = sixCenterMin + 6;
    
    assure(sixCenterMax <= feedback->numPRBs, "Spectrum allocation too small, need at least 6 PRBs");
    assure(sixCenterMin < sixCenterMax, "Spectrum allocation too small, need at least 6 PRBs"); // maybe uint is a stupid type
    
    
    if (spatialLayer == 0)
        cqis = &(feedback->cqiTb1);
    else
        cqis = &(feedback->cqiTb2);
     
    MESSAGE_SINGLE(NORMAL, logger, "LinkAdaptation for user " << userID->getName() << " with SINR threshold=" << thresholdMap[userID] << "(est. SINR = SINR + threshold) layersForPRB=" << numLayersForTB << " pdccLength=" << pdcchLength);
    MESSAGE_SINGLE(NORMAL, logger, "numRel8AntennaPorts=" << numRel8AntennaPorts << " provideRel10DMRS=" << provideRel10DMRS << " numRel10CSIrsPorts=" << numRel10CSIrsPorts << " numRel10CSIrsSets" << numRel10CSIrsSets);
                   
    for (PRBpowerOffsetMap::const_iterator iter = prbMap.begin(); iter != prbMap.end(); iter++)
    {
        imtaphy::interface::PRB prb = iter->first;
        assure((prb >= 0) && (prb < feedback->numPRBs), "PRB out of available range");
        
        wns::Ratio sinrThreshold = blerModel->getSINRthreshold(cqis->at(prb));
        
        MESSAGE_SINGLE(NORMAL, logger, "Feedback for PRB " << prb << " is CQI=" <<  cqis->at(prb) << " will assume an SINR of " << sinrThreshold << " and add SINR (power) correction of " << iter->second);
        
        if ((prb >= sixCenterMin) && (prb < sixCenterMax))
            numSixCenterPRBs++;

        // for each CQI value fed back we store the SINR that corresponds to the threshold where that CQI
        // would yield a BLER of 10%. This is a conservative assumption
        sinrs.push_back(sinrThreshold + iter->second);
        
        result.prbTracingDict[prb]["EstInTTI"] = feedback->estimatedInTTI[prb];
    }
    
    // here we want to compute an effective SINR based on the individual SINRs
    // however, we need to specify to modulation order to computer SINRs which we 
    // don't know yet. Thus, we start with 64QAM and move down to QPSK until we get a reasonable value
    
    wns::Ratio effSINR;
    
    wns::Ratio qam64EffSINR = effectiveSINR->getEffectiveSINR(sinrs, imtaphy::l2s::QAM64());
    qam64EffSINR -= thresholdMap[userID];
   
    unsigned int cqi;
    
    if (imtaphy::l2s::CQITable[blerModel->getCQI(qam64EffSINR)].modulation.getBitsPerSymbol() == imtaphy::l2s::QAM64().getBitsPerSymbol())
    {
        effSINR = qam64EffSINR;
    }
    else 
    {
        wns::Ratio qam16EffSINR =  effectiveSINR->getEffectiveSINR(sinrs, imtaphy::l2s::QAM16());
        qam16EffSINR -= thresholdMap[userID];
        
        if (imtaphy::l2s::CQITable[blerModel->getCQI(qam16EffSINR)].modulation.getBitsPerSymbol() >= imtaphy::l2s::QAM16().getBitsPerSymbol())
        {
            effSINR = qam16EffSINR;
        }
        else
        {
            effSINR =  effectiveSINR->getEffectiveSINR(sinrs, imtaphy::l2s::QPSK());
            effSINR -= thresholdMap[userID];
        }
    }

    unsigned int mcsIndex = mcsLookup->getSuitableDownlinkMCSindexForEffSINR(effSINR, 
                                                                            prbMap.size(),                        // number of PRBs for TB 
                                                                            numLayersForTB,                       // on how many layers to transmit TB 
                                                                            pdcchLength,                          // DL/UL scheduling dependent overhead for control channel
                                                                            numRel8AntennaPorts,                  // fixed for a cell
                                                                            numSixCenterPRBs,                     // number of prbs falling into central six
                                                                            tti,                                  // TTI number influences BCH/sync overhead
                                                                            provideRel10DMRS,                     // whether further overhead due to Rel10 DMRS occurs
                                                                            numRel10CSIrsPorts,                   // number of antenna ports for Rel10 CSI-RS
                                                                            numRel10CSIrsSets                     // scheduler may decide if this TTI we have CSI and how many we mute
                                                                            );

   if (prbMap.size() == 1)
   {
       // in case we only have 1 PRB, we want to use at least MCS 2 to 
       // be able to actually transmit payload (and not only headers)
       mcsIndex = std::max(2u, mcsIndex);
   }
    
   MESSAGE_SINGLE(NORMAL, logger, "eff. SINR + threshold= " << effSINR << " yields MCS=" << mcsIndex);
                                               
   result.estimatedSINR = effSINR;
   result.mcsIndex = mcsIndex;
   result.modulation = mcsLookup->getModulation(mcsIndex);
   result.codeRate = mcsLookup->getDownlinkCodeRate(mcsIndex,
                                            pdcchLength,
                                            numRel8AntennaPorts,
                                            prbMap.size(),
                                            numLayersForTB,
                                            numSixCenterPRBs,
                                            tti,
                                            provideRel10DMRS,
                                            numRel10CSIrsPorts,
                                            numRel10CSIrsSets);
   
   return result;
}

void 
SINRThresholdLinkAdaptation::updateAssociatedUsers(std::vector< wns::node::Interface* > allUsers)
{
    for (unsigned int i = 0; i < allUsers.size(); i++)
    {
        thresholdMap[allUsers[i]] = globalThreshold;
    }
}


void 
SINRThresholdLinkAdaptation::updateBLERStatistics(unsigned int tti)
{
    // we don't take this into account here
}

void 
SINRThresholdLinkAdaptation::updateFeedback(unsigned int tti)
{
    // we don't take this into account here
}
