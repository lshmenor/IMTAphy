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

#include <IMTAPHY/receivers/ProbingReceiver.hpp>
#include <IMTAPHY/detail/LinearAlgebra.hpp>
#include <IMTAPHY/Link.hpp>
#include <IMTAPHY/Channel.hpp>
#include <IMTAPHY/interface/TransmissionStatus.hpp>
#include <IMTAPHY/linkManagement/LinkManager.hpp>
#include <IMTAPHY/receivers/LteRel8Codebook.hpp>
#include <IMTAPHY/receivers/feedback/LteRel8DLFeedbackManager.hpp>

#include <WNS/probe/bus/ContextProvider.hpp>
#include <WNS/probe/bus/ContextProviderCollection.hpp>

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    imtaphy::receivers::ProbingReceiver,
    imtaphy::receivers::ReceiverInterface,
    "imtaphy.receiver.ProbingReceiver",
    imtaphy::StationModuleCreator);

using namespace imtaphy;
using namespace imtaphy::receivers;

ProbingReceiver::ProbingReceiver(imtaphy::StationPhy* _station, const wns::pyconfig::View& pyConfigView) :
    LinearReceiver(_station, pyConfigView)
{
}

void 
ProbingReceiver::channelInitialized(Channel* _channel)
{
    imtaphy::receivers::LinearReceiver::channelInitialized(_channel);
    
    wns::probe::bus::ContextProviderCollection localcpc(station->getNode()->getContextProviderCollection());
    
    
    pmiRelativeStrengthContextCollector = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(localcpc,
                                                               "pmiRelativeStrength"));

    pmiStrongestContextCollector = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(localcpc,
                                                               "pmiStrongest"));

    reuse3GainContextCollector = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(localcpc,
                                                               "reuse3Gain"));

    reuse3CapacityGainContextCollector = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(localcpc,
                                                               "reuse3CapacityGain"));

    mutualInformationContextCollector = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(localcpc, "mutualInformation"));

    mutualInformationReuse3ContextCollector = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(localcpc, "mutualInformationReuse3"));
    reuse3MIgainContextCollector = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(localcpc, "reuse3MIgain"));

    
}


