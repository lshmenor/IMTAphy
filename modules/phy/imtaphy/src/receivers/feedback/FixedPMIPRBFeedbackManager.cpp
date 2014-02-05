/*******************************************************************************
 * This file is part of IMTAphy
 * _____________________________________________________________________________
 *
 * Copyright (C) 2012
 * Institute for Communication Networks (LKN)
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

#include <IMTAPHY/receivers/feedback/FixedPMIPRBFeedbackManager.hpp>
#include <IMTAPHY/Channel.hpp>
#include <IMTAPHY/linkManagement/LinkManager.hpp>

#include <WNS/StaticFactoryBroker.hpp>
#include <WNS/PyConfigViewCreator.hpp>


#include <algorithm>
#include <iostream>

using namespace imtaphy::receivers::feedback;


STATIC_FACTORY_BROKER_REGISTER(FixedPMIPRBFeedbackManager,
                                DownlinkFeedbackManagerInterface,
                                "FixedPMIPRBFeedbackManager");


FixedPMIPRBFeedbackManager::FixedPMIPRBFeedbackManager(const wns::pyconfig::View& _config) :
    LteRel8DownlinkFeedbackManager(_config),
    randomize(_config.get<bool>("randomize")),
    fixedRank(_config.get<unsigned int>("fixedRank")),
    rngForShuffle(*wns::simulator::getRNG(), uni)
{
    wns::pyconfig::Sequence pmiSequence = _config.getSequence("pmis");
    for (wns::pyconfig::Sequence::iterator<int> iter = pmiSequence.begin<int>(); iter != pmiSequence.end<int>(); iter++)
    {
        assure(*iter >= 0, "Invalid PMI: must not be negative");
        assure(*iter < 16, "Invalid PMI: must be smaller than 16");
        
        std::cout << "Adding PMI " << *iter << " to list of allowed PMIs\n";
        
        pmis.push_back(*iter);
    }
};

void 
FixedPMIPRBFeedbackManager::beforeTTIover(unsigned int ttiNumber)
{
    // don't start at t=0 but configure the PMI patterns
    if (ttiNumber == 0)
    {
        unsigned int numPRBs = channel->getSpectrum()->getNumberOfPRBs(imtaphy::Downlink);
        imtaphy::StationList baseStations = channel->getAllBaseStations();

        for (imtaphy::StationList::const_iterator iter = baseStations.begin(); iter != baseStations.end(); iter++)
        {
            pmiMask[*iter] = std::vector<int>(numPRBs);
        }
        unsigned int numPMIs = pmis.size();
        unsigned int numSubBands = (numPRBs % prbsPerSubband) != 0 ? static_cast<unsigned int>(numPRBs / prbsPerSubband) + 1 : numPRBs / prbsPerSubband;
        std::vector<int> pmiPerSubBand(numSubBands);
        
        // we divide the whole bandwidth (numPRBs) into subbands of prbsPerSubband-size PRBs each
        // then we assign the PMIs to the subbands; most of the time, some PMIs get more subbands

        unsigned int Mh = numSubBands % numPMIs;        // number of PMIs with high sub band number
        unsigned int Ml = numPMIs - Mh;                 // number of PMIs with low sub band number
        unsigned int Nh = numSubBands / numPMIs + 1;    // number of high subbands
        unsigned int Nl = numSubBands / numPMIs;        // number of low subbands
        
        unsigned int subBandIndex;
        unsigned int pmi = 0;
        unsigned int index = 0;
        
        // first allocate the high subbands
        for (subBandIndex = 0; subBandIndex < Mh; subBandIndex++, pmi++)
        {
            for (unsigned int i = 0; i < Nh; i++, index++)
            {
                pmiPerSubBand[index] = pmi;
            }
        }
        
        assure(subBandIndex == Mh, "");
        
        // then the rest
        for (; subBandIndex < Mh + Ml; subBandIndex++, pmi++)
        {
            for (unsigned int i = 0; i < Nl; i++, index++)
            {
                pmiPerSubBand[index] = pmi;
            }
   
        }
        
//         
// //        unsigned int numSubBandsPerPMI = (numSubBands % numPMIs) != 0 ? static_cast<unsigned int>(numSubBands / numPMIs) + 1 : numSubBands / numPMIs;
//         unsigned int numSubBandsPerPMI = (numSubBands / numPMIs);
//         
//         for (; pmi < numPMIs; pmi++)
//         {
//             for (unsigned int subBand = 0; subBand < numSubBandsPerPMI; subBand++)
//             {
//                 unsigned int index = pmi * numSubBandsPerPMI + subBand;
//                 if (index < numSubBands)
//                 {
//                     pmiPerSubBand[index] = pmi;
//                 }
//             }
//         }
//         
        
        for (imtaphy::StationList::const_iterator iter = baseStations.begin(); iter != baseStations.end(); iter++)
        {
            if (randomize)
            {
                std::random_shuffle(pmiPerSubBand.begin(), pmiPerSubBand.end(), rngForShuffle);
            }
            
            std::cout << "PMI mask for " << (*iter)->getName() << ":\n";
            
            for (unsigned int prb = 0; prb < numPRBs; prb++)
            {
                pmiMask[*iter][prb] = pmiPerSubBand[prb / prbsPerSubband];
                std::cout << "PRB " << prb << "\t" << pmiMask[*iter][prb] << "\n";
            }
        }            
        
        
        return;
    }
    
    // if we don't have to do updates this TTI, return immediately
    if (((ttiNumber)% cqiUpdateFrequency) && ((ttiNumber) % rankUpdateFrequency))
        return;

    int numReceivers = receivingStations.size();
    int receiverIndex; // signed int for OpenMP parallel for loop

    // these are the variables that should be private to each thread
    wns::node::Interface* node;
    imtaphy::StationPhy* receivingStation;
    FeedbackContainer* feedbackContainer;
    
     bool probeIPNVariations = ipnVariationContextCollector->hasObservers();
    
#pragma omp parallel for private(node, receivingStation, feedbackContainer), shared(numReceivers, ttiNumber) //, default(none)
    for (receiverIndex = 0; receiverIndex < numReceivers; receiverIndex++)
    {
        node = receivingStations[receiverIndex].first;      // this is a read-only access into a vector and should be thread-safe
        assure(perNodeFeedback.find(node) != perNodeFeedback.end(), "Receiver not yet registered"); // this is a read-only access and should be thread-safe

        feedbackContainer = perNodeFeedback.find(node)->second;     // this is a read-only access and should be thread-safe
        receivingStation = receivingStations[receiverIndex].second;  // this is a read-only access into a vector and should be thread-safe

        doUpdate(receivingStation, feedbackContainer, ttiNumber);
        
        if (probeIPNVariations)
        {
            computeIPNVariation(receivingStation, ttiNumber);
        }
    } // end of parallel code region
    
   if (probeIPNVariations)
    {
        for (std::map<wns::node::Interface*, std::vector<double> >::const_iterator iter = ipnDifferences.begin(); 
             iter != ipnDifferences.end(); iter++)
        {
            unsigned int msId = iter->first->getNodeID();
          
            for (std::vector<double>::const_iterator valueIter =  iter->second.begin(); valueIter != iter->second.end(); valueIter++)
            {
                ipnVariationContextCollector->put(*valueIter, boost::make_tuple("MSID", msId));
            }
        }
    }
    
        
    if (feedbackContextCollector->hasObservers())
    {
        for (receiverIndex = 0; receiverIndex < numReceivers; receiverIndex++)
        {
            node = receivingStations[receiverIndex].first;
            feedbackContainer = perNodeFeedback.find(node)->second; 
            unsigned int index = ttiNumber % feedbackContainer->bufferLength;
            LteRel8DownlinkFeedbackPtr feedback = feedbackContainer->ringBuffer[index];
            for (unsigned int prb = 0; prb < feedback->numPRBs; prb++)
            {
                feedbackContextCollector->put(feedback->cqiTb1[prb], boost::make_tuple("PMI", feedback->pmi[prb],
                                                                            "RI", feedback->rank));
                
                if (feedback->rank > 1)
                {
                    feedbackContextCollector->put(feedback->cqiTb2[prb], boost::make_tuple("PMI", feedback->pmi[prb],
                                                                                "RI", feedback->rank));
                }
            }
        }   
    }

}


void
FixedPMIPRBFeedbackManager::determinePMIsAndCQIs(imtaphy::StationPhy* receivingStation, LteRel8DownlinkFeedbackPtr feedback, unsigned int rank, unsigned int tti)
{
    imtaphy::receivers::LinearReceiver* receiver = dynamic_cast<imtaphy::receivers::LinearReceiver*>(receivingStation->getReceiver());
    assure(receiver, "Expected a LinearReceiver instance");
    
    assure(servingBSMap.find(receivingStation) != servingBSMap.end(), "Corresponding transmitter not found");
    imtaphy::StationPhy* transmitterToEstimate = servingBSMap[receivingStation];
    
    unsigned int numSubBands = (numPRBs % prbsPerSubband) != 0 ? static_cast<unsigned int>(numPRBs / prbsPerSubband) + 1 : numPRBs / prbsPerSubband;
    unsigned int numTxAntennas = transmitterToEstimate->getAntenna()->getNumberOfElements();
    
    for (unsigned int subBand = 0; subBand < numSubBands; subBand++)
    {
        unsigned int midPRB = subBand * prbsPerSubband;
        if (midPRB + prbsPerSubband > numPRBs)
            midPRB += static_cast<unsigned int>((numPRBs - midPRB) / 2);
        else
            midPRB += static_cast <unsigned int>(prbsPerSubband / 2);

        assure(pmiMask.find(transmitterToEstimate) != pmiMask.end(), "Unknown transmitter");
        assure(pmiMask[transmitterToEstimate].size() > midPRB, "Invalid PRB");

        std::vector<imtaphy::detail::ComplexFloatMatrixPtr> precodings;
        precodings.push_back(codebook->getPrecodingMatrix(numTxAntennas, rank, pmiMask[transmitterToEstimate][midPRB]));
                
        imtaphy::receivers::ReceiverFeedback best = receiver->computeFeedback(midPRB, rank, 
                                                                              transmitterToEstimate, 
                                                                              referencePowerMap[receivingStation][midPRB], 
                                                                              precodings);

        unsigned int cqi1 = 0;
        unsigned int cqi2 = 0;

        // TB to layer mapping in LTE (according to Farooq, TS36.? source?)
        //         layer 1     layer 2     layer 3     layer 4
        // rank 1:   TB 1
        // rank 2:   TB 1         TB 2
        // rank 3:   TB 1         TB 2        TB 2
        // rank 4:   TB 1         TB 1        TB 2      TB 2

        // we should compute an effective SINR over the PRBs in that sub band but for the
        // time being, it is just for the midPRB

        std::vector<wns::Ratio> sinrsToAverage(2);
        switch (rank)
        {
            case 1:
                cqi1 = blerModel->getCQI(wns::Ratio::from_factor(best.bestSINRs[0]));
                break;
            case 2:
                cqi1 = blerModel->getCQI(wns::Ratio::from_factor(best.bestSINRs[0]));
                cqi2 = blerModel->getCQI(wns::Ratio::from_factor(best.bestSINRs[1]));
                break;
            // for the cases when we transmit a TB accross 2 layers, we take floor(average(cqi1 + cqi2))
            case 3:
                cqi1 = blerModel->getCQI(wns::Ratio::from_factor(best.bestSINRs[0]));

                // this is a floor operation:
                cqi2 = (blerModel->getCQI(wns::Ratio::from_factor(best.bestSINRs[1])) + blerModel->getCQI(wns::Ratio::from_factor(best.bestSINRs[2]))) / 2;
                break;
            case 4:
                // this is a floor operation:
                cqi1 = (blerModel->getCQI(wns::Ratio::from_factor(best.bestSINRs[0])) + blerModel->getCQI(wns::Ratio::from_factor(best.bestSINRs[1]))) / 2;
                // this is a floor operation:
                cqi2 = (blerModel->getCQI(wns::Ratio::from_factor(best.bestSINRs[2])) + blerModel->getCQI(wns::Ratio::from_factor(best.bestSINRs[3]))) / 2;

                break;
            default:
                assure(0, "Invalid rank");
        }

        for (unsigned int prb = subBand * prbsPerSubband;
            (prb < (subBand + 1) * prbsPerSubband) && (prb < numPRBs);
            prb++)
        {
            feedback->pmi[prb] = pmiMask[transmitterToEstimate][midPRB]; // we only give the receiver a list of possible PMIs so it's answer will be shifted if the first PMI index is > 0
            feedback->cqiTb1[prb] = cqi1;
            feedback->cqiTb2[prb] = cqi2;
            feedback->estimatedInTTI[prb] = tti;
            feedback->rank = rank;
            
            for (unsigned int r = 0; r < 4; r++)
            {
                if (r < rank)
                {
                    feedback->magicSINRs[prb][r] = wns::Ratio::from_factor(best.bestSINRs[r]);
                }
                else
                {
                    // use -inf dB as dummy
                    feedback->magicSINRs[prb][r] = wns::Ratio::from_factor(0);
                }
            }
        }
    } // end of loop over sub bands
}

unsigned int
FixedPMIPRBFeedbackManager::performRankUpdate(imtaphy::StationPhy* receivingStation)
{
    assure(servingBSMap.find(receivingStation) != servingBSMap.end(), "Corresponding transmitter not found");
    imtaphy::StationPhy* transmitterToEstimate = servingBSMap[receivingStation];
    
    if (fixedRank > 0)
    {
        assure(transmitterToEstimate->getAntenna()->getNumberOfElements() >= fixedRank, "Not enough antennas for rank " << fixedRank);
        return fixedRank;
    }
    
    // rank not fixed, so determine it
    
    
    imtaphy::receivers::LinearReceiver* receiver = dynamic_cast<imtaphy::receivers::LinearReceiver*>(receivingStation->getReceiver());
    assure(receiver, "Expected a LinearReceiver instance");
    
    
    
    unsigned int maxRank = receiver->getMaxRank();
    unsigned int numTxAntennas = transmitterToEstimate->getAntenna()->getNumberOfElements();

    assure((numTxAntennas == 2) || (numTxAntennas == 4), "Only 2 or 4 tx antennas supported");
    assure(maxRank <= numTxAntennas, "Rank cannot be bigger than serving BS's number of Tx antennas");

    // perform full rank update
    std::vector<float> capacities(maxRank + 1, 0.0);

    for (unsigned int prb = 0; prb < numPRBs; prb++)
    {
        for (unsigned int rank = 1; rank <= maxRank; rank++)
        {
            std::vector<imtaphy::detail::ComplexFloatMatrixPtr> precodings;
            precodings.push_back(codebook->getPrecodingMatrix(numTxAntennas, rank, pmiMask[transmitterToEstimate][prb]));
                
            imtaphy::receivers::ReceiverFeedback best = receiver->computeFeedback(prb, rank, 
                                                                              transmitterToEstimate, 
                                                                              referencePowerMap[receivingStation][prb], 
                                                                              precodings);
            assure(best.bestSINRs.size() == rank, "Wrong number of layers in result");

            for (unsigned int layer = 0; layer < rank; layer++)
            {
                // limit the contribution to the useful range in LTE between about -10 and 22 dB
                wns::Ratio sinr = wns::Ratio::from_factor(best.bestSINRs[layer]);
                if (sinr > wns::Ratio::from_dB(-10.0))
                {
                    if (sinr > wns::Ratio::from_dB(22.0))
                    {
                        sinr = wns::Ratio::from_dB(22.0);
                    }
                    capacities[rank] += log(1.0 + sinr.get_factor()) / log(2.0);
                }
            }
        }
    }

    // think about whether it makes sense to decide best rank based on sum
    // over all PMIs - only the rank with the biggest entry should be chosen
    float bestCapacity = 0.0;
    unsigned int bestRank = 1;  // if everything is bad (i.e., all sinrs < -10 dB), rank 1 will be chosen
    for (unsigned int rank = 1; rank <= maxRank; rank++)
    {
        if (capacities[rank] > bestCapacity)
        {
            bestRank = rank;
            bestCapacity = capacities[rank];
        }
    }

    return bestRank;
} // end of full rank update

void 
FixedPMIPRBFeedbackManager::registerMS(imtaphy::StationPhy* station, wns::node::Interface* node, imtaphy::receivers::LinearReceiver* receiver, imtaphy::Channel* channel_)
{
    LteRel8DownlinkFeedbackManager::registerMS(station, node, receiver, channel_);
    
}


