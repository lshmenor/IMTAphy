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
#include <WNS/ldk/buffer/Bounded.hpp>
#include <WNS/ldk/IConnectorReceptacle.hpp>

using namespace wns::ldk;
using namespace wns::ldk::buffer;


STATIC_FACTORY_REGISTER_WITH_CREATOR(
    Bounded,
    Buffer,
    "wns.buffer.Bounded",
    FUNConfigCreator);

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    Bounded,
    FunctionalUnit,
    "wns.buffer.Bounded",
    FUNConfigCreator);


Bounded::Bounded(fun::FUN* fuNet, const wns::pyconfig::View& config) :
    Buffer(fuNet, config),
    fu::Plain<Bounded>(fuNet),

    buffer(ContainerType()),
    maxSize(config.get<int>("size")),
    currentSize(),
    sizeCalculator(),
    inWakeup(false)
{
    {
        std::string pluginName = config.get<std::string>("sizeUnit");
        sizeCalculator = SizeCalculator::Factory::creator(pluginName)->create();
    }
} // Bounded


Bounded::~Bounded()
{
    buffer.clear();

    if(sizeCalculator != NULL)
    {
        delete sizeCalculator;
    }
} // ~Bounded


//
// CompoundHandler interface
//
bool
Bounded::doIsAccepting(const CompoundPtr& compound) const
{
    return currentSize + (*sizeCalculator)(compound) <= maxSize;
} // isAccepting


void
Bounded::doSendData(const CompoundPtr& compound)
{
    assure(isAccepting(compound), "sendData called although not accepting.");
    assure(compound->getRefCount() > 0, "Reference counting defect.");

    buffer.push_back(compound);
    currentSize += (*sizeCalculator)(compound);

    increaseTotalPDUs();
    probe();

    tryToSend();
} // doSendData


void
Bounded::doOnData(const CompoundPtr& compound)
{
    getDeliverer()->getAcceptor(compound)->onData(compound);
} // processIncoming


void
Bounded::doWakeup()
{
    tryToSend();
} // wakeup


void
Bounded::tryToSend()
{
    while(tryToSendOnce());

    if(inWakeup == false && currentSize < maxSize)
    {
        inWakeup = true;
        getReceptor()->wakeup();
        inWakeup = false;
    }
} // tryToSend


bool
Bounded::tryToSendOnce()
{
    if(buffer.empty() == true)
    {
        return false;
    }

    CompoundPtr compound = buffer.front();

    if(getConnector()->hasAcceptor(compound) == false)
    {
        return false;
    }

    buffer.pop_front();
    currentSize -= (*sizeCalculator)(compound);

    IConnectorReceptacle* target = getConnector()->getAcceptor(compound);
    target->sendData(compound);

    return true;
} // tryToSendOnce

//
// Buffer interface
//

unsigned long int
Bounded::getSize()
{
    return currentSize;
} // size


unsigned long int
Bounded::getMaxSize()
{
    return maxSize;
} // getMaxSize


