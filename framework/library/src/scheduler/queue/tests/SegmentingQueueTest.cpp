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

#include <WNS/scheduler/queue/SegmentingQueue.hpp>
#include <WNS/pyconfig/Parser.hpp>
#include <WNS/ldk/helper/FakePDU.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/ldk/tools/Stub.hpp>
#include <WNS/ldk/fu/Plain.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/scheduler/tests/RegistryProxyStub.hpp>
#include <WNS/scheduler/tests/ClassifierPolicyDropIn.hpp>

#include <WNS/TestFixture.hpp>

#include <cppunit/extensions/HelperMacros.h>

// Some Macros to make it easier

// Create a compound that contains a FakePDU of specified SIZE
#define CREATECOMPOUND(FUN, SIZE) new wns::ldk::Compound((FUN)->getProxy()->createCommandPool(), wns::osi::PDUPtr(new wns::ldk::helper::FakePDU(SIZE)) )

// Pass a COMPOUND through the classifier FU effectively setting the CID
#define SETCID(COMPOUND, CID) classifier_->setNextCID(CID);classifier_->sendData(COMPOUND);

namespace wns { namespace scheduler { namespace queue { namespace tests {

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

class SegmentingQueueTest:
    public wns::TestFixture
{
    CPPUNIT_TEST_SUITE( SegmentingQueueTest );
    CPPUNIT_TEST( testConstruction );
    CPPUNIT_TEST( testDynamicSegmentationFlag );
    CPPUNIT_TEST( testTestHelper );
    CPPUNIT_TEST( testIsAccepting );
    CPPUNIT_TEST( testGetHeadOfLinePDUThrows );
    CPPUNIT_TEST( testSegmentConcatenate );
    CPPUNIT_TEST( testSegmentConcatenateByteAlign );
    CPPUNIT_TEST( testGetHeadOfLinePDUbits );
    CPPUNIT_TEST( testNumBitsForCid );
    CPPUNIT_TEST( testQueueStatus );
    CPPUNIT_TEST( testMultipleCIDs );
    CPPUNIT_TEST_SUITE_END();

public:

    void
    prepare();

    void
    latePrepare();

    void
    cleanup();

    void
    testConstruction();

    void
    testDynamicSegmentationFlag();

    void
    testTestHelper();

    void
    testIsAccepting();

    void
    testGetHeadOfLinePDUThrows();

    void
    testSegmentConcatenate();

    void
    testSegmentConcatenateByteAlign();

    void
    testGetHeadOfLinePDUbits();

    void
    testNumBitsForCid();

    void
    testQueueStatus();

    void
    testMultipleCIDs();

    SegmentingQueue* testee_;

    wns::ldk::ILayer* layer_;

    wns::ldk::fun::FUN* fun_;

    wns::scheduler::tests::RegistryProxyStub* registryProxy_;

    wns::ldk::Classifier<wns::scheduler::tests::ClassifierPolicyDropIn>* classifier_;

    wns::ldk::tools::Stub* lower_;

    SegmentingCommandFUStub* commandFU_;

    bool usesPadding_;

    std::string alternateConfig;
};

CPPUNIT_TEST_SUITE_REGISTRATION( SegmentingQueueTest );

} // tests
} // queue
} // scheduler
} // wns

using namespace wns::scheduler::queue::tests;

void SegmentingQueueTest::prepare()
{
}

