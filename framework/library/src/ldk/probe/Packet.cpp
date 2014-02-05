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

#include <WNS/ldk/probe/Packet.hpp>
#include <WNS/ldk/Layer.hpp>

using namespace wns::ldk;
using namespace wns::ldk::probe;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    Packet,
    Probe,
    "wns.probe.Packet",
    FUNConfigCreator);

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    Packet,
    FunctionalUnit,
    "wns.probe.Packet",
    FUNConfigCreator);

Packet::Packet(fun::FUN* fuNet, const wns::pyconfig::View& config) :
    fu::Plain<Packet, PacketCommand>(fuNet),
    Forwarding<Packet>(),
    logger(config.get("logger"))
{
    // This is for the new probe bus
    // Note: the if part here below and in processIncoming can be removed as
    // soon as the old probes above are removed.
    wns::probe::bus::ContextProviderCollection* cpcParent = &fuNet->getLayer()->getContextProviderCollection();

    wns::probe::bus::ContextProviderCollection cpc(cpcParent);
    for (int ii = 0; ii<config.len("localIDs.keys()"); ++ii)
    {
        std::string key = config.get<std::string>("localIDs.keys()",ii);
        int value  = config.get<int>("localIDs.values()",ii);
        cpc.addProvider(wns::probe::bus::contextprovider::Constant(key, value));
        MESSAGE_SINGLE(VERBOSE, logger, "Using Local IDName '"<<key<<"' with value: "<<value);
    }

    if (!config.isNone("incomingDelayProbeName"))
        delayIncomingBus = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(cpc, config.get<std::string>("incomingDelayProbeName")));
    if (!config.isNone("outgoingDelayProbeName"))
        delayOutgoingBus = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(cpc, config.get<std::string>("outgoingDelayProbeName")));
    if (!config.isNone("incomingThroughputProbeName"))
        throughputBus = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(cpc, config.get<std::string>("incomingThroughputProbeName")));
    if (!config.isNone("outgoingSizeProbeName"))
        sizeOutgoingBus = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(cpc, config.get<std::string>("outgoingSizeProbeName")));
    if (!config.isNone("incomingSizeProbeName"))
        sizeIncomingBus = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(cpc, config.get<std::string>("incomingSizeProbeName")));
} // Packet

Packet::~Packet()
{
}

void
Packet::processOutgoing(const CompoundPtr& compound)
{
    PacketCommand* command = activateCommand(compound->getCommandPool());
    const wns::simulator::Time now = wns::simulator::getEventScheduler()->getTime();

    // record that we processed this compound
    long int compoundLength = getLengthInBits(compound);
    if(sizeOutgoingBus)
        sizeOutgoingBus->put(compound, compoundLength);

    // record outgoing timestamp for delay probe
    command->magic.t = now;
    command->magic.probingFU = this;

    MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
    m << " sent " << command->magic.t << ", size " << compoundLength;
    MESSAGE_END();

    Forwarding<Packet>::processOutgoing(compound);
} // processOutgoing


void
Packet::processIncoming(const CompoundPtr& compound)
{
    PacketCommand* command = getCommand(compound->getCommandPool());
    wns::simulator::Time now = wns::simulator::getEventScheduler()->getTime();
    long int compoundLength = getLengthInBits(compound);

    MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
    m << " sent " << command->magic.t
      << " received " << now
      << " delay " << now - command->magic.t
      << " length " << compoundLength;
    MESSAGE_END();

    double travelTime = now - command->magic.t;
    assure(travelTime > 0.0, "packet with no travel time.");

    if(throughputBus)
        throughputBus->put(compound, compoundLength / travelTime);

    // delay/size probes
    if(delayIncomingBus)
        delayIncomingBus->put(compound, travelTime);

    if(delayOutgoingBus)
        command->magic.probingFU->delayOutgoingBus->put(compound, travelTime);

    if(sizeIncomingBus)
        sizeIncomingBus->put(compound, compoundLength);

    Forwarding<Packet>::processIncoming(compound);
} // processIncoming



