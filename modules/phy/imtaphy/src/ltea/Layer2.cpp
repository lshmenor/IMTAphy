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

#include <IMTAPHY/ltea/Layer2.hpp>
#include <IMTAPHY/ltea/pdcp/eNB.hpp>
#include <IMTAPHY/ltea/l2s/PhyInterfaceRx.hpp>

#include <DLL/UpperConvergence.hpp>
#include <DLL/StationManager.hpp>

#include <WNS/ldk/utils.hpp>
#include <WNS/service/dll/StationTypes.hpp>

#include <IMTAPHY/StationPhy.hpp>

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    ltea::Layer2,
    wns::node::component::Interface,
    "ltea.Layer2",
    wns::node::component::ConfigCreator);

using namespace ltea;


Layer2::Layer2(wns::node::Interface* node, const wns::pyconfig::View& _config) :
    dll::Layer2(node, _config, NULL), // we pass a null pointer instead of
                                       // a station manager
    txService(NULL),
    phyInterface(NULL)
{
    std::string phyNotifyServiceName = config.get<std::string>("phyNotifyServiceName");
    phyDataTransmissionName = config.get<std::string>("phyDataTransmissionName");
    
    // Register the Layer2 in the 
    addService(phyNotifyServiceName, this);
    
    if (type == wns::service::dll::StationTypes::eNB())
    {
        // to allow filtering for UL/DL in probes by identfying the probing node: 0==EPC, 1==UE, 2 == BS
        node->getContextProviderCollection().addProvider(wns::probe::bus::contextprovider::Constant("NodeType", 2));
    }
    else  // it is a UE
    {
        // to allow filtering for UL/DL in probes by identfying the probing node: 0==EPC, 1==UE, 2 == BS
        node->getContextProviderCollection().addProvider(wns::probe::bus::contextprovider::Constant("NodeType", 1));
    }
}



void
Layer2::doStartup()
{
    // this is called before onNodeCreated()
    
    // let the base class perform certain initializations:
    // register pdcp as the component that offers the DLL dataTransmission service
    // register this station with the StationManager
 
    dll::Layer2::doStartup();
    
    

}


Layer2::~Layer2()
{}

void
Layer2::onShutdown()
{
    phyInterface->onShutdown();
}

void
Layer2::onWorldCreated()
{
    
}

void
Layer2::onNodeCreated()
{
    txService = getService<imtaphy::interface::DataTransmission*>(phyDataTransmissionName);

    // Trigger all FUs in the FUN to perform their dependency resolution with
    // other FUs and related tasks
    fun->onFUNCreated();

    phyInterface = fun->findFriend<ltea::l2s::PhyInterfaceRx*>("PhyInterfaceRx");
}

void 
Layer2::channelInitialized()
{
    imtaphy::StationPhy* stationPhy = dynamic_cast<imtaphy::StationPhy*>(txService);
    assure(stationPhy, "Could not get a StationPhy pointer");
    
    associations = stationPhy->getAssociatedNodes();
   
   
    for (std::vector<wns::node::Interface*>::const_iterator iter = associations.begin();
         iter != associations.end(); iter++)
    {
        MESSAGE_SINGLE(NORMAL, logger, "Associated to this station: " << (*iter)->getName());
    }
    
    
    if (type == wns::service::dll::StationTypes::eNB())
    {
        ltea::pdcp::PDCPeNB* pdcp = fun->findFriend<ltea::pdcp::PDCPeNB*>("pdcp");
        assure(pdcp, "Could not find PDCP in eNB");
        
        pdcp->setAssociatedUEs(associations);
        
    }
    else 
    {
        assure(associations.size() == 1, "A UE should be associated to exactly one eNB");
    }
    phyInterface->channelInitialized();
}

void 
Layer2::onData( std::vector<wns::ldk::CompoundPtr> transportBlocks, 
                wns::node::Interface* source, 
                imtaphy::interface::TransmissionStatusPtr status)
{
    phyInterface->onData(transportBlocks, source, status);
}



imtaphy::interface::DataTransmission* 
Layer2::getTxService()
{
    assure(txService, "getTxService called too early or pointer invalid");
    
    return txService;
}


wns::node::Interface* 
Layer2::getNodeByMACAddress(wns::service::dll::UnicastAddress macAddress)
{
    if (type == wns::service::dll::StationTypes::eNB())
    {
        // In DL we ask the station manager to resolve this
        
        return getStationManager()->getStationByMAC(macAddress)->getNode();    

    }
    else 
    {
        // In the UL, the UE sends everythin to its associated BS so return this regardless of the address
        
        assure(associations.size() == 1, "A UE should be associated to exactly one eNB");

        return associations[0];
    }
}

