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

#ifndef WNS_SCHEDULER_HARQ_HARQRETRANSMISSIONPROXY_HPP
#define WNS_SCHEDULER_HARQ_HARQRETRANSMISSIONPROXY_HPP

#include <WNS/scheduler/harq/HARQInterface.hpp>

namespace wns { namespace scheduler { namespace harq {

    /**
     * @brief HARQRetransmissionProxy is the instance of collaborating with the
     * uplink master scheduler. Will forward requests on peer retransmissions to
     * a HARQ instance in a downlink scheduler. Will throw for everything else.
     */
    class HARQRetransmissionProxy:
        public wns::scheduler::harq::HARQInterface
    {
    public:

        HARQRetransmissionProxy(const wns::pyconfig::View&);

        virtual ~HARQRetransmissionProxy();

        virtual void
        setDownlinkHARQ(HARQInterface* downlinkHARQ);

        /**
        * @brief Returns the peers that have pending retransmissions
        * Intended for uplink master scheduling.
        */
        virtual wns::scheduler::UserSet
        getPeersWithPendingRetransmissions() const;

        std::list<int>
        getPeerProcessesWithRetransmissions(wns::scheduler::UserID peer) const;
        
        /**
        * @brief Return the number of retransmissions that are pending for a peer
        * Intended for uplink master scheduling
        */
        virtual int
        getNumberOfPeerRetransmissions(wns::scheduler::UserID peer, int processID) const;
        
        virtual void
        schedulePeerRetransmissions(wns::scheduler::UserID peer, int processID);
        
        virtual void
        schedulePeerRetransmission(wns::scheduler::UserID peer, int processID);

        virtual void
        sendPendingFeedback();

        /**
         * @brief Disabled in HARQRetransmissionProxy
         */
        virtual void
        storeSchedulingTimeSlot(long int transportBlockID, const wns::scheduler::SchedulingTimeSlotPtr&);

        /**
         * @brief Disabled in HARQRetransmissionProxy
         */
        virtual void
        onTimeSlotReceived(const wns::scheduler::SchedulingTimeSlotPtr&, HARQInterface::TimeSlotInfo);

        virtual HARQInterface::DecodeStatusContainer
        decode();

        virtual wns::scheduler::UserSet
	getUsersWithRetransmissions() const;

        virtual std::list<int>
	getProcessesWithRetransmissions(wns::scheduler::UserID peer) const;

        virtual int
	getNumberOfRetransmissions(wns::scheduler::UserID, int processID);

        virtual bool
        hasFreeSenderProcess(wns::scheduler::UserID peer);

        virtual bool
        hasFreeReceiverProcess(wns::scheduler::UserID peer);

        /**
         * @brief Disabled in HARQRetransmissionProxy
         */
        virtual wns::scheduler::SchedulingTimeSlotPtr
        getNextRetransmission(wns::scheduler::UserID user, int processID);

        /**
         * @brief Disabled in HARQRetransmissionProxy
         */
        virtual wns::scheduler::SchedulingTimeSlotPtr
        peekNextRetransmission(wns::scheduler::UserID user, int processID) const;

    private:
        wns::logger::Logger logger_;

        HARQInterface* downlinkHARQ_;
};

} // harq
} // scheduler
} // wns

#endif // WNS_SCHEDULER_HARQ_HARQRETRANSMISSIONPROXY_HPP