void SegmentingQueueTest::latePrepare()
{
    wns::pyconfig::Parser config;
    if (alternateConfig == "")
    {
        config.loadString(
                           "from openwns.Scheduler import SegmentingQueue\n"
                           "testee = SegmentingQueue(\"test.commandFUName\", \"test.commandFUName\")\n"
                           "testee.fixedHeaderSize = 16\n"
                           "testee.extensionHeaderSize = 8\n"
                           "testee.logger.level = 2\n"
                         );
    }
    else
    {
        config.loadString(alternateConfig);
    }

    usesPadding_ = false;

    wns::pyconfig::View configView(config, "testee");

    testee_ = new wns::scheduler::queue::SegmentingQueue(NULL, configView);

    layer_ = new wns::ldk::tests::LayerStub();

    fun_ = new wns::ldk::fun::Main(layer_);

    registryProxy_ = new wns::scheduler::tests::RegistryProxyStub();

    // Queue size is controlled by the registry
    // Here we explicitely set it via the stub
    registryProxy_->setQueueSizeLimitPerConnection(128);

    // Compound Classifier
    wns::pyconfig::Parser emptyConfig;
    classifier_ =
        new wns::ldk::Classifier<wns::scheduler::tests::ClassifierPolicyDropIn>(fun_, emptyConfig);

    lower_ = new wns::ldk::tools::Stub(fun_, emptyConfig);

    commandFU_ = new SegmentingCommandFUStub(fun_);

    fun_->addFunctionalUnit("classifier", classifier_);

    fun_->addFunctionalUnit("lower", lower_);

    fun_->addFunctionalUnit("test.commandFUName", "test.commandFUName", commandFU_);

    classifier_->connect(lower_);

    testee_->setFUN(fun_);

    testee_->setColleagues(registryProxy_);

    registryProxy_->setFriends(dynamic_cast<wns::ldk::CommandTypeSpecifier<wns::ldk::ClassifierCommand>* >(classifier_));
}

void
SegmentingQueueTest::cleanup()
{
    delete testee_;

    delete fun_;

    delete layer_;

    delete registryProxy_;
}

void
SegmentingQueueTest::testConstruction()
{
    latePrepare();
}

void
SegmentingQueueTest::testDynamicSegmentationFlag()
{
    latePrepare();
    bool flag = testee_->supportsDynamicSegmentation();
    CPPUNIT_ASSERT_EQUAL(true, flag);
}

void
SegmentingQueueTest::testTestHelper()
{
    latePrepare();
    wns::ldk::CompoundPtr compound1(CREATECOMPOUND(fun_, 64));
    SETCID(compound1, 1);

    CPPUNIT_ASSERT_EQUAL(Bit(64), compound1->getLengthInBits());
    CPPUNIT_ASSERT_EQUAL((size_t) 1, lower_->sent.size());
    wns::ldk::ClassifierCommand* command1 =
        classifier_->getCommand( compound1->getCommandPool() );
    CPPUNIT_ASSERT_EQUAL((long) 1, command1->peer.id);
}

void
SegmentingQueueTest::testIsAccepting()
{
    latePrepare();
    wns::ldk::CompoundPtr compound1(CREATECOMPOUND(fun_, 64));
    SETCID(compound1, 1);

    wns::ldk::CompoundPtr compound2(CREATECOMPOUND(fun_, 32));
    SETCID(compound2, 1);

    wns::ldk::CompoundPtr compound3(CREATECOMPOUND(fun_, 128));
    SETCID(compound3, 1);

    wns::ldk::CompoundPtr compound4(CREATECOMPOUND(fun_, 128));
    SETCID(compound4, 2);

    bool accepted = testee_->isAccepting(compound1);
    CPPUNIT_ASSERT_EQUAL(true, accepted);
    testee_->put(compound1);

    accepted = testee_->isAccepting(compound2);
    CPPUNIT_ASSERT_EQUAL(true, accepted);
    testee_->put(compound2);

    accepted = testee_->isAccepting(compound3);
    CPPUNIT_ASSERT_EQUAL(false, accepted);

    accepted = testee_->isAccepting(compound4);
    CPPUNIT_ASSERT_EQUAL(true, accepted);
    testee_->put(compound4);
}

void
SegmentingQueueTest::testGetHeadOfLinePDUThrows()
{
    latePrepare();
    wns::ldk::CompoundPtr compound1(CREATECOMPOUND(fun_, 64));
    SETCID(compound1, 1);

    bool accepted = testee_->isAccepting(compound1);
    CPPUNIT_ASSERT_EQUAL(true, accepted);
    testee_->put(compound1);

    // This is the non-segmenting interface. It cannot be used if
    // supportsDynamicSegmentation return true. So an assure should
    // reflect that.
    WNS_ASSERT_ASSURE_EXCEPTION( testee_->getHeadOfLinePDU(1) );
}

