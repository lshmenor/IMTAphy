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

#include <WNS/scheduler/harq/HARQRetransmissionProxy.hpp>

using namespace wns::scheduler::harq;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
                                      HARQRetransmissionProxy,
                                      HARQInterface,
                                      "harqRetransmissionProxy",
                                      wns::PyConfigViewCreator);

HARQRetransmissionProxy::HARQRetransmissionProxy(const wns::pyconfig::View& config):
    logger_(config.get("logger")),
    downlinkHARQ_(NULL)
{
}

HARQRetransmissionProxy::~HARQRetransmissionProxy()
{
}

void
HARQRetransmissionProxy::setDownlinkHARQ(HARQInterface* downlinkHARQ)
{
    assure(downlinkHARQ, "Invalid downlink HARQ instance");

    downlinkHARQ_ = downlinkHARQ;
}

wns::scheduler::UserSet
HARQRetransmissionProxy::getPeersWithPendingRetransmissions() const
{
    assure(downlinkHARQ_, "There is no downlinkHARQ set.");
    return downlinkHARQ_->getPeersWithPendingRetransmissions();
}

std::list<int>
HARQRetransmissionProxy::getPeerProcessesWithRetransmissions(wns::scheduler::UserID peer) const
{
    assure(downlinkHARQ_, "There is no downlinkHARQ set.");
    return downlinkHARQ_->getPeerProcessesWithRetransmissions(peer);
}

int
HARQRetransmissionProxy::getNumberOfPeerRetransmissions(wns::scheduler::UserID peer, int processID) const
{
    assure(downlinkHARQ_, "There is no downlinkHARQ set.");
    return downlinkHARQ_->getNumberOfPeerRetransmissions(peer, processID);
}

void
HARQRetransmissionProxy::schedulePeerRetransmissions(wns::scheduler::UserID peer, int processID)
{
    assure(downlinkHARQ_, "There is no downlinkHARQ set.");
    downlinkHARQ_->schedulePeerRetransmissions(peer, processID);
}

void
HARQRetransmissionProxy::schedulePeerRetransmission(wns::scheduler::UserID peer, int processID)
{
    assure(downlinkHARQ_, "There is no downlinkHARQ set.");
    downlinkHARQ_->schedulePeerRetransmission(peer, processID);
}
void
HARQRetransmissionProxy::sendPendingFeedback()
{
    assure(false, "This is only a proxy. You cannot call sendPendingFeedback. You're in uplink dude!");
}

void
HARQRetransmissionProxy::storeSchedulingTimeSlot(long int transportBlockID,
                                                 const wns::scheduler::SchedulingTimeSlotPtr& resourceBlock)
{
    assure(false, "This is only a proxy. You cannot call storeSchedulingTimeslot. You're in uplink dude!");
}

void
HARQRetransmissionProxy::onTimeSlotReceived(const wns::scheduler::SchedulingTimeSlotPtr& resourceBlock,
                                            HARQInterface::TimeSlotInfo)
{
    assure(false, "This is only a proxy. You cannot call onSchedulingTimeslotReceived. You're in uplink dude!");
}

HARQInterface::DecodeStatusContainer
HARQRetransmissionProxy::decode()
{
    assure(false, "This is only a proxy. You cannot call decode. You're in uplink dude!");
    HARQInterface::DecodeStatusContainer a;
    return a;
}

wns::scheduler::UserSet
HARQRetransmissionProxy::getUsersWithRetransmissions() const
{
    assure(false, "This is only a proxy. You cannot call getUsersWithRetransmissions. You're in uplink dude!");
}

std::list<int>
HARQRetransmissionProxy::getProcessesWithRetransmissions(wns::scheduler::UserID peer) const
{
  assure(false, "This is only a proxy. You cannot call getProcessesWithRetransmissions. You're in uplink dude!");
}

int
HARQRetransmissionProxy::getNumberOfRetransmissions(wns::scheduler::UserID, int processID)
{
    assure(false, "This is only a proxy. You cannot call getNumberOfRetransmissions. You're in uplink dude!");
}

bool
HARQRetransmissionProxy::hasFreeSenderProcess(wns::scheduler::UserID peer)
{
    assure(false, "This is only a proxy. You cannot call hasFreeSenderProcesses. You're in uplink dude!");
    return true;
}

bool
HARQRetransmissionProxy::hasFreeReceiverProcess(wns::scheduler::UserID peer)
{
    return downlinkHARQ_->hasFreeReceiverProcess(peer);
}

wns::scheduler::SchedulingTimeSlotPtr
HARQRetransmissionProxy::getNextRetransmission(wns::scheduler::UserID user, int processID)
{
    assure(false, "This is only a proxy. You cannot call getNextRetransmission. You're in uplink dude!");
}

wns::scheduler::SchedulingTimeSlotPtr
HARQRetransmissionProxy::peekNextRetransmission(wns::scheduler::UserID user, int processID) const
{
    assure(false, "This is only a proxy. You cannot call peekNextRetransmission. You're in uplink dude!");
}
