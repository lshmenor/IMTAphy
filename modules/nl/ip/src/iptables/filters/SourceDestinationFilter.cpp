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

#include <IP/iptables/filters/SourceDestinationFilter.hpp>
#include <IP/IPHeader.hpp>

#include <WNS/pyconfig/View.hpp>

using namespace ip::iptables::filters;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	SourceDestinationFilter,
	FilterInterface,
	"sourceDestinationFilter",
	wns::PyConfigViewCreator);

SourceDestinationFilter::SourceDestinationFilter(const wns::pyconfig::View& _pyco):
	source(wns::service::nl::Address(_pyco.get<std::string>("source"))),
	sourceMask(wns::service::nl::Address(_pyco.get<std::string>("sourceMask"))),
	destination(wns::service::nl::Address(_pyco.get<std::string>("destination"))),
	destinationMask(wns::service::nl::Address(_pyco.get<std::string>("destinationMask")))
{
}

SourceDestinationFilter::SourceDestinationFilter(wns::service::nl::Address _source,
 												 wns::service::nl::Address _sourceMask,
												 wns::service::nl::Address _destination,
 												 wns::service::nl::Address _destinationMask)
{
	source = _source;
	sourceMask = _sourceMask;
	destination = _destination;
	destinationMask = _destinationMask;
}

bool
SourceDestinationFilter::fires(const IPCommand* ipHeader)
{
	return (source == (ipHeader->peer.source & sourceMask)) && (destination == (ipHeader->peer.destination & destinationMask));
}

bool
SourceDestinationFilter::fires(const IPCommand* ipHeader, const wns::service::tl::IUDPHeader*)
{
    return fires(ipHeader);
}

bool
SourceDestinationFilter::fires(const IPCommand* ipHeader, const wns::service::tl::ITCPHeader*)
{
    return fires(ipHeader);
}

bool
SourceDestinationFilter::operator ==(const SourceDestinationFilter& other) const
{
	return ((source == other.source) &&
			(sourceMask == other.sourceMask) &&
			(destination == other.destination) &&
			(destinationMask == other.destinationMask));
}
