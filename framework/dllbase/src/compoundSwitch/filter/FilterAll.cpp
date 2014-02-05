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

#include <DLL/compoundSwitch/filter/FilterAll.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/Assure.hpp>

using namespace dll::compoundSwitch::filter;


STATIC_FACTORY_REGISTER_WITH_CREATOR(FilterAll,
                                    dll::compoundSwitch::Filter,
                                    "dll.compoundSwitch.filter.FilterAll",
                                    dll::compoundSwitch::CompoundSwitchConfigCreator);

FilterAll::FilterAll(CompoundSwitch* compoundSwitch, wns::pyconfig::View& config) :
    Filter(compoundSwitch, config)
{
} // FilterAll



FilterAll::~FilterAll()
{

} // ~FilterAll


bool
FilterAll::filter(const wns::ldk::CompoundPtr&) const
{
    return true;
}
