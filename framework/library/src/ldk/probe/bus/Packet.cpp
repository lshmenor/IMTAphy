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

#include <WNS/ldk/probe/bus/Packet.hpp>
#include <WNS/probe/bus/ContextProvider.hpp>
#include <WNS/probe/bus/utils.hpp>
#include <WNS/ldk/Layer.hpp>

using namespace wns::ldk;
using namespace wns::ldk::probe::bus;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    wns::ldk::probe::bus::Packet,
    wns::ldk::probe::Probe,
    "wns.probe.PacketProbeBus",
    FUNConfigCreator);

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    wns::ldk::probe::bus::Packet,
    wns::ldk::FunctionalUnit,
    "wns.probe.PacketProbeBus",
    FUNConfigCreator);

Packet::Packet(fun::FUN* fuNet, const wns::pyconfig::View& config) :
    fu::Plain<Packet, PacketCommand>(fuNet),
    Forwarding<Packet>(),
    logger(config.get("logger"))
{
    // read the localIDs from the config
    wns::probe::bus::ContextProviderCollection registry(&fuNet->getLayer()->getContextProviderCollection());
    for (int ii = 0; ii<config.len("localIDs.keys()"); ++ii)
    {
        std::string key = config.get<std::string>("localIDs.keys()",ii);
        unsigned long int value  = config.get<unsigned long int>("localIDs.values()",ii);
        registry.addProvider(wns::probe::bus::contextprovider::Constant(key, value));
        MESSAGE_SINGLE(VERBOSE, logger, "Using Local IDName '"<<key<<"' with value: "<<value);
    }

    delayIncoming = wns::probe::bus::collector(registry, config, "incomingDelayProbeName");
    delayOutgoing = wns::probe::bus::collector(registry, config, "outgoingDelayProbeName");
    throughput = wns::probe::bus::collector(registry, config, "incomingThroughputProbeName");
    sizeOutgoing = wns::probe::bus::collector(registry, config, "outgoingSizeProbeName");
    sizeIncoming = wns::probe::bus::collector(registry, config, "incomingSizeProbeName");
} // Packet

Packet::~Packet()
{
}

void
Packet::processOutgoing(const CompoundPtr& compound)
{
    PacketCommand* command = activateCommand(compound->getCommandPool());
    const simTimeType now = wns::simulator::getEventScheduler()->getTime();

    // record that we processed this compound
    long int compoundLength = getLengthInBits(compound);
    if(sizeOutgoing)
        sizeOutgoing->put(compound, compoundLength);

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
    simTimeType now = wns::simulator::getEventScheduler()->getTime();
    long int compoundLength = getLengthInBits(compound);

    MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
    m << " sent " << command->magic.t
      << " received " << now
      << " delay " << now - command->magic.t
      << " length " << compoundLength;
    MESSAGE_END();

    double travelTime = now - command->magic.t;
    assure(travelTime > 0.0, "packet with no travel time.");

    if(throughput)
        throughput->put(compound, compoundLength / travelTime);

    // delay/size probes
    if(delayIncoming)
        delayIncoming->put(compound, travelTime);

    if(delayOutgoing)
        command->magic.probingFU->delayOutgoing->put(compound, travelTime);

    if(sizeIncoming)
        sizeIncoming->put(compound, compoundLength);

    Forwarding<Packet>::processIncoming(compound);
} // processIncoming



