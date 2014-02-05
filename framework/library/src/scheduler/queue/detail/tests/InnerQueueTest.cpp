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

#include <WNS/scheduler/queue/detail/InnerQueue.hpp>

#include <WNS/ldk/helper/FakePDU.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/ldk/fu/Plain.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/scheduler/queue/ISegmentationCommand.hpp>

#include <WNS/TestFixture.hpp>

#include <cppunit/extensions/HelperMacros.h>

// Some Macros to make it easier

// Create a compound that contains a FakePDU of specified SIZE
#define CREATECOMPOUND(FUN, SIZE) new wns::ldk::Compound((FUN)->getProxy()->createCommandPool(), wns::osi::PDUPtr(new wns::ldk::helper::FakePDU(SIZE)) )

// Pass a COMPOUND through the classifier FU effectively setting the CID
#define SETCID(COMPOUND, CID) classifier_->setNextCID(CID);classifier_->sendData(COMPOUND);

namespace wns { namespace scheduler { namespace queue { namespace detail { namespace tests {

class SegmentingCommandStub:
    public wns::scheduler::queue::ISegmentationCommand
{
public:
    SegmentingCommandStub()
    {
        peer.sn_ = 0;
        peer.isBegin_ = false;
        peer.isEnd_ = false;
        peer.headerSize_ = 0;
        peer.dataSize_ = 0;
        peer.paddingSize_ = 0;
    }

    virtual void
    setBeginFlag() { peer.isBegin_ = true; }

    virtual bool
    getBeginFlag() { return peer.isBegin_; }

    virtual void
    clearBeginFlag() { peer.isBegin_ = false; }

    virtual void
    setEndFlag() { peer.isEnd_ = true; }

    virtual bool
    getEndFlag() { return peer.isEnd_; }

    virtual void
    clearEndFlag() { peer.isEnd_ = false; }

    virtual void
    setSequenceNumber(long sn) { peer.sn_ = sn;}

    virtual long
    getSequenceNumber() { return peer.sn_; }

    virtual void
    increaseHeaderSize(Bit size) { peer.headerSize_ += size; }

    virtual Bit
    headerSize() { return peer.headerSize_; }

    virtual void
    increaseDataSize(Bit size) { peer.dataSize_ += size; }

    virtual Bit
    dataSize() { return peer.dataSize_; }

    virtual void
    increasePaddingSize(Bit size) { peer.paddingSize_ += size; }

    virtual Bit
    paddingSize() { return peer.paddingSize_; }

    virtual Bit
    totalSize() { return peer.headerSize_ + peer.dataSize_ + peer.paddingSize_; }

    virtual void
    addSDU(wns::ldk::CompoundPtr c) { peer.pdus_.push_back(c); }

    virtual unsigned int
    getNumSDUs() {return peer.pdus_.size();}

    struct {
        long sn_;
        bool isBegin_;
        bool isEnd_;
        Bit headerSize_;
        Bit dataSize_;
        Bit paddingSize_;
        std::list<wns::ldk::CompoundPtr> pdus_;
    } peer;

    struct {
    } local;

    struct {
    } magic;
};

class SegmentingCommandFUStub:
    public wns::ldk::fu::Plain<SegmentingCommandFUStub, SegmentingCommandStub>
{
public:
    SegmentingCommandFUStub(wns::ldk::fun::FUN* fun):
        wns::ldk::fu::Plain<SegmentingCommandFUStub, SegmentingCommandStub>(fun) {}

    virtual bool
    doIsAccepting(const wns::ldk::CompoundPtr&) const { return false; }

    virtual void
    doSendData(const wns::ldk::CompoundPtr&) {}

    virtual void
    doWakeup() {}

    virtual void
    doOnData(const wns::ldk::CompoundPtr&) {}

    virtual void
    calculateSizes(const wns::ldk::CommandPool* commandPool, Bit& commandPoolSize, Bit& dataSize) const
    {
        SegmentingCommandStub* command;
        command = getCommand(commandPool);

        commandPoolSize = command->peer.headerSize_;
        dataSize = command->peer.dataSize_ + command->peer.paddingSize_;
    }
};

class InnerQueueTest:
    public wns::TestFixture
{
    CPPUNIT_TEST_SUITE( InnerQueueTest );
    CPPUNIT_TEST( testConstruction );
    CPPUNIT_TEST( testNettoBitsInQueue );
    CPPUNIT_TEST( testBruttoBitsInQueue );
    CPPUNIT_TEST( testCompoundsInQueue );
    CPPUNIT_TEST( testQueueIsEmpty );
    CPPUNIT_TEST( testRetrieveBelowFixedHeaderSizeThrows );
    CPPUNIT_TEST( testRetrieveFragment );
    CPPUNIT_TEST_SUITE_END();

public:

    void
    prepare();

    void
    cleanup();

    void
    testConstruction();

    void
    testNettoBitsInQueue();

    void
    testBruttoBitsInQueue();

    void
    testCompoundsInQueue();

    void
    testQueueIsEmpty();

    void
    testRetrieveBelowFixedHeaderSizeThrows();

    void
    testRetrieveFragment();

    wns::scheduler::queue::detail::InnerQueue* testee_;

    wns::ldk::ILayer* layer_;

    wns::ldk::fun::FUN* fun_;

    SegmentingCommandFUStub* commandFU_;
};

} // tests
} // detail
} // queue
} // scheduler
} // wns

