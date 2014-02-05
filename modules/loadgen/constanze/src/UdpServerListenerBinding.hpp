/******************************************************************************
 * UDP ServerListener Binding                                                 *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2005                                                         *
 * Lehrstuhl fuer Kommunikationsnetze (ComNets)                               *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnetsrwth-aachen.de, www: http://wns.comnets.rwth-aachen.de/  *
 ******************************************************************************/

#ifndef CONSTANZE_UDPLISTENERBINDING_HPP
#define CONSTANZE_UDPLISTENERBINDING_HPP

#include <CONSTANZE/Binding.hpp>
#include <CONSTANZE/Generator.hpp>
#include <CONSTANZE/Listener.hpp>

#include <WNS/service/nl/Service.hpp>
#include <WNS/service/nl/Address.hpp>
//#include <WNS/service/nl/DataHandler.hpp>
#include <WNS/service/tl/Service.hpp>
#include <WNS/service/tl/ConnectionHandler.hpp>
#include <WNS/service/tl/DataHandler.hpp>
#include <WNS/service/tl/Connection.hpp>
#include <WNS/service/tl/PortPool.hpp>
#include <WNS/PyConfigViewCreator.hpp>

#include <WNS/logger/Logger.hpp>
#include <WNS/module/Base.hpp>

#include <WNS/container/Registry.hpp>


namespace constanze
{
	class UdpServerListenerBinding:
		public Binding,
		public wns::service::tl::ConnectionHandler,
		public wns::service::tl::DataHandler
	{
	public:
        UdpServerListenerBinding(const wns::pyconfig::View& _pyco);
		virtual ~UdpServerListenerBinding();

		// from Binding
		virtual void
		registerComponent(wns::node::component::Component* _component);

		virtual void
		sendData(const wns::osi::PDUPtr& _data);

		virtual void
		initBinding(constanze::StartTrigger* _trgger);

		virtual void
		releaseBinding(constanze::StopTrigger* _trigger);

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

		// from DataHandler
		virtual void
		onData(const wns::osi::PDUPtr& _data);

		virtual std::string printAddress() const;

	protected:
		wns::pyconfig::View pyco;

		/** @brief The Logger instance. */
		wns::logger::Logger log;

        unsigned int nodeID;
		wns::service::nl::Address ownAddress;
		wns::service::nl::Address peerAddress;
		wns::service::nl::DNSService* dns;
		wns::service::tl::Port listenPort;
		wns::service::tl::Service* udpService;
		wns::service::tl::ConnectionHandler* connectionHandler;

		constanze::Listener* listener;
		wns::node::component::Component* component;

	private:
		wns::pyconfig::View generatorConfig;
		GeneratorBase::Creator* c;
		std::list<GeneratorBase*> generatorList;
	};
} //constanze

#endif
