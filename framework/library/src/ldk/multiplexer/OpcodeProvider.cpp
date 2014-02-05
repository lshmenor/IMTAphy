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
#include <WNS/ldk/multiplexer/OpcodeProvider.hpp>

using namespace wns::ldk;
using namespace wns::ldk::multiplexer;

STATIC_FACTORY_REGISTER_WITH_CREATOR(OpcodeProvider, FunctionalUnit, "wns.multiplexer.OpcodeProvider", FUNConfigCreator);

OpcodeProvider::OpcodeProvider(fun::FUN* fuNet, const wns::pyconfig::View& config) :
        CommandTypeSpecifier<OpcodeCommand>(fuNet),
        HasReceptor<>(),
        HasConnector<>(),
        HasDeliverer<>(),
        Forwarding<OpcodeProvider>(),
        Cloneable<OpcodeProvider>(),

        opcodeSize(config.get<int>("opcodeSize")),
        logger("WNS", "Opcode")
{
} // OpcodeProvider


void
OpcodeProvider::calculateSizes(const CommandPool* commandPool, Bit& commandPoolSize, Bit& sduSize) const
{
    getFUN()->calculateSizes(commandPool, commandPoolSize, sduSize, this);

    commandPoolSize += opcodeSize;
} // calculateSizes



