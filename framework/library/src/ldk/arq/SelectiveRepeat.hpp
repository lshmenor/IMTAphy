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

#ifndef WNS_LDK_ARQ_SELECTIVEREPEAT_HPP
#define WNS_LDK_ARQ_SELECTIVEREPEAT_HPP

#include <WNS/pyconfig/View.hpp>

#include <WNS/ldk/arq/ARQ.hpp>
#include <WNS/ldk/Delayed.hpp>
#include <WNS/ldk/SuspendableInterface.hpp>
#include <WNS/ldk/SuspendSupport.hpp>
#include <WNS/ldk/DelayedDeliveryInterface.hpp>
#include <WNS/ldk/fu/Plain.hpp>

#include <WNS/logger/Logger.hpp>
#include <WNS/probe/bus/ContextCollector.hpp>
#include <WNS/events/CanTimeout.hpp>

#include <list>
#include <cmath>

namespace wns { namespace ldk { namespace arq {

    namespace tests
    {
        class SelectiveRepeatTest;
    }

    /**
     * @brief Command used by the SelectiveRepeat arq implementation.
     *
     */
    class SelectiveRepeatCommand :
        public ARQCommand
    {
    public:
        /*
         * I - Information Frame
         * ACK - received Packet (ACK)
         */
        typedef enum {I, ACK} FrameType;

        SelectiveRepeatCommand()
        {
            peer.type = I;
            peer.ns = 0;
            local.lastSentTime = 0.0;
            local.firstSentTime = 0.0;
            magic.ackSentTime = 0.0;
        }

        void setNS(SequenceNumber sn)
        {
            peer.ns = sn;
        }

        SequenceNumber getNS() const
        {
            return peer.ns;
        }

        bool isACK() const
        {
            return peer.type == ACK;
        }

        struct
        {
            simTimeType lastSentTime;
            simTimeType firstSentTime;
        }
        local;
        struct
        {
            FrameType type;
            SequenceNumber ns;
        }
        peer;
        struct
        {
            simTimeType ackSentTime;
        } magic;
    };


    /**
     * @brief SelectiveRepeat implementation of the ARQ interface.
     *
     * This implementation was made according to the implementation of the
     * SelectiveRepeat ARQ algorithm as described by Fred Halsall in "Data
     * Communications, Computer Networks and open systems".
     *
     * @note The mentioned
     * implementation relies on the in-order delivery of
     * ACKnowledgements. i.e. if an ACKnowledgement other than the next one
     * expected is received by the Sender, a packet loss is considered to be
     * the reason and the sender enters retransmission state. This makes
     * this ARQ scheme unusable in applications where the in-order-delivery
     * can not be guaranteed (e.g. in a TCP implementation). Consider using
     * N-channel Stop and Wait in that case.
     *
     * @note: the following modification has been made to make this
     * implementation work in an environment where the Sender can receive the
     * ACKs for multiple packets at the same point in time: If an
     * out-of-sequence ACK arrives, previous (unacked) packets are only
     * retransmitted if (a) they have not been retransmitted before or (b)
     * the previous retransmission took place more than 2*minRTT ago. The
     * minRTT is measured by the protocol itself.
     */
    class SelectiveRepeat :
        public ARQ,
        public wns::ldk::fu::Plain<SelectiveRepeat, SelectiveRepeatCommand>,
        public Delayed<SelectiveRepeat>,
        virtual public SuspendableInterface,
        public SuspendSupport,
        virtual public DelayedDeliveryInterface,
        public events::CanTimeout
    {
        friend class tests::SelectiveRepeatTest;
        typedef std::list<CompoundPtr> CompoundContainer;

    public:
        // FUNConfigCreator interface realisation
        SelectiveRepeat(fun::FUN* fuNet, const wns::pyconfig::View& config);
        ~SelectiveRepeat();

        // CanTimeout interface realisation
        virtual void onTimeout();

        // Delayed interface realisation
        virtual bool hasCapacity() const;
        virtual void processOutgoing(const CompoundPtr& sdu);
        // virtual const CompoundPtr hasSomethingToSend() const; // implemented by ARQ
        // virtual CompoundPtr getSomethingToSend(); // implemented by ARQ
        virtual void processIncoming(const CompoundPtr& compound);

        // ARQ interface realization
        virtual const wns::ldk::CompoundPtr hasACK() const;
        virtual const wns::ldk::CompoundPtr hasData() const;
        virtual wns::ldk::CompoundPtr getACK();
        virtual wns::ldk::CompoundPtr getData();

        // Overload of CommandTypeSpecifier Interface
        void calculateSizes(const CommandPool* commandPool, Bit& commandPoolSize, Bit& sduSize) const;

    protected:
        // Internal handlers for I- and ACK-Frames
        void onIFrame(const CompoundPtr& compound);
        void onACKFrame(const CompoundPtr& compound);

        // sort into given Compound List
        void keepSorted(const CompoundPtr& compound, CompoundContainer& conatiner);

        // remove ACKed PDU from list
        void removeACKed(const CompoundPtr& ackCompound, CompoundContainer& container);

        // prepare list of frames to retransmit
        void prepareRetransmission();

        // retransmissionState
        bool retransmissionState() const;

        virtual bool onSuspend() const;

        virtual void doDelayDelivery();
        virtual void doDeliver();

        /**
         * @brief Window size.
         */
        int windowSize;

        /**
         * @brief Divisor for Sequence Numbers.
         */
        int sequenceNumberSize;

        /**
         * @brief Number of Bits the command occupies in the Commandpool.
         */
        int commandSize;

        /**
         * @brief Sequence number of the last packet sent.
         */
        ARQCommand::SequenceNumber NS;

        /**
         * @brief Sequence number of the last packet received in order.
         */
        ARQCommand::SequenceNumber NR;

        /**
         * @brief Sequence number of the last ACK received in order.
         */
        ARQCommand::SequenceNumber LA;

        /**
         * @brief PDU to be sent.
         */
        CompoundPtr activeCompound;

        /**
         * @brief The packets sent but not acknowledged yet.
         */
        CompoundContainer sentPDUs;

        /**
         * @brief List of packets to be retransmit
         */
        CompoundContainer toRetransmit;

        /**
         * @brief list of ACK PDU to be sent.
         */
        CompoundContainer ackPDUs;

        /**
         * @brief received compounds.
         */
        CompoundContainer receivedPDUs;

        /**
         * @brief received out-of-sequence ACKs.
         */
        CompoundContainer receivedACKs;

        /**
         * @brief Remember to send the activeCompound.
         */
        bool sendNow;

        /**
         * @brief Idle (no ACK) time after which sender enters
         * retransmission state.
         */
        double resendTimeout;

        /**
         * @brief min time to be waiting between two retransmissions of
         * the same packet.
         */
        double retransmissionInterval;

        /**
         * @brief probe to count the number of retransmissions needed.
         * :TODO: this should go into the base class when the probe
         * configuration works better.
         */
        wns::probe::bus::ContextCollectorPtr transmissionAttemptsProbeBus;
        wns::probe::bus::ContextCollectorPtr ackDelayProbeBus;
        wns::probe::bus::ContextCollectorPtr roundTripTimeProbeBus;

        bool delayingDelivery;

        logger::Logger logger;

    };

}
}
}

#endif // NOT defined WNS_LDK_ARQ_SELECTIVEREPEAT_HPP


