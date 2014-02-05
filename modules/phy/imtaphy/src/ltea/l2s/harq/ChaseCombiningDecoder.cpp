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

#include <IMTAPHY/ltea/l2s/harq/ChaseCombiningDecoder.hpp>
#include <IMTAPHY/ltea/mac/DCI.hpp>
#include <IMTAPHY/ltea/mac/harq/HARQentity.hpp>
#include <IMTAPHY/interface/TransmissionStatus.hpp>
#include <WNS/simulator/ISimulator.hpp>
#include <IMTAPHY/Spectrum.hpp>


using namespace ltea::l2s::harq;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    ChaseCombiningDecoder,
    DecoderInterface,
    "ltea.ChaseCombiningDecoder",
    wns::PyConfigViewCreator);



ChaseCombiningDecoder::ChaseCombiningDecoder(const wns::pyconfig::View& config) :
    DecoderInterface(config),
    blerModel(imtaphy::l2s::TheLTEBlockErrorModel::getInstance()),
    effSinrModel(imtaphy::l2s::TheMMIBEffectiveSINRModel::getInstance()),
    uniformRandom(0.0, 1.0, wns::simulator::getRNG()),
    logger(config.get("logger"))
{
    effSINRContextCollector = new wns::probe::bus::ContextCollector("effSINR");
    
    linkAdaptationMismatchContextCollector = wns::probe::bus::ContextCollectorPtr(
        new wns::probe::bus::ContextCollector("laMismatch"));
    

}

