/******************************************************************************
 * WinProSt Protocol Stack)                                                   *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2004                                                         *
 * Lehrstuhl f?r Kommunikationsnetze (ComNets)                                *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: msg@comnets.de, www: http://winner.comnets.rwth-aachen.de/~msg      *
 ******************************************************************************/

#ifndef WNS_LDK_SAR_SEGANDCONCAT_HPP
#define WNS_LDK_SAR_SEGANDCONCAT_HPP

#include <WNS/ldk/sar/reassembly/ReorderingWindow.hpp>
#include <WNS/ldk/sar/reassembly/ReassemblyBuffer.hpp>

#include <WNS/scheduler/queue/ISegmentationCommand.hpp>
#include <WNS/ldk/Delayed.hpp>
#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/probe/bus/ContextCollector.hpp>

namespace wns { namespace ldk { namespace sar { 

    class SegAndConcatCommand:
        public wns::scheduler::queue::ISegmentationCommand
    {
    public:
        SegAndConcatCommand()
        {
            peer.isBegin_ = false;
            peer.isEnd_ = false;
            peer.sn_ = 0;
            peer.headerSize_ = 0;
            peer.dataSize_ = 0;
            peer.paddingSize_ = 0;
            peer.sdus_ = 0;
        }

        /**
         * @brief Is the first byte of the datafield the beginning of
         * an SDU
         */
        virtual void
        setBeginFlag() { peer.isBegin_ = true; }

        virtual bool
        getBeginFlag() { return peer.isBegin_; }

        virtual void
        clearBeginFlag() { peer.isBegin_ = false; }

        /**
         * @brief Is the last byte of the datafield the end of
         * an SDU
         */
        virtual void
        setEndFlag() { peer.isEnd_ = true; }

        virtual bool
        getEndFlag() { return peer.isEnd_; }

        virtual void
        clearEndFlag() { peer.isEnd_ = false; }

        /**
         * @brief Set the Sequence number of this RLC PDU
         */
        virtual void
        setSequenceNumber(long sn) { peer.sn_ = sn;}

        /**
         * @brief The Sequence number of this RLC PDU
         */
        virtual long
        getSequenceNumber() { return peer.sn_; }

        virtual void
        increaseHeaderSize(Bit size) { peer.headerSize_ += size; }

        virtual void
        increaseDataSize(Bit size) { peer.dataSize_ += size; }

        virtual void
        increasePaddingSize(Bit size) { peer.paddingSize_ += size; }

        virtual Bit
        headerSize() { return peer.headerSize_; }

        virtual Bit
        dataSize() { return peer.dataSize_; }

        virtual Bit
        paddingSize() { return peer.paddingSize_; }

        virtual Bit
        totalSize() { return peer.headerSize_ + peer.dataSize_ + peer.paddingSize_; }

        /**
         * @brief Append an SDU segment to this PDU.
         */
        virtual void
        addSDU(wns::ldk::CompoundPtr c) { peer.pdus_.push_back(c); peer.sdus_++;}

        virtual unsigned int
        getNumSDUs() {return peer.sdus_;}

        struct {
        } magic;

        struct {} local;

        struct peer
        {
            bool isBegin_;
            bool isEnd_;
            long sn_;
            Bit headerSize_;
            Bit dataSize_;
            Bit paddingSize_;
            unsigned int sdus_;
            std::list<wns::ldk::CompoundPtr> pdus_;
        } peer;
    };

    class SegAndConcat:
        virtual public wns::ldk::FunctionalUnit,
        virtual public wns::ldk::Delayed<SegAndConcat>,
        public wns::ldk::HasReceptor<>,
        public wns::ldk::HasConnector<>,
        public wns::ldk::HasDeliverer<>,
        public wns::Cloneable<SegAndConcat>,
        public wns::ldk::CommandTypeSpecifier<SegAndConcatCommand>
    {
    public:
        SegAndConcat(wns::ldk::fun::FUN* fuNet, const wns::pyconfig::View& config);

        SegAndConcat(const SegAndConcat&);

        virtual
        ~SegAndConcat();

        virtual void
        onFUNCreated();

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

        virtual void
        calculateSizes(const wns::ldk::CommandPool* commandPool, Bit& commandPoolSize, Bit& dataSize) const;

    protected:

	wns::ldk::CommandReaderInterface*
	getCommandReader() const;

        wns::logger::Logger logger_;
        bool isSegmenting_;

    private:

        void
        onReorderedPDU(long, wns::ldk::CompoundPtr);

        void
        onDiscardedPDU(long, wns::ldk::CompoundPtr);

        std::list<wns::ldk::CompoundPtr> senderPendingSegments_;

        std::string commandName_;

        Bit headerSize_;

        Bit segmentSize_;

        Bit sduLengthAddition_;

        long nextOutgoingSN_;

        reassembly::ReassemblyBuffer reassemblyBuffer_;

        reassembly::ReorderingWindow reorderingWindow_;

        wns::probe::bus::ContextCollectorPtr minDelayCC_;
        wns::probe::bus::ContextCollectorPtr maxDelayCC_;
        wns::probe::bus::ContextCollectorPtr sizeCC_;
        wns::ldk::CommandReaderInterface* probeHeaderReader_;

        wns::probe::bus::ContextCollectorPtr segmentDropRatioCC_;
        std::string segmentDropRatioProbeName_;
    };
}
}
}

#endif // WNS_LDK_SAR_SEGANDCONCAT_HPP
