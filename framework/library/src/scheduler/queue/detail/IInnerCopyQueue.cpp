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

#include <WNS/scheduler/queue/detail/IInnerCopyQueue.hpp>

using namespace wns::scheduler::queue::detail;

SimpleInnerCopyQueue::SimpleInnerCopyQueue()
{
}

SimpleInnerCopyQueue::~SimpleInnerCopyQueue()
{
}

bool
SimpleInnerCopyQueue::knowsCID(ConnectionID cid)
{
    return queue_.find(cid) != queue_.end();
}

bool
SimpleInnerCopyQueue::isEmpty(ConnectionID cid)
{
    assure(queue_.find(cid) != queue_.end(), "Unknown CID");
    return  queue_[cid].empty();
}

wns::ldk::CompoundPtr
SimpleInnerCopyQueue::getPDU(ConnectionID cid, Bit)
{
    assure(queue_.find(cid) != queue_.end(), "Unknown CID");
    assure(!queue_[cid].empty(), "Called getPDU for empty queue");

    wns::ldk::CompoundPtr c = queue_[cid].front();

    queueSize_[cid] -= getHeadofLinePDUBit(cid);
    assure(queueSize_[cid] > 0, "Queue size is below 0");

    queue_[cid].pop();

    return  c;
}
    
Bit
SimpleInnerCopyQueue::getHeadofLinePDUBit(ConnectionID cid)
{
    assure(queue_.find(cid) != queue_.end(), "Unknown CID");
    assure(!queue_[cid].empty(), "Called getHeadofLinePDUBit for empty queue");

    return queue_[cid].front()->getLengthInBits();
}
    

void
SimpleInnerCopyQueue::reset(ConnectionID cid)
{
    assure(queue_.find(cid) != queue_.end(), "Unknown CID");
    queue_[cid] = std::queue<wns::ldk::CompoundPtr>();
    queueSize_[cid] = 0;
}
    
int
SimpleInnerCopyQueue::getSize(ConnectionID cid)
{
    assure(queue_.find(cid) != queue_.end(), "Unknown CID");
    return queue_[cid].size();
}
    
int
SimpleInnerCopyQueue::getSizeInBit(ConnectionID cid)
{
    assure(queue_.find(cid) != queue_.end(), "Unknown CID");
    assure(queueSize_.find(cid) != queueSize_.end(), "Unknown CID");

    return queueSize_[cid];
}
    
void
SimpleInnerCopyQueue::setQueue(ConnectionID cid, std::queue<wns::ldk::CompoundPtr> queue)
{
    if(queue_.find(cid) != queue_.end())
        queue_[cid] = std::queue<wns::ldk::CompoundPtr>();

    queueSize_[cid] = 0;

    wns::ldk::CompoundPtr compound;

    int s = queue.size();
    for(int i = 0; i < s; i++)
    {
        compound = queue.front();
        queue_[cid].push(compound);
        queueSize_[cid] += compound->getLengthInBits();
    }
}

// SegmentingInnerCopyQueue:

SegmentingInnerCopyQueue::SegmentingInnerCopyQueue(const wns::pyconfig::View& _config) :
    fixedHeaderSize_(_config.get<Bit>("fixedHeaderSize")),
    minimumSegmentSize_(_config.get<unsigned long int>("minimumSegmentSize")),
    extensionHeaderSize_(_config.get<Bit>("extensionHeaderSize")),
    usePadding_(_config.get<bool>("usePadding")),
    byteAlignHeader_(_config.get<bool>("byteAlignHeader")),
    segmentHeaderCommandName_(_config.get<std::string>("segmentHeaderCommandName"))
{
}

SegmentingInnerCopyQueue::~SegmentingInnerCopyQueue()
{
}

void
SegmentingInnerCopyQueue::setFUN(wns::ldk::fun::FUN* fun)
{
    assure(fun != NULL, "FUN pointer  cannot be NULL");

    segmentHeaderReader_ = fun->getCommandReader(segmentHeaderCommandName_);
    assure(segmentHeaderReader_, "No reader for the Segment Header (" << segmentHeaderCommandName_ << ") available!");
}
    
bool
SegmentingInnerCopyQueue::knowsCID(ConnectionID cid)
{
    return queue_.find(cid) != queue_.end();
}

bool
SegmentingInnerCopyQueue::isEmpty(ConnectionID cid)
{
    assure(queue_.find(cid) != queue_.end(), "Unknown CID");
    return  queue_[cid].empty();
}

wns::ldk::CompoundPtr
SegmentingInnerCopyQueue::getPDU(ConnectionID cid, Bit requestedBits)
{
    assure(queue_.find(cid) != queue_.end(), "Unknown CID");
    assure(!queue_[cid].empty(), "Called getHeadofLinePDU for empty queue");
    assure(segmentHeaderReader_ != NULL, "No valid segmentHeaderReader set! You need to call setFUN() first.");

    wns::ldk::CompoundPtr segment;
    try {
        segment = queue_[cid].retrieve(requestedBits, fixedHeaderSize_, extensionHeaderSize_, usePadding_, byteAlignHeader_, segmentHeaderReader_);

        segmentHeaderReader_->commitSizes(segment->getCommandPool());

    } catch (detail::InnerQueue::RequestBelowMinimumSize e)
    {
        return wns::ldk::CompoundPtr();
    }

    return  segment;
}
    
Bit
SegmentingInnerCopyQueue::getHeadofLinePDUBit(ConnectionID cid)
{
    assure(queue_.find(cid) != queue_.end(), "Unknown CID");
    assure(!queue_[cid].empty(), "Called getHeadofLinePDUBit for empty queue");

    return queue_[cid].queuedBruttoBits(fixedHeaderSize_, extensionHeaderSize_, byteAlignHeader_);
}
    

void
SegmentingInnerCopyQueue::reset(ConnectionID cid)
{
    assure(queue_.find(cid) != queue_.end(), "Unknown CID");
    queue_.erase(cid);
}
    
int
SegmentingInnerCopyQueue::getSize(ConnectionID cid)
{
    assure(queue_.find(cid) != queue_.end(), "Unknown CID");
    return queue_[cid].queuedCompounds();
}

int
SegmentingInnerCopyQueue::getSizeInBit(ConnectionID cid)
{
    assure(queue_.find(cid) != queue_.end(), "Unknown CID");
    return queue_[cid].queuedBruttoBits(fixedHeaderSize_, extensionHeaderSize_, byteAlignHeader_);
}
    
void
SegmentingInnerCopyQueue::setQueue(ConnectionID cid, std::queue<wns::ldk::CompoundPtr> queue)
{
    if(queue_.find(cid) != queue_.end())
        queue_[cid] = InnerQueue();

    int s = queue.size();
    for(int i = 0; i < s; i++)
        queue_[cid].put(queue.front());
}

unsigned long int 
SegmentingInnerCopyQueue::getMinimumSegmentSize()
{
    return minimumSegmentSize_;
}
