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

#ifndef CUMULATIVEACK_HPP
#define CUMULATIVEACK_HPP

#include <WNS/ldk/arq/ARQ.hpp>
#include <WNS/ldk/fu/Plain.hpp>
#include <WNS/ldk/Delayed.hpp>
#include <WNS/ldk/SuspendableInterface.hpp>
#include <WNS/ldk/SuspendSupport.hpp>
#include <WNS/ldk/DelayedDeliveryInterface.hpp>

#include <WNS/ldk/fun/FUN.hpp>

#include <WNS/events/CanTimeout.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/SmartPtr.hpp>
#include <WNS/logger/Logger.hpp>

#include <vector>

namespace wns { namespace ldk { namespace arq {

/// Command used by the CumulativeACK arq implementation.
    class CumulativeACKCommand :
        public ARQCommand
    {
        public:
            CumulativeACKCommand()
        {
            peer.type = I;
            peer.NS = 0;
            peer.NR = 0;
        }
        /// Control frame types.
        typedef enum {I, RR, SREJ} FrameType;

        bool isACK() const
        {
            return (peer.type == RR) || (peer.type == SREJ);
        }

        struct
        {
        }
        local;
        
        struct
        {
            FrameType type;
            unsigned long int NS;
            unsigned long int NR; /// in case of AckPDU the number of the received PDU
        }
        peer;
        
        struct
        {
        }
        magic;

    };

    /// CumulativeACK implementation of the ARQ interface.
    /**
     * @todo Piggyback feasibility
     * @todo Implementation of SREJ Function if a SelectiveRejectFct is needed
     * @todo check if default copy c'tor is ok here
     */
    class CumulativeACK :
        public ARQ,
        public wns::ldk::fu::Plain<CumulativeACK, CumulativeACKCommand>,
        public Delayed<CumulativeACK>,
        virtual public SuspendableInterface,
        public SuspendSupport,
        virtual public DelayedDeliveryInterface,
        public events::CanTimeout
    {
    private:
        /// An element for a PDU and its ARQ-Attributes .
        class CAElement:
            public events::CanTimeout
        {
            public:
                CAElement();
                CAElement(CumulativeACK* _parent, unsigned int Time=1 ):
                    parent(_parent),
                    resendTimeout(Time),
                    compound(CompoundPtr()),
                    sendNow(false),
                    logger("WNS", "CumulativeACK")
                    {
                    };

                ~CAElement()
                {
                    //std::cout<<" Jawoi CAEl destructor has been called ! "<<std::endl;
                    parent = NULL;
                }

                void onTimeout()
                {
                    assure(compound, "no PDU during onTimeout.");
                    parent->statusCollector->onFailedTransmission(this->compound);
                    sendNow = true;
                    MESSAGE_BEGIN(NORMAL, logger, m, parent->getFUN()->getName());
                    m << " Timeout of CAElement NS -> " << parent->getCommand(compound->getCommandPool())->peer.NS;
                    MESSAGE_END();
                    parent->tryToSend();
                }

            wns::ldk::CompoundPtr getCompound(){return compound;};

            CumulativeACK* parent;
            unsigned int resendTimeout;
            wns::ldk::CompoundPtr compound;
            //true if PDU is supposed to be sent. Not being used in case of receiving
            bool sendNow;
            logger::Logger logger;

        };
        ///@brief Container for the compounds and their Attributes
        typedef std::vector<CAElement> CAElements;
        
        public:
            /// FUNConfigCreator interface realisation
            CumulativeACK(fun::FUN* fuNet, const wns::pyconfig::View& config);

            ~CumulativeACK();

            /// CanTimeout interface realisation
            virtual void onTimeout()
            {
            }

            // Delayed interface realisation
            virtual bool hasCapacity() const;
            virtual void processOutgoing(const CompoundPtr& sdu);
            // virtual const CompoundPtr hasSomethingToSend() const; // implemented by ARQ
            // virtual CompoundPtr getSomethingToSend(); // implemented by ARQ
            virtual void processIncoming(const CompoundPtr& compound);

            /// ARQ interface realization
            virtual const wns::ldk::CompoundPtr hasACK() const;
            virtual const wns::ldk::CompoundPtr hasData() const;
            virtual wns::ldk::CompoundPtr getACK();
            virtual wns::ldk::CompoundPtr getData();

            // Overload of CommandTypeSpecifier Interface
            void calculateSizes(const CommandPool* commandPool, Bit& commandPoolSize, Bit& sduSize) const;

            virtual bool onSuspend() const;

        protected:
            virtual void doDelayDelivery();
            virtual void doDeliver();

            ///ACK PDU
            wns::ldk::CompoundPtr ackCompound;
            /// Window size.
            unsigned long int wS;
            /// Sequence number of the next packet to be sent.
            unsigned long int NS;
            /// Sequence number of the first packet expected to  be acknowledged.
            unsigned long int NSack;
            /// Sequence number of the next packet expected to be received.
            unsigned long int NR;
            /// Divisor for Sequence Numbers.
            int sequenceNumberSize;
            /// Container for compounds with their Atributes.
            CAElements receivingCompounds;
            CAElements sendingCompounds;
            /// Time between two transmissions of the same PDU.
            double resendTimeout;

            bool delayingDelivery;
            int delayedDeliveryNR;

            logger::Logger logger;
        };
    }
}
}
#endif
