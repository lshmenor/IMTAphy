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

#include <IP/Resolver.hpp>
#include <IP/VirtualDHCP.hpp>

using namespace ip::resolver;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	FixedAddressResolver,
	ResolverInterface,
	"ip.resolver.fixed",
	wns::PyConfigViewCreator);

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	VirtualDHCPResolver,
	ResolverInterface,
	"ip.resolver.vdhcp",
	wns::PyConfigViewCreator);

FixedAddressResolver::FixedAddressResolver(const wns::pyconfig::View& _pyco):
  address(_pyco.get<std::string>("address")),
  subnetMask(_pyco.get<std::string>("subnetMask"))
{
}

void
FixedAddressResolver::resolve()
{
}

wns::service::nl::Address
FixedAddressResolver::getAddress()
{
	return address;
}

wns::service::nl::Address
FixedAddressResolver::getSubnetMask()
{
	return subnetMask;
}

VirtualDHCPResolver::VirtualDHCPResolver(const wns::pyconfig::View& _pyco)
{
  zone = _pyco.get<std::string>("vdhcpZone");
}

void
VirtualDHCPResolver::resolve()
{
	DHCPAck dhcpAck = TheDHCPService::Instance().getZoneManager(zone)->request();
	address = dhcpAck.address;
	subnetMask = dhcpAck.subnetMask;
}

wns::service::nl::Address
VirtualDHCPResolver::getAddress()
{
	return address;
}

wns::service::nl::Address
VirtualDHCPResolver::getSubnetMask()
{
	return subnetMask;;
}
