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

#ifndef WNS_PROBE_BUS_JSON_PROBEBUS_HPP
#define WNS_PROBE_BUS_JSON_PROBEBUS_HPP

#include <WNS/probe/bus/json/writer.hpp>
#include <WNS/probe/bus/json/elements.hpp>
#include <WNS/probe/bus/ContextCollector.hpp>

namespace wns { namespace probe  { namespace bus { namespace json {

void probeJSON(const wns::probe::bus::ContextCollectorPtr&, const wns::probe::bus::json::Object&);

} // json
} // bus
} // probe
} // wns

#endif // WNS_PROBE_BUS_JSON_PROBEBUS_HPP
