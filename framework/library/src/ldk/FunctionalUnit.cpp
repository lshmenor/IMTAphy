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

#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/ldk/Receptor.hpp>
#include <WNS/ldk/Deliverer.hpp>
#include <WNS/ldk/fun/FUN.hpp>
#include <WNS/ldk/LinkHandlerInterface.hpp>

using namespace wns::ldk;

void
FunctionalUnit::doConnect(FunctionalUnit* that, const std::string& srcPort, const std::string& dstPort)
{
    doDownConnect(that, srcPort, dstPort);
    doUpConnect(that, srcPort, dstPort);
} // _connect

void
FunctionalUnit::doDownConnect(FunctionalUnit* that, const std::string& srcPort, const std::string& dstPort)
{
    this->getFromConnectorRegistry(srcPort)->add(that->getFromConnectorReceptacleRegistry(dstPort));
    that->getFromReceptorRegistry(dstPort)->add(this->getFromReceptorReceptacleRegistry(srcPort));
} // _downConnect

void
FunctionalUnit::doUpConnect(FunctionalUnit* that, const std::string& srcPort, const std::string& dstPort)
{
    that->getFromDelivererRegistry(dstPort)->add(this->getFromDelivererReceptacleRegistry(srcPort));
} // _upConnect

