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

#ifndef IMTAPHY_RECEIVERS_LINEARRECEIVER_HPP
#define IMTAPHY_RECEIVERS_LINEARRECEIVER_HPP

#include <WNS/pyconfig/View.hpp>
#include <IMTAPHY/receivers/channelEstimation/covariance/PerfectIandNCovariance.hpp>
#include <IMTAPHY/receivers/channelEstimation/channel/ChannelEstimationInterface.hpp>
#include <IMTAPHY/receivers/ReceiverInterface.hpp>
#include <IMTAPHY/detail/LinearAlgebra.hpp>
#include <IMTAPHY/spatialChannel/SpatialChannelModelInterface.hpp>
#include <IMTAPHY/receivers/feedback/LteFeedback.hpp>
#include <WNS/probe/bus/ContextCollector.hpp>
#include <WNS/PowerRatio.hpp>
#include <WNS/logger/Logger.hpp>
#include <IMTAPHY/receivers/filters/FilterInterface.hpp>
#include <WNS/evaluation/statistics/moments.hpp>
#include <IMTAPHY/Channel.hpp>

namespace imtaphy {

    namespace receivers {
        
        namespace tests {
            class MRCReceiverTest;
            class MMSEReceiverTest;
            class LinearReceiverTest;
            class QuantizedChannelTest;
        }
        
        struct ReceiverFeedback
        {
            std::vector<unsigned int> pmiRanking; // pmiRanking[0] gives best capacity
            std::vector<float> bestSINRs; // the sinrs belonging to pmiRanking[0]
        };

        struct QuantizedChannel
        {
            unsigned int codebookEntry; 
            float metric; 
            float channel_norm;
            QuantizedChannel(){
                codebookEntry = 0; 
                metric = 0; 
                channel_norm = 0;
            }
        };
        
        class Reception
        {
        public:
            Reception() {};
            Reception(const Reception &copy) :
                transportBlocks(copy.transportBlocks),
                source(copy.source),
                status(copy.status)
            {};
            
            std::vector<wns::ldk::CompoundPtr> transportBlocks;
            wns::node::Interface* source;
            imtaphy::interface::TransmissionStatusPtr status;
        };
        
        class SINRComputationResult
        {
        public:
            SINRComputationResult(unsigned int numServingLayers) :
                rxPower(numServingLayers, wns::Power::from_mW(0.0)),
                interferenceAndNoisePower(numServingLayers, wns::Power::from_mW(1e-42)), // avoid division by zero
                scaledNoisePower(numServingLayers, wns::Power::from_mW(0.0))
            {
            }
            std::vector<wns::Power> rxPower;
            std::vector<wns::Power> interferenceAndNoisePower;
            std::vector<wns::Power> scaledNoisePower;
            
        };
        typedef boost::shared_ptr<SINRComputationResult> SINRComputationResultPtr;
        
