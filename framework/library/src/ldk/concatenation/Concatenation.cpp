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

#include <WNS/ldk/concatenation/Concatenation.hpp>
#include <WNS/ldk/Layer.hpp>

using namespace wns::ldk;
using namespace wns::ldk::concatenation;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    Concatenation,
    FunctionalUnit,
    "wns.concatenation.Concatenation",
    FUNConfigCreator);

Concatenation::Concatenation(fun::FUN* fuNet, const wns::pyconfig::View& config) :
    fu::Plain<Concatenation, ConcatenationCommand>(fuNet),
    Delayed<Concatenation>(),

    maxSize(config.get<Bit>("maxSize")),
    currentSDUSize(0),
    currentCommandPoolSize(0),
    currentCompound(CompoundPtr()),
    nextCompound(CompoundPtr()),
    maxEntries(config.get<int>("maxEntries")),
    currentEntries(0),

    numBitsIfConcatenated(config.get<Bit>("numBitsIfConcatenated")),
    numBitsPerEntry(config.get<Bit>("numBitsPerEntry")),
    numBitsIfNotConcatenated(config.get<Bit>("numBitsIfNotConcatenated")),
    countPCISizeOfEntries(config.get<bool>("countPCISizeOfEntries")),
    entryPaddingBoundary(config.get<Bit>("entryPaddingBoundary")),

    logger(config.get("logger"))

{
    MESSAGE_SINGLE(VERBOSE, logger, "created");

} // Concatenation


Concatenation::~Concatenation()
{
} // ~Concatenation


void
Concatenation::processIncoming(const CompoundPtr& compound)
{
    // Extract the compounds from the PCI of the PDU
    ConcatenationCommand* command = getCommand(compound->getCommandPool());

    MESSAGE_BEGIN(NORMAL, logger, m, "Fragmenting incoming container into ");
    m << command->peer.compounds.size() << " part(s)";
    MESSAGE_END();

    //The PCI hold the compounds, which are send to the upper FU
    for(std::vector<CompoundPtr>::iterator it = command->peer.compounds.begin();
        it != command->peer.compounds.end();
        ++it)
    {
        getDeliverer()->getAcceptor(*it)->onData(*it);
    }

    // the originally received PDU is dropped
} // processIncoming


bool
Concatenation::hasCapacity() const
{
    return (this->nextCompound == CompoundPtr()) and (currentEntries <= maxEntries);
} // hasCapacity

void
Concatenation::processOutgoing(const CompoundPtr& compound)
{
    activateCommand(compound->getCommandPool());

    if(this->currentCompound == CompoundPtr())
    {
        // start new container compound which has the same PCI, but no
        // data
        this->currentCompound = this->createContainer(compound);
        this->calculateSizes(this->currentCompound->getCommandPool(), currentCommandPoolSize, currentSDUSize);
        this->currentEntries = getCommand(this->currentCompound->getCommandPool())->peer.compounds.size();

        MESSAGE_BEGIN(NORMAL, logger, m, "Start new container compound with");
        m << " SDU Size: " << currentSDUSize;
        m << " PCI Size: " << currentCommandPoolSize;
        m << " Entries: " << currentEntries;
        MESSAGE_END();

        return;
    }

    // try to add command
    ConcatenationCommand* command = getCommand(this->currentCompound->getCommandPool());
    command->peer.compounds.push_back(compound);
    int wouldBeEntries = currentEntries + 1;
    Bit wouldBeCommandPoolSize;
    Bit wouldBeSDUSize;
    this->calculateSizes(this->currentCompound->getCommandPool(), wouldBeCommandPoolSize, wouldBeSDUSize);

    if((wouldBeSDUSize + wouldBeCommandPoolSize) <= maxSize and wouldBeEntries <= maxEntries)
    {
        // addition is permitted
        ++currentEntries;
        currentSDUSize = wouldBeSDUSize;
        currentCommandPoolSize = wouldBeCommandPoolSize;

        MESSAGE_BEGIN(NORMAL, logger, m, "Adding compound " << currentEntries);
        m << " SDU Size: " << currentSDUSize;
        m << " PCI Size: " << currentCommandPoolSize;
        MESSAGE_END();
    }
    else
    {
        // revert operation
        command->peer.compounds.pop_back();

        //this->nextCompound will be sent next
        this->nextCompound = this->createContainer(compound);

        MESSAGE_BEGIN(NORMAL, logger, m, "Accepted compound exceeds aggregation size:");
        m << " bits: " << (wouldBeSDUSize + wouldBeCommandPoolSize);
        m << " entries: " << wouldBeEntries;
        MESSAGE_END();
    }
} // processOutgoing

