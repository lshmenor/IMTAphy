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

#include <IMTAPHY/receivers/LinearReceiver.hpp>
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
    imtaphy::receivers::LinearReceiver,
    imtaphy::receivers::ReceiverInterface,
    "imtaphy.receiver.LinearReceiver",
    imtaphy::StationModuleCreator);

using namespace imtaphy;
using namespace imtaphy::receivers;

LinearReceiver::LinearReceiver(imtaphy::StationPhy* _station, const wns::pyconfig::View& pyConfigView) :
    ReceiverInterface(_station, pyConfigView),
    station(_station),
    noiseFigure(pyConfigView.get<wns::Ratio>("noiseFigure")),
    logger(pyConfigView.get("logger")),
    scm(NULL),
    numRxAntennas(0),
    maxRank(1), // can be overwritten from filter or anybody else using setMaxRank method
    channelEstimation(NULL),
    numPRBs(0),
    perfectCovarianceTimestamp(0),
    estimatedCovarianceTimestamp(0),
    estimatedChannelTimestamp(0),
    config(pyConfigView)
{
    // create the filter module and pass its config 
    wns::pyconfig::View filterConfig = pyConfigView.get("filter");
    filter = wns::StaticFactory<wns::PyConfigViewCreator<imtaphy::receivers::filters::ReceiveFilterInterface> >::creator(filterConfig.get<std::string>("nameInFactory"))->create(filterConfig);
}   

void 
LinearReceiver::probeOnShutdown()
{
    assure(perUserLinSINR.size() == perUserLinIoT.size(), "You should always probe to both");
    
    for (NodeMomentsVectorMap::iterator iter = perUserLinSINR.begin(); iter != perUserLinSINR.end(); iter++)
    {
        double sinrSum = 0;
        double iotSum = 0;
        double sinrCounter = 0;
        double iotCounter = 0;

        for (unsigned int prb = 0; prb < numPRBs; prb++)
        {
            if (iter->second[prb].trials() > 0)
            {
                assure(perUserLinIoT[iter->first][prb].trials() > 0, "You should always probe to both");
                iotSum += perUserLinIoT[iter->first][prb].mean(); iotCounter += 1.0;
                sinrSum += iter->second[prb].mean(); sinrCounter += 1.0;
            }
        }
        
        int iot = roundToInt(wns::Ratio::from_factor(iotSum/iotCounter).get_dB());
        int sinr = roundToInt(wns::Ratio::from_factor(sinrSum/sinrCounter).get_dB());

        
        int msID; 
        if (receivingInDirection == imtaphy::Downlink)
        {
            msID = station->getNode()->getNodeID();
        }
        else
        {
            msID = iter->first->getNodeID();
        }
        
         avgSINRContextCollector->put(wns::Ratio::from_factor(sinrSum/sinrCounter).get_dB(), 
                                     boost::make_tuple(
//                                                    "propagation", node2LinkMap[iter->first]->getPropagation(),
//                                                "outdoorPropagation", node2LinkMap[iter->first]->getOutdoorPropagation(),
                                    "MSID", msID,
                                    "AvgIoT", iot,
                                    "MSpos.x", node2LinkMap[iter->first]->getMS()->getPosition().getX(),
                                    "MSpos.y", node2LinkMap[iter->first]->getMS()->getPosition().getY()));
   
         avgIoTContextCollector->put(wns::Ratio::from_factor(iotSum/iotCounter).get_dB(),
                                                    boost::make_tuple(
//                                                        "propagation", node2LinkMap[iter->first]->getPropagation(),
    //                                                    "outdoorPropagation", node2LinkMap[iter->first]->getOutdoorPropagation(),
                                                        "MSID", msID,
                                                        "AvgSINR", sinr,
                                                        "MSpos.x", node2LinkMap[iter->first]->getMS()->getPosition().getX(),
                                                        "MSpos.y", node2LinkMap[iter->first]->getMS()->getPosition().getY()));
        for (unsigned int prb = 0; prb < numPRBs; prb++)
        {
            if (iter->second[prb].trials() > 0)
            {
                assure(perUserLinIoT[iter->first][prb].trials() > 0, "You should always probe to both");
                
                stdDevIoTContextCollector->put(perUserLinIoT[iter->first][prb].deviation() / perUserLinIoT[iter->first][prb].mean(),
                                boost::make_tuple(
//                                                "propagation", node2LinkMap[iter->first]->getPropagation(),
//                                                "outdoorPropagation", node2LinkMap[iter->first]->getOutdoorPropagation(),
                                    "MSID", msID,
                                    "PRBP", prb,
                                    "AvgIoT",iot,
//                                    "AvgSINR", sinr,
                                    "MSpos.x", node2LinkMap[iter->first]->getMS()->getPosition().getX(),
                                    "MSpos.y", node2LinkMap[iter->first]->getMS()->getPosition().getY()));

                stdDevSINRContextCollector->put(iter->second[prb].deviation() / iter->second[prb].mean(),
                                                    boost::make_tuple(
    //                                                    "propagation", node2LinkMap[iter->first]->getPropagation(),
    //                                                    "outdoorPropagation", node2LinkMap[iter->first]->getOutdoorPropagation(),
                                                        "PRB", prb,
                                                        "MSID", msID,
                                                        "AvgIoT",iot,
//                                                        "AvgSINR", sinr,
                                                        "MSpos.x", node2LinkMap[iter->first]->getMS()->getPosition().getX(),
                                                        "MSpos.y", node2LinkMap[iter->first]->getMS()->getPosition().getY()));

            }
        }
    }


}   