ReceiverFeedback
ProbingReceiver::computeFeedback(unsigned int prb, 
                                unsigned int rank,
                                imtaphy::StationPhy* nodeToEstimate,
                                wns::Power assumedTxPower,
                                std::vector<imtaphy::detail::ComplexFloatMatrixPtr>& precodingsToTest)
{
    unsigned int numTxAntennasServingBS = nodeToEstimate->getAntenna()->getNumberOfElements();
    
    assure(channel, "Need access to channel");
    assure(rank <= std::min(numRxAntennas, numTxAntennasServingBS), "Rank not supported by antenna configuration");
    
    ReceiverFeedback result;
    result.bestSINRs.resize(rank);
    result.pmiRanking.resize(precodingsToTest.size());
    
    std::vector<float> bestSINRs(rank, 0.0);
    
    imtaphy::TransmissionVector allTransmissions = channel->getTransmissionsOnPRB(receivingInDirection, prb);

    // determine all inter-cell interference sources on the considered PRB
    InterferersCollectionPtr interferers = getInterferersOnPRB(prb, ExcludeNode, TransmissionPtr(static_cast<Transmission*>(NULL)), nodeToEstimate);
    imtaphy::detail::ComplexFloatMatrixPtr perfectIandNoiseCovarianceMatrix = getPerfectInterferenceAndNoiseCovariance(prb, interferers);

    std::multimap<double, unsigned int> capacitiesForPMIs;
    double currentBestCapacity = 0.0;

    // get serving channel and precode it
    // the precoded channel is also a u-by-m complex matrix
    std::complex<float>* HsPerfect;
    HsPerfect = allMyLinks[nodeToEstimate]->getRawChannelMatrix(receivingInDirection, prb);

    imtaphy::detail::ComplexFloatMatrixPtr HsEstimated;

    if (channelEstimation)
    {
        HsEstimated = getEstimatedChannel(prb, allMyLinks[nodeToEstimate], assumedTxPower);
    }
        
    
    imtaphy::detail::ComplexFloatMatrix precodedServingChannelPerfect(numRxAntennas, rank);

    // the receiveFilter is a u-by-m complex matrix  
    imtaphy::detail::ComplexFloatMatrix receiveFilter(numRxAntennas, rank);
    
    
    std::map<unsigned int, wns::Ratio> pmiStrengthThisPRB;
    
    // loop over all possible precodings to see which one gives the highest capacity
    for (unsigned int pmi = 0; pmi < precodingsToTest.size();  pmi++)
    {
        // prepare everything needed to compute filters
        imtaphy::detail::ComplexFloatMatrixPtr precoding = precodingsToTest[pmi];
        
        // apply precoding to the serving channel
        imtaphy::detail::matrixMultiplyCequalsAB(precodedServingChannelPerfect, HsPerfect, *precoding);

        // scale the precoded serving channel by the transmit power
        imtaphy::detail::scaleMatrixA(precodedServingChannelPerfect, 
                                      static_cast<float>(sqrt(assumedTxPower.get_mW())));

        if (channelEstimation)
        {   // compute the filter based on the estimated channel
            imtaphy::detail::ComplexFloatMatrix precodedServingChannelEstimated(numRxAntennas, rank);
            imtaphy::detail::matrixMultiplyCequalsAB(precodedServingChannelEstimated, *HsEstimated, *precoding);

            filter->computeFilter(receiveFilter, precodedServingChannelEstimated, interferers, prb, nodeToEstimate, rank);

        }
        else // no channel estimation model, just use perfect channel knowledge to compute the filter
        {
            filter->computeFilter(receiveFilter, precodedServingChannelPerfect, interferers, prb, nodeToEstimate, rank);
        }
        
       
        // W is u-by-m and Hs is u-by-s
        // and the hermitian of it a m-by-u matrix
        imtaphy::detail::ComplexFloatMatrixPtr receiveFilterHermitian = imtaphy::detail::matrixHermitian<float>(receiveFilter);
        
        SINRComputationResultPtr sinrResult = computeSINRs(precodedServingChannelPerfect,
                                                            receiveFilter,
                                                            *receiveFilterHermitian,
                                                            *perfectIandNoiseCovarianceMatrix,
                                                            numRxAntennas, 
                                                            numTxAntennasServingBS, 
                                                            rank);

        assure((sinrResult->interferenceAndNoisePower.size() == rank) &&
               (sinrResult->rxPower.size() == rank) &&
               (sinrResult->scaledNoisePower.size() == rank), "wrong number of layers");
        
        imtaphy::interface::SINRVector sinrs(rank);
        for (unsigned int i = 0; i < rank; i++)
        {
            sinrs[i] = wns::Ratio::from_factor(sinrResult->rxPower[i].get_mW() / sinrResult->interferenceAndNoisePower[i].get_mW());
        }
        
        
        double capacity = 0.0;
        double log2 = 1.0 / log(2.0);
    
        for (unsigned int m = 0; m < sinrs.size(); m++)
        {
            // the map[] operator is supposed to default-construct the value with 0
            // so it is safe to use it like this
            pmiStrength[pmi] += sinrs[m].get_factor();
            pmiStrengthThisPRB[pmi] += sinrs[m];
            pmiCounter[pmi]++;
            
            // limit the contribution to the useful range in LTE below 22 dB
            wns::Ratio sinr = sinrs[m];
            if (sinr > wns::Ratio::from_dB(22.0))
                sinr = wns::Ratio::from_dB(22.0);
            capacity += log(1.0 + sinr.get_factor()) * log2;
        }
        
        capacitiesForPMIs.insert(std::make_pair<double, unsigned int>(capacity, pmi));
        
        if (capacity > currentBestCapacity)
        {
            currentBestCapacity = capacity;
            for (unsigned int layer = 0; layer < rank; layer++)
                bestSINRs[layer] = sinrs[layer].get_factor();
        }
    }

    unsigned int i = 0;
    for (std::multimap<double, unsigned int>::reverse_iterator iter = capacitiesForPMIs.rbegin(); iter != capacitiesForPMIs.rend(); i++, iter++)
    {
        // the best 
        relativeStrengths[i].put((pmiStrengthThisPRB[capacitiesForPMIs.rbegin()->second] - // SINR for best PMI
                                 pmiStrengthThisPRB[iter->second]).get_dB()                // SINR for this PMI
        );
        result.pmiRanking[i] = iter->second;
    }

    result.bestSINRs = bestSINRs;

    return result;
}

