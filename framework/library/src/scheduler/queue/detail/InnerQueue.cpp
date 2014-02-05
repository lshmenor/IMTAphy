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

#include <WNS/scheduler/queue/detail/InnerQueue.hpp>

#include <WNS/scheduler/queue/ISegmentationCommand.hpp>
#include <WNS/ldk/CommandTypeSpecifier.hpp>

#include <WNS/simulator/ISimulator.hpp>
#include <WNS/ldk/probe/TickTack.hpp>


using namespace wns::scheduler::queue::detail;

InnerQueue::InnerQueue():
    nettoBits_(0),
    sequenceNumber_(0),
    frontSegmentSentBits_(0)
{
}

Bit
InnerQueue::queuedNettoBits() const
{
    return nettoBits_;
}

Bit
InnerQueue::queuedBruttoBits(Bit fixedHeaderSize, Bit extensionHeaderSize, bool byteAlignHeader) const
{
    if (queuedNettoBits() == 0)
    {
        return 0;
    }
    else
    {
        Bit headerSize = fixedHeaderSize + (pduQueue_.size() - 1) * extensionHeaderSize;
        if (byteAlignHeader)
        {
            headerSize += headerSize % 8;
        }
        return headerSize + queuedNettoBits();
    }
}

int
InnerQueue::queuedCompounds() const
{
    return pduQueue_.size();
}

bool
InnerQueue::empty() const
{
    return pduQueue_.empty();
}

void
InnerQueue::put(const wns::ldk::CompoundPtr& compound)
{
    pduQueue_.push(compound);

    nettoBits_ += compound->getLengthInBits();
}

wns::ldk::CompoundPtr
InnerQueue::retrieve(Bit requestedBits, Bit fixedHeaderSize, Bit extensionHeaderSize, 
    bool usePadding, bool byteAlignHeader, wns::ldk::CommandReaderInterface* reader,
    const wns::probe::bus::ContextCollectorPtr& probeCC,
    wns::ldk::CommandReaderInterface* probeCmdReader)
{
    if (requestedBits <= fixedHeaderSize)
    {
        throw RequestBelowMinimumSize(requestedBits, fixedHeaderSize);
    }

    if (pduQueue_.size() <= 0)
    {
        throw RetrieveException("Queue is empty");
    }

    wns::ldk::CompoundPtr pdu(pduQueue_.front()->copy());

    assure(reader != NULL, "No valid segmentHeaderReader given!");

    reader->activateCommand(pdu->getCommandPool());

    ISegmentationCommand* header = reader->readCommand<ISegmentationCommand>(pdu->getCommandPool());

    header->increaseHeaderSize(fixedHeaderSize);

    header->setSequenceNumber(sequenceNumber_);
    sequenceNumber_ += 1;

    (frontSegmentSentBits_ == 0) ? header->setBeginFlag():header->clearBeginFlag();

    while (header->totalSize() < requestedBits)
    {
        wns::ldk::CompoundPtr c = pduQueue_.front();
        Bit length = c->getLengthInBits() - frontSegmentSentBits_; // netto
        Bit capacity = requestedBits - header->totalSize(); // netto
        if (capacity >= length)
        {
            // fits in completely
            header->addSDU(c->copy());
            header->increaseDataSize(length);
            probe(c, probeCC, probeCmdReader); 
            pduQueue_.pop();
            frontSegmentSentBits_ = 0;
            nettoBits_ -= length;

            Bit headerPadding = 0;
            if ( byteAlignHeader )
            {
                headerPadding = (header->headerSize() + extensionHeaderSize) % 8;
            }

            if ( (header->totalSize() + extensionHeaderSize + headerPadding < requestedBits) &&
                 (pduQueue_.size() > 0))
            {
                header->increaseHeaderSize(extensionHeaderSize);
            }
            else
            {
                break;
            }
        }
        else
        {
            Bit headerPadding = 0;
            if ( byteAlignHeader )
            {
                headerPadding = header->headerSize() % 8;
            }
            // only a fraction fits in
            header->addSDU(c->copy());
            header->increaseDataSize(capacity - headerPadding);
            header->increaseHeaderSize(headerPadding);
            frontSegmentSentBits_ += capacity - headerPadding;
            nettoBits_ -= capacity - headerPadding;
            assure(frontSegmentSentBits_ < pduQueue_.front()->getLengthInBits(), "frontSegmentSentBits_ is larger than the front PDU size!");
        }
    }

    // remaining bits:
    if (pduQueue_.size()==0)
    {
        nettoBits_ = 0;
    }

    // Is the last segment in the PDU a fragment? Then clear end flag
    (frontSegmentSentBits_ == 0) ? header->setEndFlag():header->clearEndFlag();

    if (byteAlignHeader)
    {
        header->increaseHeaderSize(header->headerSize() % 8);
    }

    // Rest is padding (optional)
    if (usePadding) {
        header->increasePaddingSize(requestedBits - header->totalSize());
    }

    assure(header->totalSize()<=requestedBits,"pdulength="<<header->totalSize()<<" > bits="<<requestedBits);
    return pdu;
}

std::queue<wns::ldk::CompoundPtr> 
InnerQueue::getQueueCopy()
{
    return pduQueue_;
}

void
InnerQueue::probe(const wns::ldk::CompoundPtr& compound,
    const wns::probe::bus::ContextCollectorPtr& probeCC,
    wns::ldk::CommandReaderInterface* cmdReader)
{
    assure(compound != wns::ldk::CompoundPtr(), "No valid compound");

    if(cmdReader == NULL ||
        probeCC == wns::probe::bus::ContextCollectorPtr())
        return;

    wns::ldk::probe::TickTackCommand* cmd;
    cmd = cmdReader->readCommand<wns::ldk::probe::TickTackCommand>(compound->getCommandPool());
    assure(cmd != NULL, "Cannot get TickTackCommand to probe delay");

    wns::simulator::Time now = wns::simulator::getEventScheduler()->getTime();
    probeCC->put(compound, now - cmd->magic.tickTime);
}


