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

#ifndef CONSTANZE_UDPBINDING_HPP
#define CONSTANZE_UDPBINDING_HPP

#include <CONSTANZE/Binding.hpp>
#include <CONSTANZE/Generator.hpp>

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
	class UdpBinding:
		public Binding,
		public wns::service::tl::ConnectionHandler
    {
	public:
		UdpBinding(const wns::pyconfig::View& _pyco);
		virtual ~UdpBinding() {};


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

		wns::service::tl::Service* udpService;

		wns::service::tl::ConnectionHandler* connectionHandler;

		wns::service::tl::Connection* connection;

		wns::node::component::Component* component;

		wns::pyconfig::View pyco;

		wns::service::nl::FQDN domainName;

		wns::service::nl::FQDN destinationDomainName;

		wns::service::tl::Port destinationPort;

		wns::service::qos::QoSClass qosClass;

		virtual std::string	printAddress() const;

		/**
		 * @brief The Logger instance.
		 */
		wns::logger::Logger log;

	};
} //constanze

#endif
