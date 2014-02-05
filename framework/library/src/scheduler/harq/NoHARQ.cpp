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

#include <WNS/scheduler/harq/NoHARQ.hpp>

using namespace wns::scheduler::harq;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    NoHARQ,
    HARQInterface,
    "noharq",
    wns::PyConfigViewCreator);

NoHARQ::NoHARQ(const wns::pyconfig::View& config)
{
}

NoHARQ::~NoHARQ()
{
}

void
NoHARQ::storeSchedulingTimeSlot(long int transportBlockID, const wns::scheduler::SchedulingTimeSlotPtr& resourceBlock)
{
}

void
NoHARQ::onTimeSlotReceived(const wns::scheduler::SchedulingTimeSlotPtr& resourceBlock,
                         HARQInterface::TimeSlotInfo info)
{
    wns::scheduler::UserID userID = 
        resourceBlock->physicalResources[0].getSourceUserIDOfScheduledCompounds();

    assure(!resourceBlock->isHARQEnabled(), 
        "Received HARQ transmission but HARQ strategy is NoHARQ");

    resourceBlock->harq.successfullyDecoded = true;
    receivedNonHARQTimeslots_.push_back(HARQInterface::DecodeStatusContainerEntry(resourceBlock, info));
    return;
}

HARQInterface::DecodeStatusContainer
NoHARQ::decode()
{
    HARQInterface::DecodeStatusContainer tmp;
    tmp = receivedNonHARQTimeslots_;
    receivedNonHARQTimeslots_.clear();

    return tmp;
}

wns::scheduler::UserSet
NoHARQ::getUsersWithRetransmissions() const
{
    wns::scheduler::UserSet empty;
    return empty;
}

std::list<int>
NoHARQ::getProcessesWithRetransmissions(wns::scheduler::UserID peer) const
{
    std::list<int> empty;
    return empty;
}

int
NoHARQ::getNumberOfRetransmissions(wns::scheduler::UserID, int processID)
{
    int numberOfRetransmissions = 0;
    return numberOfRetransmissions;
}

bool
NoHARQ::hasFreeSenderProcess(wns::scheduler::UserID peer)
{
    return true;
}

bool
NoHARQ::hasFreeReceiverProcess(wns::scheduler::UserID peer)
{
    return true;
}

wns::scheduler::SchedulingTimeSlotPtr
NoHARQ::getNextRetransmission(wns::scheduler::UserID, int processID)
{
    return wns::scheduler::SchedulingTimeSlotPtr(); // empty
}

wns::scheduler::SchedulingTimeSlotPtr
NoHARQ::peekNextRetransmission(wns::scheduler::UserID, int processID) const
{
    return wns::scheduler::SchedulingTimeSlotPtr(); // empty
}

void
NoHARQ::setDownlinkHARQ(HARQInterface* downlinkHARQ)
{
}

wns::scheduler::UserSet
NoHARQ::getPeersWithPendingRetransmissions() const
{
    wns::scheduler::UserSet u;
    return u;
}

std::list<int>
NoHARQ::getPeerProcessesWithRetransmissions(wns::scheduler::UserID peer) const
{
    std::list<int> empty;
    return empty;
}

int
NoHARQ::getNumberOfPeerRetransmissions(wns::scheduler::UserID peer, int processID) const
{
    return 0;
}

void
NoHARQ::schedulePeerRetransmissions(wns::scheduler::UserID peer, int processID)
{
}

void
NoHARQ::schedulePeerRetransmission(wns::scheduler::UserID peer, int processID)
{
}

void
NoHARQ::sendPendingFeedback()
{
}


