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

#include <TCP/ConnectionControl.hpp>
#include <TCP/ThreeWayHandshake.hpp>

#include <WNS/Exception.hpp>


using namespace tcp;


STATIC_FACTORY_REGISTER_WITH_CREATOR(Closed,
				     ConnectionControlInterface::StateInterface,
				     "tcp_fsm_closed",
				     wns::fsm::FSMConfigCreator);

Closed::StateInterface*
Closed::syn()
{
	vars().resetTCPFlags();
	vars().syn = true;
	return getFSM()->createState<Syn_sent>();
}

Closed::StateInterface*
Closed::syn_ack()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Closed"));
	return this;
}

Closed::StateInterface*
Closed::ack()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Closed"));
	return this;
}

Closed::StateInterface*
Closed::fin()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Closed"));
	return this;
}

Closed::StateInterface*
Closed::rst()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Closed"));
	return this;
}

Closed::StateInterface*
Closed::fin_ack()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Closed"));
	return this;
}


STATIC_FACTORY_REGISTER_WITH_CREATOR(Listen,
				     ConnectionControlInterface::StateInterface,
				     "tcp_fsm_listen",
				     wns::fsm::FSMConfigCreator);

Listen::StateInterface*
Listen::syn()
{
	throw(wns::Exception("Invalid signal in state Listen"));
	return this;
}

Listen::StateInterface*
Listen::syn_ack()
{
	vars().resetTCPFlags();
	vars().syn = true;
	vars().ack = true;
	return getFSM()->createState<Syn_rcvd>();
}

Listen::StateInterface*
Listen::ack()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Listen"));
	return this;
}

Listen::StateInterface*
Listen::fin()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Listen"));
	return this;
}

Listen::StateInterface*
Listen::rst()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Listen"));
	return this;
}

Listen::StateInterface*
Listen::fin_ack()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Listen"));
	return this;
}


STATIC_FACTORY_REGISTER_WITH_CREATOR(Syn_rcvd,
				     ConnectionControlInterface::StateInterface,
				     "tcp_fsm_syn_rcvd",
				     wns::fsm::FSMConfigCreator);

Syn_rcvd::StateInterface*
Syn_rcvd::syn()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Syn_rcvd"));
	return this;
}

Syn_rcvd::StateInterface*
Syn_rcvd::syn_ack()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Syn_rcvd"));
	return this;
}

Syn_rcvd::StateInterface*
Syn_rcvd::ack()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Syn_rcvd"));
	return this;
}

Syn_rcvd::StateInterface*
Syn_rcvd::fin()
{
	vars().resetTCPFlags();
	vars().fin = true;
	return getFSM()->createState<Fin_wait_1>();
}

Syn_rcvd::StateInterface*
Syn_rcvd::rst()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Syn_rcvd"));
	return this;
}

Syn_rcvd::StateInterface*
Syn_rcvd::fin_ack()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Syn_rcvd"));
	return this;
}



STATIC_FACTORY_REGISTER_WITH_CREATOR(Syn_sent,
				     ConnectionControlInterface::StateInterface,
				     "tcp_fsm_syn_sent",
				     wns::fsm::FSMConfigCreator);

Syn_sent::StateInterface*
Syn_sent::syn()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Syn_sent"));
	return this;
}

Syn_sent::StateInterface*
Syn_sent::syn_ack()
{
	// if syn received
	// simultaneous open (not provided by server)
	//return getFSM()->createState<Syn_rcvd>();
	throw(wns::Exception("Invalid signal in state Syn_sent"));
	return this;
}

Syn_sent::StateInterface*
Syn_sent::ack()
{
	// if syn, ack received
	vars().resetTCPFlags();
	vars().ack = true;
	return getFSM()->createState<Established>();
}

Syn_sent::StateInterface*
Syn_sent::fin()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Syn_sent"));
	return this;
}

Syn_sent::StateInterface*
Syn_sent::rst()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Syn_sent"));
	return this;
}

Syn_sent::StateInterface*
Syn_sent::fin_ack()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Syn_sent"));
	return this;
}


STATIC_FACTORY_REGISTER_WITH_CREATOR(Established,
				     ConnectionControlInterface::StateInterface,
				     "tcp_fsm_established",
				     wns::fsm::FSMConfigCreator);

Established::StateInterface*
Established::syn()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Established"));
	return this;
}

Established::StateInterface*
Established::syn_ack()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Established"));
	return this;
}

Established::StateInterface*
Established::ack()
{
	vars().resetTCPFlags();
	vars().ack = true;
	// if signal fin received
	return getFSM()->createState<Close_wait>();
}

Established::StateInterface*
Established::fin()
{
	// if connection closed by application
	vars().resetTCPFlags();
	vars().fin = true;
	return getFSM()->createState<Fin_wait_1>();
}

Established::StateInterface*
Established::rst()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Established"));
	return this;
}

Established::StateInterface*
Established::fin_ack()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Established"));
	return this;
}



STATIC_FACTORY_REGISTER_WITH_CREATOR(Close_wait,
				     ConnectionControlInterface::StateInterface,
				     "tcp_fsm_close_wait",
				     wns::fsm::FSMConfigCreator);

Close_wait::StateInterface*
Close_wait::syn()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Close_wait"));
	return this;
}

Close_wait::StateInterface*
Close_wait::syn_ack()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Close_wait"));
	return this;
}

