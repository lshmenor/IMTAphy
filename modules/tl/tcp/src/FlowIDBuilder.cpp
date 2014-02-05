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

#include <TCP/FlowIDBuilder.hpp>
#include <TCP/UpperConvergence.hpp>

#include <WNS/ldk/FlowSeparator.hpp>


using namespace tcp;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	FlowIDBuilder,
	wns::ldk::KeyBuilder,
	"tcp.FlowIDBuilder",
	wns::ldk::FUNConfigCreator);

FlowIDBuilder::FlowIDBuilder(wns::ldk::fun::FUN* _fun,
			     const wns::pyconfig::View& _config) :
	config(_config),
    fun(_fun),
    tcpHeaderReader(NULL)
{
}

FlowIDKey::FlowIDKey(const wns::service::tl::FlowID& flowID):
    flowID_(flowID)
{
}

bool
FlowIDKey::operator<(const Key& _other) const
{
    assureType(&_other, const FlowIDKey*);
    const FlowIDKey* other = static_cast<const FlowIDKey*>(&_other);

    return flowID_ < other->flowID_;
}

std::string
FlowIDKey::str() const
{
    return flowID_.doToString();
}

void
FlowIDBuilder::onFUNCreated()
{
    tcpHeaderReader = fun->getCommandReader("tcp.tcpHeader");
    assure(tcpHeaderReader, "No reader for the TCP Header available!");
}

wns::ldk::ConstKeyPtr
FlowIDBuilder::operator()(const wns::ldk::CompoundPtr& _compound, int /*_direction*/) const
{
    assure(tcpHeaderReader, "No reader for the TCP Header available!");
    TCPCommand* tcpHeader = tcpHeaderReader->readCommand<TCPCommand>(
        _compound->getCommandPool());

    wns::ldk::ConstKeyPtr key = wns::ldk::ConstKeyPtr(new FlowIDKey(tcpHeader->peer.flowID));

    return key;
}