using namespace wns::scheduler::queue::detail::tests;

CPPUNIT_TEST_SUITE_REGISTRATION( InnerQueueTest );

void
InnerQueueTest::prepare()
{
    wns::ldk::CommandProxy::clearRegistries();

    testee_ = new wns::scheduler::queue::detail::InnerQueue();

    layer_ = new wns::ldk::tests::LayerStub();

    fun_ = new wns::ldk::fun::Main(layer_);

    commandFU_ = new SegmentingCommandFUStub(fun_);

    fun_->addFunctionalUnit("test.commandFUName", "test.commandFUName", commandFU_);
}

void
InnerQueueTest::cleanup()
{
    delete testee_;

    delete fun_;

    delete layer_;
}

void
InnerQueueTest::testConstruction()
{
    CPPUNIT_ASSERT_EQUAL(Bit(0), testee_->queuedNettoBits());

    // No matter which header sizes are given, if queue is empty
    // then brutto bits is empty, too
    CPPUNIT_ASSERT_EQUAL(Bit(0), testee_->queuedBruttoBits(2334, 5654, false));

    // No matter which header sizes are given, if queue is empty
    // then brutto bits is empty, too. This must also hold if byteAlignHeader is set
    CPPUNIT_ASSERT_EQUAL(Bit(0), testee_->queuedBruttoBits(2334, 5654, true));
    
}

void
InnerQueueTest::testNettoBitsInQueue()
{
    CPPUNIT_ASSERT_EQUAL(Bit(0), testee_->queuedNettoBits());

    wns::ldk::CompoundPtr compound1(CREATECOMPOUND(fun_, 16));

    testee_->put(compound1);

    CPPUNIT_ASSERT_EQUAL(Bit(16), testee_->queuedNettoBits());

    wns::ldk::CompoundPtr compound2(CREATECOMPOUND(fun_, 9));

    testee_->put(compound2);

    CPPUNIT_ASSERT_EQUAL(Bit(25), testee_->queuedNettoBits());
}

