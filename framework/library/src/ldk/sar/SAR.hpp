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

#ifndef WNS_LDK_SAR_HPP
#define WNS_LDK_SAR_HPP

#include <WNS/logger/Logger.hpp>

#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/ldk/Delayed.hpp>
#include <WNS/ldk/SuspendableInterface.hpp>
#include <WNS/ldk/SuspendSupport.hpp>

#include <WNS/pyconfig/View.hpp>

#include <list>

namespace wns { namespace ldk { namespace sar {

    class SARCommand :
        public Command
    {
    public:
        SARCommand()
        {
            peer.lastFragment = true;
            magic.pos = -1;
            magic.fragmentNumber = -1;
            magic.segmentSize = -1;
            magic.preserving = true;
        } // SARCommand

        struct
        {
        }
        local;

        struct
        {
            bool lastFragment;
        }
        peer;

        struct
        {
            int pos;
            int fragmentNumber;
            int segmentSize;
            bool preserving;
        } magic;
    };

    /**
     * @brief Segmentation and Reassembly
     *
     * SAR supports changing the segment size during the segmentation of a PDU.
     * If you use this feature, you should configure the header size to a sensible value.
     */
    template <typename COMMANDTYPE>
    class SAR :
        virtual public FunctionalUnit,
        virtual public DelayedInterface,
        public CommandTypeSpecifier<COMMANDTYPE>,
        virtual public SuspendableInterface,
        public SuspendSupport
    {
    public:
        SAR(fun::FUN* fuNet, const wns::pyconfig::View& config) :
                CommandTypeSpecifier<COMMANDTYPE>(fuNet),
                SuspendSupport(fuNet, config),
                outgoing(),
                nextSegment(),
                nextPos(),
                totalSize(),
                segmentSize(config.get<Bit>("segmentSize")),
                headerSize(config.get<Bit>("headerSize")),
                preserving(config.get<bool>("preserving")),
                logger(config.get("logger"))
        {
        } // SAR

        virtual
        ~SAR()
        {
            outgoing = CompoundPtr();
            nextSegment = CompoundPtr();
            backupCompound = CompoundPtr();
        } // ~SAR


        //
        //  interface
        //

        void setSegmentSize(const Bit size)
        {
            MESSAGE_BEGIN(NORMAL, logger, m, "");
            m
                << "changing segment size from "
                << segmentSize
                << " to "
                << size
                << " bits.";
            MESSAGE_END();

            // undo preparation of last segment
            Bit capacity = segmentSize - this->getCommandSize() - ( preserving ? oldPCISize : 0 );
            nextPos -= capacity;
            --fragmentNumber;

            if(outgoing == CompoundPtr() && nextSegment != CompoundPtr() )
            {
                outgoing = backupCompound->copy();
            }

            nextSegment = CompoundPtr();

            // set new segment size
            segmentSize = size;


            // and re-do the preparation
            prepareSegment();

        } // setMTU

        Bit getSegmentSize() const
        {
            return segmentSize;
        } // getSegmentSize

        virtual void processOutgoing(const CompoundPtr& compound)
        {
            assure(hasCapacity(), "processOutgoing called although not accepting.");

            outgoing = compound;
            backupCompound = compound->copy();

            nextPos = 0;
            fragmentNumber = 0;

            CommandPool* commandPool = outgoing->getCommandPool();

            getFUN()->calculateSizes(commandPool,
                         oldPCISize,
                         oldSDUSize);

            totalSize = oldPCISize + oldSDUSize;

            MESSAGE_BEGIN(NORMAL, logger, m, "");
            m
                << "size of previous Commands: "
                << oldPCISize
                << " size of SDU: "
                << oldSDUSize
                << " - cutting "
                << totalSize
                << " bits into segments of " << segmentSize << " bits.";
            MESSAGE_END();

            prepareSegment();
        } // processOutgoing

        virtual CompoundPtr getSomethingToSend()
        {
            assure(hasSomethingToSend(), "getSomethingToSend called although there's nothing to send.");

            CompoundPtr returnValue = nextSegment; // store next segment as
                                                   // return value
            prepareSegment(); // and prepare the subsequent segment

            return returnValue;
        } // getSomethingToSend

        // partial implementation of delayed
        virtual bool hasCapacity() const
        {
            return (outgoing == CompoundPtr() && nextSegment == CompoundPtr());
        } // hasCapacity

        virtual const CompoundPtr hasSomethingToSend() const
        {
            trySuspend();

            return nextSegment;
        }

        //
        // PCI size calculation
        //
        Bit
        getCommandSize() const
        {
            return headerSize;
        }

        //
        // PDU size modification
        //
        virtual void calculateSizes(const CommandPool* commandPool, Bit& commandPoolSize, Bit& dataSize) const
        {
            getFUN()->calculateSizes(commandPool, commandPoolSize, dataSize, this);

            // now we have the size of all previous Commands and of the original Compound

            COMMANDTYPE* command = this->getCommand(commandPool);

            Bit capacity = command->magic.segmentSize - this->getCommandSize() - ( command->magic.preserving ? commandPoolSize : 0 );

            if(command->peer.lastFragment)
            {
                dataSize = dataSize - command->magic.pos + ( command->magic.preserving ? 0 : commandPoolSize );
            } else
                dataSize = capacity;

            // overwrite the commandPoolSize, because after the segmentation, only our own
            // command is visible until the reassembly is performed.
            // :FIXME: at a later point, we may also have to deal with flows :-(
            if (command->magic.preserving)
                commandPoolSize += this->getCommandSize();
            else
                commandPoolSize = this->getCommandSize();
        }


    protected:
        CompoundPtr outgoing;
        CompoundPtr nextSegment;
        /**
           @todo (msg, ksw, swen) This should be removed. The problem
           here is that the Delayed interface should have two methods to
           be called in advance and after getSomethingToSend(),
           respectively.
        */
        CompoundPtr backupCompound;

        Bit nextPos;
        int fragmentNumber;

        Bit totalSize;
        Bit oldPCISize;
        Bit oldSDUSize;

        Bit segmentSize;
        Bit headerSize;
        bool preserving;

        logger::Logger logger;

        virtual
        void prepareSegment()
        {
            if (!outgoing)
            {
                // nothing to do
                nextSegment = wns::ldk::CompoundPtr();

                return;
            }

            SARCommand* command;

            Bit capacity = segmentSize - this->getCommandSize() - ( preserving ? oldPCISize : 0 );
            MESSAGE_BEGIN(NORMAL, logger, m, "");
            m << "SAR: capacity=" << capacity;
            m << ", Commandsize=" << this->getCommandSize();
            MESSAGE_END();
            if( totalSize - nextPos > capacity )
            {
                nextSegment = outgoing->copy();
                command = activateCommand(nextSegment->getCommandPool());
                command->peer.lastFragment = false;
                command->magic.pos = nextPos;
                nextPos += capacity;
                command->magic.fragmentNumber = fragmentNumber;
                ++fragmentNumber;
                command->magic.segmentSize = segmentSize;
                command->magic.preserving = preserving;
            }

            else

            {
                nextSegment = outgoing;
                command = activateCommand(nextSegment->getCommandPool());
                command->peer.lastFragment = true;
                command->magic.pos = nextPos;
                nextPos += capacity;
                command->magic.fragmentNumber = fragmentNumber;
                command->magic.segmentSize = segmentSize;
                command->magic.preserving = preserving;
                ++fragmentNumber;
                outgoing = CompoundPtr();
            }

            this->commitSizes(nextSegment->getCommandPool());

            MESSAGE_BEGIN(NORMAL, logger, m, "");
            m   << "#" << command->magic.fragmentNumber
                << ", length="
                << nextSegment->getLengthInBits()
                << " last="
                << ( command->peer.lastFragment ? "yes" : "no");
            if (!command->peer.lastFragment)
                m << " nextPos=" << nextPos;
            MESSAGE_END();
        }

    private:
        virtual bool
        onSuspend() const
        {
            return hasCapacity();
        }

    };
}
}
}

#endif // NOT defined WNS_LDK_SAR_HPP


