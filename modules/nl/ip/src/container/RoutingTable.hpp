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

#ifndef IP_CONTAINER_ROUTINGTABLEENTRY_HPP
#define IP_CONTAINER_ROUTINGTABLEENTRY_HPP

#include <WNS/pyconfig/View.hpp>
#include <WNS/node/Node.hpp>
#include <WNS/service/nl/Address.hpp>

namespace ip { namespace container {

	class RoutingTableEntry
	{
	public:
		RoutingTableEntry() :
			netaddress(0),
			netmask(0),
			gateway(0),
			dllName("")
		{}

		RoutingTableEntry(const wns::pyconfig::View& _pyco);

		void operator=(const RoutingTableEntry& that)
		{
			netaddress = that.netaddress;
			netmask = that.netmask;
			gateway = that.gateway;
			dllName = that.dllName;
		}

		wns::service::nl::Address netaddress;
		wns::service::nl::Address netmask;
		wns::service::nl::Address gateway;
		std::string dllName;
	};

	typedef std::list<RoutingTableEntry> RoutingTable;

} //container
} //ip
#endif // NOT defined IP_CONTAINER_ROUTINGTABLEENTRY_HPP
