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

#include <WNS/ldk/buffer/Dropping.hpp>

using namespace wns::ldk;
using namespace wns::ldk::buffer;
using namespace wns::ldk::buffer::dropping;


STATIC_FACTORY_REGISTER_WITH_CREATOR(
    Dropping,
    Buffer,
    "wns.buffer.Dropping",
    FUNConfigCreator);

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    Dropping,
    FunctionalUnit,
    "wns.buffer.Dropping",
    FUNConfigCreator);

//
// strategies
//

// Drop strategies
using namespace wns::ldk::buffer::dropping::drop;

CompoundPtr
Tail::operator()(ContainerType& container) const
{
    CompoundPtr it = container.back();
    container.pop_back();
    return it;
} // Tail()
STATIC_FACTORY_REGISTER(Tail, Drop, "Tail");


CompoundPtr
Front::operator()(ContainerType& container) const
{
    CompoundPtr it = container.front();
    container.pop_front();
    return it;
} // Front()
STATIC_FACTORY_REGISTER(Front, Drop, "Front");


//
// FunctionalUnit implementation
//

Dropping::Dropping(fun::FUN* fuNet, const wns::pyconfig::View& config) :
        Buffer(fuNet, config),

        fu::Plain<Dropping>(fuNet),
        Delayed<Dropping>(),

        buffer(ContainerType()),
        maxSize(config.get<int>("size")),
        currentSize(0),
        sizeCalculator(),
        dropper(),
        totalPDUs(),
        droppedPDUs(),
        logger("WNS", config.get<std::string>("name"))
{
    {
        std::string pluginName = config.get<std::string>("sizeUnit");
        sizeCalculator = std::auto_ptr<SizeCalculator>(SizeCalculator::Factory::creator(pluginName)->create());
    }

    {
        std::string pluginName = config.get<std::string>("drop");
        dropper = std::auto_ptr<Drop>(Drop::Factory::creator(pluginName)->create());
    }
} // Dropping

Dropping::Dropping(const Dropping& other) :
    CompoundHandlerInterface<FunctionalUnit>(other),
    CommandTypeSpecifierInterface(other),
    HasReceptorInterface(other),
    HasConnectorInterface(other),
    HasDelivererInterface(other),
    CloneableInterface(other),
    IOutputStreamable(other),
    PythonicOutput(other),
    FunctionalUnit(other),
    DelayedInterface(other),
    Buffer(other),
    fu::Plain<Dropping>(other),
    Delayed<Dropping>(other),
    buffer(other.buffer),
    maxSize(other.maxSize),
    currentSize(other.currentSize),
    sizeCalculator(wns::clone(other.sizeCalculator)),
    dropper(wns::clone(other.dropper)),
    totalPDUs(other.totalPDUs),
    droppedPDUs(other.droppedPDUs),
    logger(other.logger)
{
}

Dropping::~Dropping()
{
} // ~Dropping


//
// Delayed interface
//
void
Dropping::processIncoming(const CompoundPtr& compound)
{
    getDeliverer()->getAcceptor(compound)->onData(compound);
} // processIncoming


bool
Dropping::hasCapacity() const
{
    return true;
} // hasCapacity


void
Dropping::processOutgoing(const CompoundPtr& compound)
{
    buffer.push_back(compound);
    currentSize += (*sizeCalculator)(compound);

    while(currentSize > maxSize)
    {

        MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
        m << " dropping a PDU! maxSize reached : " << maxSize;
        m << " current size is " << currentSize;
        MESSAGE_END();

        CompoundPtr toDrop = (*dropper)(buffer);
        int pduSize = (*sizeCalculator)(toDrop);
        currentSize -= pduSize;
        increaseDroppedPDUs(pduSize);
    }

    increaseTotalPDUs();
    probe();
} // processOutgoing


const CompoundPtr
Dropping::hasSomethingToSend() const
{
    if(buffer.empty())
        return CompoundPtr();

    return buffer.front();
} // somethingToSend


CompoundPtr
Dropping::getSomethingToSend()
{
    CompoundPtr compound = buffer.front();
    buffer.pop_front();

    currentSize -= (*sizeCalculator)(compound);
    probe();

    return compound;
} // getSomethingToSend


//
// Buffer interface
//

unsigned long int
Dropping::getSize()
{
    return currentSize;
} // getSize


unsigned long int
Dropping::getMaxSize()
{
    return maxSize;
} // getMaxSize