void
SegmentingQueueTest::testSegmentConcatenate()
{
    latePrepare();
    //      ---------------------------------------------
    // Front|PDU1 16|PDU2 64                 |PDU3 32   |
    //      ---------------------------------------------
    wns::ldk::CompoundPtr compound1(CREATECOMPOUND(fun_, 16));
    SETCID(compound1, 4);
    testee_->put(compound1);
    wns::ldk::CompoundPtr compound2(CREATECOMPOUND(fun_, 64));
    SETCID(compound2, 4);
    testee_->put(compound2);
    wns::ldk::CompoundPtr compound3(CREATECOMPOUND(fun_, 32));
    SETCID(compound3, 4);
    testee_->put(compound3);

    // First Segment 48
    // -------------------------------------------------
    // |Fixed Header 16|Extension 8|PDU1 16| Seg1PDU2 8|
    // -------------------------------------------------
    // Queue after First Segment
    //      -------------------------------------
    // Front|RestPDU2 56             |PDU3 32   |
    //      -------------------------------------
    wns::ldk::CompoundPtr seg1 = testee_->getHeadOfLinePDUSegment(4, 48);
    CPPUNIT_ASSERT_EQUAL(Bit(48), seg1->getLengthInBits());
    SegmentingCommandStub* commandSeg1 = commandFU_->getCommand(seg1->getCommandPool());
    CPPUNIT_ASSERT_EQUAL(true, commandSeg1->getBeginFlag());
    CPPUNIT_ASSERT_EQUAL(false, commandSeg1->getEndFlag());
    CPPUNIT_ASSERT_EQUAL((long) 0, commandSeg1->getSequenceNumber());
    CPPUNIT_ASSERT_EQUAL(Bit(24), commandSeg1->peer.headerSize_);
    CPPUNIT_ASSERT_EQUAL(Bit(24), commandSeg1->peer.dataSize_);
    CPPUNIT_ASSERT_EQUAL(Bit(0), commandSeg1->peer.paddingSize_);
    CPPUNIT_ASSERT_EQUAL((size_t)(2), commandSeg1->peer.pdus_.size());

    testee_->frameStarts();

    // Second Segment 48
    // -----------------------------
    // |Fixed Header 16|Seg2PDU2 32|
    // -----------------------------
    // Queue after Second Segment
    //      --------------------------
    // Front|RestPDU2 24  |PDU3 32   |
    //      --------------------------
    wns::ldk::CompoundPtr seg2 = testee_->getHeadOfLinePDUSegment(4, 48);
    CPPUNIT_ASSERT_EQUAL(Bit(48), seg2->getLengthInBits());
    SegmentingCommandStub* commandSeg2 = commandFU_->getCommand(seg2->getCommandPool());
    CPPUNIT_ASSERT_EQUAL(false, commandSeg2->getBeginFlag());
    CPPUNIT_ASSERT_EQUAL(false, commandSeg2->getEndFlag());
    CPPUNIT_ASSERT_EQUAL((long) 1, commandSeg2->getSequenceNumber());
    CPPUNIT_ASSERT_EQUAL(Bit(16), commandSeg2->peer.headerSize_);
    CPPUNIT_ASSERT_EQUAL(Bit(32), commandSeg2->peer.dataSize_);
    CPPUNIT_ASSERT_EQUAL(Bit(0), commandSeg2->peer.paddingSize_);
    CPPUNIT_ASSERT_EQUAL((size_t)(1), commandSeg2->peer.pdus_.size());

    testee_->frameStarts();

    // Third Segment 64
    // -----------------------------------------------------
    // |Fixed Header 16|Extension 8|Seg3PDU2 24|Seg1PDU3 16|
    // -----------------------------------------------------
    // Queue after Third Segment
    //      ----------------
    // Front|RestPDU3 16   |
    //      ----------------
    wns::ldk::CompoundPtr seg3 = testee_->getHeadOfLinePDUSegment(4, 64);
    CPPUNIT_ASSERT_EQUAL(Bit(64), seg3->getLengthInBits());
    SegmentingCommandStub* commandSeg3 = commandFU_->getCommand(seg3->getCommandPool());
    CPPUNIT_ASSERT_EQUAL(false, commandSeg3->getBeginFlag());
    CPPUNIT_ASSERT_EQUAL(false, commandSeg3->getEndFlag());
    CPPUNIT_ASSERT_EQUAL((long) 2, commandSeg3->getSequenceNumber());
    CPPUNIT_ASSERT_EQUAL(Bit(24), commandSeg3->peer.headerSize_);
    CPPUNIT_ASSERT_EQUAL(Bit(40), commandSeg3->peer.dataSize_);
    CPPUNIT_ASSERT_EQUAL(Bit(0), commandSeg3->peer.paddingSize_);
    CPPUNIT_ASSERT_EQUAL((size_t)(2), commandSeg3->peer.pdus_.size());

    testee_->frameStarts();

    // Fourth Segment 48
    // ----------------------------------------
    // |Fixed Header 16|Seg2PDU2 16|Padding 16|
    // ----------------------------------------
    // Queue after Fourth Segment
    //      --
    // Front||
    //      --
    wns::ldk::CompoundPtr seg4 = testee_->getHeadOfLinePDUSegment(4, 48);
    if (this->usesPadding_ == true)
    {
        CPPUNIT_ASSERT_EQUAL(Bit(48), seg4->getLengthInBits());
    }
    else
    {
        CPPUNIT_ASSERT_EQUAL(Bit(32), seg4->getLengthInBits());
    }
    SegmentingCommandStub* commandSeg4 = commandFU_->getCommand(seg4->getCommandPool());
    CPPUNIT_ASSERT_EQUAL(false, commandSeg4->getBeginFlag());
    CPPUNIT_ASSERT_EQUAL(true, commandSeg4->getEndFlag());
    CPPUNIT_ASSERT_EQUAL((long) 3, commandSeg4->getSequenceNumber());
    CPPUNIT_ASSERT_EQUAL(Bit(16), commandSeg4->peer.headerSize_);
    CPPUNIT_ASSERT_EQUAL(Bit(16), commandSeg4->peer.dataSize_);
    if (this->usesPadding_ == true)
    {
        CPPUNIT_ASSERT_EQUAL(Bit(16), commandSeg4->peer.paddingSize_);
    }
    else
    {
        CPPUNIT_ASSERT_EQUAL(Bit(0), commandSeg4->peer.paddingSize_);
    }

    CPPUNIT_ASSERT_EQUAL((size_t)(1), commandSeg4->peer.pdus_.size());
}

