/*******************************************************************************
 * This file is part of IMTAphy
 * _____________________________________________________________________________
 *
 * Copyright (C) 2011
 * Institute of Communication Networks (LKN)
 * Department of Electrical Engineering and Information Technology (EE & IT)
 * Technische Universitaet Muenchen
 * Arcisstr. 21
 * 80333 Muenchen - Germany
 * http://www.lkn.ei.tum.de/~jan/imtaphy/index.html
 * 
 * _____________________________________________________________________________
 *
 *   IMTAphy is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   IMTAphy is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with IMTAphy.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

// based on code from openWNS with the following license:

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

#include <IMTAPHY/ltea/helper/Window.hpp>
#include <WNS/ldk/Layer.hpp>

using namespace wns::ldk;
using namespace wns::ldk::probe;
using namespace ltea::helper;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    Window,
    Probe,
    "ltea.helper.probe.Window",
    FUNConfigCreator);

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    Window,
    FunctionalUnit,
    "ltea.helper.probe.Window",
    FUNConfigCreator);

Window::Window(fun::FUN* fuNet, const wns::pyconfig::View& config) :
    Probe(),
    fu::Plain<Window, WindowCommand>(fuNet),
    Forwarding<Window>(),
    wns::events::PeriodicTimeout(),

    windowSize(config.get<wns::simulator::Time>("windowSize")),

    cumulatedBitsIncoming(0),
    cumulatedPDUsIncoming(0),
    cumulatedBitsOutgoing(0),
    cumulatedPDUsOutgoing(0),
    aggregatedThroughputInBit(0),
    aggregatedThroughputInPDUs(0),
    settlingTime(config.get<wns::simulator::Time>("settlingTime")),

    //logger("WNS", config.get<std::string>("name"))
    logger(config.get("logger"))
{
    assure(windowSize > 0,
        "windowSize must be > 0");

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


    // start after settlingTime and wait until first window is full, then sample every windowSize seconds
    this->startPeriodicTimeout(windowSize, settlingTime + windowSize);
} // Window

Window::~Window()
{}

void
Window::processOutgoing(const CompoundPtr& compound)
{
    WindowCommand* command = this->activateCommand(compound->getCommandPool());
    command->magic.probingFU = this;

    if (wns::simulator::getEventScheduler()->getTime() >= settlingTime)
    {
        Bit commandPoolSize;
        Bit dataSize;
        this->getFUN()->calculateSizes(compound->getCommandPool(), commandPoolSize, dataSize, this);
        
        const long int compoundLength = commandPoolSize + dataSize;

        MESSAGE_BEGIN(NORMAL, logger, m, this->getFUN()->getName());
        m << " outgoing"
        << " length " << compoundLength;
        MESSAGE_END();

        cumulatedBitsOutgoing += compoundLength;
        cumulatedPDUsOutgoing++;
    }
    
    Forwarding<Window>::processOutgoing(compound);
} // processOutgoing


void
Window::processIncoming(const CompoundPtr& compound)
{
    if (wns::simulator::getEventScheduler()->getTime() >= settlingTime)
    {
        Bit commandPoolSize;
        Bit dataSize;
        this->getFUN()->calculateSizes(compound->getCommandPool(), commandPoolSize, dataSize, this);
        const long int compoundLength = commandPoolSize + dataSize;


        cumulatedBitsIncoming += compoundLength;
        cumulatedPDUsIncoming++;

        WindowCommand* command = this->getCommand(compound->getCommandPool());
        command->magic.probingFU->aggregatedThroughputInBit += compoundLength;
        command->magic.probingFU->aggregatedThroughputInPDUs++;
        
        MESSAGE_BEGIN(NORMAL, logger, m, this->getFUN()->getName());
        m << " incoming"
        << " length " << compoundLength << " from peer Window probe FU " << command->magic.probingFU->getFUN()->getName();
        MESSAGE_END();

        
    }
    
    Forwarding<Window>::processIncoming(compound);
} // processIncoming


void
Window::periodically()
{
    this->bitsOutgoingBus->put(static_cast<double>(cumulatedBitsOutgoing) / windowSize);
    this->compoundsOutgoingBus->put(static_cast<double>(cumulatedPDUsOutgoing)  / windowSize);
    this->bitsIncomingBus->put(static_cast<double>(cumulatedBitsIncoming) / windowSize);
    this->compoundsIncomingBus->put(static_cast<double>(cumulatedPDUsIncoming) / windowSize);
    this->bitsAggregatedBus->put(static_cast<double>(aggregatedThroughputInBit) / windowSize);
    this->compoundsAggregatedBus->put(static_cast<double>(aggregatedThroughputInPDUs) / windowSize);
    
    cumulatedBitsIncoming = 0;
    cumulatedBitsOutgoing = 0;
    cumulatedPDUsIncoming = 0;
    cumulatedPDUsOutgoing = 0;
    aggregatedThroughputInBit = 0;
    aggregatedThroughputInPDUs = 0;
} // periodically



