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

#include <WNS/ldk/sar/reassembly/ReassemblyBuffer.hpp>
#include <WNS/ldk/sar/SegAndConcat.hpp>

#include <WNS/simulator/ISimulator.hpp>
#include <WNS/ldk/probe/TickTack.hpp>

#include <sstream>

using namespace wns::ldk::sar::reassembly;

ReassemblyBuffer::ReassemblyBuffer():
    commandReader_(NULL),
    delayProbingEnabled_(false)
{
}

void
ReassemblyBuffer::initialize(wns::ldk::CommandReaderInterface* cr)
{
    assure(cr != NULL, "Invalid command reader given in ReassemblyBuffer");

    commandReader_ = cr;

    assure(integrityCheck(), "Invalid state after initialization in ReassemblyBuffer");
}

bool
ReassemblyBuffer::isEmpty()
{
    return buffer_.size() == 0;
}

long
ReassemblyBuffer::getNextExpectedSN()
{
    if (isEmpty())
    {
        return -1;
    }
    else
    {
        return (1 + readCommand(buffer_.back())->peer.sn_);
    }
}

bool
ReassemblyBuffer::isNextExpectedSegment(const wns::ldk::CompoundPtr& c)
{
    // accept if the buffer is empty and either the begin flag is set
    // or the segment contains at least one other SDU which must be the beginning of a fragment
    if (isEmpty() && (readCommand(c)->getBeginFlag() || readCommand(c)->peer.pdus_.size() > 1))
    {
        return true;
    }

    long expectedSN = getNextExpectedSN();
    long currentSN = readCommand(c)->peer.sn_;

    if (expectedSN == currentSN)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
ReassemblyBuffer::accepts(const wns::ldk::CompoundPtr& c)
{
    SegAndConcatCommand* command = NULL;
    command = readCommand(c);

    // accept if the buffer is empty and either the begin flag is set
    // or the segment contains at least one other SDU which must be the beginning of a fragment
    if (isEmpty() && (readCommand(c)->getBeginFlag() || readCommand(c)->peer.pdus_.size() > 1))
    {
        return true;
    }
    else
    {
        return isNextExpectedSegment(c);
    }
}

void
ReassemblyBuffer::insert(wns::ldk::CompoundPtr c)
{
    assure(accepts(c), "Trying to insert an unacceptable compound");

    if(isEmpty() && !readCommand(c)->getBeginFlag())
    {
        assure(readCommand(c)->peer.pdus_.size() > 1, "Buffer Emtpy, isBegin flag set to false and segment contains only fragments of one SDU: should have been dropped by accepts()");
        readCommand(c)->peer.pdus_.pop_front();
        readCommand(c)->setBeginFlag();
    }

    buffer_.push_back(c);

    integrityCheck();
}

wns::ldk::sar::SegAndConcatCommand*
ReassemblyBuffer::readCommand(const wns::ldk::CompoundPtr& c)
{
    wns::ldk::sar::SegAndConcatCommand* command = NULL;
    command = commandReader_->readCommand<SegAndConcatCommand>(c->getCommandPool());
    assure(command!=NULL, "Command could not be retrieved");
    return command;
}

ReassemblyBuffer::SegmentContainer
ReassemblyBuffer::getReassembledSegments(int &reassembledSegmentCounter)
{
    delays_.clear();

    reassembledSegmentCounter = 0;

    assure(integrityCheck(), "IntegrityCheck failed!");

    ReassemblyBuffer::SegmentContainer sc;

    while (buffer_.size() > 0)
    {
        assure(integrityCheck(), "IntegrityCheck failed!");

        SegAndConcatCommand* frontCommand = readCommand(buffer_.front());

        // Segment starts with an entire PDU and ends with an entire PDU, 
        // we can reassemble all PDUs within this segment
        if (frontCommand->getEndFlag())
        {
            std::list<wns::ldk::CompoundPtr>::iterator it;

            for(it = frontCommand->peer.pdus_.begin(); it!=frontCommand->peer.pdus_.end(); ++it)
            {
                sc.push_back( (*it) );
                prepareForProbing(sc.size() - 1, buffer_.front());
            }

            buffer_.pop_front();
            reassembledSegmentCounter++;

            // Whole PDU(s) was/were unpacked. IntegrityCheck must hold.
            assure(integrityCheck(), "IntegrityCheck failed!");
        }
        else
        {
            // Is there a follow-up segment with either more than one PDU
            // or with the end flag set? If yes, we can reassemble.
            // Else we cannot reassamble any more.
            ReassemblyBuffer::SegmentContainer::iterator endOfSDU;

            for(endOfSDU = buffer_.begin();
                endOfSDU!=buffer_.end();
                ++endOfSDU)
            {
                SegAndConcatCommand* currentCommand = readCommand(*endOfSDU);

                if (currentCommand->getEndFlag() || (currentCommand->peer.pdus_.size() > 1))
                {
                    break;
                }
            }

            // If we can reassemble
            if (endOfSDU != buffer_.end())
            {
                while(buffer_.size() > 0)
                {
                    SegAndConcatCommand* frontCommand = readCommand(buffer_.front());

                    // A whole PDU was in this one segment, more PDUs
                    // behind it in this segment. Take out the one PDU
                    // and mark the next segment as the start segment of the next PDU
                    if (frontCommand->peer.pdus_.size() > 1)
                    {
                        sc.push_front(frontCommand->peer.pdus_.front());
                        prepareForProbing(sc.size() - 1, buffer_.front());

                        frontCommand->peer.pdus_.pop_front();
                        reassembledSegmentCounter++;
                        frontCommand->setBeginFlag();
                        break;
                    }
                    // One PDU spans over the entire segment. But it could
                    // span over even more segments
                    else
                    {
                        sc.push_back(frontCommand->peer.pdus_.front());
                        prepareForProbing(sc.size() - 1, buffer_.front());
                    }

                    // The PDU does not span over more segments
                    if (frontCommand->getEndFlag())
                    {
                        buffer_.pop_front();
                        reassembledSegmentCounter++;
                        break;
                    }
                    // The PDU does span over more segments.
                    // Take the rest of the PDU out of the next
                    // segment and assure the counter is right after that.
                    // The next segment now starts with a new PDU marked
                    // by the "StartFlag".
                    else
                    {
                        buffer_.pop_front();
                        reassembledSegmentCounter++;
                        // The next segment continues this PDU
                        // The PDU needs to be removed from the list of the next segment,
                        // since we already pushed the PDU in this segment.
                        reassembledSegmentCounter += dropSegmentsOfSDU(sc.size() - 1);
                        break;
                    }
                }
            }
            else
            {
                break;
            }
        }
    }

    probe(sc); 
    return sc;
} // getReassembledSegments

std::string
ReassemblyBuffer::dump()
{
    std::stringstream s;
    SegmentContainer::iterator it;

    for(it=buffer_.begin(); it!=buffer_.end(); ++it)
    {
        SegAndConcatCommand* c = readCommand(*it);

        s << "|(" << std::string(c->getBeginFlag() ? "B":"")
          << std::string(c->getEndFlag() ? "E":"")
          << ")|" << c->peer.pdus_.size() << ":(";

        std::list<wns::ldk::CompoundPtr>::iterator pit;

        for(pit=c->peer.pdus_.begin(); pit!=c->peer.pdus_.end(); ++pit)
        {
            s << (*pit)->getBirthmark() << ",";
        }
        s << ")   ";
    }

    return s.str();
}

void
ReassemblyBuffer::clear()
{
    buffer_.clear();
}

size_t
ReassemblyBuffer::size()
{
    return buffer_.size();
}

bool
ReassemblyBuffer::integrityCheck()
{
    if (commandReader_ == NULL)
        return false;

    if (buffer_.size() == 0)
    {
        return true;
    }

    if (!(readCommand(buffer_.front())->peer.isBegin_))
    {
        return false;
    }

    return true;
}

// We have extracted a PDU from the last segment but the PDU continues in the next segments
int
ReassemblyBuffer::dropSegmentsOfSDU(int index)
{
    assure(buffer_.size() > 0, "You cannot drop SDU segments from an empty buffer!");
    // This first Segment in buffer_ is the first to be dropped
    // until the next segment border
    int droppedSegmentCounter = 0;

    while(true)
    {
        SegAndConcatCommand* frontCommand = readCommand(buffer_.front());
        // The PDU ends in this segment. There are no other PDUs in the segment
        if ( (frontCommand->peer.pdus_.size() == 1) && (frontCommand->getEndFlag()) )
        {
            prepareForProbing(index, buffer_.front());
            buffer_.pop_front();
            droppedSegmentCounter++;
            break;
        }

        // The PDU ends in this segment. There are other PDUs in the segment.
        // Take out the PDU and set the beginning of the segment to the next PDU.
        if (frontCommand->peer.pdus_.size() > 1)
        {
            prepareForProbing(index, buffer_.front());
            frontCommand->peer.pdus_.pop_front();
            frontCommand->setBeginFlag();
            break;
        }

        // The PDU spans through the entire segment and continues in the next. 
        // Continue the loop until we find the end of the PDU.
        if ( (frontCommand->peer.pdus_.size() == 1) && (!frontCommand->getEndFlag()) )
        {
            prepareForProbing(index, buffer_.front());
            buffer_.pop_front();
            droppedSegmentCounter++;
            continue;
        }
    }
    return droppedSegmentCounter;
}

void
ReassemblyBuffer::enableDelayProbing(const wns::probe::bus::ContextCollectorPtr& minDelayCC,
        const wns::probe::bus::ContextCollectorPtr& maxDelayCC,
        wns::ldk::CommandReaderInterface* cmdReader)
{
    assure(minDelayCC != NULL, "No valid min delay probe");
    assure(maxDelayCC != NULL, "No valid max delay probe");
    assure(cmdReader != NULL, "No valid command reader");

    delayProbingEnabled_ = true;
    minDelayCC_ = minDelayCC;
    maxDelayCC_ = maxDelayCC;
    probeCmdReader_ = cmdReader;
}

void
ReassemblyBuffer::prepareForProbing(int position,
    const wns::ldk::CompoundPtr& segment)
{
    if(delayProbingEnabled_)
    {
        assure(position >= 0, "Invalid position " << position);
        assure(segment != wns::ldk::CompoundPtr(), "No valid compound");
        assure(probeCmdReader_ != NULL, "No valid command reader");

        if(delays_.find(position) == delays_.end())
            delays_[position] = std::set<wns::simulator::Time>();

        wns::ldk::probe::TickTackCommand* cmd;
        cmd = probeCmdReader_->readCommand<wns::ldk::probe::TickTackCommand>(segment->getCommandPool());
        assure(cmd != NULL, "Cannot get TickTackCommand to probe delay");

        wns::simulator::Time now = wns::simulator::getEventScheduler()->getTime();

        delays_[position].insert(now - cmd->magic.tickTime);
    }
}

void
ReassemblyBuffer::probe(const SegmentContainer& sc)
{
    if(delayProbingEnabled_)
    {
        assure(minDelayCC_ != NULL, "No valid min delay probe");
        assure(maxDelayCC_ != NULL, "No valid max delay probe");

        SegmentContainer::const_iterator it;
        int i = 0;
        for (it = sc.begin(); it != sc.end(); ++it)
        {
            assure(!delays_[i].empty(), "No delay probed for " << i << "th PDU.");
            minDelayCC_->put((*it), *(delays_[i].begin()));
            maxDelayCC_->put((*it), *(delays_[i].rbegin()));
            i++;
        }
    }
}
