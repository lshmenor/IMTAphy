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

#include <WNS/ldk/probe/Window.hpp>
#include <WNS/ldk/Layer.hpp>

using namespace wns::ldk;
using namespace wns::ldk::probe;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    Window,
    Probe,
    "wns.probe.Window",
    FUNConfigCreator);

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    Window,
    FunctionalUnit,
    "wns.probe.Window",
    FUNConfigCreator);

Window::Window(fun::FUN* fuNet, const wns::pyconfig::View& config) :
    Probe(),
    fu::Plain<Window, WindowCommand>(fuNet),
    Forwarding<Window>(),
    events::PeriodicTimeout(),

    sampleInterval(config.get<wns::simulator::Time>("sampleInterval")),

    cumulatedBitsIncoming(config.get<wns::simulator::Time>("windowSize")),
    cumulatedPDUsIncoming(config.get<wns::simulator::Time>("windowSize")),
    cumulatedBitsOutgoing(config.get<wns::simulator::Time>("windowSize")),
    cumulatedPDUsOutgoing(config.get<wns::simulator::Time>("windowSize")),
    aggregatedThroughputInBit(config.get<wns::simulator::Time>("windowSize")),
    aggregatedThroughputInPDUs(config.get<wns::simulator::Time>("windowSize")),

    //logger("WNS", config.get<std::string>("name"))
    logger(config.get("logger"))
{
    assure(
        sampleInterval <= config.get<wns::simulator::Time>("windowSize"),
        "sampleInterval length must be shorter or equal to windowSize");

    // this is for the new probe bus
    wns::probe::bus::ContextProviderCollection* cpcParent = &fuNet->getLayer()->getContextProviderCollection();

    wns::probe::bus::ContextProviderCollection cpc(cpcParent);

    for (int ii = 0; ii<config.len("localIDs.keys()"); ++ii)
    {
        std::string key = config.get<std::string>("localIDs.keys()",ii);
        int value  = config.get<int>("localIDs.values()",ii);
        cpc.addProvider(wns::probe::bus::contextprovider::Constant(key, value));
        MESSAGE_SINGLE(VERBOSE, logger, "Using Local IDName '"<<key<<"' with value: "<<value);
    }

    bitsIncomingBus = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(cpc, config.get<std::string>("incomingBitThroughputProbeName")));
    compoundsIncomingBus = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(cpc, config.get<std::string>("incomingCompoundThroughputProbeName")));
    bitsOutgoingBus = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(cpc, config.get<std::string>("outgoingBitThroughputProbeName")));
    compoundsOutgoingBus = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(cpc, config.get<std::string>("outgoingCompoundThroughputProbeName")));
    bitsAggregatedBus = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(cpc, config.get<std::string>("aggregatedBitThroughputProbeName")));
    compoundsAggregatedBus = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(cpc, config.get<std::string>("aggregatedCompoundThroughputProbeName")));


    // start after first window is full, then sample every sampleInterval seconds
    this->startPeriodicTimeout(sampleInterval, config.get<wns::simulator::Time>("windowSize"));
} // Window

Window::~Window()
{
}

void
Window::processOutgoing(const CompoundPtr& compound)
{
    WindowCommand* command = this->activateCommand(compound->getCommandPool());
    command->magic.probingFU = this;

    Bit commandPoolSize;
    Bit dataSize;
    this->getFUN()->calculateSizes(compound->getCommandPool(), commandPoolSize, dataSize, this);
    const long int compoundLength = commandPoolSize + dataSize;

    MESSAGE_BEGIN(NORMAL, logger, m, this->getFUN()->getName());
    m << " outgoing"
      << " length " << compoundLength;
    MESSAGE_END();

    this->cumulatedBitsOutgoing.put(compoundLength);
    this->cumulatedPDUsOutgoing.put(1);

    Forwarding<Window>::processOutgoing(compound);
} // processOutgoing


void
Window::processIncoming(const CompoundPtr& compound)
{
    Bit commandPoolSize;
    Bit dataSize;
    this->getFUN()->calculateSizes(compound->getCommandPool(), commandPoolSize, dataSize, this);
    const long int compoundLength = commandPoolSize + dataSize;

    MESSAGE_BEGIN(NORMAL, logger, m, this->getFUN()->getName());
    m << " incoming"
      << " length " << compoundLength;
    MESSAGE_END();

    this->cumulatedBitsIncoming.put(compoundLength);
    this->cumulatedPDUsIncoming.put(1);

    WindowCommand* command = this->getCommand(compound->getCommandPool());
    command->magic.probingFU->aggregatedThroughputInBit.put(compoundLength);
    command->magic.probingFU->aggregatedThroughputInPDUs.put(1);

    Forwarding<Window>::processIncoming(compound);
} // processIncoming


void
Window::periodically()
{
    this->bitsOutgoingBus->put(this->cumulatedBitsOutgoing.getPerSecond());
    this->compoundsOutgoingBus->put(this->cumulatedPDUsOutgoing.getPerSecond());
    this->bitsIncomingBus->put(this->cumulatedBitsIncoming.getPerSecond());
    this->compoundsIncomingBus->put(this->cumulatedPDUsIncoming.getPerSecond());
    this->bitsAggregatedBus->put(this->aggregatedThroughputInBit.getPerSecond());
    this->compoundsAggregatedBus->put(this->aggregatedThroughputInPDUs.getPerSecond());
} // periodically



