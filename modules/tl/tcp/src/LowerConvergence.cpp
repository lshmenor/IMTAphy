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

#include <TCP/Service.hpp>
#include <TCP/LowerConvergence.hpp>
#include <TCP/UpperConvergence.hpp>

using namespace tcp;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
				     LowerConvergence,
				     wns::ldk::FunctionalUnit,
				     "tcp.lowerConvergence",
				     wns::ldk::FUNConfigCreator
				     );

LowerConvergence::LowerConvergence(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& _config) :
  wns::ldk::CommandTypeSpecifier<>(fun),
  dataTransmissionService(NULL),
  config(_config),
  logger(config.get("logger")),
  tcpHeaderReader(NULL)
{
}

LowerConvergence::~LowerConvergence()
{
}

void
LowerConvergence::doSendData(const wns::ldk::CompoundPtr& compound)
{
  assure(compound, "doSendData called with an invalid compound.");

  MESSAGE_BEGIN(VERBOSE, logger, m, getFUN()->getName());
  m << ": TCP::LowerConvergence sendData: Compound backtrace"
    << compound->dumpJourney(); // JOURNEY
  MESSAGE_END();

  assure(tcpHeaderReader, "No reader for the TCP Header available!");
  TCPCommand* tcpHeader = tcpHeaderReader->readCommand<TCPCommand>(compound->getCommandPool());

  assure(flowIDMapper.knows(tcpHeader->peer.flowID), "You must first establish a flow for :"<<tcpHeader->peer.flowID);

  wns::service::dll::FlowID dllFlowID = flowIDMapper.find(tcpHeader->peer.flowID);

  MESSAGE_SINGLE(NORMAL, logger, "Sending Data (DLL-FlowID="<<dllFlowID<<") TL-FlowID: " << tcpHeader->peer.flowID);

  getDataTransmissionService()->sendData(tcpHeader->peer.flowID.srcAddress,
					 tcpHeader->peer.flowID.dstAddress,
					 compound, this->protocolNumber,
					 dllFlowID);
}

void
LowerConvergence::doOnData(const wns::ldk::CompoundPtr& compound)
{
  assure(compound, "doOnData called with an invalid compound.");

  assure(tcpHeaderReader, "No reader for the TCP Header available!");
  TCPCommand* tcpHeader = tcpHeaderReader->readCommand<TCPCommand>(compound->getCommandPool());

  // swap the src/dst information of the flow ID
  wns::service::tl::FlowID tmpFlowID(tcpHeader->peer.flowID.dstAddress,
				     tcpHeader->peer.flowID.dstPort,
				     tcpHeader->peer.flowID.srcAddress,
				     tcpHeader->peer.flowID.srcPort);
  tcpHeader->peer.flowID = tmpFlowID;

  // save the incoming DLL-FlowID, in case of an incoming SYN there is not yet a DLL FlowID existent
  if(!flowIDMapper.knows(tcpHeader->peer.flowID))
    mapFlowID(tcpHeader->peer.flowID, wns::service::dll::NoFlowID);

  MESSAGE_SINGLE(NORMAL, logger, "Incoming data for flow id: " << tcpHeader->peer.flowID);

  if (isAccepting(compound))
    getDeliverer()->getAcceptor(compound)->onData(compound);
}

void
LowerConvergence::onFUNCreated()
{
  tcpHeaderReader = getFUN()->getCommandReader("tcp.tcpHeader");
  assure(tcpHeaderReader, "No reader for the TCP Header available!");
}

void
LowerConvergence::setDataTransmissionService(wns::service::nl::Service* service)
{
  assure(!dataTransmissionService, "dataTransmissionService already set");
  dataTransmissionService = service;
}

wns::service::nl::Service*
LowerConvergence::getDataTransmissionService() const
{
  assure(dataTransmissionService, "dataTransmissionService not set");
  return dataTransmissionService;
}

bool
LowerConvergence::doIsAccepting(const wns::ldk::CompoundPtr& /*compound*/) const
{
  // always accepting since IP will not block
  return true;
}

void
LowerConvergence::doWakeup()
{
  getReceptor()->wakeup();
}

void
LowerConvergence::setProtocolNumber(wns::service::nl::protocolNumber _protocolNumber)
{
  this->protocolNumber = _protocolNumber;
}

void
LowerConvergence::setTLService(Service* _tlService)
{
  this->tlService = _tlService;
}

void
LowerConvergence::mapFlowID(wns::service::tl::FlowID flowID, wns::service::dll::FlowID dllFlowID)
{
  flowIDMapper.insert(flowID, dllFlowID);
  MESSAGE_SINGLE(NORMAL, logger, "DLLFlowID "<< dllFlowID <<" added for TLFlowID " << flowID);
}

void
LowerConvergence::unmapFlowID(wns::service::tl::FlowID flowID)
{
  flowIDMapper.erase(flowID);
  MESSAGE_SINGLE(NORMAL, logger, "DLLFlowID deleted for TLFlowID " << flowID);
}
