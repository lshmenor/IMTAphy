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

#ifndef LTESENDER_HPP
#define LTESENDER_HPP

#include <WNS/node/component/Component.hpp>
#include <WNS/node/Interface.hpp>
#include <IMTAPHY/interface/DataTransmission.hpp>
#include <IMTAPHY/interface/DataReception.hpp>
#include <IMTAPHY/interface/IMTAphyObserver.hpp>
#include <IMTAPHY/interface/TransmissionStatus.hpp>
#include <WNS/Observer.hpp>

#include <IMTAPHY/Channel.hpp>
#include <IMTAPHY/StationPhy.hpp>
#include <IMTAPHY/Link.hpp>
#include <IMTAPHY/detail/LinearAlgebra.hpp>


namespace imtaphy { namespace scanner {

        class LteSender :
            public wns::node::component::Component,
            public imtaphy::interface::PhyNotify,
            public wns::Observer<imtaphy::interface::IMTAphyObserver>  
        {
        public:
            LteSender(wns::node::Interface* _node, const wns::pyconfig::View& _pyco);
                
            void onWorldCreated();
            void onNodeCreated() {};
            void onShutdown() {};
            void doStartup() {};
                
            void onNewTTI(unsigned int tti) {};
            void beforeTTIover(unsigned int ttiNumber);

            void onData(std::vector<wns::ldk::CompoundPtr> transportBlocks, 
                                wns::node::Interface* source, 
                                imtaphy::interface::TransmissionStatusPtr status) {};
            void channelInitialized() {};
                
        private:
            std::string txServiceName;
            std::string rxServiceName;
            wns::Power txPowerPerPRB;
            unsigned int tti;
                
            imtaphy::interface::DataTransmission* txService;
            imtaphy::Channel* channel;
                
            std::vector<StationPhy*> mobiles;
            unsigned int bsId;
            unsigned int numberOfRounds;
            unsigned int nextMS;
            
            unsigned int numberOfTxAntennas;
            unsigned int numberOfLayers;
            imtaphy::interface::PrbPowerPrecodingMap prbPowerPrecodingMap;
            
            StationPhy* station;
            StationPhy* dummyMobile;
        
            static unsigned int maxNumberOfMobiles ;
        };
    
    }}

#endif // LTESENDER_HPP
