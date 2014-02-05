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

#include <WNS/ldk/FirstServeConnector.hpp>
#include <WNS/ldk/FunctionalUnit.hpp>

using namespace wns::ldk;


bool
FirstServeConnector::hasAcceptor(const wns::ldk::CompoundPtr& compound)
{
    assure(!recs.empty(),
           "FirstServeConnector::hasAcceptor: Is called, even though no FU is available!");

    for(ExchangeContainer::const_iterator it = recs.begin();
        it != recs.end(); ++it)
    {
        if((*it)->isAccepting(compound))
            return true;
    }

    return false;
} //hasAcceptor


IConnectorReceptacle*
FirstServeConnector::getAcceptor(const wns::ldk::CompoundPtr& compound)
{
    assure(!recs.empty(),
           "FirstServeConnector::getAcceptor: Called getAccptor even though no FU available!");

    for(ExchangeContainer::const_iterator it = recs.begin();
        it != recs.end(); ++it)
    {
        if((*it)->isAccepting(compound))
            return (*it);
    }

    assure(0,
           "FirstServeConnector::getAcceptor: Is called,  but there is no accepting FUs!");
    return NULL;
} // getAcceptor

