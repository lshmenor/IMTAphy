/*******************************************************************************
 * This file is part of IMTAphy
 * _____________________________________________________________________________
 *
 * Copyright (C) 2010
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

#ifndef LTESCANNER_HPP
#define LTESCANNER_HPP

#include <WNS/node/component/Component.hpp>
#include <IMTAPHY/interface/DataTransmission.hpp>
#include <IMTAPHY/interface/DataReception.hpp>
#include <IMTAPHY/interface/IMTAphyObserver.hpp>
#include <IMTAPHY/interface/TransmissionStatus.hpp>

#include <WNS/Observer.hpp>
#include <WNS/probe/bus/ContextCollector.hpp>

#include <IMTAPHY/Channel.hpp>
#include <IMTAPHY/StationPhy.hpp>
#include <WNS/evaluation/statistics/moments.hpp>
#include <WNS/evaluation/statistics/pdf.hpp>

namespace imtaphy { namespace scanner {
    
        class LteScanner :
            public wns::node::component::Component,
            public imtaphy::interface::PhyNotify,
            public wns::Observer<imtaphy::interface::IMTAphyObserver>  
        {
        public:
            LteScanner(wns::node::Interface* _node, const wns::pyconfig::View& _pyco);
                
            void onWorldCreated();
            void onNodeCreated() {};
            void onShutdown() {};
            void doStartup() {};
                
            // LTEPhyObserver interface
            void onNewTTI(unsigned int ttiNumber){};
            void beforeTTIover(unsigned int ttiNumber);

            // LTE DataReception Interface
            void onData(std::vector<wns::ldk::CompoundPtr> transportBlocks, 
                                wns::node::Interface* source, 
                                imtaphy::interface::TransmissionStatusPtr status);
            void channelInitialized() {};

            void setSimulationDuration(unsigned int numTTIs);
        
        protected:
            imtaphy::Channel* channel;
            unsigned int numTTIs;
        
        private:
            void initProbes();  
            unsigned int getSourceId();
                
            std::string txServiceName;
            std::string rxServiceName;
            wns::Power txPower;
            unsigned int tti;
        
                
            imtaphy::interface::DataTransmission* txService;
                
                    
            wns::pyconfig::View config;
            unsigned int currentSourceId;
            StationPhy* station;
                
            wns::probe::bus::ContextCollectorPtr wblContextCollector;
            wns::probe::bus::ContextCollectorPtr sinrContextCollector;
            wns::probe::bus::ContextCollectorPtr pathlossContextCollector;
            wns::probe::bus::ContextCollectorPtr shadowingContextCollector;
            wns::probe::bus::ContextCollectorPtr avgSINRContextCollector;
            wns::probe::bus::ContextCollectorPtr medianSINRContextCollector;

            
            wns::evaluation::statistics::Moments moments;
            wns::evaluation::statistics::PDF sinrPDF;
        };
    
    
    }}

#endif // LTESCANNER_HPP
