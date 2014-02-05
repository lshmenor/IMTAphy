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

#ifndef TCP_CONNECTIONCONTROL_HPP
#define TCP_CONNECTIONCONTROL_HPP

#include <WNS/fsm/FSM.hpp>


namespace tcp {

	/**
	 * @brief The interface to define the transitions of TCP's
	 * finite state machine.
	 */
	class ConnectionControlSignals
	{
	public:
		virtual ConnectionControlSignals*
		syn() = 0;

		virtual ConnectionControlSignals*
		syn_ack() = 0;

		virtual ConnectionControlSignals*
		ack() = 0;

		virtual ConnectionControlSignals*
		fin() = 0;

		virtual ConnectionControlSignals*
		rst() = 0;

		virtual ConnectionControlSignals*
		fin_ack() = 0;

		virtual
		~ConnectionControlSignals(){}

	protected:
		/**
		 * @brief The constructor is called by derived class exclusively.
		 */
		ConnectionControlSignals(){}

	private:
		// disallow copy contructor
		ConnectionControlSignals(const ConnectionControlSignals&);
	};

	struct ConnectionVariables
	{
		ConnectionVariables() : activeOpen(false),
					urg(false),
					ack(false),
					psh(false),
					rst(false),
					syn(false),
					fin(false)
		{}

		/**
		 * @brief The transition from SYN_RCVD back to LISTEN is valid
		 * only if the SYN_RCVD state was entered from the LISTEN state (normal
		 * scenario)
		 * A client sets activeOpen on true, for a server this varible's
		 * value is the complement.
		 */
		bool activeOpen;
		bool urg;
		bool ack;
		bool psh;
		bool rst;
		bool syn;
		bool fin;

		void resetTCPFlags()
		{
			urg = false;
			ack = false;
			psh = false;
			rst = false;
			syn = false;
			fin = false;
		}
	};

	typedef wns::fsm::FSM<ConnectionControlSignals, ConnectionVariables> ConnectionControlInterface;

	class Closed :
		public ConnectionControlInterface::StateInterface
	{
	public:
		Closed(ConnectionControlInterface* cci) : ConnectionControlInterface::StateInterface(cci, "tcp_fsm_closed")
		{}

		virtual StateInterface*
		syn();

		virtual StateInterface*
		syn_ack();

		virtual StateInterface*
		ack();

		virtual StateInterface*
		fin();

		virtual StateInterface*
		rst();

		virtual StateInterface*
		fin_ack();
	};

	class Listen : 
		public ConnectionControlInterface::StateInterface
	{
	public:
		Listen(ConnectionControlInterface* cci) : ConnectionControlInterface::StateInterface(cci, "tcp_fsm_listen")
		{}

		virtual StateInterface*
		syn();

		virtual StateInterface*
		syn_ack();

		virtual StateInterface*
		ack();

		virtual StateInterface*
		fin();

		virtual StateInterface*
		rst();

		virtual StateInterface*
		fin_ack();
	};

	class Syn_rcvd : 
		public ConnectionControlInterface::StateInterface
	{
	public:
		Syn_rcvd(ConnectionControlInterface* cci) : ConnectionControlInterface::StateInterface(cci, "tcp_fsm_syn_rcvd")
		{}

		virtual StateInterface*
		syn();

		virtual StateInterface*
		syn_ack();

		virtual StateInterface*
		ack();

		virtual StateInterface*
		fin();

		virtual StateInterface*
		rst();

		virtual StateInterface*
		fin_ack();
	};

	class Syn_sent : 
		public ConnectionControlInterface::StateInterface
	{
	public:
		Syn_sent(ConnectionControlInterface* cci) : ConnectionControlInterface::StateInterface(cci, "tcp_fsm_syn_sent")
		{}

		virtual StateInterface*
		syn();

		virtual StateInterface*
		syn_ack();

		virtual StateInterface*
		ack();

		virtual StateInterface*
		fin();

		virtual StateInterface*
		rst();

		virtual StateInterface*
		fin_ack();
	};

	class Established : public ConnectionControlInterface::StateInterface
	{
	public:
		Established(ConnectionControlInterface* cci) : ConnectionControlInterface::StateInterface(cci, "tcp_fsm_established")
		{}

		virtual StateInterface*
		syn();

		virtual StateInterface*
		syn_ack();

		virtual StateInterface*
		ack();

		virtual StateInterface*
		fin();

		virtual StateInterface*
		rst();

		virtual StateInterface*
		fin_ack();
	};

	class Close_wait : public ConnectionControlInterface::StateInterface
	{
	public:
		Close_wait(ConnectionControlInterface* cci) : ConnectionControlInterface::StateInterface(cci, "tcp_fsm_close_wait")
		{}

		virtual StateInterface*
		syn();

		virtual StateInterface*
		syn_ack();

		virtual StateInterface*
		ack();

		virtual StateInterface*
		fin();

		virtual StateInterface*
		rst();

		virtual StateInterface*
		fin_ack();
	};

	class Last_ack : public ConnectionControlInterface::StateInterface
	{
	public:
		Last_ack(ConnectionControlInterface* cci) : ConnectionControlInterface::StateInterface(cci, "tcp_fsm_last_ack")
		{}

		virtual StateInterface*
		syn();

		virtual StateInterface*
		syn_ack();

		virtual StateInterface*
		ack();

		virtual StateInterface*
		fin();

		virtual StateInterface*
		rst();

		virtual StateInterface*
		fin_ack();
	};

	class Fin_wait_1 : public ConnectionControlInterface::StateInterface
	{
	public:
		Fin_wait_1(ConnectionControlInterface* cci) : ConnectionControlInterface::StateInterface(cci, "tcp_fsm_fin_wait_1")
		{}

		virtual StateInterface*
		syn();

		virtual StateInterface*
		syn_ack();

		virtual StateInterface*
		ack();

		virtual StateInterface*
		fin();

		virtual StateInterface*
		rst();

		virtual StateInterface*
		fin_ack();
	};

	class Closing : public ConnectionControlInterface::StateInterface
	{
	public:
		Closing(ConnectionControlInterface* cci) : ConnectionControlInterface::StateInterface(cci, "tcp_fsm_closing")
		{}

		virtual StateInterface*
		syn();

		virtual StateInterface*
		syn_ack();

		virtual StateInterface*
		ack();

		virtual StateInterface*
		fin();

		virtual StateInterface*
		rst();

		virtual StateInterface*
		fin_ack();
	};

	class Fin_wait_2 : public ConnectionControlInterface::StateInterface
	{
	public:
		Fin_wait_2(ConnectionControlInterface* cci) : ConnectionControlInterface::StateInterface(cci, "tcp_fsm_fin_wait_2")
		{}

		virtual StateInterface*
		syn();

		virtual StateInterface*
		syn_ack();

		virtual StateInterface*
		ack();

		virtual StateInterface*
		fin();

		virtual StateInterface*
		rst();

		virtual StateInterface*
		fin_ack();
	};

	class Time_wait : public ConnectionControlInterface::StateInterface
	{
	public:
		Time_wait(ConnectionControlInterface* cci) : ConnectionControlInterface::StateInterface(cci, "tcp_fsm_time_wait")
		{}

		virtual StateInterface*
		syn();

		virtual StateInterface*
		syn_ack();

		virtual StateInterface*
		ack();

		virtual StateInterface*
		fin();

		virtual StateInterface*
		rst();

		virtual StateInterface*
		fin_ack();
	};
} // namespace tcp


#endif // NOT defined TCP_CONNECTIONCONTROL_HPP
