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

#include <WNS/probe/bus/PassThroughProbeBus.hpp>

using namespace wns::probe::bus;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    PassThroughProbeBus,
    wns::probe::bus::ProbeBus,
    "PassThroughProbeBus",
    wns::PyConfigViewCreator);

PassThroughProbeBus::PassThroughProbeBus()
{
}

PassThroughProbeBus::PassThroughProbeBus(const wns::pyconfig::View&)
{
}

bool
PassThroughProbeBus::accepts(const wns::simulator::Time&, const IContext&)
{
    // We always accept everything
    return true;
}

void
PassThroughProbeBus::onMeasurement(const wns::simulator::Time&,
                              const double&,
                              const IContext&)
{
    // We do not do anything with a measurement
    // ProbeBus will forward to all attached servers
}

void
PassThroughProbeBus::output()
{
    // Nothing needs to be done here
}
