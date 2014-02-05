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

#include <WNS/ldk/probe/TickTack.hpp>
#include <WNS/probe/bus/ContextProviderCollection.hpp>
#include <WNS/ldk/Layer.hpp>

using namespace wns::ldk;
using namespace wns::ldk::probe;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    Tick,
    FunctionalUnit,
    "wns.probe.Tick",
    FUNConfigCreator);

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    Tack,
    FunctionalUnit,
    "wns.probe.Tack",
    FUNConfigCreator);

Tick::Tick(fun::FUN* fuNet, const wns::pyconfig::View& config) :
    fu::Plain<Tick, TickTackCommand>(fuNet),
    probeName_(config.get<std::string>("commandName")),
    probeOutgoing_(config.get<bool>("probeOutgoing")),
    logger_(config.get("logger"))
{
    wns::probe::bus::ContextProviderCollection* cpcParent = 
        &fuNet->getLayer()->getContextProviderCollection();

    wns::probe::bus::ContextProviderCollection cpc(cpcParent);

    outSizeProbe_ = wns::probe::bus::ContextCollectorPtr(
        new wns::probe::bus::ContextCollector(cpc, probeName_ + ".start.compoundSize"));
}

Tick::~Tick()
{
}

void
Tick::onFUNCreated()
{
}  // onFUNCreated

void
Tick::doSendData(const CompoundPtr& compound)
{
    if(probeOutgoing_)
        writeCommand(compound);

    getConnector()->getAcceptor(compound)->sendData(compound);
}

void
Tick::doOnData(const CompoundPtr& compound)
{
    if(!probeOutgoing_)
        writeCommand(compound);

    getDeliverer()->getAcceptor(compound)->onData(compound);
}

void
Tick::writeCommand(const CompoundPtr& compound)
{
    activateCommand(compound->getCommandPool());

    MESSAGE_BEGIN(NORMAL, logger_, m, getFUN()->getName());
    m << " passing through Tick probing " << probeName_;
    m << " direction " << (probeOutgoing_?"outgoing":"incoming");
    MESSAGE_END();

    outSizeProbe_->put(compound, compound->getLengthInBits());
}

void
Tick::probeOutgoing()
{
    probeOutgoing_ = true;
}

void
Tick::probeIncoming()
{
    probeOutgoing_ = false;
}




Tack::Tack(fun::FUN* fuNet, const wns::pyconfig::View& config) :
    fu::Plain<Tack>(fuNet),
    probeName_(config.get<std::string>("probeAndCommandName")),
    probeOutgoing_(config.get<bool>("probeOutgoing")),
    commandReader_(NULL),
    fun_(fuNet),
    logger_(config.get("logger"))
{
    wns::probe::bus::ContextProviderCollection* cpcParent = 
        &fuNet->getLayer()->getContextProviderCollection();

    wns::probe::bus::ContextProviderCollection cpc(cpcParent);

    delayProbe_ = wns::probe::bus::ContextCollectorPtr(
        new wns::probe::bus::ContextCollector(cpc, probeName_ + ".delay"));

    inSizeProbe_ = wns::probe::bus::ContextCollectorPtr(
        new wns::probe::bus::ContextCollector(cpc, probeName_ + ".stop.compoundSize"));
}

Tack::~Tack()
{
}

void
Tack::onFUNCreated()
{
    commandReader_ = fun_->getCommandReader(probeName_);
    assure(commandReader_ != NULL, "Cannot find command reader: " + probeName_);
}  // onFUNCreated

void
Tack::doSendData(const CompoundPtr& compound)
{
    if(probeOutgoing_)
        probeIfNotProbed(compound);

    getConnector()->getAcceptor(compound)->sendData(compound);
}

void
Tack::doOnData(const CompoundPtr& compound)
{
    if(!probeOutgoing_)
        probeIfNotProbed(compound);

    getDeliverer()->getAcceptor(compound)->onData(compound);
}

void
Tack::probeOutgoing()
{
    probeOutgoing_ = true;
}

void
Tack::probeIncoming()
{
    probeOutgoing_ = false;
}

void
Tack::probeIfNotProbed(const CompoundPtr& compound)
{
    assure(commandReader_ != NULL, "Need command reader: " + probeName_);
    assure(commandReader_->commandIsActivated(compound->getCommandPool()),
        "Command " + probeName_ + " not activated.");

    TickTackCommand* command;
    command = commandReader_->readCommand<TickTackCommand>(compound->getCommandPool());
    if(command->magic.probed == false)
    {
        command->magic.probed = true;
        wns::simulator::Time now = wns::simulator::getEventScheduler()->getTime();
        delayProbe_->put(compound, now - command->magic.tickTime);
        inSizeProbe_->put(compound, compound->getLengthInBits());

        MESSAGE_BEGIN(NORMAL, logger_, m, getFUN()->getName());
        m << " probing delay " << probeName_ << ": ";
        m << now - command->magic.tickTime;
        m << " and size: " << compound->getLengthInBits();
        MESSAGE_END();
    }
}


