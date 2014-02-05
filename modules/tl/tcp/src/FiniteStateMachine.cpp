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

#include <TCP/FiniteStateMachine.hpp>


using namespace tcp;

FiniteStateMachine::FiniteStateMachine(const ConnectionControlInterface::VariablesType& _v, wns::logger::Logger _logger) :
	ConnectionControlInterface(_v),
	logger(_logger)
{
	changeState(createState<Closed>());
	MESSAGE_SINGLE(NORMAL, logger, "FSM initialized to state " + getStateName());
}

void
FiniteStateMachine::syn()
{
	MESSAGE_SINGLE(NORMAL, logger, "Sending SYN.");
	changeState(getState()->syn());
}

void
FiniteStateMachine::syn_ack()
{
	MESSAGE_SINGLE(NORMAL, logger, "Sending SYN_ACK.");
	changeState(getState()->syn_ack());
}

void
FiniteStateMachine::ack()
{
	MESSAGE_SINGLE(NORMAL, logger, "Sending ACK.");
	changeState(getState()->ack());
}

void
FiniteStateMachine::fin()
{
	MESSAGE_SINGLE(NORMAL, logger, "Sending FIN.");
	changeState(getState()->fin());
}

void
FiniteStateMachine::rst()
{
	MESSAGE_SINGLE(NORMAL, logger, "Sending RST.");
	changeState(getState()->rst());
}

void
FiniteStateMachine::fin_ack()
{
	MESSAGE_SINGLE(NORMAL, logger, "Sending FIN_ACK.");
	changeState(getState()->fin_ack());
}
