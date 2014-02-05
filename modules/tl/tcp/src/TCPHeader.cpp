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

#include <TCP/TCPHeader.hpp>

using namespace tcp;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    TCPHeader,
    wns::ldk::FunctionalUnit,
    "tcp.tcpHeader",
    wns::ldk::FUNConfigCreator);


TCPHeader::TCPHeader(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& pyco):
    wns::ldk::fu::Plain<TCPHeader, TCPCommand>(fun),
    headerSize(pyco.get<Bit>("headerSize"))
{
}

wns::ldk::CommandPool*
TCPHeader::createReply(const wns::ldk::CommandPool* original) const
{
    TCPCommand* originalCommand = getCommand(original);

    wns::ldk::CommandPool* reply = getFUN()->getProxy()->createCommandPool();
    TCPCommand* replyCommand = activateCommand(reply);

    /**
	 * Just return the flow id getting from the ARQ FU.
	 * The source/destination addresses/ports are swaped
	 * in lowerConvergence (bottom FU of the TCP's FUN).
	 */
    replyCommand->peer.flowID = originalCommand->peer.flowID;

    return reply;
}

void
TCPHeader::calculateSizes(const wns::ldk::CommandPool* commandPool,
                         Bit& commandPoolSize, Bit& dataSize) const
{
    getFUN()->calculateSizes(commandPool, commandPoolSize, dataSize, this);

    commandPoolSize += headerSize;
}
