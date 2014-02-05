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

#include <IMTAPHY/receivers/feedback/LteRel8DLFeedbackManager.hpp>
#include <IMTAPHY/Channel.hpp>
#include <IMTAPHY/linkManagement/LinkManager.hpp>

#include <WNS/StaticFactoryBroker.hpp>
#include <WNS/PyConfigViewCreator.hpp>


#include <algorithm>
#include <iostream>

using namespace imtaphy::receivers::feedback;


DownlinkFeedbackManagerInterface*
DownlinkFeedbackManagerInterface::getFeedbackManager()
{
    typedef wns::PyConfigViewCreator<DownlinkFeedbackManagerInterface> DownlinkFeedbackManagerCreator;
    typedef wns::StaticFactoryBroker<DownlinkFeedbackManagerInterface, DownlinkFeedbackManagerCreator> BrokerType;
    
    std::string name = wns::simulator::getInstance()->getConfiguration().get<std::string>("modules.imtaphy.downlinkFeedbackManager.name");

    return wns::SingletonHolder<BrokerType>::getInstance()->procure(name,
                                                                    wns::simulator::getInstance()->getConfiguration().getView("modules.imtaphy.downlinkFeedbackManager"));
    
}

STATIC_FACTORY_BROKER_REGISTER(LteRel8DownlinkFeedbackManager,
                                DownlinkFeedbackManagerInterface,
                                "DefaultLteRel8DownlinkFeedbackManager");


LteRel8DownlinkFeedbackManager::LteRel8DownlinkFeedbackManager(const wns::pyconfig::View& _config) :
    config(_config),
    numPRBs(0),
    precodingModeString(config.get<std::string>("precodingMode")),
    feedbackTotalDelay(config.get<unsigned int>("feedbackTotalDelay")),
    prbsPerSubband(config.get<unsigned int>("numPrbsPerSubband")),
    cqiUpdateFrequency(config.get<unsigned int>("cqiUpdateFrequency")),
    rankUpdateFrequency(config.get<unsigned int>("rankUpdateFrequency")),
    initialized(false),
    codebook(&imtaphy::receivers::TheLteRel8CodebookFloat::Instance()),
    effSINRModel(imtaphy::l2s::TheMMIBEffectiveSINRModel::getInstance()),
    enabled(config.get<bool>("enabled")),
    blerModel(imtaphy::l2s::TheLTEBlockErrorModel::getInstance()),
    dummyIPNPtr(new imtaphy::detail::ComplexFloatMatrix(1,1))
{
    assure(feedbackTotalDelay >= 1, "0ms delay is not supported");
    
    assure(rankUpdateFrequency > 0, "rank update frequency (actually period in TTIs) must be greater than 0");
    assure(cqiUpdateFrequency > 0, "cqi and pmi update frequency (actually period in TTIs) must be greater than 0");
    
    std::transform(precodingModeString.begin(), precodingModeString.end(), precodingModeString.begin(), toupper);

    if (precodingModeString == "CLOSEDLOOPCODEBOOKBASED")
    {
        precodingMode = ClosedLoopCodebookBased;
    }
    else if (precodingModeString == "NOPRECODING")
    {
        precodingMode = NoPrecoding;
    }
    else if (precodingModeString == "SINGLEANTENNA")
    {
        precodingMode = SingleAntenna;
    }
    else
    {
        precodingMode = NoPrecoding;
        assure(0, "Unknown precoding mode");
    }
    
    feedbackContextCollector = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector("feedback"));
    ipnVariationContextCollector = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector("ipnVariation"));
};

