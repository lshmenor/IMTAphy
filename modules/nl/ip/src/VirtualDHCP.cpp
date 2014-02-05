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

#include <IP/VirtualDHCP.hpp>
#include <WNS/module/Base.hpp>
#include <WNS/logger/Logger.hpp>

using namespace ip;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	VirtualDHCP,
	wns::node::component::Interface,
	"ip.VDHCP",
	wns::node::component::ConfigCreator);

VirtualDHCPService::VirtualDHCPService():
	log("IP", "VDHCP", wns::simulator::getMasterLogger())
{
	MESSAGE_SINGLE(NORMAL, log, "Starting VDHCP Service.");
}

void
VirtualDHCPService::addSubnet(std::string subnetIdentifier,
							  VirtualDHCP* server)
{
	servers.insert(subnetIdentifier, server);
}

VirtualDHCP*
VirtualDHCPService::getZoneManager(std::string subnetIdentifier)
{
	return servers.find(subnetIdentifier);
}

VirtualDHCP::VirtualDHCP(wns::node::Interface* _node, const wns::pyconfig::View& _pyco) :
	wns::node::component::Component(_node, _pyco),
	log(_pyco.get("logger")),
	ipPool(_pyco.get<simTimeType>("unbindDelay"),
	       VirtualDHCP::Address(_pyco.get<std::string>("startAddress")),
	       VirtualDHCP::Address(_pyco.get<std::string>("endAddress"))),
	subnetMask(_pyco.get<std::string>("subnetMask"))
{
}


DHCPAck
VirtualDHCP::request()
{
	VirtualDHCP::Address ip = ipPool.suggestPort();
	ipPool.bind(ip);

	MESSAGE_BEGIN(NORMAL, log, m, "Received request. Leasing ");
	m << ip;
	MESSAGE_END();

	return DHCPAck(ip, subnetMask);
}

void
VirtualDHCP::release(VirtualDHCP::Address address)
{
	MESSAGE_BEGIN(NORMAL, log, m, "Received release for address ");
	m << address;
	MESSAGE_END();
}


VirtualDHCP::~VirtualDHCP()
{
}

void
VirtualDHCP::doStartup()
{
	TheDHCPService::Instance().addSubnet(getConfig().get<std::string>("subnetIdentifier"),
										 this);
	MESSAGE_BEGIN(NORMAL, log, m, "New DHCP zone for ");
	m << getConfig().get<std::string>("subnetIdentifier");
	m << " (Start : " << getConfig().get<std::string>("startAddress");
	m << " End : " << getConfig().get<std::string>("endAddress") << ")";
	MESSAGE_END();

	MESSAGE_SINGLE(NORMAL, log, "SubnetMask is : " << subnetMask);


}

void
VirtualDHCP::onNodeCreated()
{
}

void
VirtualDHCP::onWorldCreated()
{
}

void
VirtualDHCP::onShutdown()
{
}



