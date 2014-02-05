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

#include <WNS/ldk/tools/ConstantDelay.hpp>

using namespace wns::ldk::tools;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    ConstantDelay,
    wns::ldk::FunctionalUnit,
    "wns.ldk.tools.ConstantDelay",
    wns::ldk::FUNConfigCreator);

ConstantDelay::ConstantDelay(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config_) :
    wns::ldk::fu::Plain<ConstantDelay, wns::ldk::EmptyCommand>(fun),
    wns::ldk::Delayed<ConstantDelay>(),
    currentFrame(),
    delay(config_.get<wns::simulator::Time>("delayDuration")),
    logger(config_.get("logger"))
{

}

void
ConstantDelay::processIncoming(const wns::ldk::CompoundPtr& compound)
{
    getDeliverer()->getAcceptor(compound)->onData(compound);
}

void
ConstantDelay::processOutgoing(const wns::ldk::CompoundPtr& compound)
{
    assure(hasCapacity(), "called processOutgoing although no capacity");
    MESSAGE_SINGLE(NORMAL, this->logger, "Ougoing compound, delay for " << delay);
    this->currentFrame = compound;
    this->setTimeout(delay);
}

bool
ConstantDelay::hasCapacity() const
{
    // There is room for exactly one compound
    return this->currentFrame == wns::ldk::CompoundPtr();
}

const wns::ldk::CompoundPtr
ConstantDelay::hasSomethingToSend() const
{
    if(hasTimeoutSet())
    {
        return(wns::ldk::CompoundPtr());
    }
    else
    {
        return(this->currentFrame);
    }
}

wns::ldk::CompoundPtr
ConstantDelay::getSomethingToSend()
{
    assure(hasSomethingToSend(), "Called getSomethingToSend without something to send");
    wns::ldk::CompoundPtr it = this->currentFrame;
    this->currentFrame = wns::ldk::CompoundPtr();

    return it;
}

void
ConstantDelay::onTimeout()
{
    MESSAGE_SINGLE(NORMAL, this->logger, "Delay has passed, send compound");
    tryToSend();
}