void
SegmentingQueueTest::testSegmentConcatenateByteAlign()
{
    alternateConfig = "from openwns.Scheduler import SegmentingQueue\n"
        "testee = SegmentingQueue(\"test.commandFUName\", \"test.commandFUName\")\n"
        "testee.fixedHeaderSize = 8\n"
        "testee.extensionHeaderSize = 12\n"
        "testee.byteAlignHeader = True\n"
        "testee.logger.level = 2\n";

    latePrepare();
    //      -----------------------------------------------------
    // Front|PDU1 16|PDU2 64                 |PDU3 32   |PDU4 32|
    //      -----------------------------------------------------
    wns::ldk::CompoundPtr compound1(CREATECOMPOUND(fun_, 16));
    SETCID(compound1, 4);
    testee_->put(compound1);
    wns::ldk::CompoundPtr compound2(CREATECOMPOUND(fun_, 64));
    SETCID(compound2, 4);
    testee_->put(compound2);
    wns::ldk::CompoundPtr compound3(CREATECOMPOUND(fun_, 32));
    SETCID(compound3, 4);
    testee_->put(compound3);
    wns::ldk::CompoundPtr compound4(CREATECOMPOUND(fun_, 16));
    SETCID(compound4, 4);
    testee_->put(compound4);

    // If there are two segments in a PDU with have one extension header
    // The header would thus be 8+12=20 bit long. This is not byte align and
    // the implementation shall add 4 bit to make it byte aligned at 24bit
    wns::ldk::CompoundPtr seg1 = testee_->getHeadOfLinePDUSegment(4, 48);

    CPPUNIT_ASSERT_EQUAL(Bit(48), seg1->getLengthInBits());

    SegmentingCommandStub* commandSeg1 = commandFU_->getCommand(seg1->getCommandPool());
    CPPUNIT_ASSERT_EQUAL(Bit(24), commandSeg1->peer.headerSize_);
    CPPUNIT_ASSERT_EQUAL(Bit(24), commandSeg1->peer.dataSize_);

    testee_->frameStarts();

    // Get the rest
    wns::ldk::CompoundPtr seg2 = testee_->getHeadOfLinePDUSegment(4, 512);

    CPPUNIT_ASSERT_EQUAL(Bit(56 + 32 + 16 + 8 + 12 + 12), seg2->getLengthInBits());

    SegmentingCommandStub* commandSeg2 = commandFU_->getCommand(seg2->getCommandPool());
    CPPUNIT_ASSERT_EQUAL(Bit(8 + 12 + 12), commandSeg2->peer.headerSize_);
    CPPUNIT_ASSERT_EQUAL(Bit(56 + 32 + 16), commandSeg2->peer.dataSize_);
}

