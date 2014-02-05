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

#include <WNS/probe/bus/ContextFilterProbeBus.hpp>

#include <sstream>

using namespace wns::probe::bus;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    ContextFilterProbeBus,
    wns::probe::bus::ProbeBus,
    "ContextFilterProbeBus",
    wns::PyConfigViewCreator);

ContextFilterProbeBus::ContextFilterProbeBus(const wns::pyconfig::View& _pyco):
    idName(_pyco.get<std::string>("idName")),
    values()
{
    for (int ii = 0; ii < _pyco.len("idValues"); ++ii)
    {
        int value = _pyco.get<int>("idValues",ii);
        values.insert(value);
    }
}

ContextFilterProbeBus::~ContextFilterProbeBus()
{
}

void
ContextFilterProbeBus::output()
{
    // We do not do any output ourself
}

void
ContextFilterProbeBus::onMeasurement(const wns::simulator::Time&,
                                     const double&,
                                     const IContext&)
{
    // Nothing done here, we only filter within accepts
}

bool
ContextFilterProbeBus::accepts(const wns::simulator::Time&,
                               const IContext& reg)
{
    try
    {
        return values.find( reg.getInt(idName) ) != values.end();
    }
    catch (context::NotFound)
    {
        return false;
    }
}
