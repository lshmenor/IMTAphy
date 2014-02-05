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

#include <WNS/ldk/command/FlowControl.hpp>

#include <WNS/ldk/fun/FUN.hpp>

#include <iostream>

using namespace wns::ldk;
using namespace wns::ldk::command;

STATIC_FACTORY_REGISTER_WITH_CREATOR(FlowControl, FunctionalUnit, "wns.command.FlowControl", FUNConfigCreator);

FlowControl::FlowControl(fun::FUN* fuNet, const wns::pyconfig::View& _config) :
        CommandTypeSpecifier<FlowControlCommand>(fuNet),
        HasReceptor<>(),
        HasConnector<>(),
        HasDeliverer<>(),
        Delayed<FlowControl>(),
        Cloneable<FlowControl>(),

        config(_config),
        toSend(CompoundPtr()),
        logger("WNS", config.get<std::string>("name"))
{
    friends.gate = 0;
} // FlowControl


void
FlowControl::open()
{
    MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
    m << " open";
    MESSAGE_END();

    friends.gate->setOutgoingState(tools::GateInterface::OPEN);
    send(FlowControlCommand::START);
} // open


void
FlowControl::close()
{
    MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
    m << " close";
    MESSAGE_END();

    friends.gate->setOutgoingState(tools::GateInterface::CLOSED);
    send(FlowControlCommand::STOP);
} // close


bool
FlowControl::hasCapacity() const
{
    return false;
} // hasCapacity


void
FlowControl::processOutgoing(const CompoundPtr& /* compound */)
{
    assure(false, "command::FlowControl never accepts compounds.");
} // processOutgoing


void
FlowControl::processIncoming(const CompoundPtr& compound)
{
    FlowControlCommand* command = getCommand(compound->getCommandPool());

    switch(command->peer.type)
    {
    case FlowControlCommand::START:
        MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
        m << " received START";
        MESSAGE_END();

        friends.gate->setOutgoingState(tools::GateInterface::OPEN);
        break;

    case FlowControlCommand::STOP:
        MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
        m << " received STOP";
        MESSAGE_END();

        friends.gate->setOutgoingState(tools::GateInterface::CLOSED);
        break;
    }
} // doOnData


const CompoundPtr
FlowControl::hasSomethingToSend() const
{
    return toSend;
} // hasSomethingToSend


CompoundPtr
FlowControl::getSomethingToSend()
{
    CompoundPtr it = toSend;
    toSend = CompoundPtr();
    return it;
} // getSomethingToSend


void
FlowControl::send(FlowControlCommand::FrameType type)
{
    toSend = CompoundPtr(getFUN()->createCompound());

    FlowControlCommand* command = activateCommand(toSend->getCommandPool());
    command->peer.type = type;

    tryToSend();
} // send


void
FlowControl::onFUNCreated()
{
    std::string gateName = config.get<std::string>("gateName");
    friends.gate = getFUN()->findFriend<tools::GateInterface*>(gateName);
} // onFUNCreated


void
FlowControl::calculateSizes(const CommandPool* commandPool, Bit& commandPoolSize, Bit& dataSize) const
{
    getFUN()->calculateSizes(commandPool, commandPoolSize, dataSize, this);

    commandPoolSize += 1;				// START / STOP bit
} // calculateSizes


CommandPool*
FlowControl::createReply(const CommandPool* /* original */) const
{
    MESSAGE_BEGIN(NORMAL, logger, m, "createReply");
    MESSAGE_END();

    CommandPool* commandPool = getFUN()->createCommandPool();

    return commandPool;
} // createReply



