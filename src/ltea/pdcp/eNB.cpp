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
 * Kopernikusstr. 16, D-52074 Aachen, Germany
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


// We don't model multiple entities, nor specific bearers or control plane data

/* PDCP is specified in TS 36.323 (Version 10):

Each RB (i.e. DRB and SRB, except for SRB0) is associated with one PDCP entity. Each PDCP entity is associated 
with one or two (one for each direction) RLC entities depending on the RB characteristic (i.e. uni-directional or bidirectional) and RLC mode. The PDCP entities are located in the PDCP sublayer. 

The PDCP entities are located in the PDCP sublayer. Several PDCP entities may be defined for a UE. Each PDCP 
entity carrying user plane data may be configured to use header compression. 
Each PDCP entity is carrying the data of one radio bearer. In this version of the specification, only the robust header 
compression protocol (ROHC), is supported. Every PDCP entity uses at most one ROHC instance. 
A PDCP entity is associated either to the control plane or the user plane depending on which radio bearer it is carrying 
data for.

PDCP provides its services to the RRC and user plane upper layers at the UE or to the relay at the evolved Node B 
(eNB). The following services are provided by PDCP to upper layers: 
- transfer of user plane data; 
- transfer of control plane data; 
- header compression; 
- ciphering; 
- integrity protection. 
The maximum supported size of a PDCP SDU is 8188 octets

PDCP is used for SRBs and DRBs mapped on DCCH and DTCH type of logical channels. PDCP is not used for any 
other type of logical channels

The PDCP Data PDU is used to convey: 
- a PDCP SDU SN; and 
- user plane data containing an uncompressed PDCP SDU; or 
- user plane data containing a compressed PDCP SDU; or 
- control plane data; and 
- a MAC-I field for SRBs only;


RB Radio Bearer
DRB Data Radio Bearer carrying user plane data
SRB Signalling Radio Bearer carrying control plane data 

MAC-I Message Authentication Code for Integrity
SN Sequence Number

6.2.2 PDCP PDU for control plane SRBs:
1 Byte (type bit + SN) and 4 Bytes MAC-I (at the end) plus payload

6.2.3 PDCP PDU for DRBs in RLC UM or RLC AM mode with 12 bit SN
2 Bytes Header (type bit + SN) + Data

6.2.4 PDCP PDU for DRBs in RLC UM mode with short sequence number:
1 Byte Header + Data


*/



#include <IMTAPHY/ltea/pdcp/eNB.hpp>
#include <IMTAPHY/ltea/pdcp/PDCPCommand.hpp>

#include <DLL/Layer2.hpp>


#include <DLL/StationManager.hpp>
#define A2N(a) layer2->getStationManager()->getStationByMAC(a)->getName()

using namespace ltea::pdcp;

STATIC_FACTORY_REGISTER_WITH_CREATOR(PDCPeNB,
                                     wns::ldk::FunctionalUnit,
                                     "ltea.pdcp.eNB",
                                     wns::ldk::FUNConfigCreator);

PDCPeNB::PDCPeNB(wns::ldk::fun::FUN* _fun, const wns::pyconfig::View& config) :
    dll::APUpperConvergence(_fun, config),
    layer2(NULL),
    pdcpReader(NULL),
    epcGW(NULL)
//    flowManagementRang(NULL)
{
}

void
PDCPeNB::onFUNCreated()
{
    layer2 = getFUN()->getLayer<dll::ILayer2*>();
    pdcpReader   = getFUN()->getCommandReader("pdcp");
    assure(pdcpReader, "pdcpReader not set");
    dll::UpperConvergence::onFUNCreated();
}

void
PDCPeNB::registerHandler(wns::service::dll::protocolNumber proto,
                    wns::service::dll::Handler* dh)
{
    dll::APUpperConvergence::registerHandler(proto, dh);

    assureType(dh, ltea::EPCgw*);

    epcGW = dynamic_cast<ltea::EPCgw*>(dh);

    assure(epcGW, "PDCPeNB failed to register EPC GW");
}

void
PDCPeNB::sendData(
    const wns::service::dll::UnicastAddress& _peer,
    const wns::osi::PDUPtr& pdu,
    wns::service::dll::protocolNumber protocol,
    wns::service::dll::FlowID epsBearer)
{
    
    // received compound from EPCgw
    // epsBearer is valid between EPCgw and ENB
    MESSAGE_SINGLE(NORMAL, logger,"Sending Compound with EPCgw-DLL-FlowID="<< epsBearer<<" to: "<<A2N(_peer));
    dll::UpperConvergence::sendData(_peer, pdu, protocol, epsBearer);
}

void
PDCPeNB::processIncoming(const wns::ldk::CompoundPtr& compound)
{
    // we can only set the UpperCommand in the FU because we derive from this UpperConvergence
    // check if we can completely separate the pdcp from upper convergence
    

    assure(dataHandler, "no data handler set");
    // as opposed to the UT upper convergence, we have to tell the EPCgw who we
    // are and where the Packet comes from.
    dll::UpperCommand* myCommand = getCommand(compound->getCommandPool());
    dataHandler->onData(compound->getData(),
                        myCommand->peer.sourceMACAddress,
                        this,
                        0);

    MESSAGE_BEGIN(VERBOSE, logger, m, getFUN()->getName());
    m << ": Compound backtrace"
      << compound->dumpJourney(); // JOURNEY
    MESSAGE_END();
}


void 
PDCPeNB::setAssociatedUEs(std::vector< wns::node::Interface* > associatedUEs)
{
    if(epcGW != NULL)
        epcGW->setAssociatedUEs(associatedUEs, this);
}