void 
LinearReceiver::channelInitialized(Channel* _channel)
{
    channel = _channel;
    numRxAntennas = station->getAntenna()->getNumberOfElements();

    wns::probe::bus::ContextProviderCollection localcpc(station->getNode()->getContextProviderCollection());
    
    wblContextCollector = wns::probe::bus::ContextCollectorPtr(
                                                    new wns::probe::bus::ContextCollector(localcpc,
                                                    "wbl"));
    
    avgSINRContextCollector = wns::probe::bus::ContextCollectorPtr(
        new wns::probe::bus::ContextCollector(localcpc, "avgSINR"));


    stdDevSINRContextCollector = wns::probe::bus::ContextCollectorPtr(
        new wns::probe::bus::ContextCollector(localcpc, "stdDevSINR"));

    avgIoTContextCollector = wns::probe::bus::ContextCollectorPtr(
        new wns::probe::bus::ContextCollector(localcpc, "avgIoT"));

    stdDevIoTContextCollector = wns::probe::bus::ContextCollectorPtr(
        new wns::probe::bus::ContextCollector(localcpc, "stdDevIoT"));
    
    thermalNoiseInclNF = wns::Power::from_dBm(-174.0);
    thermalNoiseInclNF = thermalNoiseInclNF * wns::Ratio::from_factor(channel->getSpectrum()->getPRBbandWidthHz());
    thermalNoiseInclNF = thermalNoiseInclNF * noiseFigure;  

    // init noise covariance with identity
    NoiseCovariance = imtaphy::detail::ComplexFloatMatrixPtr(new imtaphy::detail::ComplexFloatMatrix(*(wns::SingletonHolder<imtaphy::detail::Identity<std::complex<float> > >::Instance().get(numRxAntennas))));
    
    // and now scale with thermal noise power + receiver noise figure
    imtaphy::detail::scaleMatrixA(*NoiseCovariance, static_cast<float>(thermalNoiseInclNF.get_mW()));
    
    scm = channel->getSpatialChannelModel();

    if (station->getStationType() == MOBILESTATION)
    { // only mobiles should compute feedback

        imtaphy::receivers::feedback::DownlinkFeedbackManagerInterface::getFeedbackManager()->registerMS(station,
                                                                                                         station->getNode(),
                                                                                                         this,
                                                                                                         channel);

        receivingInDirection = imtaphy::Downlink;
    }
    else
    {
        receivingInDirection = imtaphy::Uplink;
    }
    
    // cache all links towards my station so that lookups from Linear Receiver will be faster later
    allMyLinks = channel->getLinkManager()->getAllLinksForStation(station);
    
    node2LinkMap.clear();
    for (imtaphy::LinkMap::const_iterator iter = allMyLinks.begin(); iter != allMyLinks.end(); iter++)
    {
        node2LinkMap[iter->first->getNode()] = iter->second;
    }
    
    // the perfect I+N covariance computation can be hard-coded
    perfectIandNoiseCovariance = new imtaphy::receivers::channelEstimation::covariance::PerfectInterferenceAndNoiseCovariance(this);
 
    wns::pyconfig::View covarianceEstimationConfig(config.get("covarianceEstimation"));
    // the imperfect ("estimated") covariance computation routine can be configured by the PyConfig
    imperfectIandNoiseCovariance = wns::StaticFactory<imtaphy::receivers::ReceiverModuleCreator<imtaphy::receivers::channelEstimation::covariance::NoiseAndInterferenceCovarianceInterface> >::creator(covarianceEstimationConfig.get<std::string>("nameInFactory"))->create(this, covarianceEstimationConfig);

    if (!config.isNone("channelEstimation"))
    {
        wns::pyconfig::View channelEstimationConfig(config.get("channelEstimation"));
        channelEstimation = wns::StaticFactory<imtaphy::receivers::ReceiverModuleCreator<imtaphy::receivers::channelEstimation::channel::ChannelEstimationInterface> >::creator(channelEstimationConfig.get<std::string>("nameInFactory"))->create(this, channelEstimationConfig);
    }
    else
    {   // means no channel estimation, i.e., use perfect channels
        channelEstimation = NULL;
    }
    
    
    numPRBs = channel->getSpectrum()->getNumberOfPRBs(receivingInDirection);
    perfectCovarianceCache.resize(numPRBs);
    estimatedCovarianceCache.resize(numPRBs);
    
    filter->initFilterComputation(channel, this);
}

