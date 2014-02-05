/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 16, D-52074 Aachen, Germany
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

#include <WNS/probe/bus/tests/ProbeBusStub.hpp>

#include <sstream>

using namespace wns::probe::bus::tests;

ProbeBusStub::ProbeBusStub()
{
    receivedCounter = 0;
    providerName = "";
    filter = 0;
}

ProbeBusStub::~ProbeBusStub()
{
}

void
ProbeBusStub::onMeasurement(const wns::simulator::Time& timestamp,
                            const double& value,
                            const IContext& c)
{
    ++receivedCounter;
    receivedTimestamps.push_back(timestamp);
    receivedValues.push_back(value);

    std::stringstream s;
    s << c;
    lastContext = s.str();
}


void
ProbeBusStub::output()
{
}

bool
ProbeBusStub::accepts(const wns::simulator::Time& /*timestamp*/,
                      const IContext& reg)
{
    if(providerName == "")
    {
        return true;
    }

    if(reg.getInt(providerName) == filter)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void
ProbeBusStub::setFilter(std::string _providerName, long int _filter)
{
    providerName = _providerName;
    filter = _filter;
}
