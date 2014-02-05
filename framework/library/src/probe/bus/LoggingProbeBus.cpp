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

#include <WNS/probe/bus/LoggingProbeBus.hpp>

#include <iomanip>

using namespace wns::probe::bus;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    LoggingProbeBus,
    wns::probe::bus::ProbeBus,
    "LoggingProbeBus",
    wns::PyConfigViewCreator);

LoggingProbeBus::LoggingProbeBus(const wns::pyconfig::View& pyco):
    logger_(pyco.get<wns::pyconfig::View>("logger"))
{
}

LoggingProbeBus::~LoggingProbeBus()
{
}

bool
LoggingProbeBus::accepts(const wns::simulator::Time&, const IContext&)
{
    return true;
}

void
LoggingProbeBus::onMeasurement(const wns::simulator::Time&,
                               const double& aValue,
                               const IContext& reg)
{
    MESSAGE_BEGIN(NORMAL, logger_, m, "");
    m << std::resetiosflags(std::ios::fixed)
      << std::resetiosflags(std::ios::scientific)
      << std::resetiosflags(std::ios::right)
      << std::setiosflags(std::ios::right)
      << std::setiosflags(std::ios::fixed)
      << std::setiosflags(std::ios::dec)
      << std::setprecision(7)
      << std::setw(11)
	  << aValue << " "
      << reg;
    MESSAGE_END();
}

void
LoggingProbeBus::output()
{
}


