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

#ifndef CONSTANZE_TCPSERVERBINDING_HPP
#define CONSTANZE_TCPSERVERBINDING_HPP

#include <CONSTANZE/Binding.hpp>
#include <CONSTANZE/Generator.hpp>

#include <WNS/service/nl/Service.hpp>
#include <WNS/service/nl/Address.hpp>
#include <WNS/service/tl/Service.hpp>
#include <WNS/service/tl/ConnectionHandler.hpp>
#include <WNS/service/tl/DataHandler.hpp>
#include <WNS/service/tl/PortPool.hpp>
#include <WNS/service/tl/Connection.hpp>

#include <WNS/logger/Logger.hpp>
#include <WNS/module/Base.hpp>


namespace constanze
{
	class TcpServerBinding:
		public Binding,
		public wns::service::tl::ConnectionHandler
	{
	public:
		TcpServerBinding(wns::service::nl::Address _address, wns::service::tl::Connection* _connection, wns::logger::Logger& logger);
		virtual ~TcpServerBinding() {};


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
		registerListener(constanze::Listener* _listener);

		// from ConnectionHandler
		virtual void
		onConnectionEstablished(wns::service::nl::Address _sourceIpAddress, wns::service::tl::Connection* _newConnection);

		virtual void
		onConnectionClosed(wns::service::tl::Connection* _connection);

		virtual void
		onConnectionClosedByPeer(wns::service::tl::Connection* _connection);

		virtual void
		onConnectionLost(wns::service::tl::Connection* _connection);

    protected:
		constanze::StartTrigger* startTrigger;
		constanze::StopTrigger* stopTrigger;
		//wns::service::tl::Service* tcpService;

		wns::service::tl::ConnectionHandler* connectionHandler;

		wns::node::component::Component* component;

		//wns::service::nl::FQDN domainName;
		//wns::service::nl::FQDN destinationDomainName;
		//wns::service::tl::Port destinationPort;

		virtual std::string
		printAddress() const;

		/** @brief The Logger instance. */
		wns::logger::Logger log;
	private:
		/** @brief peerAddress (the peer connected to us) */
		wns::service::nl::DNSService* dns;
		wns::service::nl::Address peerAddress;
		wns::service::tl::Connection* connection;
	};
} //constanze

#endif
