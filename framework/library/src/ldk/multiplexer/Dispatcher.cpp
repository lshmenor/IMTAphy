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

#include <WNS/ldk/multiplexer/Dispatcher.hpp>
#include <WNS/ldk/multiplexer/OpcodeSetter.hpp>

using namespace wns::ldk;
using namespace wns::ldk::multiplexer;

STATIC_FACTORY_REGISTER_WITH_CREATOR(Dispatcher, wns::ldk::FunctionalUnit,
                                     "wns.multiplexer.Dispatcher",
                                     wns::ldk::FUNConfigCreator);

Dispatcher::Dispatcher(fun::FUN* fuNet, const pyconfig::View& _config) :
        CommandTypeSpecifier<OpcodeCommand>(fuNet),
        HasReceptor<RoundRobinReceptor>(),
        HasConnector<>(),
        HasDeliverer<OpcodeDeliverer>(),
        Processor<Dispatcher>(),
        Cloneable<Dispatcher>(),
        config(_config),
        opcodeSetters(),
        opcodeSize(config.get<int>("opcodeSize")),
        opcode(),
        logger(_config.get("logger"))
{
    getDeliverer()->setOpcodeProvider(this);
}


Dispatcher::~Dispatcher()
{
// Call destructor for all opcodeSetters
for(std::list<OpcodeSetter*>::const_iterator it = opcodeSetters.begin();
        it != opcodeSetters.end();)
    {
        OpcodeSetter* opcodeSetter;
        opcodeSetter = (*it);
        ++it;

        delete opcodeSetter;
    }

}


FunctionalUnit*
Dispatcher::whenConnecting()
{
    MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
    m << " adding new OpcodeSetter for opcode "
      << opcode;
    MESSAGE_END();

    OpcodeSetter *opcodeSetter = new OpcodeSetter(getFUN(), this, config, opcode++);

    opcodeSetters.push_front(opcodeSetter);

    opcodeSetter->getConnector()->add(this);
    this->getReceptor()->add(opcodeSetter);

    this->getDeliverer()->add(opcodeSetter);

    return opcodeSetter;
} // whenConnecting


void
Dispatcher::processOutgoing(const CompoundPtr& compound)
{
    OpcodeCommand* command = getCommand(compound->getCommandPool());

    MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
    m << " sending opcode "
      << command->peer.opcode;
    MESSAGE_END();
} // processOutgoing


void
Dispatcher::processIncoming(const CompoundPtr& compound)
{
    OpcodeCommand* command = getCommand(compound->getCommandPool());

    MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
    m << " delivering opcode "
      << command->peer.opcode;
    MESSAGE_END();
} // processIncoming


void
Dispatcher::calculateSizes(const CommandPool* commandPool, Bit& commandPoolSize, Bit& sduSize) const
{
    getFUN()->calculateSizes(commandPool, commandPoolSize, sduSize, this);

    commandPoolSize += opcodeSize;
} // calculateSizes



