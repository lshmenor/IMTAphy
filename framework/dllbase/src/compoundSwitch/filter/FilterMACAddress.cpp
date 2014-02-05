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

#include <DLL/compoundSwitch/filter/FilterMACAddress.hpp>
#include <DLL/UpperConvergence.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/Assure.hpp>
#include <WNS/logger/Logger.hpp>

using namespace dll::compoundSwitch::filter;


STATIC_FACTORY_REGISTER_WITH_CREATOR(FilterMACAddress,
                                    dll::compoundSwitch::Filter,
                                    "dll.compoundSwitch.filter.FilterMACAddress",
                                    dll::compoundSwitch::CompoundSwitchConfigCreator);

FilterMACAddress::FilterMACAddress(CompoundSwitch* compoundSwitch,
                                wns::pyconfig::View& config) :
    Filter(compoundSwitch, config),
    ucCommandName_(config.get<std::string>("upperConvergenceCommandName")),
    filterAddress_(config.get<wns::service::dll::UnicastAddress>("filterMACAddress")),
    filterTarget_(config.get<bool>("filterTarget"))
{
} // Filter



FilterMACAddress::~FilterMACAddress()
{
} // ~Filter



void
FilterMACAddress::onFUNCreated()
{
}

bool
FilterMACAddress::filter(const wns::ldk::CompoundPtr& compound) const
{
    dll::UpperCommand* uc = compoundSwitch_->getFUN()->getCommandReader(ucCommandName_)->readCommand<dll::UpperCommand>(compound->getCommandPool());

    if(this->filterTarget_)
    {
        if(uc->peer.targetMACAddress == filterAddress_)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        if(uc->peer.sourceMACAddress == filterAddress_)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}
