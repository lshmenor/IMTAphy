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

#include <WNS/ldk/probe/bus/Window.hpp>
#include <WNS/ldk/Layer.hpp>
#include <WNS/probe/bus/utils.hpp>

using namespace wns::ldk;
using namespace wns::ldk::probe::bus;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    wns::ldk::probe::bus::Window,
    wns::ldk::probe::Probe,
    "wns.probe.WindowProbeBus",
    FUNConfigCreator);

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    wns::ldk::probe::bus::Window,
    wns::ldk::FunctionalUnit,
    "wns.probe.WindowProbeBus",
    FUNConfigCreator);

Window::Window(fun::FUN* fuNet, const wns::pyconfig::View& config) :
    Probe(),
    fu::Plain<Window, WindowCommand>(fuNet),
    Forwarding<Window>(),
    events::PeriodicTimeout(),

    sampleInterval(config.get<simTimeType>("sampleInterval")),

    bitsIncoming(),
    compoundsIncoming(),
    bitsOutgoing(),
    compoundsOutgoing(),
    bitsAggregated(),
    compoundsAggregated(),
    relativeBitsGoodput(),
    relativeCompoundsGoodput(),

    cumulatedBitsIncoming(config.get<simTimeType>("windowSize")),
    cumulatedPDUsIncoming(config.get<simTimeType>("windowSize")),
    cumulatedBitsOutgoing(config.get<simTimeType>("windowSize")),
    cumulatedPDUsOutgoing(config.get<simTimeType>("windowSize")),
    aggregatedThroughputInBit(config.get<simTimeType>("windowSize")),
    aggregatedThroughputInPDUs(config.get<simTimeType>("windowSize")),

    //logger("WNS", config.get<std::string>("name"))
    logger(config.get("logger"))
{
    assure(
        sampleInterval <= config.get<simTimeType>("windowSize"),
        "sampleInterval length must be shorter or equal to windowSize");

    // read the localContext from the config
    wns::probe::bus::ContextProviderCollection localContext(&fuNet->getLayer()->getContextProviderCollection());
    for (int ii = 0; ii<config.len("localIDs.keys()"); ++ii)
    {
        std::string key = config.get<std::string>("localIDs.keys()",ii);
        unsigned long int value  = config.get<unsigned long int>("localIDs.values()",ii);
        localContext.addProvider(wns::probe::bus::contextprovider::Constant(key, value));
        MESSAGE_SINGLE(VERBOSE, logger, "Using Local IDName '"<<key<<"' with value: "<<value);
    }

    this->bitsIncoming = wns::probe::bus::collector(localContext, config, "incomingBitThroughputProbeName");
    this->compoundsIncoming = wns::probe::bus::collector(localContext, config, "incomingCompoundThroughputProbeName");
    this->bitsOutgoing = wns::probe::bus::collector(localContext, config, "outgoingBitThroughputProbeName");
    this->compoundsOutgoing = wns::probe::bus::collector(localContext, config, "outgoingCompoundThroughputProbeName");
    this->bitsAggregated = wns::probe::bus::collector(localContext, config, "aggregatedBitThroughputProbeName");
    this->compoundsAggregated = wns::probe::bus::collector(localContext, config, "aggregatedCompoundThroughputProbeName");
    this->relativeBitsGoodput = wns::probe::bus::collector(localContext, config, "relativeBitsGoodputProbeName");
    this->relativeCompoundsGoodput = wns::probe::bus::collector(localContext, config, "relativeCompoundsGoodputProbeName");

    // start after first window is full, then sample every sampleInterval seconds
    this->startPeriodicTimeout(sampleInterval, config.get<simTimeType>("windowSize"));
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
    this->bitsOutgoing->put(this->cumulatedBitsOutgoing.getPerSecond());
    this->compoundsOutgoing->put(this->cumulatedPDUsOutgoing.getPerSecond());
    this->bitsIncoming->put(this->cumulatedBitsIncoming.getPerSecond());
    this->compoundsIncoming->put(this->cumulatedPDUsIncoming.getPerSecond());
    this->bitsAggregated->put(this->aggregatedThroughputInBit.getPerSecond());
    this->compoundsAggregated->put(this->aggregatedThroughputInPDUs.getPerSecond());

	if(this->cumulatedBitsOutgoing.getPerSecond() > 0)
    {
        this->relativeBitsGoodput->put(this->aggregatedThroughputInBit.getPerSecond()/this->cumulatedBitsOutgoing.getPerSecond());
        this->relativeCompoundsGoodput->put(this->aggregatedThroughputInPDUs.getPerSecond()/this->cumulatedPDUsOutgoing.getPerSecond());
    }

} // periodically



