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

#include <IMTAPHY/ltea/pdcp/UE.hpp>
#include <IMTAPHY/ltea/helper/Types.hpp>
#include <IMTAPHY/ltea/pdcp/PDCPCommand.hpp>

#include <DLL/StationManager.hpp>

using namespace ltea::pdcp;

STATIC_FACTORY_REGISTER_WITH_CREATOR(PDCPUE,
                                     wns::ldk::FunctionalUnit,
                                     "ltea.pdcp.UE",
                                     wns::ldk::FUNConfigCreator);


PDCPUE::PDCPUE(wns::ldk::fun::FUN* _fun,
                                       const wns::pyconfig::View& config) :
  dll::UTUpperConvergence(_fun, config),
  fun(_fun),
  pdcpReader(NULL),
  layer2(NULL)
{
    assure(fun, "FUN not set");
}

void
PDCPUE::onFUNCreated()
{
    layer2 = fun->getLayer<dll::Layer2*>();
    assure(layer2, "Layer2 not set");

    pdcpReader   = fun->getCommandReader("pdcp");
    assure(pdcpReader, "pdcpReader not set");
}

void
PDCPUE::sendData(const wns::service::dll::UnicastAddress& _peer,
                 const wns::osi::PDUPtr& pdu,
                 wns::service::dll::protocolNumber protocol,
                 wns::service::dll::FlowID _epsBearer)
{
    wns::ldk::CompoundPtr compound(new wns::ldk::Compound(getFUN()->createCommandPool(), pdu));
    dll::UpperConvergence::sendData(_peer, pdu, protocol, _epsBearer);
}


void
PDCPUE::processIncoming(const wns::ldk::CompoundPtr& compound)
{
    // we can only set the UpperCommand in the FU because we derive from this UpperConvergence
    // check if we can completely separate the pdcp from upper convergence
    

  //the FlowID is read out of the RLC command to be forwarded to the datahandler
  dll::UpperCommand* pdcpCommand = pdcpReader->readCommand<dll::UpperCommand>(compound->getCommandPool());
  assure(pdcpCommand, "RlcCommand not set");

  int protocol = wns::service::dll::protocolNumberOf(compound->getData());
  
  MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
  m << "UE PDCP::processIncoming(), forwarding packet from "<< pdcpCommand->peer.sourceMACAddress << " to upper Component (IP), protocol "<<protocol;
  MESSAGE_END();

  
  //  ????
  
  if(protocol == 1)
    {
      dataHandlerRegistry.find(wns::service::dll::protocolNumberOf(compound->getData()))->onData(compound->getData(), 0);
    }
  else
    {
      dataHandlerRegistry.find(wns::service::dll::protocolNumberOf(compound->getData()))->onData(compound->getData());
    }
  
  MESSAGE_BEGIN(VERBOSE, logger, m, getFUN()->getName());
  m << ": Compound backtrace"
    << compound->dumpJourney(); // JOURNEY
  MESSAGE_END();
}

void
PDCPUE::registerHandler(wns::service::dll::protocolNumber protocol,
                                    wns::service::dll::Handler* dh)
{
  assureNotNull(dh);
  dataHandlerRegistry.insert(protocol, dh);
  
  MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
  m << ": UEUpperConv registered dataHandler for protocol number " << protocol;
  MESSAGE_END();
}

void
PDCPUE::registerFlowHandler(wns::service::dll::FlowHandler* flowHandler)
{
  tlFlowHandler = flowHandler;
}

void
PDCPUE::establishFlow(wns::service::tl::FlowID flowID, wns::service::qos::QoSClass qosClass)
{
  MESSAGE_SINGLE(NORMAL, logger, "FlowEstablishment called from TL for: " <<flowID);
  assure(tlFlowHandler, "FlowHandler not set!");
  
  // this is a quick hack to enable Uplink traffic generators. We don't consider different EPS bearer IDs
  
  wns::service::dll::FlowID epsBearer = 0;
  tlFlowHandler->onFlowEstablished(flowID, epsBearer);
} // establishFlow