SINRComputationResultPtr
LinearReceiver::computeSINRs(imtaphy::detail::ComplexFloatMatrix& precodedH,  // is a numRxAntennas-by-numServingLayers matrix
                             imtaphy::detail::ComplexFloatMatrix& W, // is a numRxAntennas-by-numServingLayers
                             imtaphy::detail::ComplexFloatMatrix& Whermitian, // is a numServingLayers-by-numRxAntennas
                             imtaphy::detail::ComplexFloatMatrix& iAndNoiseCovariance,
                             unsigned int numRxAntennas, 
                             unsigned int numServingTxAntennas, 
                             unsigned int numServingLayers) const
{
    assure(precodedH.getColumns() == numServingLayers, "Equivalent channel precodedH has wrong number of columns");
    assure(precodedH.getRows() == numRxAntennas, "Equivalent channel precodedH has wrong number of rows");
    assure(W.getColumns() == numServingLayers, "Filter matrix W has wrong number of columns");
    assure(W.getRows() == numRxAntennas, "Filter matrix W has wrong number of rows");
    assure(Whermitian.getColumns() == numRxAntennas, "Hermitian of filter matrix W has wrong number of columns");
    assure(Whermitian.getRows() == numServingLayers, "Hermitian of filter matrix W has wrong number of rows");
    assure(iAndNoiseCovariance.getColumns() == numRxAntennas, "Noise and interference covariance matrix should be #rxAntennas x #rxAntennas");
    assure(iAndNoiseCovariance.getRows() == numRxAntennas, "Noise and interference covariance matrix should be #rxAntennas x #rxAntennas");

    
    
    // space to hold the result for norm of Whermitian x precodedH, size numServingLayers-by-numServingLayers
    // find a nicer name for this (stream-to-stream power or something)
    imtaphy::detail::FloatMatrix X(numServingLayers, numServingLayers);
    imtaphy::detail::ComplexFloatMatrix Y(numServingLayers, numServingLayers);
    imtaphy::detail::ComplexFloatMatrix temp(numServingLayers, numRxAntennas);
    
    // the function is called for each PRB, so return list of sinrs per layer for that PRB
    SINRComputationResultPtr result(new SINRComputationResult(numServingLayers));

    imtaphy::detail::matrixMultiplyCequalsNormOfAB<float>(X, Whermitian, precodedH);

    // count the diagonal elements in row i as desired power for stream i
    // and count the off-diagonal elements in row i as contributions to interstream interference affecting stream i
    for (unsigned int i = 0; i <  numServingLayers; i++)
    {   
        for (unsigned int j = 0; j < numServingLayers; j++)
            if (i != j)                    
                result->interferenceAndNoisePower[i] += wns::Power::from_mW(X[i][j]);
            else
                result->rxPower[i] = wns::Power::from_mW(X[i][j]);
            
        for (unsigned int r = 0; r < numRxAntennas; r++)
        {
            result->scaledNoisePower[i] += thermalNoiseInclNF * std::norm(Whermitian[i][r]);
        }
    }

    
    // Y = W^H * sumH * W, with sumH being the sum of interferers' covaricances plus noise
    imtaphy::detail::matrixMultiplyCequalsAB<float>(temp, Whermitian, iAndNoiseCovariance);
    imtaphy::detail::matrixMultiplyCequalsAB<float>(Y, temp, W);
   
    // collect the interferer's interference contributions to each of our layers                                                      
    for (unsigned int i = 0; i <  numServingLayers; i++)
    {
//         //TODO: find a better way to check for this. it seems that for very big real parts, the 
//         // imaginary component can also deviate significantly from 0 due to precision problems (float!)
//         if ((Y[i][i].real() != 0) && !((Y[i][i].imag() / Y[i][i].real() > -1e-4) && (Y[i][i].imag() / Y[i][i].real() < 1e-4)))
//         {
//             std::cout << "This should be a hermitian matrix with real diagonal:\n";
//             imtaphy::detail::displayMatrix<std::complex<float> >(Y);
//         }

//        assure((Y[i][i].imag() / Y[i][i].real() > -1e-4) && (Y[i][i].imag() / Y[i][i].real() < 1e-4), "The diagonal should be all real");

        // in rare cases the I+N covariance can be so ill-conditioned, that the operations above yield slightly negative values
        result->interferenceAndNoisePower[i] += wns::Power::from_mW(fabs(Y[i][i].real()));
    }
    
    return result;
}

