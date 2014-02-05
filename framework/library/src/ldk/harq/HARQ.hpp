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

#ifndef WNS_LDK_HARQ_HARQ_HPP
#define WNS_LDK_HARQ_HARQ_HPP

#include <WNS/ldk/harq/softcombining/Container.hpp>
#include <WNS/ldk/harq/softcombining/IDecoder.hpp>
#include <WNS/ldk/Compound.hpp>

#include <WNS/ldk/fu/Plain.hpp>
#include <WNS/ldk/Delayed.hpp>

namespace wns { namespace ldk { namespace harq {

    class HARQCommand :
        public Command
    {
    public:

        typedef enum
        {
            I,
            ACK,
            NACK,
            INVALID
        } FrameType;

        HARQCommand()
        {
            peer.type = INVALID;
            peer.NDI = false;
            peer.processId = -1;
            peer.rv = 0;
        }

        struct
        {
        }
        local;

        struct
        {
            FrameType type;
            bool NDI;
            int processId;
            int rv;
        }
        peer;

        struct
        {
        }
        magic;

    };

    class HARQFU :
            public fu::Plain<HARQFU, HARQCommand>,
            public Delayed<HARQFU>
    {
    public:
        HARQFU(fun::FUN* fuNet, const wns::pyconfig::View& config);

        ~HARQFU();

        virtual void
        onFUNCreated();

        // SDU and PCI size calculation
        void
        calculateSizes(const CommandPool* commandPool,
                       Bit& commandPoolSize,
                       Bit& dataSize) const;

    private:

        class HARQSenderProcess
        {
        public:

            HARQSenderProcess(int processID, int numRVs, HARQFU* entity, wns::logger::Logger logger);

            bool
            hasCapacity() const;

            void
            enqueueTransmission(const wns::ldk::CompoundPtr& compound);

            void
            ackReceived();

            void
            nackReceived();

        private:
            int processID_;

            int numRVs_;

            HARQFU* entity_;

            wns::logger::Logger logger_;

            wns::ldk::CompoundPtr buffer_;

            bool waitingForFeedback_;

            int retransmissionCounter_;

        };

        class HARQReceiverProcess
        {
        public:
            HARQReceiverProcess(wns::pyconfig::View config, int processID, HARQFU* entity);

            void
            receive(const wns::ldk::CompoundPtr&);

            virtual void
            onFUNCreated();

        private:
            int processID_;

            int numRVs_;

            HARQFU* entity_;

            wns::logger::Logger logger_;

            softcombining::Container<wns::ldk::CompoundPtr> receptionBuffer_;

            wns::SmartPtr<softcombining::IDecoder> decoder_;
        };

        virtual void
        processIncoming(const wns::ldk::CompoundPtr& compound);

        virtual void
        processOutgoing(const wns::ldk::CompoundPtr&);

        virtual bool
        hasCapacity() const;

        virtual const wns::ldk::CompoundPtr
        hasSomethingToSend() const;

        virtual wns::ldk::CompoundPtr
        getSomethingToSend();

        void
        addToSendQueue(wns::ldk::CompoundPtr compound);

        wns::logger::Logger logger_;

        int numSenderProcesses_;

        int numReceiverProcesses_;

        int numRVs_;

        std::vector<HARQSenderProcess> senderProcesses_;

        std::vector<HARQFU::HARQReceiverProcess> receiverProcesses_;

        std::list<wns::ldk::CompoundPtr> sendQueue_;
    };

} // harq
} // ldk
} // wns

#endif // WNS_LDK_HARQ_HARQ_HPP
