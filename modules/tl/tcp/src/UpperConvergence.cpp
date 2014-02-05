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

#include <TCP/UpperConvergence.hpp>
#include <TCP/Connection.hpp>
#include <TCP/Service.hpp>


using namespace tcp;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	UpperConvergence,
	wns::ldk::FunctionalUnit,
	"tcp.upperConvergence",
	wns::ldk::FUNConfigCreator);

UpperConvergence::UpperConvergence(wns::ldk::fun::FUN* _fun, const wns::pyconfig::View& _pyco) :
	wns::ldk::CommandTypeSpecifier<>(_fun),
	wns::ldk::HasReceptor<>(),
	wns::ldk::HasConnector<>(),
	wns::ldk::HasDeliverer<>(),
	wns::ldk::Forwarding<UpperConvergence>(),
	wns::Cloneable<UpperConvergence>(),
	pyco(_pyco),
	logger(_pyco.get("logger")),
    tlService(NULL),
    tcpHeaderFU(NULL),
    tcpHeaderReader(NULL)
{
	MESSAGE_SINGLE(NORMAL,  logger, "UpperConvergence started." );
}

UpperConvergence::~UpperConvergence()
{
}

void
UpperConvergence::onFUNCreated()
{
    tcpHeaderFU = getFUN()->findFriend<TCPHeader*>("tcp.tcpHeader");
    assure(tcpHeaderFU, "No FU for the TCP Header available!");
    tcpHeaderReader = getFUN()->getCommandReader("tcp.tcpHeader");
    assure(tcpHeaderReader, "No reader for the TCP Header available!");
} // onFUNCreated

void
UpperConvergence::sendData(const wns::service::tl::FlowID& _flowID, const wns::osi::PDUPtr& sdu)
{
	MESSAGE_SINGLE(NORMAL, logger, "sendData.");

	wns::ldk::CompoundPtr compound(new wns::ldk::Compound(getFUN()->getProxy()->createCommandPool(), sdu));

    assure(tcpHeaderFU, "No FU for the TCP Header available!");
	tcpHeaderFU->activateCommand(compound->getCommandPool());

    assure(tcpHeaderReader, "No reader for the TCP Header available!");
    TCPCommand* tcpHeader = tcpHeaderReader->readCommand<TCPCommand>(compound->getCommandPool());

    tcpHeader->peer.flowID = _flowID;

	//forwarding to flow separator
	if (isAccepting(compound))
	{
		wns::ldk::Forwarding<UpperConvergence>::doSendData(compound);
	}
	else
	{
		// do nothing (throw away compound)
		assure(false, "TCP does not accept any data from upper layer.");
	}
}

void
UpperConvergence::processIncoming(const wns::ldk::CompoundPtr& _compound)
{
    assure(tcpHeaderReader, "No reader for the TCP Header available!");
    TCPCommand* tcpHeader = tcpHeaderReader->readCommand<TCPCommand>(_compound->getCommandPool());

    wns::service::tl::FlowID flowID = tcpHeader->peer.flowID;

	MESSAGE_SINGLE(NORMAL, logger, "processIncoming called for flow id: " << flowID);

	/**
	 * Incoming compounds are always valid at this point. So just ask for
	 * the corresponding connection.
	 */
	tlService->getConnection(flowID)->onData(_compound->getData());
}

void
UpperConvergence::setTLService(Service* _tlService)
{
	this->tlService = _tlService;
}