Close_wait::StateInterface*
Close_wait::ack()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Close_wait"));
	return this;
}

Close_wait::StateInterface*
Close_wait::fin()
{
	vars().resetTCPFlags();
	vars().fin = true;
	return getFSM()->createState<Last_ack>();
}

Close_wait::StateInterface*
Close_wait::rst()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Close_wait"));
	return this;
}

Close_wait::StateInterface*
Close_wait::fin_ack()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Close_wait"));
	return this;
}


STATIC_FACTORY_REGISTER_WITH_CREATOR(Last_ack,
				     ConnectionControlInterface::StateInterface,
				     "tcp_fsm_last_ack",
				     wns::fsm::FSMConfigCreator);

Last_ack::StateInterface*
Last_ack::syn()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Last_ack"));
	return this;
}

Last_ack::StateInterface*
Last_ack::syn_ack()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Last_ack"));
	return this;
}

Last_ack::StateInterface*
Last_ack::ack()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Last_ack"));
	return this;
}

Last_ack::StateInterface*
Last_ack::fin()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Last_ack"));
	return this;
}

Last_ack::StateInterface*
Last_ack::rst()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Last_ack"));
	return this;
}

Last_ack::StateInterface*
Last_ack::fin_ack()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Last_ack"));
	return this;
}



STATIC_FACTORY_REGISTER_WITH_CREATOR(Closing,
				     ConnectionControlInterface::StateInterface,
				     "tcp_fsm_closing",
				     wns::fsm::FSMConfigCreator);

Closing::StateInterface*
Closing::syn()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Closing"));
	return this;
}

Closing::StateInterface*
Closing::syn_ack()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Closing"));
	return this;
}

Closing::StateInterface*
Closing::ack()
{
	throw(wns::Exception("Invalid signal in state Closing"));
	return this;
}

Closing::StateInterface*
Closing::fin()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Closing"));
	return this;
}

Closing::StateInterface*
Closing::rst()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Closing"));
	return this;
}

Closing::StateInterface*
Closing::fin_ack()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Closing"));
	return this;
}


STATIC_FACTORY_REGISTER_WITH_CREATOR(Fin_wait_1,
				     ConnectionControlInterface::StateInterface,
				     "tcp_fsm_fin_wait_1",
				     wns::fsm::FSMConfigCreator);

Fin_wait_1::StateInterface*
Fin_wait_1::syn()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Fin_wait_1"));
	return this;
}

Fin_wait_1::StateInterface*
Fin_wait_1::syn_ack()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Fin_wait_1"));
	return this;
}

Fin_wait_1::StateInterface*
Fin_wait_1::ack()
{
	//return getFSM()->createState<Time_wait>();
	// change: drop state time_wait (timeout)
	// directly go to closed
	vars().resetTCPFlags();
	vars().ack = true;
	return getFSM()->createState<Closing>();
}

Fin_wait_1::StateInterface*
Fin_wait_1::fin()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Fin_wait_1"));
	return this;
}

Fin_wait_1::StateInterface*
Fin_wait_1::rst()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Fin_wait_1"));
	return this;
}

Fin_wait_1::StateInterface*
Fin_wait_1::fin_ack()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Fin_wait_1"));
	return this;
}




STATIC_FACTORY_REGISTER_WITH_CREATOR(Fin_wait_2,
				     ConnectionControlInterface::StateInterface,
				     "tcp_fsm_fin_wait_2",
				     wns::fsm::FSMConfigCreator);

Fin_wait_2::StateInterface*
Fin_wait_2::syn()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Fin_wait_2"));
	return this;
}

Fin_wait_2::StateInterface*
Fin_wait_2::syn_ack()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Fin_wait_2"));
	return this;
}

Fin_wait_2::StateInterface*
Fin_wait_2::ack()
{
	// if signal fin received
	vars().resetTCPFlags();
	vars().ack = true;
	//return getFSM()->createState<Time_wait>();
	// change: drop state time_wait (timeout)
	// directly go to closed
	return getFSM()->createState<Closed>();
}

Fin_wait_2::StateInterface*
Fin_wait_2::fin()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Fin_wait_2"));
	return this;
}

Fin_wait_2::StateInterface*
Fin_wait_2::rst()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Fin_wait_2"));
	return this;
}

Fin_wait_2::StateInterface*
Fin_wait_2::fin_ack()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Fin_wait_2"));
	return this;
}




STATIC_FACTORY_REGISTER_WITH_CREATOR(Time_wait,
				     ConnectionControlInterface::StateInterface,
				     "tcp_fsm_time_wait",
				     wns::fsm::FSMConfigCreator);

Time_wait::StateInterface*
Time_wait::syn()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Time_wait"));
	return this;
}

Time_wait::StateInterface*
Time_wait::syn_ack()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Time_wait"));
	return this;
}

Time_wait::StateInterface*
Time_wait::ack()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Time_wait"));
	return this;
}

Time_wait::StateInterface*
Time_wait::fin()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Time_wait"));
	return this;
}

Time_wait::StateInterface*
Time_wait::rst()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Time_wait"));
	return this;
}

Time_wait::StateInterface*
Time_wait::fin_ack()
{
	// invalid signal
	throw(wns::Exception("Invalid signal in state Time_wait"));
	return this;
}

