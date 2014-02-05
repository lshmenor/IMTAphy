/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 5, D-52074 Aachen, Germany
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

#include <IP/tunnel/TunnelExitComponent.hpp>

#include <WNS/TypeInfo.hpp>

using namespace ip::tunnel;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	TunnelExitComponent,
	wns::node::component::Interface,
	"ip.tunnelExit",
	wns::node::component::ConfigCreator);

TunnelExitComponent::TunnelExitComponent(wns::node::Interface* _node, const wns::pyconfig::View& _pyco):
	wns::node::component::Component(_node, _pyco),
	dllHandler(NULL),
	log(_pyco.get("logger"))
{
}

void
TunnelExitComponent::doStartup()
{
	// We are a DLL
	addService(getConfig().get<std::string>("dllNotification"), this);
}

TunnelExitComponent::~TunnelExitComponent()
{
}

void
TunnelExitComponent::onNodeCreated()
{
	MESSAGE_SINGLE(NORMAL, log, "Registering with IP for IPinIP Tunneling Protocol");

	std::string notname= getConfig().get<std::string>("ipNotification");
	wns::service::nl::Notification* notification = getService<wns::service::nl::Notification*>(notname);
	assure(notification, "No IP Notification could be found!");

	// We receive IP in IP encapsulated packets
	notification->registerHandler(wns::service::nl::IP, this);
}

void
TunnelExitComponent::onWorldCreated()
{
}

void
TunnelExitComponent::onShutdown()
{
}

void
TunnelExitComponent::onData(wns::service::nl::Address /*_sourceIP*/, const wns::osi::PDUPtr& _pdu)
{

	MESSAGE_SINGLE(NORMAL, log, "Got data from IP");

	assure(this->dllHandler, "No DLL handler set for delivering packets at tunnel exit");

//	MESSAGE_SINGLE(NORMAL, log, "Type of getUserData() is : " << wns::TypeInfo::create(*(_pdu->getUserData())).demangled());

	// Unpack inner IP packet and reinject to IP
	this->dllHandler->onData(wns::osi::PDUPtr(_pdu));
}

void
TunnelExitComponent::registerHandler(wns::service::dll::protocolNumber protocol, wns::service::dll::Handler* handler)
{
	MESSAGE_SINGLE(NORMAL, log, "Got registration of DLL data handler from IP");

	assure(protocol == wns::service::dll::IP, "You tried to register a handler other than IP to an IP Tunnel interface");

	this->dllHandler = handler;
}