void 
ProbingReceiver::receive(TransmissionPtr transmission)
{
    wns::node::Interface* source = transmission->getSource()->getNode();
    imtaphy::Link* link = transmission->getLink();

    unsigned int numberOfLayers = transmission->getNumLayers();
    unsigned int numTxAntennas = transmission->getNumTxAntennas();

    const imtaphy::interface::PrbPowerPrecodingMap& prbPowerPrecodingMap = transmission->getPrbPowerPrecodingMap();
    
    // maybe we can get rid of this prb vector thing alltogether (refactoring transmission status?)
    imtaphy::interface::PRBVector prbs(prbPowerPrecodingMap.size());
    unsigned int n = 0;
    for (imtaphy::interface::PrbPowerPrecodingMap::const_iterator iter = prbPowerPrecodingMap.begin(); iter != prbPowerPrecodingMap.end(); iter++, n++)
    {
        prbs[n] = iter->first;
    }  
    
    imtaphy::interface::TransmissionStatusPtr status(new imtaphy::interface::TransmissionStatus(numberOfLayers,
                                                                                                          prbs));
    
    // the receiveFilter is a u-by-m complex matrix
    imtaphy::detail::ComplexFloatMatrix receiveFilter(numRxAntennas, numberOfLayers);
    // the precoded channel is also a u-by-m complex matrix
    imtaphy::detail::ComplexFloatMatrix precodedServingChannelPerfect(numRxAntennas, numberOfLayers);
    
     
    // loop over all PRBs used in the transmission
    for (unsigned int i = 0; i < prbs.size(); i++)
    {   
        imtaphy::interface::PRB prb = prbs[i];
        
        std::complex<float>* Hs = link->getRawChannelMatrix(receivingInDirection, prb);
        imtaphy::detail::ComplexFloatMatrixPtr precoding = transmission->getPrecodingMatrix(prb);

        // apply precoding to the serving channel
        imtaphy::detail::matrixMultiplyCequalsAB(precodedServingChannelPerfect, Hs, *precoding);

        // scale the precoded serving channel by the transmit power
        imtaphy::detail::scaleMatrixA(precodedServingChannelPerfect, 
                                      static_cast<float>(sqrt(transmission->getTxPower(prb).get_mW())));
        // get a list of interfering transmissions
        InterferersCollectionPtr interferers = getInterferersOnPRB(prb, ExcludeTransmission, transmission, static_cast<StationPhy*>(NULL));

        if (channelEstimation)
        {   // compute the filter based on the estimated channel
            
            imtaphy::detail::ComplexFloatMatrixPtr Hestimated = getEstimatedChannel(prb, link, transmission->getTxPower(prb));
            imtaphy::detail::ComplexFloatMatrix precodedServingChannelEstimated(numRxAntennas, numberOfLayers);
            // apply precoding to the serving channel
            imtaphy::detail::matrixMultiplyCequalsAB(precodedServingChannelEstimated, *Hestimated, *precoding);

            filter->computeFilter(receiveFilter, precodedServingChannelEstimated, interferers, prb, transmission->getSource(), numberOfLayers);
        }
        else // no channel estimation model, just use perfect channel knowledge to compute the filter
        {
            filter->computeFilter(receiveFilter, precodedServingChannelPerfect, interferers, prb, transmission->getSource(), numberOfLayers);
        }
            
        // X is m-by-m, W is u-by-m and Hs is u-by-s
        imtaphy::detail::ComplexFloatMatrixPtr receiveFilterHermitian = imtaphy::detail::matrixHermitian<float>(receiveFilter);
        
        imtaphy::detail::ComplexFloatMatrixPtr perfectIandNoiseCovarianceMatrix = getPerfectInterferenceAndNoiseCovariance(prb, interferers);
        SINRComputationResultPtr sinrResult = computeSINRs(precodedServingChannelPerfect,
                                                            receiveFilter,
                                                            *receiveFilterHermitian,
                                                            *perfectIandNoiseCovarianceMatrix,
                                                            numRxAntennas, 
                                                            numTxAntennas,
                                                            numberOfLayers);

        assure((sinrResult->interferenceAndNoisePower.size() == numberOfLayers) &&
               (sinrResult->rxPower.size() == numberOfLayers) &&
               (sinrResult->scaledNoisePower.size() == numberOfLayers), "wrong number of layers");
        
        
        imtaphy::interface::SINRVector sinrs(numberOfLayers);
        imtaphy::interface::IoTVector interferenceOverThermal(numberOfLayers);
        for (unsigned int i = 0; i < numberOfLayers; i++)
        {
            sinrs[i] = wns::Ratio::from_factor(sinrResult->rxPower[i].get_mW() / sinrResult->interferenceAndNoisePower[i].get_mW());
            interferenceOverThermal[i] = wns::Ratio::from_factor(sinrResult->interferenceAndNoisePower[i].get_mW() / sinrResult->scaledNoisePower[i].get_mW());

            if (perUserLinSINR.find(source) == perUserLinSINR.end())
            {
                perUserLinSINR[source] = MomentsVector(numPRBs);
            }
            if (perUserLinIoT.find(source) == perUserLinIoT.end())
            {
                perUserLinIoT[source] = MomentsVector(numPRBs);
            }
            
            perUserLinSINR[source][prb].put(sinrs[i].get_factor());
            perUserLinIoT[source][prb].put(interferenceOverThermal[i].get_factor());
        }

        double mi = computeMIMOMutualInformation(precodedServingChannelPerfect, *perfectIandNoiseCovarianceMatrix);
        if (mi >= 0)
        {
            perUserSumMI[source] += mi;
        }
        
        status->setSINRsForPRB(prb, sinrs);
        status->setIoTsForPRB(prb, interferenceOverThermal);
        
        bool compareAgainstReuse3 = true;
        if (compareAgainstReuse3)
        {
            InterferersCollectionPtr reuse3Interferers(new InterferersCollection());
            InterferersSet& allInterferers = interferers->getInterferersSet();
            
            for (InterferersSet::iterator iter = allInterferers.begin(); iter != allInterferers.end(); iter++)
            {
                if ((iter->interferingTransmission->getLink()->getBS()->getStationID() % 3) == (transmission->getLink()->getBS()->getStationID() % 3))
                {
                    reuse3Interferers->insert(const_cast<Interferer&>(*iter));
                }
            }
            
            reuse3Interferers->seal();
            
            if (channelEstimation)
            {   // compute the filter based on the estimated channel
                
                imtaphy::detail::ComplexFloatMatrixPtr Hestimated = getEstimatedChannel(prb, link, transmission->getTxPower(prb));
                imtaphy::detail::ComplexFloatMatrix precodedServingChannelEstimated(numRxAntennas, numberOfLayers);
                // apply precoding to the serving channel
                imtaphy::detail::matrixMultiplyCequalsAB(precodedServingChannelEstimated, *Hestimated, *precoding);

                filter->computeFilter(receiveFilter, precodedServingChannelEstimated, reuse3Interferers, prb, transmission->getSource(), numberOfLayers);
            }
            else // no channel estimation model, just use perfect channel knowledge to compute the filter
            {
                filter->computeFilter(receiveFilter, precodedServingChannelPerfect, reuse3Interferers, prb, transmission->getSource(), numberOfLayers);
            }
                
            // X is m-by-m, W is u-by-m and Hs is u-by-s
            imtaphy::detail::ComplexFloatMatrixPtr receiveFilterHermitian = imtaphy::detail::matrixHermitian<float>(receiveFilter);
            
            // 
            imtaphy::detail::ComplexFloatMatrixPtr reuse3IPNCov = getPerfectInterferenceAndNoiseCovariance(prb, reuse3Interferers);
            SINRComputationResultPtr reuse3SinrResult = computeSINRs(precodedServingChannelPerfect,
                                                                receiveFilter,
                                                                *receiveFilterHermitian,
                                                                *reuse3IPNCov,
                                                                numRxAntennas, 
                                                                numTxAntennas,
                                                                numberOfLayers);
            imtaphy::interface::SINRVector reuse3Sinrs(numberOfLayers);
            imtaphy::interface::IoTVector reuse3InterferenceOverThermal(numberOfLayers);
            for (unsigned int i = 0; i < numberOfLayers; i++)
            {
                reuse3Sinrs[i] = wns::Ratio::from_factor(reuse3SinrResult->rxPower[i].get_mW() / reuse3SinrResult->interferenceAndNoisePower[i].get_mW());
                reuse3InterferenceOverThermal[i] = wns::Ratio::from_factor(reuse3SinrResult->interferenceAndNoisePower[i].get_mW() / reuse3SinrResult->scaledNoisePower[i].get_mW());
                
                reuse3SINR[source].put(reuse3Sinrs[i].get_factor());
                
                reuse1Capacity[source] += log(1.0 + sinrs[i].get_factor());
                reuse3Capacity[source] += 1.0 / 3.0 * log(1.0 + reuse3Sinrs[i].get_factor());
            }
            
            double reuse3MI = computeMIMOMutualInformation(precodedServingChannelPerfect, *reuse3IPNCov) / 3.0;
            if (reuse3MI >= 0)
            {
                perUserReuse3SumMI[source] += reuse3MI;
            }
            
            
        }        
    }
    

    // store what we have received until it is "time" to deliver it to the station/Layer2
    // the reason for this two-step process is avoiding nasty race conditions (e.g. updating the
    // event scheduler) when receive is called in parallel from multiple threads
    
    Reception currentReception;
    currentReception.transportBlocks = transmission->getTransportBlocks();
    currentReception.source = source;
    currentReception.status = status;

    currentReceptions[transmission] = currentReception;
}