InterferersCollectionPtr
LinearReceiver::getInterferersOnPRB(imtaphy::interface::PRB prb, InterfererExclusionMode exclusionMode, TransmissionPtr interferedTransmission, StationPhy* node)
{
    InterferersCollectionPtr result(new InterferersCollection());
    
    TransmissionVector allTransmissions = channel->getTransmissionsOnPRB(receivingInDirection, prb);
        
    for (unsigned int i = 0; i < allTransmissions.size(); i++)
    {
        if (exclusionMode == ExcludeTransmission)
        {
            // do not count the transmission as interference to itself
            if (allTransmissions[i] == interferedTransmission)
                continue;
        }
        else
        { // ExcludeNode
            // Exclude transmission from our own cell (the transmission comes from our Base Station (either us (UL) or the node to estimate (DL))
            // as interference
            if ((allTransmissions[i]->getLink()->getBS() == station) || (allTransmissions[i]->getLink()->getBS() == node))
                continue;
        }

        Interferer interferer;
                    
        interferer.interferingTransmission = allTransmissions[i];

        assure(allMyLinks.find(interferer.interferingTransmission->getSource()) != allMyLinks.end(), "No link to interferer");
        interferer.interferingLink = allMyLinks.find(interferer.interferingTransmission->getSource())->second;

        result->insert(interferer);
    }
    
    result->seal();
    return result;
}