bool
ChaseCombiningDecoder::canDecode(SoftCombiningBuffer& receivedRedundancyVersions)
{
    // we could store the current state after the first transmission attempt so we don't have to rebuild everything
    // on each new redundancy version
    
    assure(entity, "Need access to the HARQ entity for DCI reader access");

    // Here we assume non-adaptive HARQ retransmissions meaning that we will have retransmission with the same number of
    // prbs used (i.e. same code rate) and the same modulation.

    RedundancyVersionStatus initialTransmission = receivedRedundancyVersions.front();

    ltea::mac::DownlinkControlInformation* dci = entity->getDCIReader()->readCommand<ltea::mac::DownlinkControlInformation>(initialTransmission.first->getCommandPool());
    assure(dci, "could not read DCI");

    imtaphy::Direction direction = dci->magic.direction;
    
    double codeRate = dci->peer.codeRate;
    imtaphy::l2s::ModulationScheme modulation = dci->peer.modulation;
    unsigned int blockSize = dci->peer.blockSize;
    unsigned int numLayers = dci->peer.assignedToLayers.size();
    unsigned int numPRBsPerLayer = initialTransmission.second->getNumberOfPRBs();
    unsigned int totalNumPRBs = numPRBsPerLayer * numLayers;

    // In Downlink put the SINRs of all PRBs on all involved layers in a vector and add it element-wise 
    // for each redundancy version. From that sum vector we take the effective SINR
    // In Uplink we compute a post MMSE Frequency Domain Equalization SINR (over the involved PRBs) for each layer,
    // put the layers into the vector, add that post MMSE SINR SINRs vector and take the effective SINR over the per 
    // layer sum SINR
    
    std::vector<wns::Ratio> sumSINRs;
    if (direction == imtaphy::Downlink)
    {
        sumSINRs.resize(totalNumPRBs, wns::Ratio::from_factor(0));
    }
    else // Uplink
    {
        sumSINRs.resize(numLayers, wns::Ratio::from_factor(0));
    }
    
    // See, e.g., IEEE 802.16m EMD section 4.6
    // we put together a vector of 
    
    for (SoftCombiningBuffer::const_iterator iter = receivedRedundancyVersions.begin();
         iter != receivedRedundancyVersions.end(); iter++)
    {
        RedundancyVersionStatus transmission = *iter;

        assure(transmission.second->getNumberOfPRBs() == numPRBsPerLayer, "Different number of PRBs in redundancy versions - currently not supported");
        dci = entity->getDCIReader()->readCommand<ltea::mac::DownlinkControlInformation>(transmission.first->getCommandPool());
        assure(dci, "could not read DCI");
        assure(direction ==  dci->magic.direction, "Transmission direction (DL/UL) cannot change between different redundancy versions");
        assure(numLayers == dci->peer.assignedToLayers.size(), "Number of layers cannot change between retransmissions");
        assure(codeRate == dci->peer.codeRate, "code rate cannot change between CC retransmissions");
        assure(blockSize == dci->peer.blockSize, "Transport block size cannot change between HARQ retransmissions");
        assure(modulation.getBitsPerSymbol() == dci->peer.modulation.getBitsPerSymbol(), "This implementation does not support different modulation schemes between HARQ retransmissions");

        imtaphy::interface::PRBVector prbs = transmission.second->getPRBs();
        
        for (unsigned int m = 0; m < numLayers; m++)
        {       
            if (direction == imtaphy::Downlink)
            {            
                for (unsigned int f = 0; f < numPRBsPerLayer; f++)
                {
                    unsigned int prb = prbs[f];
                    // layers counted from 1..max
                    sumSINRs[f + m*numPRBsPerLayer] = wns::Ratio::from_factor(sumSINRs[f + m*numPRBsPerLayer].get_factor() + transmission.second->getSINR(prb, dci->peer.assignedToLayers[m]).get_factor());
                }
            }
            else // Uplink -> perform frequency domain MMSE equalization
            {
                imtaphy::interface::SINRVector sinrPerLayer = transmission.second->getSINRsForLayer(dci->peer.assignedToLayers[m]);
                
                wns::Ratio postMMSE_FDE_SINR = uplinkMMSEFDE.getEffectiveSINR(sinrPerLayer, modulation);
                sumSINRs[m] = wns::Ratio::from_factor(sumSINRs[m].get_factor() + postMMSE_FDE_SINR.get_factor());
            }
        }
        MESSAGE_SINGLE(NORMAL, logger, "After adding sinrs, we have the following sinrs:");
        for (unsigned int i = 0; i < sumSINRs.size(); i++)
            MESSAGE_SINGLE(NORMAL, logger, "Entry " << i << " has current accumulated SINR of " << sumSINRs[i]);
    }        

    wns::Ratio effectiveSINR = effSinrModel->getEffectiveSINR(sumSINRs, modulation);
    dci->magic.effSINR = effectiveSINR;
    
    effSINRContextCollector->put(effectiveSINR.get_dB());
    
    
    linkAdaptationMismatchContextCollector->put((effectiveSINR - dci->magic.estimatedLinkAdaptationSINR).get_dB(),
                                                boost::make_tuple("attempt", dci->magic.transmissionAttempts,
                                                                  "mcsIndex", dci->magic.mcsIndex,
                                                                  "direction", dci->magic.direction,
                                                                  "stream", dci->magic.spatialID
                                                                 ));
    
    
    MESSAGE_SINGLE(NORMAL, logger, "Effective SINR of TB with modulation " << modulation.getName() << " is " << effectiveSINR);

    double bler = blerModel->getBlockErrorRate(effectiveSINR,
                                            modulation,
                                            codeRate,
                                            blockSize);
    dci->magic.bler = bler;    
    
    assure((bler <= 1.0) && (bler >= 0.0), "This is not a valid probability");
    
    MESSAGE_SINGLE(NORMAL, logger, "This effectiveSINR with a code rate of " << codeRate << " and a block size of " << blockSize << " results in a BLER of " << bler);
    
    if (uniformRandom() > bler)
    {
        MESSAGE_SINGLE(NORMAL, logger, "Chase combiner can decode this TB");
        return true;
        
    }
    else
    {
        // we don't forward the transport block further
        // that's it - until we do HARQ
        MESSAGE_SINGLE(NORMAL, logger, "Chase combiner cannot decode this TB");
        return false;
    }
}