void
InnerQueueTest::testBruttoBitsInQueue()
{
    CPPUNIT_ASSERT_EQUAL(Bit(0), testee_->queuedBruttoBits(987887, 2983, false));

    wns::ldk::CompoundPtr compound1(CREATECOMPOUND(fun_, 1034));

    testee_->put(compound1);

    CPPUNIT_ASSERT_EQUAL(Bit(1050), testee_->queuedBruttoBits(16, 23, false));

    wns::ldk::CompoundPtr compound2(CREATECOMPOUND(fun_, 234));

    testee_->put(compound2);

    // 4 Bit extra for byte alignment of the header 18 + 34 + 4 = 56; 56 % 8 == 0
    CPPUNIT_ASSERT_EQUAL(Bit(1320 + 4), testee_->queuedBruttoBits(18, 34, true));
}

void
InnerQueueTest::testCompoundsInQueue()
{
    CPPUNIT_ASSERT_EQUAL(0, testee_->queuedCompounds());

    wns::ldk::CompoundPtr compound1(CREATECOMPOUND(fun_, 1034));

    testee_->put(compound1);

    CPPUNIT_ASSERT_EQUAL(1, testee_->queuedCompounds());

    wns::ldk::CompoundPtr compound2(CREATECOMPOUND(fun_, 1034));

    testee_->put(compound2);

    CPPUNIT_ASSERT_EQUAL(2, testee_->queuedCompounds());

    wns::ldk::CompoundPtr pdu = testee_->retrieve(1100, 16, 32, false, false, fun_->getCommandReader("test.commandFUName"));

    CPPUNIT_ASSERT_EQUAL(1, testee_->queuedCompounds());

    pdu = testee_->retrieve(1100, 16, 32, false, false, fun_->getCommandReader("test.commandFUName"));

    CPPUNIT_ASSERT_EQUAL(0, testee_->queuedCompounds());
}

void
InnerQueueTest::testQueueIsEmpty()
{
    CPPUNIT_ASSERT_EQUAL(true, testee_->empty());

    wns::ldk::CompoundPtr compound1(CREATECOMPOUND(fun_, 1034));

    testee_->put(compound1);

    CPPUNIT_ASSERT_EQUAL(false, testee_->empty());

    wns::ldk::CompoundPtr compound2(CREATECOMPOUND(fun_, 1034));

    testee_->put(compound2);

    CPPUNIT_ASSERT_EQUAL(false, testee_->empty());

    wns::ldk::CompoundPtr pdu = testee_->retrieve(2600, 16, 32, false, false, fun_->getCommandReader("test.commandFUName"));

    CPPUNIT_ASSERT_EQUAL(true, testee_->empty());
}

void
InnerQueueTest::testRetrieveBelowFixedHeaderSizeThrows()
{
    CPPUNIT_ASSERT_THROW( testee_->retrieve(8, 16, 32, false, false, NULL), InnerQueue::RequestBelowMinimumSize );
}

