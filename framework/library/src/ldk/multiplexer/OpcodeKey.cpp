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

#include <WNS/ldk/multiplexer/OpcodeKey.hpp>

#include <WNS/ldk/multiplexer/OpcodeProvider.hpp>

using namespace wns::ldk;
using namespace wns::ldk::multiplexer;

STATIC_FACTORY_REGISTER_WITH_CREATOR(OpcodeKeyBuilder, KeyBuilder, "multiplex.Opcode", FUNConfigCreator);


OpcodeKey::OpcodeKey(const OpcodeKeyBuilder* builder, const CompoundPtr& compound)
{
    OpcodeCommand* command = builder->friends.opcodeProvider->getCommand(compound->getCommandPool());
    opcode = command->peer.opcode;
}


bool
OpcodeKey::operator < (const Key& _other) const
{
    assure(dynamic_cast<const OpcodeKey*>(&_other), "Comparing Keys of different types.");

    const OpcodeKey* other = static_cast<const OpcodeKey*>(&_other);
    return opcode < other->opcode;
} // <


std::string
OpcodeKey::str() const
{
    std::stringstream ss;
    ss << "opcode:" << opcode;
    return ss.str();
} // str


OpcodeKeyBuilder::OpcodeKeyBuilder(const fun::FUN* _fuNet, const pyconfig::View& _config) :
        fuNet(_fuNet),
        config(_config)
{
}


void
OpcodeKeyBuilder::onFUNCreated()
{
    friends.opcodeProvider = fuNet->findFriend<OpcodeProvider*>(config.get<std::string>("opcode"));
} // onFUNCreated


wns::ldk::ConstKeyPtr
OpcodeKeyBuilder::operator() (const CompoundPtr& compound, int /* direction */) const
{
    return ConstKeyPtr(new OpcodeKey(this, compound));
}



