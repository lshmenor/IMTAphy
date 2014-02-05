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

#include <IP/tunnel/TunnelEntryComponent.hpp>

using namespace ip::tunnel;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	TunnelEntryComponent,
	wns::node::component::Interface,
	"ip.tunnelEntry",
	wns::node::component::ConfigCreator);

TunnelEntryComponent::TunnelEntryComponent(wns::node::Interface* _node, const wns::pyconfig::View& _pyco) :
	wns::node::component::Component(_node, _pyco),
	ipService(NULL),
	log(_pyco.get("logger"))
{
}

void
TunnelEntryComponent::doStartup()
{
	this->tunnelEntryAddress = wns::service::nl::Address(getConfig().get<std::string>("tunnelEntryAddress"));
	this->tunnelExitAddress = wns::service::nl::Address(getConfig().get<std::string>("tunnelExitAddress"));

	// We are a DLL, I got the transmission with me!
	addService(getConfig().get<std::string>("dataTransmission"), this);
}

TunnelEntryComponent::~TunnelEntryComponent()
{
}

void
TunnelEntryComponent::onNodeCreated()
{
	MESSAGE_SINGLE(NORMAL, log, "Retrieving IP service to inject tunneled packets.");
	std::string ipname = getConfig().get<std::string>("ipService");
	this->ipService = getService<wns::service::nl::Service*>(ipname);
	assure(this->ipService, "No IP Service could be found!");
}

void
TunnelEntryComponent::onWorldCreated()
{
}

void
TunnelEntryComponent::onShutdown()
{
}

void
TunnelEntryComponent::sendData(const wns::service::dll::UnicastAddress&,
							   const wns::osi::PDUPtr& data,
							   wns::service::dll::protocolNumber,
							   int /*_dllFlowID*/)
{
	MESSAGE_SINGLE(NORMAL, log, "Tunneling a packet.");
	assure(this->ipService, "No IP Service could be found!");
	this->ipService->sendData(this->tunnelEntryAddress, this->tunnelExitAddress, data, wns::service::nl::IP);
}

wns::service::dll::UnicastAddress
TunnelEntryComponent::getMACAddress() const
{
	MESSAGE_SINGLE(NORMAL, log, "getMACAddress called");
	return wns::service::dll::UnicastAddress();
}
