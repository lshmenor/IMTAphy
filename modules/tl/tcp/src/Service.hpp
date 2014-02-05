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

#ifndef TCP_SERVICE_HPP
#define TCP_SERVICE_HPP

#include <set>

#include <TCP/HandshakeStrategyHandlerInterface.hpp>
#include <TCP/FlowHandler.hpp>

#include <WNS/container/Registry.hpp>
#include <WNS/service/tl/Service.hpp>
#include <WNS/service/tl/PortPool.hpp>
#include <WNS/service/nl/Service.hpp>
#include <WNS/service/tl/FlowID.hpp>
#include <WNS/service/dll/FlowEstablishmentAndRelease.hpp>
#include <WNS/service/dll/FlowID.hpp>
#include <WNS/service/qos/QoSClasses.hpp>
#include <WNS/ldk/Group.hpp>
#include <WNS/ldk/FlowSeparator.hpp>


namespace tcp {

	class Connection;
	class UpperConvergence;
	class LowerConvergence;
	class HandshakeStrategyInterface;
	class FlowHandler;

	/**
	 * @brief TCP service implementation.
	 * In FUN world the service implements the upperconvergence,
	 * connected to the flow separator FU.
	 */
	class Service:
		virtual public wns::service::tl::Service,
		virtual public HandshakeStrategyHandlerInterface,
		public wns::ldk::ControlService,
		virtual public wns::ldk::flowseparator::FlowInfoProvider
	{
	public:
		Service(UpperConvergence* _upperConvergence,
			LowerConvergence* _lowerConvergence,
			wns::ldk::FlowSeparator* _flowSeparator,
			const wns::pyconfig::View& _pyco,
			tcp::FlowHandler* _tcpFlowHandler,
			wns::service::dll::FlowEstablishmentAndRelease* _fear,
			wns::ldk::ControlServiceRegistry* _csr);

		virtual
		~Service();

		virtual void
		listenOnPort(
			wns::service::tl::Port _port,
			wns::service::tl::ConnectionHandler* _ch);

		/**
		 * @brief OpenConnection method is divided into two methods,
		 * because of FlowHandling at DLL.
		 *
		 * openConnectionContinue(..) method continues with the
		 * connection establishment after the DLL flow has been established.
		 * @return bool
		 */
		virtual void
		openConnection(
			wns::service::tl::Port _port,
			wns::service::nl::FQDN _source,
			wns::service::nl::FQDN _peerAddress,
			wns::service::qos::QoSClass _qosClass,
			wns::service::tl::ConnectionHandler* _ch);

		virtual void
		openConnectionContinue(
			wns::service::tl::FlowID flowID, wns::service::dll::FlowID dllFlowID = 0);

		void
		dllFlowChanged(wns::service::tl::FlowID flowID, wns::service::dll::FlowID dllFlowID);

		virtual void
		closeConnection(wns::service::tl::Connection* _connection);

		/**
		 * @brief Open connection passively.
		 * @param[in] wns::service::tl::FlowID
		 * This method is called by the lower convergence to create an
		 * instance of the flow separator's prototype iff any application
		 * is listening to the source port. The method returns true if
		 * an instance is created, false otherwise.
		 * @return bool
		 */
		void
		passiveOpenConnection(const wns::service::tl::FlowID& _flowID);

		/**
		 * @brief Stop listening on Port
		 * @param _port
		 */
		void
		stopListenOnPort(wns::service::tl::Port _port);

		/**
		 * @brief The service from IP. Needed for DataTransmission.
		 */
		void
		setDataTransmissionService(wns::service::nl::Service* service);

		void
		setDNSService(wns::service::nl::DNSService* service);

		void
		setTcpFlowHandlerService(tcp::FlowHandler* service);

		/**
		 * @brief Query wether a upper layer service is listening to a
		 * certain port
		 */
		bool
		isListening(wns::service::tl::Port _port);

		/**
		 * @brief Returns instance of underlying IP layer.
		 * Needed to get the node's ip address.
		 */
		wns::service::nl::Service*
		getIPService();

		void
		deleteFlowSeparatorInstance(const wns::service::tl::FlowID& _flowID);

		/**
		 * @brief This method is called by the handshake strategy if a
		 * connection was established.
		 */
		virtual void
		connectionEstablished(const wns::service::tl::FlowID& _flowID);

		virtual void
		connectionClosed(const wns::service::tl::FlowID& _flowID);

		/**
		 * @brief This method is called by the TCP finite state machine
		 * to inform the application of a passively closed connection
		 */
		virtual void
		passiveClosed(const wns::service::tl::FlowID& _flowID);

		/**
		 * @brief Add new instance of flow separator's prototype
		 * according to flowID. This method is used by the
		 * lowerConvergence which is also responsible for creation of
		 * instances of the prototype.
		 */
		void
		addFlowSeparatorInstance(const wns::service::tl::FlowID& _flowID);

		/**
		 * @brief Return HandshakeStrategy FU according to flowID
		 */
		HandshakeStrategyInterface*
		getHandshakeStrategyFU(const wns::service::tl::FlowID& _flowID);

		/**
		 * @brief Return the corresponding connection to flow id
		 * @param[in] _flowID
		 */
		Connection*
		getConnection(const wns::service::tl::FlowID& _flowID);

		/**
		 * @brief The FlowInfoProvider interface
		 */
		virtual bool
		isValidFlow(const wns::ldk::ConstKeyPtr& _key) const;

	private:
		/**
		 * @brief Callback event to delete instance of flow separator
		 */
		class RemoveFlowSeparatorInstance
		{
		public:
			/**
			 * @brief Constructor
			 * @param[in] The instance of the TL service.
			 * @param[in] The flow ID of the flow separator's
			 * instance being deleted.
			 */
			RemoveFlowSeparatorInstance(Service* _tlService, const wns::service::tl::FlowID& _flowID)
			{
				assure(_tlService, "No valid instance of TL service!");
				tlService = _tlService;
				flowID = _flowID;
			}

			/**
			 * @brief This is called by the event handler. The flow
			 * separator's intance according to the flow ID is now removed.
			 */
			void
			operator()()
			{
				tlService->deleteFlowSeparatorInstance(flowID);
			}

		private:
			/**
			 * @brief The instance of the TL service
			 */
			Service* tlService;

			/**
			 * @brief Which instance of flow separator has to be removed
			 */
			wns::service::tl::FlowID flowID;
		};

		/**
		 * @brief The delay between call to remove the flow separator's instance.
		 */
		simTimeType removeDelay;

		/**
		 * @brief TL layer needs to know the IP address
		 */
		wns::service::nl::Service* ipService;

		wns::service::nl::DNSService* dns;

		tcp::FlowHandler* tcpFlowHandler;

		/**
		 * @brief Mapping of flow id to corresponding opened connection
		 */
		wns::container::Registry<
			wns::service::tl::FlowID,
			Connection*,
			wns::container::registry::NoneOnErase> flowIDToConnections;

		/**
		 * @brief Mapping of flowID <--> connectionHandler
		 */
		wns::container::Registry<
            wns::service::tl::FlowID,
			wns::service::tl::ConnectionHandler*,
			wns::container::registry::NoneOnErase> flowIDToConnectionHandler;

		/**
		 * @brief Registration of ports a server is listening to
		 */
		wns::container::Registry<wns::service::tl::Port, wns::service::tl::ConnectionHandler*> listeners;

		/**
		 * @brief The transport layer's port pool
		 */
		wns::service::tl::PortPool portPool;

		/**
		 * @brief The TL service's logger.
		 */
		wns::logger::Logger logger;

		wns::pyconfig::View pyco;

		/**
		 * @brief The upper FU of TCP's FUN
		 */
		UpperConvergence* upperConvergence;

		/**
		 * @brief The lower FU of TCP's FUN
		 */
		LowerConvergence* lowerConvergence;

		/**
		 * @brief The flow separator FU of TCP's FUN
		 */
		wns::ldk::FlowSeparator* flowSeparator;
		wns::service::dll::FlowEstablishmentAndRelease* flowEstablishmentAndRelease;
	};
} // namespace tcp

#endif // NOT defined TCP_SERVICE_HPP
