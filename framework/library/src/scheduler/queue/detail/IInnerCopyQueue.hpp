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

#ifndef WNS_SCHEDULER_QUEUE_DETAIL_INNERCOPYQUEUE_HPP
#define WNS_SCHEDULER_QUEUE_DETAIL_INNERCOPYQUEUE_HPP

#include <WNS/ldk/Compound.hpp>
#include <WNS/simulator/Bit.hpp>
#include <WNS/scheduler/SchedulerTypes.hpp>
#include <WNS/scheduler/queue/detail/InnerQueue.hpp>
#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <queue>
#include <map>

namespace wns { namespace scheduler { namespace queue { namespace detail {

    class IInnerCopyQueue
    {
    public:
        virtual void
        setFUN(wns::ldk::fun::FUN* fun) = 0;
        
        virtual bool
        knowsCID(ConnectionID cid) = 0;
    
        virtual bool
        isEmpty(ConnectionID cid) = 0;
    
        virtual wns::ldk::CompoundPtr
        getPDU(ConnectionID cid, Bit bit = 0) = 0;
    
        virtual Bit
        getHeadofLinePDUBit(ConnectionID cid) = 0;
    
        virtual void
        reset(ConnectionID cid) = 0;
    
        virtual int
        getSize(ConnectionID cid) = 0;
    
        virtual int
        getSizeInBit(ConnectionID cid) = 0;
    
        virtual void
        setQueue(ConnectionID cid, std::queue<wns::ldk::CompoundPtr> queue) = 0;
    };
    
    class SimpleInnerCopyQueue:
        public IInnerCopyQueue
    {
    public:
        SimpleInnerCopyQueue();
        
        ~SimpleInnerCopyQueue();

        virtual void
        setFUN(wns::ldk::fun::FUN* fun){};

        virtual bool
        knowsCID(ConnectionID cid);

        virtual bool
        isEmpty(ConnectionID cid);

        virtual wns::ldk::CompoundPtr
        getPDU(ConnectionID cid, Bit bit = 0);
    
        virtual Bit
        getHeadofLinePDUBit(ConnectionID cid);
    
        virtual void
        reset(ConnectionID cid);
    
        virtual int
        getSize(ConnectionID cid);
    
        virtual int
        getSizeInBit(ConnectionID cid);

        virtual void
        setQueue(ConnectionID cid, std::queue<wns::ldk::CompoundPtr> queue);

    private:
        std::map<wns::scheduler::ConnectionID, std::queue<wns::ldk::CompoundPtr> > queue_; 
        std::map<wns::scheduler::ConnectionID, int > queueSize_; 
    };
    
    class SegmentingInnerCopyQueue:
        public IInnerCopyQueue
    {
    public:
        SegmentingInnerCopyQueue(const wns::pyconfig::View& _config);
        
        ~SegmentingInnerCopyQueue();
        
        virtual void
        setFUN(wns::ldk::fun::FUN* fun);

        virtual bool
        knowsCID(ConnectionID cid);
    
        virtual bool
        isEmpty(ConnectionID cid);

        virtual wns::ldk::CompoundPtr
        getPDU(ConnectionID cid, Bit bit);
    
        virtual Bit
        getHeadofLinePDUBit(ConnectionID cid);
    
        virtual void
        reset(ConnectionID cid);
    
        virtual int
        getSize(ConnectionID cid);
    
        virtual int
        getSizeInBit(ConnectionID cid);
    
        virtual void
        setQueue(ConnectionID cid, std::queue<wns::ldk::CompoundPtr> queue);

        unsigned long int
        getMinimumSegmentSize();
    
    private:
        std::map<ConnectionID, InnerQueue> queue_;

        Bit fixedHeaderSize_;
        Bit extensionHeaderSize_;
        unsigned long int minimumSegmentSize_;
        bool usePadding_;
        bool byteAlignHeader_;
        std::string segmentHeaderCommandName_;

        wns::ldk::CommandReaderInterface* segmentHeaderReader_;
};

} // detail
} // queue
} // scheduler
} // wns

#endif // WNS_SCHEDULER_QUEUE_DETAIL_INNERCOPYQUEUE_HPP
