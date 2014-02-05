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

#include <WNS/ldk/multiplexer/OpcodeSetter.hpp>

#include <WNS/pyconfig/Parser.hpp>

#include <sstream>

using namespace wns::ldk;
using namespace wns::ldk::multiplexer;

STATIC_FACTORY_REGISTER_WITH_CREATOR(OpcodeSetter, wns::ldk::FunctionalUnit,
                     "wns.multiplexer.OpcodeSetter",
                     wns::ldk::FUNConfigCreator);

OpcodeSetter::OpcodeSetter(fun::FUN* fuNet, const pyconfig::View& _config) :
        CommandTypeSpecifier<>(fuNet),
        HasReceptor<>(),
        HasConnector<>(),
        HasDeliverer<>(),
        Processor<OpcodeSetter>(),
        Cloneable<OpcodeSetter>(),

        config(_config),
        opcode(config.get<int>("opcode")),

        logger("WNS", "OpcodeSetter")
{
    friends.opcodeProvider = 0;
}


OpcodeSetter::OpcodeSetter(fun::FUN* fuNet, FunctionalUnit* _opcodeProvider, const pyconfig::View& _config, int _opcode) :
        CommandTypeSpecifier<>(fuNet),
        HasReceptor<>(),
        HasConnector<>(),
        HasDeliverer<>(),
        Processor<OpcodeSetter>(),
        Cloneable<OpcodeSetter>(),

        config(_config),
        opcode(_opcode),

        logger(_config.get("opcodeLogger"))
{
    friends.opcodeProvider = _opcodeProvider;

    // give each opcodeProvider a unique rolename
    std::stringstream commandName;
    commandName << friends.opcodeProvider->getName() << "_opcode_" << opcode;

    setName(commandName.str());
}


void
OpcodeSetter::onFUNCreated()
{
    if(friends.opcodeProvider)
        return;

    std::string name = config.get<std::string>("opcodeProvider");
    friends.opcodeProvider = getFUN()->findFriend<OpcodeProvider*>(name);
    assure(friends.opcodeProvider, "required friend not found.");
} // onFUNCreated


void
OpcodeSetter::processOutgoing(const CompoundPtr& compound)
{
    OpcodeCommand* command =
        dynamic_cast<OpcodeCommand*>(friends.opcodeProvider->activateCommand(compound->getCommandPool()));
    command->peer.opcode = opcode;

    MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
    m << " this is OpcodeSetter writing opcode " << opcode;
    MESSAGE_END();
} // processOutgoing


void
OpcodeSetter::processIncoming(const CompoundPtr& /* compound */)
{
    MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
    m << " this is OpcodeSetter receiving opcode " << opcode;
    MESSAGE_END();
} // processIncoming



