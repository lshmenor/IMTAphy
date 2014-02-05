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

#ifndef IP_CONTAINER_DATALINK_HPP
#define IP_CONTAINER_DATALINK_HPP

#include <IP/Resolver.hpp>
#include <IP/VirtualARP.hpp>
#include <IP/trace/TraceCollector.hpp>

#include <WNS/service/dll/DataTransmission.hpp>
#include <WNS/service/dll/Handler.hpp>
#include <WNS/container/Registry.hpp>

namespace ip { namespace container {

	class DataLink
	{
	public:
		DataLink();

		DataLink(std::string name,
				 wns::service::dll::UnicastDataTransmission* dts,
				 wns::service::dll::Notification* notify,
				 std::string arpZone,
				 ip::resolver::ResolverInterface* resolver,
				 bool pointToPoint,
				 bool traceEnabled,
				 trace::TraceCollector* traceCollector);

		~DataLink() {};
		std::string name;
		wns::service::dll::UnicastDataTransmission* dts;
		wns::service::dll::Notification* notify;
		std::string arpZone;
		ip::resolver::ResolverInterface* resolver;
		bool pointToPoint;
		bool traceEnabled;
		trace::TraceCollector* traceCollector;
  };

  typedef wns::container::Registry<std::string, DataLink> DataLinkContainer;

} // container
} // ip

#endif // IP_CONTAINER_DATALINK_HPP
