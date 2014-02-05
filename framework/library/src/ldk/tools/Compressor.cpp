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

#include <WNS/ldk/tools/Compressor.hpp>

using namespace wns::ldk::tools;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    Compressor,
    wns::ldk::FunctionalUnit,
    "wns.ldk.tools.Compressor",
    wns::ldk::FUNConfigCreator);

Compressor::Compressor(fun::FUN* fun, const wns::pyconfig::View& config):
    wns::ldk::fu::Plain<Compressor, wns::ldk::EmptyCommand>(fun),
    reduction_(config.get<Bit>("reduction")),
    byteAlign_(config.get<bool>("byteAlign"))
{
}

void Compressor::calculateSizes(const CommandPool* commandPool, Bit& commandPoolSize, Bit& dataSize) const
{
    getFUN()->getProxy()->calculateSizes(commandPool, commandPoolSize, dataSize, this);
    Command* command = getFUN()->getProxy()->getCommand(commandPool, this);

    dataSize -= reduction_;
    if(byteAlign_ && dataSize % 8 != 0)
        dataSize += (8 - dataSize % 8);

    assure(dataSize >= 0, "Compressor has reduced size of payload data below 0");
}


void
Compressor::doSendData( const CompoundPtr& compound )
{
    activateCommand(compound->getCommandPool());
    getConnector()->getAcceptor( compound )->sendData(compound);
}

void
Compressor::doOnData( const CompoundPtr& compound )
{
    getDeliverer()->getAcceptor( compound )->onData( compound );
}

bool
Compressor::doIsAccepting(const CompoundPtr& compound) const
{
    CompoundPtr copy = compound->copy();
    activateCommand( copy->getCommandPool() );
    return getConnector()->hasAcceptor(copy);
}

void
Compressor::doWakeup()
{
    getReceptor()->wakeup();
}
