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

#ifndef WNS_LDK_ARQ_GOBACKN_HPP
#define WNS_LDK_ARQ_GOBACKN_HPP

#include <WNS/pyconfig/View.hpp>

#include <WNS/ldk/arq/ARQ.hpp>
#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/ldk/Delayed.hpp>
#include <WNS/ldk/SuspendableInterface.hpp>
#include <WNS/ldk/SuspendSupport.hpp>
#include <WNS/ldk/DelayedDeliveryInterface.hpp>
#include <WNS/ldk/fu/Plain.hpp>

#include <WNS/Cloneable.hpp>
#include <WNS/events/CanTimeout.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/probe/bus/ContextCollector.hpp>

#include <list>
#include <cmath>

namespace wns { namespace ldk { namespace arq {

    namespace tests
    {
        class GoBackNTest;
    }

    /**
     * @brief Command used by the GoBackN ARQ implementation.
     *
     * @author Rainer Schoenen <rs@comnets.rwth-aachen.de>
     */
    class GoBackNCommand :
        public ARQCommand
    {
    public:
        /**
         * I - Information Frame
         * ACK - received packet acknowledged (ACK)
         * NAK - certain packet missing (NAK)
         */
        typedef enum {I, ACK, NAK} FrameType;

        GoBackNCommand()
        {
            peer.type = I;
            peer.NS = 0;
        }

        void
        setNS(const SequenceNumber& SN)
        {
           peer.NS = SN;
        }

        SequenceNumber
        getNS() const
        {
            return peer.NS;
        }

        FrameType
        getFrameType() const
        {
            return peer.type;
        }

        bool
        isACK() const
        {
            return (peer.type == ACK) || (peer.type == NAK);
        }

        struct
        {
        }
        local;

        struct
        {
            FrameType type;
            int NS;
        }
        peer;

        struct
        {
        }
        magic;
    };

    /**
     * @brief GoBackN implementation of the ARQ interface.
     *
     * @author Rainer Schoenen <rs@comnets.rwth-aachen.de>
     */
    class GoBackN :
        public ARQ,
        public wns::ldk::fu::Plain<GoBackN, GoBackNCommand>,
        public Delayed<GoBackN>,
        virtual public SuspendableInterface,
        public SuspendSupport,
        virtual public DelayedDeliveryInterface,
        public events::CanTimeout
    {
        friend class ::wns::ldk::arq::tests::GoBackNTest;
        typedef std::list<CompoundPtr> CompoundContainer;

    public:
        // FUNConfigCreator interface realisation
        //GoBackN(FUN* fun, wns::pyconfig::View& config);
        GoBackN(fun::FUN* fuNet, const wns::pyconfig::View& config);

        virtual
        ~GoBackN();

        // CanTimeout interface realisation
        virtual void
        onTimeout();

        // Delayed interface realisation
        /**
         * @brief hasCapacity() is true if we
         * 1. don't have an active PDU we are processing
         * 2. aren't occupied with retransmissions
         * 3. don't exceed the sending window
         * So the gate is only open if we are free for something new
         * All other queueing must happen in a unit above us
         */
        virtual bool
        hasCapacity() const;

        /**
         * @brief Downstack on transmitter side
         */
        virtual void
        processOutgoing(const CompoundPtr& sdu);

        /**
         * @brief Upstack on receiver side
         */
        virtual void
        processIncoming(const CompoundPtr& compound);

        // ARQ interface realization
        /**
         * @brief hasACK() is asked by lower layer FU's
         */
        virtual const wns::ldk::CompoundPtr
        hasACK() const;

        /**
         * @brief hasData() is asked by lower layer FU's
         */
        virtual const wns::ldk::CompoundPtr
        hasData() const;

        virtual wns::ldk::CompoundPtr
        getACK();

        /**
         * @brief in retransmissionState: returns the HOL packet of	toRetransmit queue
         * in normal state: returns the (HOL) packet stored in activeCompound
         */
        virtual wns::ldk::CompoundPtr
        getData();

        // Overload of CommandTypeSpecifier Interface
        void
        calculateSizes(const CommandPool* commandPool, Bit& commandPoolSize, Bit& sduSize) const;

    protected:
        // Internal handlers for I- and ACK-Frames
        void
        onIFrame(const CompoundPtr& compound);

        void
        onACKFrame(const CompoundPtr& compound);

        void
        onNAKFrame(const CompoundPtr& compound);

        // remove ACKed PDU from list
        void
        removeACKed(const ARQCommand::SequenceNumber ackedNS, CompoundContainer& container);

        // prepare list of frames to retransmit
        void
        prepareRetransmission();

        // retransmissionState
        /**
         * @brief return whether we are in retransmission mode (Retransmit queue nonempty)
         */
        bool
        retransmissionState() const;

        /**
         * @brief logger output of sequence numbers
         */
        void
        show_seqnr_list(const char* name, CompoundContainer& compoundList) const;

        virtual bool
        onSuspend() const;

        virtual void
        doDelayDelivery();

        virtual void
        doDeliver();

        /**
         * @brief Window size (so many packets may be sent without ACKs).
         */
        int windowSize;

        /**
         * @brief Divisor/Modulo for Sequence Numbers (>=2*windowSize). Power of 2.
         */
        int sequenceNumberSize;

        /**
         * @brief Number of bits added to the header for each I-Frame
         */
        int bitsPerIFrame; // = (int) ceil(log(sequenceNumberSize) / log(2));

        /**
         * @brief Number of bits added to the header for each ACK-Frame
         */
        int bitsPerACKFrame; // = (int) ceil(log(sequenceNumberSize) / log(2));

        /**
         * @brief Sequence number of the last packet sent (Transmitter side).
         */
        ARQCommand::SequenceNumber NS;

        /**
         * @brief Sequence number of the last packet received in order (Rx side).
         */
        ARQCommand::SequenceNumber NR;

        /**
         * @brief Sequence number of the last ACK received in order (Tx side).
         */
        ARQCommand::SequenceNumber LA; // LA=1 means seqnr=0 was acked before

        /**
         * @brief PDU to be sent (Tx side).
         */
        CompoundPtr activeCompound;

        /**
         * @brief The packets sent but not acknowledged yet (Tx side).
         */
        CompoundContainer sentPDUs;

        /**
         * @brief List of packets to be retransmit (Tx side)
         */
        CompoundContainer toRetransmit;

        /**
         * @brief list of ACK PDU to be sent (Rx side).
         */
        CompoundContainer ackPDUs;

        /**
         * @brief received compounds (Rx side).
         */
        CompoundContainer receivedPDUs;

        /**
         * @brief Remember to send the activeCompound (transmit buffer nonidle).
         */
        bool sendNow;

        /**
         * @brief Time between two transmissions of the same PDU.
         */
        double resendTimeout;

        /**
         * @brief probe to count the number of retransmissions needed.
         * @todo unknownPerson: this should go into the base class when the probe
         * configuration works better.
         */
        wns::probe::bus::ContextCollectorPtr transmissionAttempts;

        bool delayingDelivery;

        ARQCommand::SequenceNumber delayedDeliveryNR;

        logger::Logger logger;

    };

}
}
}

#endif // NOT defined WNS_LDK_ARQ_GOBACKN_HPP