        class LinearReceiver :
            public ReceiverInterface
        {
        public:
            LinearReceiver(StationPhy* station, const wns::pyconfig::View& pyConfigView);
                
            virtual void channelInitialized(Channel* channel);
                
            virtual void receive(TransmissionPtr transmission);
            virtual void deliverReception(TransmissionPtr transmission);
            
            virtual ReceiverFeedback computeFeedback(unsigned int prb, 
                                                     unsigned int rank,
                                                     imtaphy::StationPhy* nodeToEstimate,
                                                     wns::Power assumedTxPower,
                                                     std::vector<imtaphy::detail::ComplexFloatMatrixPtr>& precodingsToTest);
                                          
            void setMaxRank(unsigned int rank) {
                maxRank = rank;
                assure((rank > 0) && (rank <= numRxAntennas), "invalid rank, must be 1..numRxAntennas"); 
            }

            unsigned int getMaxRank() const {return maxRank;}

            unsigned int getNumRxAntennas() const {return numRxAntennas;}
            
            StationPhy* getStation() const {return station;}
            
            imtaphy::detail::ComplexFloatMatrixPtr getNoiseCovariance() const {return NoiseCovariance;}
            
            virtual void onShutdown();
            

            typedef enum {
                ExcludeTransmission,
                ExcludeNode
            } InterfererExclusionMode;
            InterferersCollectionPtr getInterferersOnPRB(imtaphy::interface::PRB prb, InterfererExclusionMode exclusionMode, TransmissionPtr interferedTransmission, StationPhy* node);
           
            
            imtaphy::detail::ComplexFloatMatrixPtr getPerfectInterferenceAndNoiseCovariance(imtaphy::interface::PRB prb, InterferersCollectionPtr interferers)
            {
                if (channel->getTTI() != perfectCovarianceTimestamp)
                {
                    perfectCovarianceCache.clear();
                    perfectCovarianceCache.resize(numPRBs);
                    perfectCovarianceTimestamp = channel->getTTI();
                    
                }
                
                if (perfectCovarianceCache[prb].find(interferers) == perfectCovarianceCache[prb].end())
                {
                    perfectCovarianceCache[prb][interferers] = perfectIandNoiseCovariance->computeNoiseAndInterferenceCovariance(interferers, NoiseCovariance, prb);
                }
           
                
                return perfectCovarianceCache[prb][interferers];
            }

            
            imtaphy::detail::ComplexFloatMatrixPtr getEstimatedInterferenceAndNoiseCovariance(imtaphy::interface::PRB prb, InterferersCollectionPtr interferers)
            {
                if (channel->getTTI() != estimatedCovarianceTimestamp)
                {
                    estimatedCovarianceCache.clear();
                    estimatedCovarianceCache.resize(numPRBs);
                    estimatedCovarianceTimestamp = channel->getTTI();
                    
                }
                
                if (estimatedCovarianceCache[prb].find(interferers) == estimatedCovarianceCache[prb].end())
                {
                    estimatedCovarianceCache[prb][interferers] = imperfectIandNoiseCovariance->computeNoiseAndInterferenceCovariance(interferers, NoiseCovariance, prb);
                }
           
                
                return estimatedCovarianceCache[prb][interferers];
            }
            
            imtaphy::detail::ComplexFloatMatrixPtr getEstimatedChannel(imtaphy::interface::PRB prb, imtaphy::Link* link, wns::Power txPower)
            {
                if (channel->getTTI() != estimatedChannelTimestamp)
                {
                    estimatedChannelCache.clear();
                    estimatedChannelCache.resize(numPRBs);
                    txPowerUsedForEstimation.clear();
                    txPowerUsedForEstimation.resize(numPRBs);
                    estimatedChannelTimestamp = channel->getTTI();
                    
                }
                
                if (estimatedChannelCache[prb].find(link) == estimatedChannelCache[prb].end() ||
                    (txPowerUsedForEstimation[prb][link] != txPower))
                    // redo estimation if not in cache or if it was done with a different power
                {
                    if (channelEstimation) 
                    {
                        estimatedChannelCache[prb][link] = channelEstimation->estimateChannel(prb, link, txPower);
                        txPowerUsedForEstimation[prb][link] = txPower;
                    }
                    else // no channel estimation, return perfect channel
                    {
                        // do not scale matrix in place
                        imtaphy::detail::ComplexFloatMatrixPtr scaledChannel(new imtaphy::detail::ComplexFloatMatrix(*(link->getChannelMatrix(receivingInDirection, prb))));
                        imtaphy::detail::scaleMatrixA<>(*scaledChannel,
                                                        static_cast<float>(sqrt(txPower.get_mW())));
                        estimatedChannelCache[prb][link] = scaledChannel;
                        txPowerUsedForEstimation[prb][link] = txPower;
                    }
                }
                
                
                return estimatedChannelCache[prb][link];
            }
            
            
            imtaphy::receivers::QuantizedChannel getQuantizedChannel(unsigned int prb, imtaphy::Link* link, wns::Power assumedTxPower, std::vector<imtaphy::detail::ComplexFloatMatrixPtr>* quantizationCodebook)
            {     
                imtaphy::detail::ComplexFloatMatrixPtr channel = getEstimatedChannel(prb, link, assumedTxPower);
                assure(channel->getRows() == 1,"Expecting row vector as channel");
                QuantizedChannel qc;
                qc.metric = 0;
                qc.codebookEntry = 0;
                
                float metric;
                float channel_norm;
                std::complex<float> runner;
                
                for (unsigned int index = 0; index < quantizationCodebook->size(); index++)
                {   
                    assure((*quantizationCodebook)[index]->getColumns() == channel->getColumns(),"Columns do not match");
                    assure((*quantizationCodebook)[index]->getRows() == 1,"Codebook entry should be a row vector");
                    channel_norm = imtaphy::detail::matrixNormSquared(*channel);
                    
                    runner = (0,0);
                    for (unsigned int i = 0; i < channel->getColumns(); i++)
                    {    
                        runner += std::conj((*(*quantizationCodebook)[index])[0][i]) * (*channel)[0][i];
                    }
                    metric = norm(runner)/channel_norm;
                    // std::cout << "the metric " << metric << " the norm " << channel_norm << "\n" << std::endl;
                   // metric = norm(imtaphy::detail::dotProductOfAColumnAndConjugateBColumn(*(entry->get()),col,*channel,col));
                    if (metric > qc.metric)
                    {
                        qc.metric = metric;
                        qc.codebookEntry = index;
                        qc.channel_norm = channel_norm;                            
                    }
                 }   
                 return qc;
            }
            
            wns::Power getThermalNoiseIncludingNoiseFigure() const {return thermalNoiseInclNF;}
            
            imtaphy::Direction getDirection() const {return receivingInDirection;}
            imtaphy::Link* getLinkToBS(imtaphy::StationPhy* nodeToEstimate){return allMyLinks[nodeToEstimate];}           
            
       
            friend class imtaphy::receivers::tests::LinearReceiverTest;
            friend class imtaphy::receivers::tests::MMSEReceiverTest;
            friend class imtaphy::receivers::tests::MRCReceiverTest;
            friend class imtaphy::receivers::tests::QuantizedChannelTest;
            
        protected:
            SINRComputationResultPtr computeSINRs(imtaphy::detail::ComplexFloatMatrix& precodedH,  // is a numRxAntennas-by-numServingLayers matrix
                                                  imtaphy::detail::ComplexFloatMatrix& W, // is a numRxAntennas-by-numServingLayers
                                                  imtaphy::detail::ComplexFloatMatrix& Whermitian, // is a numServingLayers-by-numRxAntennas
                                                  imtaphy::detail::ComplexFloatMatrix& iAndNoiseCovariance,
                                                  unsigned int numRxAntennas, 
                                                  unsigned int numServingTxAntennas, 
                                                  unsigned int numServingLayers) const;
                                
            void probeOnShutdown();
            
            int roundToInt(double x)
            {
                return static_cast<int>( x < 0 ? x - 0.5 : x + 0.5 );
            }

            
            StationPhy* station;
            Channel* channel;
            
            wns::Ratio noiseFigure;
            wns::logger::Logger logger;
            
            imtaphy::scm::SpatialChannelModelInterface<float>* scm;
            
            unsigned int numRxAntennas;
            
            imtaphy::detail::ComplexFloatMatrixPtr receiveFilter;
            
            wns::Power thermalNoiseInclNF;
            unsigned int maxRank;
            
            
            imtaphy::receivers::channelEstimation::covariance::PerfectInterferenceAndNoiseCovariance* perfectIandNoiseCovariance;
            imtaphy::receivers::channelEstimation::covariance::NoiseAndInterferenceCovarianceInterface* imperfectIandNoiseCovariance; 
            
            imtaphy::receivers::channelEstimation::channel::ChannelEstimationInterface* channelEstimation;

            imtaphy::detail::ComplexFloatMatrixPtr NoiseCovariance;
            imtaphy::LinkMap allMyLinks;

            
            std::map<TransmissionPtr, Reception> currentReceptions;
            imtaphy::Direction receivingInDirection; // DL or UL
            
            imtaphy::receivers::filters::ReceiveFilterInterface* filter;
            
            typedef std::vector<wns::evaluation::statistics::Moments> MomentsVector;
            typedef std::map<wns::node::Interface*, MomentsVector> NodeMomentsVectorMap;
            typedef std::map<wns::node::Interface*, wns::evaluation::statistics::Moments> NodeMomentsMap;
            
            NodeMomentsVectorMap perUserLinSINR;
            NodeMomentsVectorMap perUserLinIoT;
            std::map<wns::node::Interface*, imtaphy::Link*> node2LinkMap;

            std::list<double> instSINRprobeValues;
            std::list<double> instIoTprobeValues;
            
            wns::probe::bus::ContextCollectorPtr wblContextCollector;
            
            wns::probe::bus::ContextCollectorPtr avgSINRContextCollector;
            wns::probe::bus::ContextCollectorPtr stdDevSINRContextCollector;
            
            wns::probe::bus::ContextCollectorPtr avgIoTContextCollector;
            wns::probe::bus::ContextCollectorPtr stdDevIoTContextCollector;

            unsigned int numPRBs;

        private:
            typedef std::map<InterferersCollectionPtr, imtaphy::detail::ComplexFloatMatrixPtr, InterferersCollectionCompare> CovarianceCache;
            typedef std::map<imtaphy::Link*, imtaphy::detail::ComplexFloatMatrixPtr> ChannelCache;
            typedef std::map<imtaphy::Link*, wns::Power> TxPowerCache;
            
            
            std::vector<CovarianceCache> perfectCovarianceCache;
            std::vector<CovarianceCache> estimatedCovarianceCache;
            std::vector<ChannelCache> estimatedChannelCache;
            std::vector<TxPowerCache> txPowerUsedForEstimation;
            
            unsigned int perfectCovarianceTimestamp;
            unsigned int estimatedCovarianceTimestamp;
            unsigned int estimatedChannelTimestamp;

            wns::pyconfig::View config;
            };
            

}}

#endif //