void
SegmentingQueueTest::testGetHeadOfLinePDUbits()
{
    latePrepare();
    //      ---------------------------------------------
    // Front|PDU1 16|PDU2 64                 |PDU3 32   |
    //      ---------------------------------------------
    wns::ldk::CompoundPtr compound1(CREATECOMPOUND(fun_, 16));
    SETCID(compound1, 4);
    testee_->put(compound1);
    wns::ldk::CompoundPtr compound2(CREATECOMPOUND(fun_, 64));
    SETCID(compound2, 4);
    testee_->put(compound2);
    wns::ldk::CompoundPtr compound3(CREATECOMPOUND(fun_, 32));
    SETCID(compound3, 4);
    testee_->put(compound3);

    wns::ldk::CompoundPtr seg3 = testee_->getHeadOfLinePDUSegment(4, 19);

    testee_->frameStarts();

    //      ---------------------------------------------
    // Front|SegPDU1 13|PDU2 64                 |PDU3 32   |
    //      ---------------------------------------------

    // getHeadOfLinePDUbits should deliver brutto bits in queue
    // i.e. netto + fixedHeader(16) + (N-1) * extensionHeader(16)
    // 13 + 64 + 32 + 16 + 2*8 = 141
    Bit queuedEffectiveBits = testee_->getHeadOfLinePDUbits(4);
    // If it was a single segment
    // 1 * fixedHeaderSize + SegPDU1 Length
    CPPUNIT_ASSERT_EQUAL( Bit(141), queuedEffectiveBits);
}

void
SegmentingQueueTest::testNumBitsForCid()
{
    latePrepare();
    //      ---------------------------------------------
    // Front|PDU1 16|PDU2 64                 |PDU3 32   |
    //      ---------------------------------------------
    wns::ldk::CompoundPtr compound1(CREATECOMPOUND(fun_, 16));
    SETCID(compound1, 4);
    testee_->put(compound1);
    wns::ldk::CompoundPtr compound2(CREATECOMPOUND(fun_, 64));
    SETCID(compound2, 4);
    testee_->put(compound2);
    wns::ldk::CompoundPtr compound3(CREATECOMPOUND(fun_, 32));
    SETCID(compound3, 4);
    testee_->put(compound3);

    Bit queuedEffectiveBits = testee_->numBitsForCid(4);
    // If it was a single segment
    // 1 * fixedHeaderSize + 2 * extension Header + Total PDU Length
    CPPUNIT_ASSERT_EQUAL( Bit(16 + 16 + 16 + 64 + 32), queuedEffectiveBits);
}

