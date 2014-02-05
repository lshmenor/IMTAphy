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

#include <IP/container/DataLink.hpp>

using namespace ip::container;

DataLink::DataLink():
  name(""),
  dts(NULL),
  notify(NULL),
  resolver(NULL)
{}

DataLink::DataLink(std::string _name,
				   wns::service::dll::UnicastDataTransmission* _dts,
				   wns::service::dll::Notification* _notify,
				   std::string _arpZone,
				   ip::resolver::ResolverInterface* _resolver,
				   bool _pointToPoint,
				   bool _traceEnabled,
				   trace::TraceCollector* _traceCollector
				   )
{
  name = _name;
  dts = _dts;
  notify = _notify;
  arpZone = _arpZone;
  resolver = _resolver;
  pointToPoint = _pointToPoint;
  traceEnabled = _traceEnabled;
  assure(_traceCollector, "No valid Tracecollector given!");
  traceCollector = _traceCollector;
}