CompoundPtr
Concatenation::createContainer(const CompoundPtr& firstEntry)
{
    CommandPool* containerCommandPool = getFUN()->createCommandPool();
    // new container compound has the same PCI as firstEntry, but no data
    getFUN()->getProxy()->partialCopy(this, containerCommandPool, firstEntry->getCommandPool());
    // add firstEntry to container
    ConcatenationCommand* containerCommand = activateCommand(containerCommandPool);
    containerCommand->peer.compounds.push_back(firstEntry);
    // create compound
    CompoundPtr containerCompound(new Compound(containerCommandPool));

    return(containerCompound);
}

const CompoundPtr
Concatenation::hasSomethingToSend() const
{
    return this->currentCompound;
} // somethingToSend


CompoundPtr
Concatenation::getSomethingToSend()
{
    assure(hasSomethingToSend(), "no concatenated PDU has been constructed");

    MESSAGE_SINGLE(NORMAL, logger, "Sending container with " << currentEntries << " entries");

    // For the next transmit this->nextCompound will be send first
    CompoundPtr compound = this->currentCompound;
    this->currentCompound = this->nextCompound;
    this->nextCompound = CompoundPtr();

    if(this->currentCompound)
    {
                // If there is something to send
        this->calculateSizes(this->currentCompound->getCommandPool(), currentCommandPoolSize, currentSDUSize);
        currentEntries = getCommand(this->currentCompound->getCommandPool())->peer.compounds.size();
    }
    else
    {
        currentEntries = 0;
        currentSDUSize = 0;
        currentCommandPoolSize = 0;
    }

    return compound;
} // getSomethingToSend

void
Concatenation::calculateSizes(const CommandPool* commandPool, Bit& commandPoolSize, Bit& sduSize) const
{
    // Get the entries of the container
    ConcatenationCommand* command = getCommand(commandPool);

    if(command->peer.compounds.size() > 1)
    {
        MESSAGE_SINGLE(VERBOSE, logger, "calculateSizes: Container with " << command->peer.compounds.size() << " entries.");

        // initialize with zero
        commandPoolSize = 0;
        sduSize = 0;

        // control bits for concatenation itself
        commandPoolSize += this->numBitsIfConcatenated;

        for(std::vector<CompoundPtr>::iterator it = command->peer.compounds.begin();
            it != command->peer.compounds.end();
            ++it)
        {
            // Sizes for the upper layers of the entry
            Bit entrySDUSize;
            Bit entryCommandPoolSize;
            (*it)->getCommandPool()->calculateSizes(entryCommandPoolSize, entrySDUSize);

            if(countPCISizeOfEntries)
            {
                entryCommandPoolSize = entryCommandPoolSize + this->numBitsPerEntry;
            }
            else
            {
                entryCommandPoolSize = this->numBitsPerEntry;
            }

            sduSize += (entrySDUSize + this->calculatePadding(entrySDUSize + entryCommandPoolSize, this->entryPaddingBoundary));
            commandPoolSize += entryCommandPoolSize;
        }
        MESSAGE_BEGIN(VERBOSE, logger, m, " ");
        m << " -> " << sduSize << "+" << commandPoolSize;
        MESSAGE_END();
    }
    else
    {
        if(command->peer.compounds.size() == 1)
        {
            MESSAGE_SINGLE(VERBOSE, logger, "calculateSizes: Container with single entry.");
            // calculate size of single entry in container
            getFUN()->calculateSizes(command->peer.compounds[0]->getCommandPool(), commandPoolSize, sduSize, this);
            commandPoolSize += this->numBitsIfNotConcatenated;
        }
        else
        {
            MESSAGE_SINGLE(VERBOSE, logger, "calculateSizes: No container command.");
            // Sizes for the upper layers of the non-container command
            getFUN()->calculateSizes(commandPool, commandPoolSize, sduSize, this);
        }
    }
} // calculateSizes

Bit
Concatenation::calculatePadding(Bit size, Bit paddingBoundary) const
{
    Bit padding = paddingBoundary - (size % paddingBoundary);
    if(padding < paddingBoundary)
    {
        return(padding);
    }
    else
    {
        return(0);
    }
} // appendPadding


