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

#ifndef WNS_LDK_ARQ_STOPANDWAIT_HPP
#define WNS_LDK_ARQ_STOPANDWAIT_HPP

#include <WNS/ldk/arq/ARQ.hpp>
#include <WNS/ldk/Delayed.hpp>
#include <WNS/ldk/SuspendableInterface.hpp>
#include <WNS/ldk/SuspendSupport.hpp>
#include <WNS/ldk/fu/Plain.hpp>

#include <WNS/events/CanTimeout.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>

namespace wns { namespace ldk { namespace arq {

    /**
     * @brief Command used by the StopAndWait ARQ implementation.
     */
    class StopAndWaitCommand :
        public ARQCommand
    {
    public:
        /**
         * I - Information Frame (Data)
         * RR - Receive Ready Frame (ACK)
         */
        typedef enum {I, RR} FrameType;

        StopAndWaitCommand()
        {
            peer.type = I;
            peer.NS = 0;
        }

        struct
        {
        }
        local;

        struct
        {
            // Typically 1 Bit in reality
            FrameType type;
            // Typically 1 Bit in reality
            SequenceNumber NS;
        }
        peer;

        struct
        {
        }
        magic;

        // ARQCommand interface realization
        virtual bool
        isACK() const
        {
            return peer.type == RR;
        }
    };


    /**
     * @brief StopAndWait implementation of the ARQ interface.
     */
    class StopAndWait :
        public ARQ,
        public wns::ldk::fu::Plain<StopAndWait, StopAndWaitCommand>,
        public Delayed<StopAndWait>,
        virtual public SuspendableInterface,
        public SuspendSupport,
        public events::CanTimeout
    {
    public:
        // FUNConfigCreator interface realisation
        StopAndWait(fun::FUN* fuNet, const wns::pyconfig::View& config);

        virtual
        ~StopAndWait();

        // CanTimeout interface realisation
        virtual void
        onTimeout();

        // Delayed interface realisation
        virtual bool
        hasCapacity() const;

        virtual void
        processOutgoing(const CompoundPtr& sdu);

        virtual void
        processIncoming(const CompoundPtr& compound);

        // ARQ interface realization
        virtual const wns::ldk::CompoundPtr
        hasACK() const;

        virtual const wns::ldk::CompoundPtr
        hasData() const;

        virtual wns::ldk::CompoundPtr
        getACK();

        virtual wns::ldk::CompoundPtr
        getData();

        // SDU and PCI size calculation
        void
        calculateSizes(const CommandPool* commandPool, Bit& commandPoolSize, Bit& dataSize) const;

    protected:
        virtual bool
        onSuspend() const;

        /**
         * @brief Time between two transmissions of the same PDU.
         */
        double resendTimeout;

        /**
         * @brief Number of bits added to the header for each I-Frame
         */
        int bitsPerIFrame;

        /**
         * @brief Number of bits added to the header for each RR-Frame
         */
        int bitsPerRRFrame;

        /**
         * @brief Sequence number of the last packet sent.
         */
        ARQCommand::SequenceNumber NS;

        /**
         * @brief Sequence number of the last packet received.
         */
        ARQCommand::SequenceNumber NR;

        /**
         * @brief The last packet sent but not acknowledged yet.
         *
         * If the activeCompound is set to CompoundPtr(), this means no
         * Compound is currently waiting for acknowledgment
         */
        CompoundPtr activeCompound;

        /**
         * @brief An ACK to be sent.
         */
        CompoundPtr ackCompound;

        /**
         * @brief Remember to send the activeCompound.
         */
        bool sendNow;

        logger::Logger logger;
    };

}
}
}

#endif // NOT defined WNS_LDK_ARQ_STOPANDWAIT_HPP