void 
LteRel8DownlinkFeedbackManager::beforeTTIover(unsigned int ttiNumber)
{
    // don't start at t=0
    if (ttiNumber == 0)
        return;
    
    // if we don't have to do updates this TTI, return immediately
    if (((ttiNumber)% cqiUpdateFrequency) && ((ttiNumber) % rankUpdateFrequency))
        return;

    int numReceivers = receivingStations.size();
    int receiverIndex; // signed int for OpenMP parallel for loop

    // these are the variables that should be private to each thread
    wns::node::Interface* node;
    imtaphy::StationPhy* receivingStation;
    FeedbackContainer* feedbackContainer;
    
    bool
    probeIPNVariations = ipnVariationContextCollector->hasObservers();
    
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

unsigned int
LteRel8DownlinkFeedbackManager::performRankUpdate(imtaphy::StationPhy* receivingStation)
{
    imtaphy::receivers::LinearReceiver* receiver = dynamic_cast<imtaphy::receivers::LinearReceiver*>(receivingStation->getReceiver());
    assure(receiver, "Expected a LinearReceiver instance");
    
    assure(servingBSMap.find(receivingStation) != servingBSMap.end(), "Corresponding transmitter not found");
    imtaphy::StationPhy* transmitterToEstimate = servingBSMap[receivingStation];
    
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
            unsigned int firstPMI = 0;
            unsigned int numPMIs = 0;
            
            switch (numTxAntennas) 
            {
                case 2:
                    if (rank == 1)
                        { firstPMI = 0; numPMIs = 4; }
                    else
                        { firstPMI = 1; numPMIs = 2; }
                    break;
                case 4:
                    firstPMI = 0;
                    numPMIs = 16;
                    break;
                default:
                    assure(0, "Unsupported number of tx antennas");
            }
            
            std::vector<imtaphy::detail::ComplexFloatMatrixPtr> precodings;
            for (unsigned int pmi = firstPMI; pmi < firstPMI + numPMIs; pmi++)
                precodings.push_back(codebook->getPrecodingMatrix(numTxAntennas, rank, pmi));
            
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
LteRel8DownlinkFeedbackManager::determinePMIsAndCQIs(imtaphy::StationPhy* receivingStation, LteRel8DownlinkFeedbackPtr feedback, unsigned int rank, unsigned int tti)
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

        
        unsigned int firstPMI = 0;
        unsigned int numPMIs = 0;
        
        switch (numTxAntennas) 
        {
            case 2:
                if (rank == 1)
                    { firstPMI = 0; numPMIs = 4; }
                else
                    { firstPMI = 1; numPMIs = 2; }
                break;
            case 4:
                firstPMI = 0;
                numPMIs = 16;
                break;
            default:
                assure(0, "Unsupported number of tx antennas for spatial multiplexing mode");
        }
        
        std::vector<imtaphy::detail::ComplexFloatMatrixPtr> precodings;
        for (unsigned int pmi = firstPMI; pmi < firstPMI + numPMIs; pmi++)
            precodings.push_back(codebook->getPrecodingMatrix(numTxAntennas, rank, pmi));
                
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
                
//                cqi2 = std::min(blerModel->getCQI(wns::Ratio::from_factor(best.bestSINRs[1])), blerModel->getCQI(wns::Ratio::from_factor(best.bestSINRs[2])));
                
//                 sinrsToAverage[0] = wns::Ratio::from_factor(best.bestSINRs[1]);
//                 sinrsToAverage[1] = wns::Ratio::from_factor(best.bestSINRs[2]);
//                 // assume QPSK, should not make a big difference
//                 cqi2 = blerModel->getCQI(effSINRModel->getEffectiveSINR(sinrsToAverage, imtaphy::l2s::QAM16()));
                break;
            case 4:
                // this is a floor operation:
                cqi1 = (blerModel->getCQI(wns::Ratio::from_factor(best.bestSINRs[0])) + blerModel->getCQI(wns::Ratio::from_factor(best.bestSINRs[1]))) / 2;
                // this is a floor operation:
                cqi2 = (blerModel->getCQI(wns::Ratio::from_factor(best.bestSINRs[2])) + blerModel->getCQI(wns::Ratio::from_factor(best.bestSINRs[3]))) / 2;

//                cqi1 = std::min(blerModel->getCQI(wns::Ratio::from_factor(best.bestSINRs[0])), blerModel->getCQI(wns::Ratio::from_factor(best.bestSINRs[1])));
//                cqi2 = std::min(blerModel->getCQI(wns::Ratio::from_factor(best.bestSINRs[2])), blerModel->getCQI(wns::Ratio::from_factor(best.bestSINRs[3])));

                
//                 sinrsToAverage[0] = wns::Ratio::from_factor(best.bestSINRs[0]);
//                 sinrsToAverage[1] = wns::Ratio::from_factor(best.bestSINRs[1]);
//                 // assume QPSK, should not make a big difference
//                 cqi1 = blerModel->getCQI(effSINRModel->getEffectiveSINR(sinrsToAverage, imtaphy::l2s::QAM64()));
// 
//                 sinrsToAverage[0] = wns::Ratio::from_factor(best.bestSINRs[2]);
//                 sinrsToAverage[1] = wns::Ratio::from_factor(best.bestSINRs[3]);
//                 cqi2 = blerModel->getCQI(effSINRModel->getEffectiveSINR(sinrsToAverage, imtaphy::l2s::QAM64()));
                break;
            default:
                assure(0, "Invalid rank");
        }

        for (unsigned int prb = subBand * prbsPerSubband;
            (prb < (subBand + 1) * prbsPerSubband) && (prb < numPRBs);
            prb++)
        {
            feedback->pmi[prb] = best.pmiRanking[0] + firstPMI; // we only give the receiver a list of possible PMIs so it's answer will be shifted if the first PMI index is > 0
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

void 
LteRel8DownlinkFeedbackManager::determineNoPrecodingCQIs(imtaphy::StationPhy* receivingStation, LteRel8DownlinkFeedbackPtr feedback, unsigned int rank, PrecodingMode precodingMode, unsigned int tti)
{   // do only cqi updates
    imtaphy::receivers::LinearReceiver* receiver = dynamic_cast<imtaphy::receivers::LinearReceiver*>(receivingStation->getReceiver());
    assure(receiver, "Expected a LinearReceiver instance");

    assure(servingBSMap.find(receivingStation) != servingBSMap.end(), "Corresponding transmitter not found");
    imtaphy::StationPhy* transmitterToEstimate = servingBSMap[receivingStation];
    
    // TODO: parts of the code in this class are duplicated, we should try to avoid that
    unsigned int numSubBands = (numPRBs % prbsPerSubband) != 0 ? static_cast<unsigned int>(numPRBs / prbsPerSubband) + 1 : numPRBs / prbsPerSubband;
    
    for (unsigned int subBand = 0; subBand < numSubBands; subBand++)
    {
        unsigned int midPRB = subBand * prbsPerSubband;
        if (midPRB + prbsPerSubband > numPRBs)
            midPRB += static_cast<unsigned int>((numPRBs - midPRB) / 2);
        else
            midPRB += static_cast <unsigned int>(prbsPerSubband / 2);


        std::vector<imtaphy::detail::ComplexFloatMatrixPtr> precoding;
        int pmi = imtaphy::receivers::feedback::FirstAntenna;
        
        switch(precodingMode)
        {
            case NoPrecoding:
                pmi = imtaphy::receivers::feedback::EqualPower;
                break;
            case SingleAntenna:
                pmi = imtaphy::receivers::feedback::FirstAntenna;
                break;
            default:
                assure(0, "Unknown precoding mode");
        }
        precoding.push_back(codebook->getPrecodingMatrix(transmitterToEstimate->getAntenna()->getNumberOfElements(),
                                                         1,
                                                         pmi));
        
        imtaphy::receivers::ReceiverFeedback best = receiver->computeFeedback(midPRB, rank, 
                                                                              transmitterToEstimate, 
                                                                              referencePowerMap[receivingStation][midPRB], 
                                                                              precoding);

        unsigned int cqi1 = blerModel->getCQI(wns::Ratio::from_factor(best.bestSINRs[0]));
        unsigned int cqi2 = 0;
        
        for (unsigned int prb = subBand * prbsPerSubband;
            (prb < (subBand + 1) * prbsPerSubband) && (prb < numPRBs);
            prb++)
        {
            feedback->pmi[prb] = pmi;
            feedback->cqiTb1[prb] = cqi1;
            feedback->cqiTb2[prb] = cqi2;
            feedback->estimatedInTTI[prb] = tti;
            feedback->rank = rank;
            
            feedback->magicSINRs[prb][0] = wns::Ratio::from_factor(best.bestSINRs[0]);
            for (unsigned int r = 1; r < 4; r++)
            {
                // use -inf dB as dummy
                feedback->magicSINRs[prb][r] = wns::Ratio::from_factor(0);
            }
        }
    }
}

void
LteRel8DownlinkFeedbackManager::doUpdate(imtaphy::StationPhy* receivingStation, FeedbackContainer* feedbackContainer, unsigned int ttiNumber)
{
    imtaphy::receivers::LinearReceiver* receiver = dynamic_cast<imtaphy::receivers::LinearReceiver*>(receivingStation->getReceiver());
    assure(receiver, "Expected a LinearReceiver instance");
    
    unsigned int bufferLength = feedbackContainer->bufferLength;
    unsigned int index = ttiNumber % bufferLength;

    LteRel8DownlinkFeedbackPtr feedback = feedbackContainer->ringBuffer[index];

    // If the BS only has one antenna, we cannot do precoding
    if (servingBSMap[receivingStation]->getAntenna()->getNumberOfElements() == 1)
    {
        precodingMode = SingleAntenna;
    }
    
    // if it's time to do a rank update
    if (((ttiNumber) % rankUpdateFrequency) == 0)
    {
        // Only do rank updates if: 
        // - we are in Closed Loop Spatial Multiplexing mode
        // - if the receiver's max. supported rank is bigger than 1
        // - if this TTI it's time for a rank update
        // tti counting starts at 1
        if ((receiver->getMaxRank() > 1) &&
            (precodingMode == ClosedLoopCodebookBased))
        {
            // note that the newly computed rank will only be updated in the FeedbackPtr itself
            // during a subsequent PMI/CQI update
            feedbackContainer->currentRank = performRankUpdate(receivingStation);
        }
    }

    // if it's time to do a PMI/CQI update and set the new rank:
    if (((ttiNumber) % cqiUpdateFrequency) == 0)
    {
        if (precodingMode == ClosedLoopCodebookBased)
        {
            // do cqi and pmi update
            determinePMIsAndCQIs(receivingStation, feedback, feedbackContainer->currentRank, ttiNumber);
        }
        
        else if ((precodingMode == NoPrecoding) || (precodingMode == SingleAntenna))
        {
            if (((ttiNumber) % cqiUpdateFrequency) == 0)
            {
                determineNoPrecodingCQIs(receivingStation, feedback, feedbackContainer->currentRank, precodingMode, ttiNumber);
            }

        }
        else
        {
            assure(0, "unsupported precoding mode");
        }

        // the cqi only gets updated every cqiUpdateFrequency-th TTI, so the current feedback
        // will also be valid during those TTIs
        for (unsigned int delta = 1; delta < cqiUpdateFrequency; delta++)
        {
                /*            std::cout << "Copying current index " << ttiNumber % bufferLength
                *                 << " to future index " << (ttiNumber + delta) % bufferLength
                *                 << " for receiver " << receivers[receiverIndex].first->getName() << "\n";*/
            feedbackContainer->ringBuffer[(ttiNumber + delta) % bufferLength] =
            feedbackContainer->ringBuffer[index];
        }
    }
}

void 
LteRel8DownlinkFeedbackManager::setReferencePerPRBTxPowerForUser(wns::node::Interface* node, wns::Power referencePower)
{
    assure(node2StationLookup.find(node) != node2StationLookup.end(), "Got reference power for unknown node");

    referencePowerMap[node2StationLookup[node]] = std::vector<wns::Power>(numPRBs, referencePower);
}

void 
LteRel8DownlinkFeedbackManager::setReferencePerPRBTxPowerForUser(wns::node::Interface* node, imtaphy::interface::PRB prb, wns::Power referencePower)
{
    assure(node2StationLookup.find(node) != node2StationLookup.end(), "Got reference power for unknown node");
    assure(prb < numPRBs, "Invalid PRB");
    
    if (referencePowerMap.find(node2StationLookup[node]) == referencePowerMap.end())
    {
        referencePowerMap[node2StationLookup[node]] = std::vector<wns::Power>(numPRBs, referencePower);
    }
    else
    {
        referencePowerMap[node2StationLookup[node]][prb] = referencePower;
    }
}


void 
LteRel8DownlinkFeedbackManager::registerMS(imtaphy::StationPhy* station, wns::node::Interface* node, imtaphy::receivers::LinearReceiver* receiver, imtaphy::Channel* channel_)
{
    if (!initialized)
    {
        channel = channel_;
        if (enabled)
        {
            this->startObserving(channel);
        }
        
        numPRBs = channel->getSpectrum()->getNumberOfPRBs(imtaphy::Downlink);
        initialized = true;

        defaultFeedback = LteRel8DownlinkFeedbackPtr(new LteRel8DownlinkFeedback(numPRBs));
    }

    // always extending the vector might be inefficient during startup but will be easier to traverse
    // during runtime especially for multi-threaded execution
    receivingStations.push_back(std::make_pair<wns::node::Interface*, imtaphy::StationPhy*>(node, station));

    unsigned int bufferLength = cqiUpdateFrequency + feedbackTotalDelay + 1;
    perNodeFeedback[node] = new FeedbackContainer(numPRBs, bufferLength);
    
    // we need the serving BS that transmits to that MS
    imtaphy::Link* link = channel->getLinkManager()->getServingLinkForMobileStation(station);
    servingBSMap[station] = link->getBS();
    
    lastIPNCovariances[node] = std::vector<imtaphy::detail::ComplexFloatMatrixPtr>(numPRBs, dummyIPNPtr);
    ipnDifferences[node] = std::vector<double>();
    
    
    node2StationLookup[node] = station;
}

LteRel8DownlinkFeedbackPtr 
LteRel8DownlinkFeedbackManager::getFeedback(wns::node::Interface* node, unsigned int ttiNumber)
{
    // get a delayed entry from the ringbuffer

    // 3GPP assumes e.g. 6ms delay total by which they mean that a measurement in subframe n is used in subframe n+6
    // For us, the update and scheduling are done at the beginning of a 1ms subframe
    // The sequence as controlled by Channel.cpp::periodically is
    // 1) with old channel / TTI, signal all beforeTTIover-observers (like us) that the TTI is about to end
    // 2) transmissions from the old TTI get evaluated with old channel
    // 3) TTI++ and channel evolved one time step
    // 4) onNewTTI called to inform e.g., schedulers for now new TTI
    // So, if the Feedback Manager estimates the feedback with channel at TTI=100,
    // the scheduler would have to get it at TTI=106 so that it will take effect immediately in TTI=106

    
    if (perNodeFeedback.find(node) == perNodeFeedback.end())
    {
        std::cout << "Requested receiver not found, returning default feedback\n";
        return defaultFeedback;
    }
    else
    {

        FeedbackContainer* feedbackContainer = perNodeFeedback[node];
        unsigned int index = (ttiNumber + feedbackContainer->bufferLength - feedbackTotalDelay) % feedbackContainer->bufferLength;

        return feedbackContainer->ringBuffer[index];
    }
}

wns::Ratio 
LteRel8DownlinkFeedbackManager::computeGeometry(wns::node::Interface* user)
{
    if (geometryCache.find(user) == geometryCache.end())
    {
    
        assure(node2StationLookup.find(user) != node2StationLookup.end(), "Unknown user");
        imtaphy::StationPhy* station = node2StationLookup[user];
        
        imtaphy::LinkMap allLinks = channel->getLinkManager()->getAllLinksForStation(station);
        
        assure(station->getStationType() == imtaphy::MOBILESTATION, "DL Feedback Manager can only compute geometry for UEs");
        
        imtaphy::StationList allBSs = channel->getAllBaseStations();
        imtaphy::StationPhy* servingBS = servingBSMap[station];
        
        wns::Power referencePower = wns::Power::from_dBm(24);
        wns::Power signal = wns::Power::from_mW(0);
        
        imtaphy::receivers::LinearReceiver* linearReceiver = dynamic_cast<imtaphy::receivers::LinearReceiver*>(station->getReceiver());
        assure(linearReceiver, "Could not get linear receiver pointer");
        wns::Power interference = linearReceiver->getThermalNoiseIncludingNoiseFigure();
        
        for (imtaphy::StationList::const_iterator iter = allBSs.begin(); iter != allBSs.end(); iter++)
        {
            if (*iter != servingBS)
            {
                assure(allLinks.find(*iter) != allLinks.end(), "No link to interfering BS");
                interference += referencePower / allLinks[*iter]->getWidebandLoss();
            }
            else
            {
                assure(allLinks.find(*iter) != allLinks.end(), "No link to serving BS");
                signal += referencePower / allLinks[*iter]->getWidebandLoss();
            }
        }

        geometryCache[user] = signal / interference;
//        std::cout << "Computing geometry of user " << user->getName() << " to " << geometryCache[user] << "\n";
    }
    
    return geometryCache[user];
}

void
LteRel8DownlinkFeedbackManager::computeIPNVariation(imtaphy::StationPhy* receivingStation, unsigned int ttiNumber)
{
    imtaphy::receivers::LinearReceiver* receiver = dynamic_cast<imtaphy::receivers::LinearReceiver*>(receivingStation->getReceiver());
    assure(receiver, "Expected a LinearReceiver instance");
  
    wns::node::Interface* node = receivingStation->getNode();
    
    imtaphy::detail::ComplexFloatMatrix differenceMatrix(receiver->getNumRxAntennas(), receiver->getNumRxAntennas());
    
    for (unsigned int prb = 0; prb < numPRBs; prb++)
    {
        // determine all inter-cell interference sources on the considered PRB
        InterferersCollectionPtr interferers = receiver->getInterferersOnPRB(prb, imtaphy::receivers::LinearReceiver::ExcludeNode, imtaphy::TransmissionPtr(static_cast<imtaphy::Transmission*>(NULL)), receivingStation);
        imtaphy::detail::ComplexFloatMatrixPtr newIPNcovariance = receiver->getPerfectInterferenceAndNoiseCovariance(prb, interferers);

        if (lastIPNCovariances[node][prb].get() == dummyIPNPtr.get())
        {
            lastIPNCovariances[node][prb] = newIPNcovariance;
        }
        else
        {
            ipnDifferences[node].resize(numPRBs);
            
            imtaphy::detail::matrixCisAminusB(*newIPNcovariance, *(lastIPNCovariances[node][prb]), differenceMatrix);
            lastIPNCovariances[node][prb] = newIPNcovariance;
            
//             std::cout << "PRB " << prb << ":\n"; 
//             std::cout << "Old covariance=\n";
//             imtaphy::detail::displayMatrix(*(lastIPNCovariances[node][prb]));
//             
//             std::cout << "New covariance=\n";
//             imtaphy::detail::displayMatrix(*newIPNcovariance);
//             
//             std::cout << "Difference matrix=\n";
//             imtaphy::detail::displayMatrix(differenceMatrix);

            ipnDifferences[node][prb] = imtaphy::detail::matrixNormSquared(differenceMatrix) / imtaphy::detail::matrixNormSquared(*newIPNcovariance);
//             std::cout << "Squared norm of difference matrix=" << imtaphy::detail::matrixNormSquared(differenceMatrix) 
//                       << " and of IPN covariance " << imtaphy::detail::matrixNormSquared(*newIPNcovariance)
//                       << " and of ratio: " << ipnDifferences[node][prb] << "\n";
        }
    }
}
