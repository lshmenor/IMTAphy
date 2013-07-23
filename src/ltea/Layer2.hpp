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


#ifndef LTEA_LAYER2_HPP
#define LTEA_LAYER2_HPP

#include <IMTAPHY/interface/DataTransmission.hpp>
#include <IMTAPHY/interface/DataReception.hpp>

#include <DLL/Layer2.hpp>

namespace ltea
{
    namespace l2s {
        class PhyInterfaceRx;
    }
    
    class Layer2 :
        //public wns::node::component::Component
        public dll::Layer2,
        public imtaphy::interface::PhyNotify
    {
    public:
        Layer2(wns::node::Interface*, const wns::pyconfig::View&);
        virtual ~Layer2();
    
        void onNodeCreated();
        void onWorldCreated();
        void onShutdown();

        wns::node::Interface* getNodeByMACAddress(wns::service::dll::UnicastAddress macAddress);
        
        // PhyNotify Interface:
        void onData(std::vector<wns::ldk::CompoundPtr> transportBlocks, 
                    wns::node::Interface* source, 
                    imtaphy::interface::TransmissionStatusPtr status);
    
        void channelInitialized(); 
        
         imtaphy::interface::DataTransmission* getTxService();
        
    private:
        void doStartup();
        
        std::vector< wns::node::Interface*> associations;
        std::string phyDataTransmissionName;
        imtaphy::interface::DataTransmission* txService;
        
        ltea::l2s::PhyInterfaceRx* phyInterface;
        
    };
}


#endif 

