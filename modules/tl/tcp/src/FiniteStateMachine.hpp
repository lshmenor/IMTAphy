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

#ifndef TCP_FINITESTATEMACHINE_HPP
#define TCP_FINITESTATEMACHINE_HPP

#include <WNS/logger/Logger.hpp>

#include <TCP/ConnectionControl.hpp>
#include <TCP/ThreeWayHandshake.hpp>


namespace tcp {

	class FiniteStateMachine :
		public ConnectionControlInterface
	{
	public:
		/**
		 * @brief The Implementation of TCP's finite state machine
		 * @param[in] Global variables shared by all states of the fsm
		 */
		FiniteStateMachine(const ConnectionControlInterface::VariablesType& v, wns::logger::Logger _logger);

		/**
		 * @brief Definition of all signlas defined in
		 * ConnectionControl to perform the state transitions of the finite
		 * state machine.
		 */
		void
		syn();

		void
		syn_ack();

		void
		ack();

		void
		fin();

		void
		rst();

		void
		fin_ack();

		tcp::ConnectionVariables&
		ConnectionVariables()
		{
			return getVariables();
		}

		/**
		 * @brief My logger.
		 */
		wns::logger::Logger logger;

	private:
		/**
		 * @brief Copy constructor
		 * Disallow the copy constructor
		 */
		FiniteStateMachine(FiniteStateMachine& _fsm) : ConnectionControlInterface(_fsm.getVariables()){}
	};
} // namespace tcp

#endif // NOT defined TCP_FINITESTATEMACHINE_HPP