void
InnerQueueTest::testRetrieveFragment()
{
    CPPUNIT_ASSERT_EQUAL(Bit(0), testee_->queuedNettoBits());

    wns::ldk::CompoundPtr compound1(CREATECOMPOUND(fun_, 1034));

    testee_->put(compound1);

    wns::ldk::CompoundPtr pdu = testee_->retrieve(200, 16, 32, false, false, fun_->getCommandReader("test.commandFUName"));

    SegmentingCommandStub* header = NULL;
    header = this->commandFU_->getCommand(pdu->getCommandPool());

    CPPUNIT_ASSERT_EQUAL(Bit(850), testee_->queuedNettoBits());
    CPPUNIT_ASSERT_EQUAL(Bit(867), testee_->queuedBruttoBits(17,32, false));
    CPPUNIT_ASSERT_EQUAL((long) 0, header->getSequenceNumber());
    CPPUNIT_ASSERT_EQUAL(true, header->getBeginFlag());
    CPPUNIT_ASSERT_EQUAL(false, header->getEndFlag());
    CPPUNIT_ASSERT_EQUAL((size_t) 1, header->peer.pdus_.size());
    CPPUNIT_ASSERT_EQUAL(Bit(16), header->headerSize());
    CPPUNIT_ASSERT_EQUAL(Bit(184), header->dataSize());
    CPPUNIT_ASSERT_EQUAL(Bit(200), pdu->getLengthInBits());

    pdu = testee_->retrieve(18, 17, 31, false, false, fun_->getCommandReader("test.commandFUName"));

    header = NULL;
    header = this->commandFU_->getCommand(pdu->getCommandPool());

    CPPUNIT_ASSERT_EQUAL(Bit(849), testee_->queuedNettoBits());
    CPPUNIT_ASSERT_EQUAL(Bit(868), testee_->queuedBruttoBits(19,32, false));
    CPPUNIT_ASSERT_EQUAL((long) 1, header->getSequenceNumber());
    CPPUNIT_ASSERT_EQUAL(false, header->getBeginFlag());
    CPPUNIT_ASSERT_EQUAL(false, header->getEndFlag());
    CPPUNIT_ASSERT_EQUAL((size_t) 1, header->peer.pdus_.size());
    CPPUNIT_ASSERT_EQUAL(Bit(17), header->headerSize());
    CPPUNIT_ASSERT_EQUAL(Bit(1), header->dataSize());
    CPPUNIT_ASSERT_EQUAL(Bit(18), pdu->getLengthInBits());

    // Retrieve all remaining data (pdu size - data size of first sdu - data size of second sdu)
    pdu = testee_->retrieve(1034 - 184 - 1, 0, 30, false, false, fun_->getCommandReader("test.commandFUName"));

    header = NULL;
    header = this->commandFU_->getCommand(pdu->getCommandPool());

    CPPUNIT_ASSERT_EQUAL(Bit(0), testee_->queuedNettoBits());
    CPPUNIT_ASSERT_EQUAL(Bit(0), testee_->queuedBruttoBits(20,32, false));
    CPPUNIT_ASSERT_EQUAL((long) 2, header->getSequenceNumber());
    CPPUNIT_ASSERT_EQUAL(false, header->getBeginFlag());
    CPPUNIT_ASSERT_EQUAL(true, header->getEndFlag());
    CPPUNIT_ASSERT_EQUAL((size_t) 1, header->peer.pdus_.size());
    CPPUNIT_ASSERT_EQUAL(Bit(0), header->headerSize());
    CPPUNIT_ASSERT_EQUAL(Bit(849), header->dataSize());
    CPPUNIT_ASSERT_EQUAL(Bit(849), pdu->getLengthInBits());

    wns::ldk::CompoundPtr compound2(CREATECOMPOUND(fun_, 10));

    testee_->put(compound2);

    wns::ldk::CompoundPtr compound3(CREATECOMPOUND(fun_, 21));

    testee_->put(compound3);

    // Retrieve all and test if padding works (just give a brutto number well
    // above user data + header + extension header)
    pdu = testee_->retrieve(5000, 8, 29, true, false, fun_->getCommandReader("test.commandFUName"));

    header = NULL;
    header = this->commandFU_->getCommand(pdu->getCommandPool());

    CPPUNIT_ASSERT_EQUAL(Bit(0), testee_->queuedNettoBits());
    CPPUNIT_ASSERT_EQUAL(Bit(0), testee_->queuedBruttoBits(20,32, false));
    CPPUNIT_ASSERT_EQUAL((long) 3, header->getSequenceNumber());
    CPPUNIT_ASSERT_EQUAL(true, header->getBeginFlag());
    CPPUNIT_ASSERT_EQUAL(true, header->getEndFlag());
    CPPUNIT_ASSERT_EQUAL((size_t) 2, header->peer.pdus_.size());
    CPPUNIT_ASSERT_EQUAL(Bit(37), header->headerSize());
    CPPUNIT_ASSERT_EQUAL(Bit(31), header->dataSize());
    CPPUNIT_ASSERT_EQUAL(Bit(4932), header->paddingSize());
    CPPUNIT_ASSERT_EQUAL(Bit(5000), pdu->getLengthInBits());
}
