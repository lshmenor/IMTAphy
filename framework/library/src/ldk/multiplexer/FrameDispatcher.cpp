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

#include <WNS/ldk/multiplexer/FrameDispatcher.hpp>
#include <WNS/ldk/multiplexer/OpcodeSetter.hpp>

using namespace wns::ldk;
using namespace wns::ldk::multiplexer;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    FrameDispatcher,
    wns::ldk::FunctionalUnit,
    "wns.multiplexer.FrameDispatcher",
    wns::ldk::FUNConfigCreator);

FrameDispatcher::FrameDispatcher(fun::FUN* fuNet, const pyconfig::View& _config) :
        CommandTypeSpecifier<OpcodeCommand>(fuNet),
        HasReceptor<RoundRobinReceptor>(),
        HasConnector<RoundRobinConnector>(),
        HasDeliverer<OpcodeDeliverer>(),
        Processor<FrameDispatcher>(),
        Cloneable<FrameDispatcher>(),
        config(_config),
        opcodeSize(config.get<int>("opcodeSize")),
        opcode(),
        pending(),
        logger(_config.get("logger"))
{
    getDeliverer()->setOpcodeProvider(this);
}


FunctionalUnit*
FrameDispatcher::whenConnecting()
{
    MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
    m << " adding new OpcodeSetter for opcode "
      << opcode;
    MESSAGE_END();

    assure(!pending || downConnected || upConnected,
           "unit above dispatcher without corresponding unit below.");

    pending = new OpcodeSetter(getFUN(), this, config, opcode++);
    downConnected = false;
    upConnected = false;

    this->getReceptor()->add(pending);
    this->getDeliverer()->add(pending);

    return pending;
} // whenConnecting


void
FrameDispatcher::doDownConnect(FunctionalUnit* that, const std::string& srcPort, const std::string& dstPort)
{
    assure(pending, "no unit above dispatcher waiting for a connect from below.");
    assure(!downConnected, "you may only downconnect once.");

        pending->downConnect(that, srcPort, dstPort);
    downConnected = true;
} // _downConnect


void
FrameDispatcher::doUpConnect(FunctionalUnit* that, const std::string& srcPort, const std::string& dstPort)
{
    assure(pending, "no unit above dispatcher waiting for a connect from below.");
    assure(!upConnected, "you may only upconnect once.");

        pending->upConnect(that, srcPort, dstPort);
    upConnected = true;
} // _upConnect


void
FrameDispatcher::processOutgoing(const CompoundPtr& compound)
{
    OpcodeCommand* command = getCommand(compound->getCommandPool());

    MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
    m << " sending opcode "
      << command->peer.opcode;
    MESSAGE_END();
} // processOutgoing


void
FrameDispatcher::processIncoming(const CompoundPtr& compound)
{
    OpcodeCommand* command = getCommand(compound->getCommandPool());

    MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
    m << " delivering opcode "
      << command->peer.opcode;
    MESSAGE_END();
} // processIncoming


void
FrameDispatcher::calculateSizes(const CommandPool* commandPool, Bit& commandPoolSize, Bit& sduSize) const
{
    getFUN()->calculateSizes(commandPool, commandPoolSize, sduSize, this);

    commandPoolSize += opcodeSize;
} // calculateSizes


