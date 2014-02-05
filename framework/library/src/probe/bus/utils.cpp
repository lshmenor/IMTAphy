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

#include <WNS/probe/bus/utils.hpp>

using namespace wns::probe::bus;

bool
wns::probe::bus::lookup(
    const wns::pyconfig::View& config,
    std::string varName)
{
    if(!config.knows(varName))
        return false;

    if(config.isNone(varName))
        return false;

    if(!config.get<bool>(varName))
        return false;

    return true;
} // lookup


ContextCollectorPtr
wns::probe::bus::collector(
	const ContextProviderCollection& registry,
	const pyconfig::View& config,
	std::string varName)
{
    ContextCollector* cc = NULL;

    if (lookup(config, varName) == false)
	{
		wns::Exception up;
		up << "WARNING: Config has no valid member named " << varName
		   << ", config is:\n" <<config
		   << "\nNow returning dummy ContextCollector ...\n";
		throw up;
	}

	cc = new ContextCollector(registry,
							  config.get<std::string>(varName));
	return ContextCollectorPtr( cc );
} // collector

