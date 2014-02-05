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

#include <IP/VirtualDNS.hpp>
#include <WNS/module/Base.hpp>
#include <WNS/logger/Logger.hpp>

using namespace ip;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    VirtualDNS,
    wns::node::component::Interface,
    "ip.VDNS",
    wns::node::component::ConfigCreator);

VirtualDNSService::VirtualDNSService():
    log("IP", "VDNS", wns::simulator::getMasterLogger())
{
}

void
VirtualDNSService::addSubnet(std::string zoneIdentifier,
                             VirtualDNS* server)
{
    servers.insert(zoneIdentifier, server);
}

VirtualDNS*
VirtualDNSService::getZoneManager(std::string zoneIdentifier)
{
    return servers.find(zoneIdentifier);
}

VirtualDNS::VirtualDNS(wns::node::Interface* _node, const wns::pyconfig::View& _pyco) :
    wns::node::component::Component(_node, _pyco),
    log(_pyco.get("logger"))
{
    TheDNSService::Instance().addSubnet(_pyco.get<std::string>("zoneIdentifier"),
                                        this);

    MESSAGE_BEGIN(NORMAL, log, m, "New DNS zone for ");
    m << _pyco.get<std::string>("zoneIdentifier");
    MESSAGE_END();
}


VirtualDNS::NLAddress
VirtualDNS::request(DomainName name)
{
    VirtualDNS::NLAddress ip = dnsLookup.find(name);
    MESSAGE_BEGIN(NORMAL, log, m, "Received request. Resolving ");
    m << name << " to ";
    m << ip;
    MESSAGE_END();

    return ip;
}

void
VirtualDNS::bind(VirtualDNS::DomainName name, VirtualDNS::NLAddress ip)
{
    MESSAGE_BEGIN(NORMAL, log, m, "Domain ");
    m << name << " is at " << ip;
    MESSAGE_END();

    dnsLookup.insert(name, ip);
}

void
VirtualDNS::unbind(VirtualDNS::DomainName name)
{
    MESSAGE_BEGIN(NORMAL, log, m, "Unbinding ");
    m << name;
    MESSAGE_END();

    dnsLookup.erase(name);
}

VirtualDNS::~VirtualDNS()
{
}

void
VirtualDNS::doStartup()
{
    MESSAGE_SINGLE(NORMAL, log, "Starting VDNS Service.");
}

void
VirtualDNS::onNodeCreated()
{
}

void
VirtualDNS::onWorldCreated()
{
}

void
VirtualDNS::onShutdown()
{
}



