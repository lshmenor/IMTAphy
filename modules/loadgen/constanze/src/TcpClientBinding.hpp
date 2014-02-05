/******************************************************************************
 * TCP Binding                                                                *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2005                                                         *
 * Lehrstuhl fuer Kommunikationsnetze (ComNets)                               *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnetsrwth-aachen.de, www: http://wns.comnets.rwth-aachen.de/  *
 ******************************************************************************/

#ifndef CONSTANZE_TCPCLIENTBINDING_HPP
#define CONSTANZE_TCPCLIENTBINDING_HPP

#include <CONSTANZE/TcpBinding.hpp>
#include <CONSTANZE/Generator.hpp>

#include <WNS/service/nl/Service.hpp>
#include <WNS/service/nl/Address.hpp>
#include <WNS/service/tl/Service.hpp>
#include <WNS/service/tl/ConnectionHandler.hpp>
#include <WNS/service/tl/DataHandler.hpp>
#include <WNS/service/tl/PortPool.hpp>
#include <WNS/service/tl/Connection.hpp>

#include <WNS/PyConfigViewCreator.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/module/Base.hpp>


namespace constanze
{
	class TcpClientBinding:
		public Binding,
		public wns::service::tl::ConnectionHandler,
		public wns::service::tl::DataHandler
	{
	public:
		TcpClientBinding(const wns::pyconfig::View& _pyco);
		~TcpClientBinding();

		// from Binding
		virtual void
		registerComponent(wns::node::component::Component* _component);

		virtual void
		sendData(const wns::osi::PDUPtr& _data);

		virtual void
		initBinding(constanze::StartTrigger* _startTrigger);

		virtual void
		releaseBinding(constanze::StopTrigger* _stopTrigger);

		virtual void
		registerListener(constanze::Listener*);

		// from ConnectionHandler
		virtual void
		onConnectionEstablished(wns::service::nl::Address _sourceIpAddress, wns::service::tl::Connection* _newConnection);

		virtual void
		onConnectionClosed(wns::service::tl::Connection* _connection);

		virtual void
		onConnectionClosedByPeer(wns::service::tl::Connection* _connection);

		virtual void
		onConnectionLost(wns::service::tl::Connection* _connection);

		// from DataHandler
		virtual void
		onData(const wns::osi::PDUPtr& _pdu);

    protected:
		constanze::StartTrigger* startTrigger;
		constanze::StopTrigger* stopTrigger;

		wns::node::component::Component* component;

		wns::pyconfig::View pyco;

		wns::service::nl::DNSService* dns;
		wns::service::nl::FQDN domainName;
		wns::service::nl::FQDN destinationDomainName;
		wns::service::nl::Address peerAddress;
		wns::service::tl::Port destinationPort;
		wns::service::tl::Service* tcpService;
		wns::service::tl::ConnectionHandler* connectionHandler;
		wns::service::tl::Connection* connection;
		wns::service::qos::QoSClass qosClass;

		virtual std::string
		printAddress() const;

		/**
		 * @brief The Logger instance.
		 */
		wns::logger::Logger log;

		/** @brief count packets */
		long unsigned int packetCounter;

		/** @brief count bits */
		long unsigned int bitCounter;
	};
} //constanze

#endif