void 
ProbingReceiver::deliverReception(TransmissionPtr transmission)
{
    imtaphy::receivers::LinearReceiver::deliverReception(transmission);
    
}


wns::Ratio
ProbingReceiver::getAverageSINR(wns::node::Interface* source)
{
    double avg = 0.0;
    double counter = 0.0;
    for (unsigned int prb = 0; prb < numPRBs; prb++)
    {
        if (perUserLinSINR[source][prb].trials() > 0)
        {
            avg += perUserLinSINR[source][prb].mean();
            counter++;
        }
    }
    
    return wns::Ratio::from_factor(avg/counter);
}


unsigned int
ProbingReceiver::getBaseSINRIndex(wns::node::Interface* source)
{
    double shift = 10.0;
    double baseSinr = getAverageSINR(source).get_dB();
    

    // we want to compute the index for our bar chart
    // we can only probe positive indices, so the smallest values has to become 0
    // the bar chart will be centered at x.0 dB so for integer x, e.g. (x-0.5..x+0.5] should be counted
    // towards the x bin
    
    // this shifts into the positive domain
    baseSinr += shift;
    
    // and this rounds to the nearest integer
    return static_cast<int>(baseSinr + 0.5);
}

void
ProbingReceiver::onShutdown()
{
    std::multimap<double, unsigned int> strongestPMI;
    
    for (std::map<unsigned int, double>::iterator iter = pmiStrength.begin(); iter != pmiStrength.end(); iter++)
    {
        iter->second /= static_cast<double>(pmiCounter[iter->first]);
//         pmiStrengthContextCollector->put(wns::Ratio::from_factor(iter->second).get_dB(),
//                                          boost::make_tuple("PMI", iter->first));
        
        strongestPMI.insert(std::make_pair<double, unsigned int>(iter->second, iter->first));
    }
    
    if (strongestPMI.size() > 0)
    {
        pmiStrongestContextCollector->put(strongestPMI.rbegin()->second);
    }
    
    for (std::map<unsigned int,  wns::evaluation::statistics::Moments>::iterator iter = relativeStrengths.begin();
         iter != relativeStrengths.end(); iter++)
    {
        pmiRelativeStrengthContextCollector->put(iter->second.mean(), boost::make_tuple("PMI", iter->first));
    }
    
    for (NodeMomentsMap::iterator iter = reuse3SINR.begin(); iter != reuse3SINR.end(); iter++)
    {

        double reuse3Sinr = wns::Ratio::from_factor(iter->second.mean()).get_dB();
        reuse3GainContextCollector->put(reuse3Sinr - getAverageSINR(iter->first).get_dB(), 
                                        boost::make_tuple("BaseSINR", getBaseSINRIndex(iter->first),
                                                          "MSpos.x", node2LinkMap[iter->first]->getMS()->getPosition().getX(),
                                                          "MSpos.y", node2LinkMap[iter->first]->getMS()->getPosition().getY()));
    }

    for (std::map<wns::node::Interface*, double>::const_iterator iter = reuse1Capacity.begin(); iter != reuse1Capacity.end(); iter++)
    {
        reuse3CapacityGainContextCollector->put(reuse3Capacity[iter->first] / reuse1Capacity[iter->first],
                                                boost::make_tuple("BaseSINR", getBaseSINRIndex(iter->first),
                                                          "MSpos.x", node2LinkMap[iter->first]->getMS()->getPosition().getX(),
                                                          "MSpos.y", node2LinkMap[iter->first]->getMS()->getPosition().getY()));
    }
    
    
    for (std::map<wns::node::Interface*, double>::const_iterator iter = perUserSumMI.begin(); iter != perUserSumMI.end(); iter++)
    {
        // divide sum MI by number of 1 ms TTIs and by number of PRBs because it should be normalized to 1 second and 1 Hertz
        double meanMI = iter->second / static_cast<double>(channel->getTTI()) / static_cast<double>(numPRBs);
        mutualInformationContextCollector->put(meanMI,
                                               boost::make_tuple("BaseSINR", getBaseSINRIndex(iter->first),
                                                                 "MSpos.x", node2LinkMap[iter->first]->getMS()->getPosition().getX(),
                                                                 "MSpos.y", node2LinkMap[iter->first]->getMS()->getPosition().getY()));
    }
    
    for (std::map<wns::node::Interface*, double>::const_iterator iter = perUserReuse3SumMI.begin(); iter != perUserReuse3SumMI.end(); iter++)
    {
        // divide sum MI by number of 1 ms TTIs and by number of PRBs because it should be normalized to 1 second and 1 Hertz
        double meanMI = perUserSumMI[iter->first] / static_cast<double>(channel->getTTI()) / static_cast<double>(numPRBs);
        double meanReuse3MI = iter->second / static_cast<double>(channel->getTTI()) / static_cast<double>(numPRBs);

        mutualInformationReuse3ContextCollector->put(meanReuse3MI,
                                                     boost::make_tuple("BaseSINR", getBaseSINRIndex(iter->first),
                                                                       "MSpos.x", node2LinkMap[iter->first]->getMS()->getPosition().getX(),
                                                                       "MSpos.y", node2LinkMap[iter->first]->getMS()->getPosition().getY()));

        if (meanReuse3MI > 0)
        {
            reuse3MIgainContextCollector->put(meanReuse3MI / meanMI,
                                              boost::make_tuple("BaseSINR", getBaseSINRIndex(iter->first),
                                                                "MSpos.x", node2LinkMap[iter->first]->getMS()->getPosition().getX(),
                                                                "MSpos.y", node2LinkMap[iter->first]->getMS()->getPosition().getY()));
        }        
        
    }
    
    
    
    probeOnShutdown();
}

double 
inline
ProbingReceiver::computeMIMOMutualInformation(detail::ComplexFloatMatrix& precodedServingChannel, detail::ComplexFloatMatrix& ipnCovariance)
{
    // this might return a negative value in case the matrices are not invertible 
    // (hence the determinant will be zero and the log will return -inf)
    // this can also happen for numerical accuracy reasons or if matrices bigger than 3x3 are not Hermitian positive-definite
    
    detail::ComplexFloatMatrix totalCov(ipnCovariance);
    
    imtaphy::detail::matrixMultiplyCequalsAlphaSquareTimesAAhermitianPlusC(totalCov, // increment this
                                                                           precodedServingChannel, // add the alpha^2 * AA^H
                                                                           1.0f); // alpha^2
    double logDetTotal, logDetIPN;
    if (!imtaphy::detail::log2DetOfPosDefHermitianMatrix(totalCov, logDetTotal))
    {
        return -1;
    };
    
    if (!imtaphy::detail::log2DetOfPosDefHermitianMatrix(ipnCovariance, logDetIPN))
    {
        return -1;
    };

    
    return logDetTotal - logDetIPN;
}
