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

#include <WNS/ldk/SingleConnector.hpp>

using namespace wns::ldk;

SingleConnector::SingleConnector() :
    SingleLink<IConnectorReceptacle>()
{
}

SingleConnector::~SingleConnector()
{
}

bool
SingleConnector::hasAcceptor(const CompoundPtr& compound)
{
    assure(getRec() != NULL, "Called hasAccecptor even though no FU available");
    return getRec()->isAccepting(compound);
}


IConnectorReceptacle*
SingleConnector::getAcceptor(const CompoundPtr&
#ifndef NDEBUG
                             compound
#endif
    )
{
    assure(getRec() != NULL, "Called getAccptor even though no FU available");
    assure(getRec()->isAccepting(compound), "Called getAcceptor although the FU '" << this->getRec()->getFU()->toString() << "' is not accepting");

    return getRec();
}

