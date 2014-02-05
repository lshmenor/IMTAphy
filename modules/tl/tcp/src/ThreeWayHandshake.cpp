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

#include <TCP/ThreeWayHandshake.hpp>
#include <TCP/Service.hpp>
#include <TCP/UpperConvergence.hpp>
#include <TCP/ConnectionControl.hpp>
#include <TCP/FiniteStateMachine.hpp>
#include <TCP/HandshakeStrategyHandlerInterface.hpp>

#include <WNS/ldk/Layer.hpp>
#include <WNS/ldk/helper/FakePDU.hpp>
#include <WNS/ldk/ControlServiceInterface.hpp>

#include <WNS/node/Node.hpp>

using namespace tcp;


STATIC_FACTORY_REGISTER_WITH_CREATOR(
	ThreeWayHandshake,
	wns::ldk::FunctionalUnit,
	"tcp.ThreeWayHandshake",
	wns::ldk::FUNConfigCreator
	);

ThreeWayHandshake::ThreeWayHandshake(wns::ldk::fun::FUN* _fun, const wns::pyconfig::View& _pyco) :
	wns::ldk::CommandTypeSpecifier<>(_fun),
	wns::ldk::HasReceptor<>(),
	wns::ldk::HasConnector<>(),
	wns::ldk::HasDeliverer<>(),
	wns::Cloneable<ThreeWayHandshake>(),
	logger(_pyco.get("logger")),
	strategyHandler(NULL),
	pyco(_pyco),
    fsm(NULL),
    tcpHeaderFU(NULL),
    tcpHeaderReader(NULL)
{
	MESSAGE_SINGLE(NORMAL, logger, "FU for 3-way-handshake created!");

	ConnectionVariables v;

	fsm = new FiniteStateMachine(v, logger);
}

ThreeWayHandshake::~ThreeWayHandshake()
{
	delete fsm;
}

void
ThreeWayHandshake::onFUNCreated()
{
    tcpHeaderFU = getFUN()->findFriend<TCPHeader*>("tcp.tcpHeader");
    assure(tcpHeaderFU, "No FU for the TCP Header available!");
    tcpHeaderReader = getFUN()->getCommandReader("tcp.tcpHeader");
    assure(tcpHeaderReader, "No reader for the TCP Header available!");
}