/**
 * @brief Computes the best PMI and associated CQIs based on the LTE Rel. precoding codebook
 *
 * Note that this must be kept thread safe to allow calling it from parallel threads. This means
 * great care has to be taken to not create any race conditions. For example, writing to shared data
 * structures and calling functions that would have such side-effects, must be avoided.
 */

ReceiverFeedback
LinearReceiver::computeFeedback(unsigned int prb, 
                                unsigned int rank,
                                imtaphy::StationPhy* nodeToEstimate,
                                wns::Power assumedTxPower,
                                std::vector<imtaphy::detail::ComplexFloatMatrixPtr>& precodingsToTest)
{
    unsigned int numTxAntennasServingBS = nodeToEstimate->getAntenna()->getNumberOfElements();
    
    assure(channel, "Need access to channel");
//    assure(rank <= std::min(numRxAntennas, numTxAntennasServingBS), "Rank not supported by antenna configuration");
    
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
            wns::Ratio sinr = sinrs[m];

            if (rank > 1)
            {
                // limit the contribution to the useful range in LTE below 22 dB
                // in the case where we have to report a common PMI for multiple layers
                
                if (sinr > wns::Ratio::from_dB(22.0))
                    sinr = wns::Ratio::from_dB(22.0);
            }    
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
        result.pmiRanking[i] = iter->second;
    }

    result.bestSINRs = bestSINRs;

    return result;
}

/**
 * @brief Computes the SINR vectors for the indicated transmission
 *
 * Note that this must be kept thread safe to allow calling it from parallel threads. This means
 * great care has to be taken to not create any race conditions. For example, writing to shared data
 * structures and calling functions that would have such side-effects, must be avoided.
 * For this reason, the received data is not forwarded from this function but only stored for
 * forwarding in a sequential call to deliverReception later
 * 
 */

void 
LinearReceiver::receive(TransmissionPtr transmission)
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
        
        // 
        SINRComputationResultPtr sinrResult = computeSINRs(precodedServingChannelPerfect,
                                                            receiveFilter,
                                                            *receiveFilterHermitian,
                                                            *getPerfectInterferenceAndNoiseCovariance(prb, interferers),
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

        
        status->setSINRsForPRB(prb, sinrs);
        status->setIoTsForPRB(prb, interferenceOverThermal);
    } // end loop over prbs
    

    // store what we have received until it is "time" to deliver it to the station/Layer2
    // the reason for this two-step process is avoiding nasty race conditions (e.g. updating the
    // event scheduler) when receive is called in parallel from multiple threads
    
    Reception currentReception;
    currentReception.transportBlocks = transmission->getTransportBlocks();
    currentReception.source = source;
    currentReception.status = status;

    currentReceptions[transmission] = currentReception;
}

void LinearReceiver::deliverReception(TransmissionPtr transmission)
{
    assure(currentReceptions.size() != 0, "deliverReception() called without having received anything");

    if (currentReceptions.find(transmission) != currentReceptions.end())
    {
        Reception currentReception = currentReceptions[transmission];
        // each time we have received something, probe the wideband loss of our serving link to
        // have a pathgain CDF that allows us to examine potential deviations in the SINR CDF
        wblContextCollector->put(-1.0 * transmission->getLink()->getWidebandLoss().get_dB(),
                                 boost::make_tuple("propagation", transmission->getLink()->getPropagation(),
                                                   "outdoorPropagation", transmission->getLink()->getOutdoorPropagation()));
        
        station->onData(currentReception.transportBlocks,
                        currentReception.source,
                        currentReception.status);
    }
    
    currentReceptions.erase(transmission);
}

void 
LinearReceiver::onShutdown()
{
    probeOnShutdown();
}