void
SegmentingQueueTest::testQueueStatus()
{
    latePrepare();
    //      ---------------------------------------------
    // Front|PDU1 16|PDU2 64                 |PDU3 32   |
    //      ---------------------------------------------
    wns::ldk::CompoundPtr compound1(CREATECOMPOUND(fun_, 16));
    SETCID(compound1, 4);
    testee_->put(compound1);
    wns::ldk::CompoundPtr compound2(CREATECOMPOUND(fun_, 64));
    SETCID(compound2, 4);
    testee_->put(compound2);
    wns::ldk::CompoundPtr compound3(CREATECOMPOUND(fun_, 32));
    SETCID(compound3, 4);
    testee_->put(compound3);

    QueueStatusContainer qstat = testee_->getQueueStatus(false);

    CPPUNIT_ASSERT_EQUAL(true, qstat.knows(4));

    CPPUNIT_ASSERT_EQUAL(false, qstat.knows(17));

    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(112 + 16 + 2*8), qstat.find(4).numOfBits);

    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(3), qstat.find(4).numOfCompounds);
}

void
SegmentingQueueTest::testMultipleCIDs()
{
    latePrepare();
    //      ---------------------------------------------
    // Front|PDU1 16|PDU2 64                 |PDU3 32   |   (CID 4)
    //      ---------------------------------------------
    wns::ldk::CompoundPtr compound1(CREATECOMPOUND(fun_, 16));
    SETCID(compound1, 4);
    testee_->put(compound1);
    wns::ldk::CompoundPtr compound2(CREATECOMPOUND(fun_, 64));
    SETCID(compound2, 4);
    testee_->put(compound2);
    wns::ldk::CompoundPtr compound3(CREATECOMPOUND(fun_, 32));
    SETCID(compound3, 4);
    testee_->put(compound3);

    //      ---------------------------------------------
    // Front|PDU1 16|PDU2 64                 |PDU3 32   |   (CID 17)
    //      ---------------------------------------------
    wns::ldk::CompoundPtr compound4(CREATECOMPOUND(fun_, 16));
    SETCID(compound4, 17);
    testee_->put(compound4);
    wns::ldk::CompoundPtr compound5(CREATECOMPOUND(fun_, 64));
    SETCID(compound5, 17);
    testee_->put(compound5);
    wns::ldk::CompoundPtr compound6(CREATECOMPOUND(fun_, 32));
    SETCID(compound6, 17);
    testee_->put(compound6);

    Bit queuedEffectiveBits = testee_->numBitsForCid(4);
    CPPUNIT_ASSERT_EQUAL( Bit(16 + 64 + 32 + 16 + 2*8), queuedEffectiveBits);
    queuedEffectiveBits = testee_->numBitsForCid(17);
    CPPUNIT_ASSERT_EQUAL( Bit(16 + 64 + 32 + 16 + 2*8), queuedEffectiveBits);

    wns::ldk::CompoundPtr seg1 = testee_->getHeadOfLinePDUSegment(4, 19);

    wns::ldk::CompoundPtr seg2 = testee_->getHeadOfLinePDUSegment(17, 42);

    // Now the queue should not include the fixed header
    // we did not signal frameStart()
    queuedEffectiveBits = testee_->numBitsForCid(4);
    CPPUNIT_ASSERT_EQUAL( Bit(13 + 64 + 32 + 2*8), queuedEffectiveBits);
    queuedEffectiveBits = testee_->numBitsForCid(17);
    CPPUNIT_ASSERT_EQUAL( Bit(62 + 32 + 8), queuedEffectiveBits);

    SegmentingCommandStub* commandSeg1 = commandFU_->getCommand(seg1->getCommandPool());
    CPPUNIT_ASSERT_EQUAL((long) 0, commandSeg1->getSequenceNumber());

    SegmentingCommandStub* commandSeg2 = commandFU_->getCommand(seg2->getCommandPool());
    CPPUNIT_ASSERT_EQUAL((long) 0, commandSeg2->getSequenceNumber());

    wns::ldk::CompoundPtr seg3 = testee_->getHeadOfLinePDUSegment(17, 42);

    SegmentingCommandStub* commandSeg3 = commandFU_->getCommand(seg3->getCommandPool());
    CPPUNIT_ASSERT_EQUAL((long) 1, commandSeg3->getSequenceNumber());
}