void
ThreeWayHandshake::doOnData(const wns::ldk::CompoundPtr& _compound)
{
	assure(_compound, "doOnData called with an invalid compound.");

    assure(tcpHeaderReader, "No reader for the TCP Header available!");
    TCPCommand* tcpHeader = tcpHeaderReader->readCommand<TCPCommand>(
        _compound->getCommandPool());

	MESSAGE_SINGLE(NORMAL, logger, "ThreeWayHandshake::doOnData: incoming compound received!");

	if ((fsm->getVariables().activeOpen == false) && (strategyHandler == NULL))
	{
		// server in state closed, receives first compound (strategy handler not
        // assigned yet):
        // change to state listen
		assure(fsm->getStateName() == "tcp_fsm_closed", "Invalid transition: Currently not in state <closed>!");
		fsm->replaceState(fsm->createState<Listen>());
		this->flowID = tcpHeader->peer.flowID;

		strategyHandler = dynamic_cast<tcp::HandshakeStrategyHandlerInterface*>(
			getFUN()->getLayer()->getControlService<wns::ldk::ControlService>("fip"));
	}


	wns::ldk::helper::FakePDUPtr pdu;
	wns::ldk::CompoundPtr compound(new wns::ldk::Compound(getFUN()->getProxy()->createCommandPool(), pdu));
    assure(tcpHeaderFU, "No FU for the TCP Header available!");
    tcpHeaderFU->activateCommand(compound->getCommandPool());

	if (fsm->getVariables().activeOpen == false)
	{
		// server
		if (tcpHeader->peer.fin && tcpHeader->peer.ack)
		{
			// received fin_ack, send ack
			assure(fsm->getStateName() == "tcp_fsm_fin_wait_1", "Invalid transition: Currently not in state <fin_wait_1>!");
			fsm->ack();
			tcpFlagsToUpperCommand(compound);
			doSendData(compound);
		}
		else if (tcpHeader->peer.syn)
		{
			// received syn, send syn_ack
			assure(fsm->getStateName() == "tcp_fsm_listen", "Invalid transition: Currently not in state <listen>!");
			fsm->syn_ack();
			tcpFlagsToUpperCommand(compound);
			doSendData(compound);
			MESSAGE_SINGLE(NORMAL, logger, "ThreeWayHandshake: Syn rcvd, sent syn_ack for flowID " << this->flowID);
		}
		else if (tcpHeader->peer.ack)
		{
			// received ack
			// send nothing; either: syn_rcvd -> established
			//                    or closing -> closed (time_wait)
			//                    or last_ack -> closed
			assure(fsm->getStateName() == "tcp_fsm_syn_rcvd" or
			       fsm->getStateName() == "tcp_fsm_closing" or
			       fsm->getStateName() == "tcp_fsm_last_ack", "Invalid transition: Currently not in state <Syn_rcvd> or <Closing> or <Last_ack>!");

			if(fsm->getStateName() == "tcp_fsm_syn_rcvd")
			{
				fsm->replaceState(fsm->createState<Established>());
				MESSAGE_SINGLE(NORMAL, logger, "ThreeWayHandshake: 3WHS established for flowID " << this->flowID);
				strategyHandler->passiveOpenConnection(this->flowID);
			}
			else if (fsm->getStateName() == "tcp_fsm_closing")
			{
				fsm->replaceState(fsm->createState<Closed>());
			}
			else
			{ // current state: last_ack
				fsm->replaceState(fsm->createState<Closed>());
				strategyHandler->passiveClosed(this->flowID);
			}

		}
		else if (tcpHeader->peer.rst)
		{
			// received rst, send nothing
			assure(fsm->getStateName() == "tcp_fsm_syn_rcvd", "Invalid transition: Currently not in state <syn_rcvd>!");
			fsm->replaceState(fsm->createState<Listen>());
		}
		else if (tcpHeader->peer.fin)
		{
			// Either: established -> close_wait
			//      or fin_wait_1 -> closing
			// received fin, send ack
			assure(fsm->getStateName() == "tcp_fsm_established" or
			       fsm->getStateName() == "tcp_fsm_fin_wait_1", "Invalid transition: Currently not in state <established> or <fin_wait_1>!");
			fsm->ack();
			tcpFlagsToUpperCommand(compound);
			doSendData(compound);
			closeConnection(this->flowID);
		}
	}
	else
	{
		// client
		if (tcpHeader->peer.syn && tcpHeader->peer.ack)
		{
			// state transition: syn_sent -> established
			assure(fsm->getStateName() == "tcp_fsm_syn_sent", "Invalid transition: Currently not in state <syn_sent>!" + fsm->getStateName());
			fsm->ack();
			strategyHandler->connectionEstablished(tcpHeader->peer.flowID);
			tcpFlagsToUpperCommand(compound);
			doSendData(compound);
		}
		else if (tcpHeader->peer.fin && tcpHeader->peer.ack)
		{
			// rcvd fin_ack, send ack to close connection
			assure(fsm->getStateName() == "tcp_fsm_fin_wait_1", "Invalid transition: Currently not in state <fin_wait_1>!");
			fsm->ack();
			tcpFlagsToUpperCommand(compound);
			doSendData(compound);
			strategyHandler->connectionClosed(this->flowID);
		}
		else if (tcpHeader->peer.ack)
		{
			// Either: fin_wait_1 -> fin_wait_2
			//     or  closing -> closed (time_wait)
			// send nothing
			assure(fsm->getStateName() == "tcp_fsm_fin_wait_1" or
			       fsm->getStateName() == "tcp_fsm_closing", "Invalid transition: Currently not in state <closing> or <fin_wait_1>!");

			if (fsm->getStateName() == "tcp_fsm_fin_wait_1")
			{
				fsm->replaceState(fsm->createState<Fin_wait_2>());
			}
			else
			{
				// current state: closing
				fsm->replaceState(fsm->createState<Closed>());
				strategyHandler->connectionClosed(this->flowID);
			}
		}
		else if (tcpHeader->peer.fin)
		{
			// Either: fin_wait_1 -> closing
			//     or: fin_wait_2 -> closed (time_wait)
			// At client side both transitions are equal!
			assure(fsm->getStateName() == "tcp_fsm_syn_rcvd" or
			       fsm->getStateName() == "tcp_fsm_fin_wait_2", "Invalid transition: Currently not in state <fin_wait_1> or <fin_wait_2>!");
			fsm->ack();
			tcpFlagsToUpperCommand(compound);
			doSendData(compound);
			// Additionally for fin_wait_2 (but now already in state closed!!!):
			if(fsm->getStateName() == "tcp_fsm_closed")
				strategyHandler->connectionClosed(this->flowID);
		}
		else
		{
			throw(wns::Exception("Invalid signal received for client in state " + fsm->getStateName()));
		}
	}
}

