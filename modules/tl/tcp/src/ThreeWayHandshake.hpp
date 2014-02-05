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

#ifndef TCP_THREEWAYHANDSHAKE_HPP
#define TCP_THREEWAYHANDSHAKE_HPP

#include <TCP/HandshakeStrategyInterface.hpp>
#include <TCP/TCPHeader.hpp>

#include <WNS/logger/Logger.hpp>

#include <WNS/service/nl/Service.hpp>

#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/ldk/Command.hpp>


namespace tcp {

	class Service;
	class UpperCommand;
	class FiniteStateMachine;
	class HandshakeStrategyHandlerInterface;

	/**
	 * @brief This FU holds the TCP's finite state machine and delegates the
	 * control messages between it and the dispatcher
	 */
	class ThreeWayHandshake :
		public virtual HandshakeStrategyInterface,
		public virtual wns::ldk::FunctionalUnit,
		public wns::ldk::CommandTypeSpecifier<>,
		public wns::ldk::HasReceptor<>,
		public wns::ldk::HasConnector<>,
		public wns::ldk::HasDeliverer<>,
		public wns::Cloneable<ThreeWayHandshake>
	{
	public:
		ThreeWayHandshake(wns::ldk::fun::FUN* _fun, const wns::pyconfig::View& _pyco);

		/**
		 * @brief Component's destructor
		 */
		virtual
		~ThreeWayHandshake();


		/**
		 * @brief Implementing the CompoundHandlerInterface
		 */
		virtual bool
		doIsAccepting(const wns::ldk::CompoundPtr& _compound) const;

		/**
		 * @brief Find my friends. This is currently the connection
		 * comprising state while connection establishing or termination
		 */
		virtual void
		onFUNCreated();

		/**
		 *
		 */
		virtual void
		doOnData(const wns::ldk::CompoundPtr& _compound);

		/**
		 *
		 */
		virtual void
		doSendData(const wns::ldk::CompoundPtr& _compound);

		/**
		 * @brief 
		 */
		virtual void
		activeOpen(const wns::service::tl::FlowID& _flowID);

		/**
		 * @brief 
		 */
		virtual void
		passiveOpen(const wns::service::tl::FlowID& _flowID);

		/**
		 * @brief 
		 */
		virtual void
		closeConnection(const wns::service::tl::FlowID& _flowID);

		/**
		 * @brief Register the TCP service
		 */
		virtual void
		registerStrategyHandler(HandshakeStrategyHandlerInterface* _strategyHandler);

		/**
		 * @brief This function the boolean flags of the UpperCommand
		 * according to the flags set by the state transitions.
		 */
		void
		tcpFlagsToUpperCommand(wns::ldk::CompoundPtr& _compound);

	private:
		bool
		doIsAccepting(const wns::ldk::CompoundPtr& _compound);

		void
		doWakeup();

		/**
		 * @brief My logger.
		 */
		wns::logger::Logger logger;

		/**
		 * @brief The service of the TCP component
		 */
		HandshakeStrategyHandlerInterface* strategyHandler;

		/**
		 * @brief The flow ID of this instance
		 */
		wns::service::tl::FlowID flowID;

		/**
		 * @brief Configuration for this component
		 */
		wns::pyconfig::View pyco;

		/**
		 * @brief The implementation of TCP's finite state machine.
		 */
		FiniteStateMachine* fsm;

        TCPHeader* tcpHeaderFU;

        wns::ldk::CommandReaderInterface* tcpHeaderReader;
	};
} // namespace tcp

#endif // NOT defined TCP_THREEWAYHANDSHAKE_HPP
