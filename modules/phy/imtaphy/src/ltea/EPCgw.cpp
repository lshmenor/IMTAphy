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

// based on code from openWNS with the following license:

/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 5, D-52074 Aachen, Germany
 * phone: ++49-241-80-27910,
 * fax: ++49-241-80-22242
 * email: info@openwns.org
 * www: http://www.openwns.org
 * _____________________________________________________________________________
 *
 * openWNS is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License version 2 as published by the
 * Free Software Foundation;
 *
 * openWNS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#include <IMTAPHY/ltea/EPCgw.hpp>
#include <IMTAPHY/ltea/pdcp/eNB.hpp>

#include <DLL/Layer2.hpp>
#include <DLL/StationManager.hpp>

#include <WNS/module/Base.hpp>

using namespace ltea;

STATIC_FACTORY_REGISTER_WITH_CREATOR(EPCgw,
                                     wns::node::component::Interface,
                                     "ltea.EPCgw",
                                     wns::node::component::ConfigCreator);

EPCgw::EPCgw(wns::node::Interface* node, const wns::pyconfig::View& _config) :
    dll::RANG(node, _config),
    config(_config),
    logger(config.get("logger"))
{
    MESSAGE_SINGLE(NORMAL, logger, "LTEA::EPCgw created");
    
    // to allow filtering for UL/DL in probes by identfying the probing node: 0==EPC, 1==UE, 2==BS
    node->getContextProviderCollection().addProvider(wns::probe::bus::contextprovider::Constant("NodeType", 0));
}

void
EPCgw::sendData(
           const wns::service::dll::UnicastAddress& _peer,
           const wns::osi::PDUPtr& pdu,
           wns::service::dll::protocolNumber protocol,
           wns::service::dll::FlowID _epsBearer)
{
    MESSAGE_SINGLE(NORMAL, logger, "Outgoing PDU with EPS Radio Bearer: "<<_epsBearer<<" for UE: "<<_peer);
    
    if (accessPointLookup.knows(_peer))
    {
        dll::RANG::sendData(_peer, pdu, protocol, _epsBearer);


    }
    else
    {
        MESSAGE_SINGLE(NORMAL, logger, "No eNB registered that knows UE " << _peer);
        MESSAGE_SINGLE(NORMAL, logger, "Dropping PDU.");
    }
}

void 
EPCgw::setAssociatedUEs(std::vector< wns::node::Interface* > associatedUEs, ltea::pdcp::PDCPeNB* pdcp)
{
    for (std::vector<wns::node::Interface*>::const_iterator iter = associatedUEs.begin();
         iter != associatedUEs.end(); iter++)
    {        
        dll::UpperConvergence* dataTransmissionService = 
            (*iter)->getService<dll::UpperConvergence*>(config.get<std::string>("ueDllDataTransmissionServiceName"));
        
        assure(dataTransmissionService, "Could not get remote DLL data transmission service from UE");    
            
        wns::service::dll::UnicastAddress macAddress = dataTransmissionService->getMACAddress();
        
        
        MESSAGE_SINGLE(NORMAL, logger, "Registering UE "<< (*iter)->getName() << " at " << pdcp->getFUN()->getLayer()->getNodeName());
        accessPointLookup.insert(macAddress, pdcp);

    }
}


