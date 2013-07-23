/*******************************************************************************
 * This file is part of IMTAphy
 * _____________________________________________________________________________
 *
 * Copyright (C) 2012
 * Institute for Communication Networks (LKN)
 * Associate Institute for Signal Processing (MSV)
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

#include <IMTAPHY/receivers/feedback/PU2RCFeedbackManager.hpp>
#include <IMTAPHY/Channel.hpp>
#include <IMTAPHY/linkManagement/LinkManager.hpp>

#include <WNS/StaticFactoryBroker.hpp>
#include <WNS/PyConfigViewCreator.hpp>
#include <algorithm>
#include <iostream>

using namespace imtaphy::receivers::feedback;

STATIC_FACTORY_BROKER_REGISTER(PU2RCFeedbackManager,
                               DownlinkFeedbackManagerInterface,
                               "PU2RCFeedbackManager");


void 
PU2RCFeedbackManager::determinePU2RCFeedback(imtaphy::StationPhy* receivingStation, LteRel8DownlinkFeedbackPtr feedback, unsigned int tti)
{
    PU2RCFeedback* pu2rcFeedback = dynamic_cast<PU2RCFeedback*>(feedback.get());
    assure(pu2rcFeedback, "No PU2RCFeedback");
    
    imtaphy::receivers::LinearReceiver* receiver = dynamic_cast<imtaphy::receivers::LinearReceiver*>(receivingStation->getReceiver());
    assure(receiver, "Expected a LinearReceiver instance");

    
    receiver->setMaxRank(1);
    assure(receiver->getMaxRank() == 1, "Only supporting rank 1 reception");


    assure(servingBSMap.find(receivingStation) != servingBSMap.end(), "Corresponding transmitter not found");
    imtaphy::StationPhy* transmitterToEstimate = servingBSMap[receivingStation];
    assure(transmitterToEstimate->getAntenna()->getNumberOfElements() == 4, "Only supporting 4Tx antennas");
    
    unsigned int numSubBands = (numPRBs % prbsPerSubband) != 0 ? static_cast<unsigned int>(numPRBs / prbsPerSubband) + 1 : numPRBs / prbsPerSubband;
    
    for (unsigned int subBand = 0; subBand < numSubBands; subBand++)
    {
        unsigned int midPRB = subBand * prbsPerSubband;
        if (midPRB + prbsPerSubband > numPRBs)
            midPRB += static_cast<unsigned int>((numPRBs - midPRB) / 2);
        else
            midPRB += static_cast <unsigned int>(prbsPerSubband / 2);

        std::pair<unsigned int, unsigned int> bestPair;
        wns::Ratio bestSINR = wns::Ratio::from_factor(0);
        
        std::vector<imtaphy::detail::ComplexFloatMatrixPtr> precoding(1);
        for (unsigned int pmiIndex = 0; pmiIndex < pmis.size(); pmiIndex++)
        {
            unsigned int pmi = pmis[pmiIndex];
            precoding[0] = codebook->getPrecodingMatrix(4, 4, pmi);
       
        
            imtaphy::receivers::ReceiverFeedback best = receiver->computeFeedback(midPRB, 4, 
                                                                                  transmitterToEstimate, 
                                                                                  referencePowerMap[receivingStation][midPRB], 
                                                                                  precoding);

            for (unsigned int column = 0; column < 4; column++)
            {
                wns::Ratio thisSINR = wns::Ratio::from_factor(best.bestSINRs[column]);
                if (thisSINR > bestSINR)
                {
                    bestSINR = thisSINR;
                    bestPair = std::make_pair<unsigned int, unsigned int>(pmi, column);
                }

                for (unsigned int prb = subBand * prbsPerSubband; (prb < (subBand + 1) * prbsPerSubband) && (prb < numPRBs); prb++)
                {
                    pu2rcFeedback->sinrMatrix[prb][pmi][column] = thisSINR;
                }
            }
        }
        
        for (unsigned int prb = subBand * prbsPerSubband; (prb < (subBand + 1) * prbsPerSubband) && (prb < numPRBs); prb++)
        {
            pu2rcFeedback->columnIndicator[prb] = bestPair.second;
            pu2rcFeedback->pmi[prb] = bestPair.first;
            pu2rcFeedback->cqiTb1[prb] = blerModel->getCQI(bestSINR);
        }
    }
    pu2rcFeedback->estimatedInTTI = std::vector<unsigned int>(numPRBs, tti);
    pu2rcFeedback->rank = 1;
}

void
PU2RCFeedbackManager::doUpdate(imtaphy::StationPhy* receivingStation, FeedbackContainer* feedbackContainer, unsigned int ttiNumber)
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
    
    // if it's time to do a PMI/CQI update and set the new rank:
    if (((ttiNumber) % cqiUpdateFrequency) == 0)
    {
        determinePU2RCFeedback(receivingStation, feedback, ttiNumber);

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
PU2RCFeedbackManager::registerMS(imtaphy::StationPhy* station, wns::node::Interface* node, imtaphy::receivers::LinearReceiver* receiver, imtaphy::Channel* channel_)
{
    LteRel8DownlinkFeedbackManager::registerMS(station, node, receiver, channel_);

    // replace LteRel8Feedback by PU2RCFeedback
    defaultFeedback = LteRel8DownlinkFeedbackPtr(new PU2RCFeedback(numPRBs, 16, 4));
    
    // dirty hack
    for (unsigned int i = 0; i < perNodeFeedback[node]->ringBuffer.size(); i++)
    {
         perNodeFeedback[node]->ringBuffer[i] = LteRel8DownlinkFeedbackPtr(new PU2RCFeedback(numPRBs, 16, 4));
    }
    
}

