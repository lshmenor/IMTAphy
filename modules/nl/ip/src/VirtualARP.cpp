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

#include <IP/VirtualARP.hpp>
#include <WNS/module/Base.hpp>
#include <WNS/logger/Logger.hpp>

using namespace ip;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	VirtualARP,
	wns::node::component::Interface,
	"ip.VARP",
	wns::node::component::ConfigCreator);

VirtualARPService::VirtualARPService():
	log("IP", "VARP", wns::simulator::getMasterLogger())
{
	MESSAGE_SINGLE(NORMAL, log, "Starting VARP Service.");
}

void
VirtualARPService::addSubnet(std::string subnetIdentifier,
							  VirtualARP* server)
{
	servers.insert(subnetIdentifier, server);
}

VirtualARP*
VirtualARPService::getZoneManager(std::string subnetIdentifier)
{
	return servers.find(subnetIdentifier);
}

VirtualARP::VirtualARP(wns::node::Interface* _node, const wns::pyconfig::View& _pyco) :
	wns::node::component::Component(_node, _pyco),
	resolveDelay(_pyco.get<simTimeType>("resolveDelay")),
	log(_pyco.get("logger"))
{
	TheARPService::Instance().addSubnet(_pyco.get<std::string>("subnetIdentifier"),
										 this);
	MESSAGE_BEGIN(NORMAL, log, m, "New ARP zone for ");
	m << _pyco.get<std::string>("subnetIdentifier");
	MESSAGE_END();
}


void
VirtualARP::request(ResolveCallback& requester, NLAddress ip)
{
	VirtualARP::DLLAddress macid = arpLookup.find(ip);
	MESSAGE_BEGIN(NORMAL, log, m, "Received request. Resolving ");
	m << ip << " to ";
	m << macid << " in t=" << resolveDelay;
	MESSAGE_END();

	wns::simulator::getEventScheduler()->scheduleDelay(ResolveTimeout(requester, macid), resolveDelay);
}

void
VirtualARP::bind(VirtualARP::DLLAddress macid, VirtualARP::NLAddress ip)
{
	MESSAGE_BEGIN(NORMAL, log, m, "MAC address ");
	m << macid << " has " << ip;
	MESSAGE_END();

	arpLookup.insert(ip, macid);
}

void
VirtualARP::unbind(VirtualARP::NLAddress ip)
{
	MESSAGE_BEGIN(NORMAL, log, m, "Unbinding ");
	m << ip;
	MESSAGE_END();

	arpLookup.erase(ip);
}

VirtualARP::~VirtualARP()
{
}

void
VirtualARP::doStartup()
{
}

void
VirtualARP::onNodeCreated()
{
}

void
VirtualARP::onWorldCreated()
{
}

void
VirtualARP::onShutdown()
{
}



