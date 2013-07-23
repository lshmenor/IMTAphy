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

#ifndef IMTAPHY_RECEIVERS_PROBINGRECEIVER_HPP
#define IMTAPHY_RECEIVERS_PROBINGRECEIVER_HPP

#include <IMTAPHY/receivers/LinearReceiver.hpp>

namespace imtaphy {

    namespace receivers {

        
        class ProbingReceiver :
            public LinearReceiver
        {
        public:
            ProbingReceiver(StationPhy* station, const wns::pyconfig::View& pyConfigView);
                
            void receive(TransmissionPtr transmission);
            void deliverReception(TransmissionPtr transmission);

            void channelInitialized(Channel* _channel);
            
            ReceiverFeedback
            computeFeedback(unsigned int prb, 
                                unsigned int rank,
                                imtaphy::StationPhy* nodeToEstimate,
                                wns::Power assumedTxPower,
                                std::vector<imtaphy::detail::ComplexFloatMatrixPtr>& precodingsToTest);            
            
            void onShutdown();
           
        protected:
        private:
            wns::Ratio getAverageSINR(wns::node::Interface* source);
            unsigned int getBaseSINRIndex(wns::node::Interface* source);
          
            double computeMIMOMutualInformation(imtaphy::detail::ComplexFloatMatrix& precodedServingChannel, imtaphy::detail::ComplexFloatMatrix& ipnCovariance);
            
            // PMI probing
            std::map<unsigned int, double> pmiStrength;
            std::map<unsigned int,  wns::evaluation::statistics::Moments> relativeStrengths;
            std::map<unsigned int, unsigned int> pmiCounter;
            wns::probe::bus::ContextCollectorPtr pmiRelativeStrengthContextCollector;
            wns::probe::bus::ContextCollectorPtr pmiStrongestContextCollector;
            wns::probe::bus::ContextCollectorPtr reuse3GainContextCollector;
            wns::probe::bus::ContextCollectorPtr reuse3CapacityGainContextCollector;

            wns::probe::bus::ContextCollectorPtr mutualInformationContextCollector;
            wns::probe::bus::ContextCollectorPtr mutualInformationReuse3ContextCollector;
            wns::probe::bus::ContextCollectorPtr reuse3MIgainContextCollector;

            
            
            NodeMomentsMap reuse3SINR;
            std::map<wns::node::Interface*, double> reuse1Capacity;
            std::map<wns::node::Interface*, double> reuse3Capacity;
            
            std::map<wns::node::Interface*, double> perUserSumMI;
            std::map<wns::node::Interface*, double> perUserReuse3SumMI;
        };
            
            

}}

#endif //