void
ThreeWayHandshake::doSendData(const wns::ldk::CompoundPtr& _compound)
{

	if(isAccepting(_compound))
	{
		getConnector()->getAcceptor(_compound)->sendData(_compound);
	}
}

bool
ThreeWayHandshake::doIsAccepting(const wns::ldk::CompoundPtr& /*_compound*/)
{
	return true;
}

void
ThreeWayHandshake::doWakeup()
{
}

bool
ThreeWayHandshake::doIsAccepting(const wns::ldk::CompoundPtr& /*_compound*/) const
{
	return true;
}

void
ThreeWayHandshake::activeOpen(const wns::service::tl::FlowID& _flowID)
{
	assure(fsm->getStateName() == "tcp_fsm_closed", "Not in state closed!");

	this->flowID = _flowID;

	MESSAGE_SINGLE(NORMAL, logger, "ThreeWayHandshake::activeOpen() called for flowID " << _flowID);

	wns::ldk::helper::FakePDUPtr pdu;
	wns::ldk::CompoundPtr compound(new wns::ldk::Compound(getFUN()->getProxy()->createCommandPool(), pdu));
    assure(tcpHeaderFU, "No FU for the TCP Header available!");
    tcpHeaderFU->activateCommand(compound->getCommandPool());

	fsm->syn();
	tcpFlagsToUpperCommand(compound);
	fsm->getVariables().activeOpen = true;

	doSendData(compound);
}

void
ThreeWayHandshake::passiveOpen(const wns::service::tl::FlowID& _flowID)
{
	MESSAGE_SINGLE(NORMAL, logger, "ThreeWayHandshake::passiveOpen() called for flowID " << _flowID);

	strategyHandler->connectionEstablished(_flowID);
}

void
ThreeWayHandshake::registerStrategyHandler(HandshakeStrategyHandlerInterface* _strategyHandler)
{
	strategyHandler = _strategyHandler;
}

void
ThreeWayHandshake::closeConnection(const wns::service::tl::FlowID& _flowID)
{
	flowID = _flowID;

	assure(fsm->getStateName() == "tcp_fsm_established" // server
	       or fsm->getStateName() == "tcp_fsm_syn_rcvd" // server/client
	       or fsm->getStateName() == "tcp_fsm_syn_sent"
	       or fsm->getStateName() == "tcp_fsm_close_wait", //client
	       "Connection termination not initiated. Current state: " + fsm->getStateName());

	MESSAGE_SINGLE(NORMAL, logger, "Closing connection for flowID: " << flowID << " initiated.");

	wns::ldk::helper::FakePDUPtr pdu;
    wns::ldk::CompoundPtr compound(new wns::ldk::Compound(getFUN()->getProxy()->createCommandPool(), pdu));
    assure(tcpHeaderFU, "No FU for the TCP Header available!");
    tcpHeaderFU->activateCommand(compound->getCommandPool());

	fsm->fin();

	tcpFlagsToUpperCommand(compound);

	doSendData(compound);
}

void
ThreeWayHandshake::tcpFlagsToUpperCommand(wns::ldk::CompoundPtr& _compound)
{
    TCPCommand* tcpHeader = tcpHeaderReader->readCommand<TCPCommand>(
        _compound->getCommandPool());

	tcpHeader->peer.urg = fsm->getVariables().urg;
	tcpHeader->peer.ack = fsm->getVariables().ack;
	tcpHeader->peer.psh = fsm->getVariables().psh;
	tcpHeader->peer.rst = fsm->getVariables().rst;
	tcpHeader->peer.syn = fsm->getVariables().syn;
	tcpHeader->peer.fin = fsm->getVariables().fin;

	tcpHeader->peer.flowID = flowID;
}
