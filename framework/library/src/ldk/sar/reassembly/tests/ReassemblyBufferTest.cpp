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

#include <WNS/ldk/sar/SegAndConcat.hpp>
#include <WNS/ldk/sar/reassembly/ReassemblyBuffer.hpp>

#include <WNS/TestFixture.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/tools/Stub.hpp>
#include <WNS/pyconfig/Parser.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace wns { namespace ldk { namespace sar { namespace reassembly { namespace tests {

class SegAndConcatStubFU:
    public wns::ldk::tools::StubBase,
    public wns::ldk::CommandTypeSpecifier<SegAndConcatCommand>,
    public wns::Cloneable<SegAndConcatStubFU>
{
public:
    SegAndConcatStubFU(wns::ldk::fun::FUN* fun, wns::pyconfig::View&):
        wns::ldk::tools::StubBase(),
        wns::ldk::CommandTypeSpecifier<SegAndConcatCommand>(fun),
        wns::Cloneable<SegAndConcatStubFU>()
    {
    }

    virtual void
    calculateSizes(const wns::ldk::CommandPool* commandPool, Bit& commandPoolSize, Bit& dataSize) const
    {
        StubBase::calculateSizes(commandPool, commandPoolSize, dataSize);
    } // calculateSizes
};

class ReassemblyBufferTest:
    public wns::TestFixture
{
    CPPUNIT_TEST_SUITE( ReassemblyBufferTest );
    CPPUNIT_TEST( testIntegrity );
    CPPUNIT_TEST( testNoSegment );
    CPPUNIT_TEST( testSingleSegment );
    CPPUNIT_TEST( testNextExpectedSN ); 
    CPPUNIT_TEST( testIsNextExpectedSegment);
    CPPUNIT_TEST( testClear);
    CPPUNIT_TEST( testDropSegmentsOfSDU );
    CPPUNIT_TEST( testDropSegmentBug1 );
    CPPUNIT_TEST( testTwoSegmentsTypeA );
    CPPUNIT_TEST( testTwoSegmentsTypeB );
    CPPUNIT_TEST_SUITE_END();

    wns::ldk::ILayer* layer_;
    wns::ldk::fun::FUN* fun_;
    SegAndConcatStubFU* um_;

    ReassemblyBuffer* testee_;

public:

    void
    prepare();

    void
    cleanup();

    void
    testIntegrity();

    void
    testNoSegment();

    void
    testSingleSegment();

    void
    testNextExpectedSN();

    void
    testIsNextExpectedSegment();

    void
    testClear();

    void
    testDropSegmentsOfSDU();

    void
    testDropSegmentBug1();

    void
    testTwoSegmentsTypeA();

    void
    testTwoSegmentsTypeB();
};

CPPUNIT_TEST_SUITE_REGISTRATION( ReassemblyBufferTest );

void ReassemblyBufferTest::prepare()
{
    wns::pyconfig::Parser emptyConfig;

    layer_ = new wns::ldk::tests::LayerStub();
    fun_ = new wns::ldk::fun::Main(layer_);

    um_ = new SegAndConcatStubFU(fun_, emptyConfig);
    fun_->addFunctionalUnit("um", um_);

    testee_ = new ReassemblyBuffer();
    testee_->initialize(fun_->getCommandReader("um"));
}

void
ReassemblyBufferTest::cleanup()
{
    delete testee_;
    delete layer_;
    delete fun_;
}

void
ReassemblyBufferTest::testIntegrity()
{
    bool integrity = testee_->integrityCheck();
    CPPUNIT_ASSERT_EQUAL(true, integrity);
}

void
ReassemblyBufferTest::testNoSegment()
{
    int counter = 0;
    CPPUNIT_ASSERT_EQUAL((size_t) 0, testee_->getReassembledSegments(counter).size());
}

void
ReassemblyBufferTest::testSingleSegment()
{
    int counter = 0;
    wns::ldk::CompoundPtr c = fun_->createCompound();
    SegAndConcatCommand* command = um_->activateCommand(c->getCommandPool());

    command->setBeginFlag();
    command->setEndFlag();
    command->setSequenceNumber(0);
    command->addSDU(fun_->createCompound());

    CPPUNIT_ASSERT(testee_->accepts(c));

    testee_->insert(c);

    CPPUNIT_ASSERT_EQUAL((size_t) 1, testee_->getReassembledSegments(counter).size());
    CPPUNIT_ASSERT_EQUAL((int) 1, counter);

    // First call removes all SDUs
    CPPUNIT_ASSERT_EQUAL((size_t) 0, testee_->getReassembledSegments(counter).size());
    CPPUNIT_ASSERT_EQUAL((int) 0, counter);
}

void
ReassemblyBufferTest::testNextExpectedSN()
{
    wns::ldk::CompoundPtr c = fun_->createCompound();
    SegAndConcatCommand* command = um_->activateCommand(c->getCommandPool());

    command->setBeginFlag();
    command->setEndFlag();
    command->setSequenceNumber(0);
    command->addSDU(fun_->createCompound());

    CPPUNIT_ASSERT_EQUAL( (long) -1, testee_->getNextExpectedSN());

    testee_->insert(c);

    CPPUNIT_ASSERT_EQUAL( (long) 1, testee_->getNextExpectedSN());
}

void
ReassemblyBufferTest::testIsNextExpectedSegment()
{
    wns::ldk::CompoundPtr c = fun_->createCompound();
    SegAndConcatCommand* command = um_->activateCommand(c->getCommandPool());

    command->setBeginFlag();
    command->setEndFlag();
    command->setSequenceNumber(0);
    command->addSDU(fun_->createCompound());

    testee_->insert(c);

    wns::ldk::CompoundPtr c1 = fun_->createCompound();
    command = um_->activateCommand(c1->getCommandPool());

    command->setBeginFlag();
    command->setEndFlag();
    command->setSequenceNumber(1);
    command->addSDU(fun_->createCompound());

    wns::ldk::CompoundPtr c2 = fun_->createCompound();
    command = um_->activateCommand(c2->getCommandPool());

    command->setBeginFlag();
    command->setEndFlag();
    command->setSequenceNumber(2);
    command->addSDU(fun_->createCompound());

    CPPUNIT_ASSERT_EQUAL(testee_->isNextExpectedSegment(c1), true);
    CPPUNIT_ASSERT_EQUAL(testee_->isNextExpectedSegment(c2), false);
}

void
ReassemblyBufferTest::testClear()
{
    wns::ldk::CompoundPtr c = fun_->createCompound();
    SegAndConcatCommand* command = um_->activateCommand(c->getCommandPool());

    command->setBeginFlag();
    command->setEndFlag();
    command->setSequenceNumber(0);
    command->addSDU(fun_->createCompound());

    CPPUNIT_ASSERT(testee_->isEmpty());
    testee_->insert(c);
    CPPUNIT_ASSERT(!testee_->isEmpty());
    testee_->clear();
    CPPUNIT_ASSERT(testee_->isEmpty());

}

void
ReassemblyBufferTest::testDropSegmentsOfSDU()
{
    // tests internal method of ReassemblyBuffer
    // internal state is modified during preparation
    // THIS METHOD IS NOT AVAILABLE PUBLICLY

    wns::ldk::CompoundPtr frag1 = fun_->createCompound();
    wns::ldk::CompoundPtr frag2 = fun_->createCompound();
    wns::ldk::CompoundPtr frag3 = fun_->createCompound();
    wns::ldk::CompoundPtr frag4 = fun_->createCompound();
    wns::ldk::CompoundPtr c = fun_->createCompound();
    SegAndConcatCommand* command = um_->activateCommand(c->getCommandPool());

    command->setBeginFlag();
    command->setSequenceNumber(0);
    command->addSDU(frag1);

    CPPUNIT_ASSERT(testee_->accepts(c));

    testee_->insert(c);

    c = fun_->createCompound();

    command = um_->activateCommand(c->getCommandPool());

    command->clearBeginFlag();
    command->clearEndFlag();
    command->setSequenceNumber(1);
    command->addSDU(frag2);

    CPPUNIT_ASSERT(testee_->accepts(c));

    testee_->insert(c);

    c = fun_->createCompound();
    command = um_->activateCommand(c->getCommandPool());

    command->clearBeginFlag();
    command->setEndFlag();
    command->setSequenceNumber(2);
    command->addSDU(frag3);
    command->addSDU(frag4);

    CPPUNIT_ASSERT(testee_->accepts(c));

    testee_->insert(c);

    // Pimp internal state. 
    testee_->buffer_.pop_front();

    // This should drop compound(SN=1) completely
    // and the first segment in compound(SN=2)
    testee_->dropSegmentsOfSDU();

    CPPUNIT_ASSERT_EQUAL((size_t) 1, testee_->buffer_.size());
    c = testee_->buffer_.front();
    command = um_->getCommand(c->getCommandPool());

    CPPUNIT_ASSERT_EQUAL((size_t) 1, command->peer.pdus_.size());
    CPPUNIT_ASSERT_EQUAL(frag4->getBirthmark(), command->peer.pdus_.front()->getBirthmark());
}

void
ReassemblyBufferTest::testDropSegmentBug1()
{
    int counter = 0;
    wns::ldk::CompoundPtr frag1 = fun_->createCompound();
    wns::ldk::CompoundPtr frag2 = fun_->createCompound();
    wns::ldk::CompoundPtr frag3 = fun_->createCompound();

    wns::ldk::CompoundPtr c = fun_->createCompound();
    SegAndConcatCommand* command = um_->activateCommand(c->getCommandPool());

    command->setBeginFlag();
    command->setSequenceNumber(0);
    command->addSDU(frag1);

    CPPUNIT_ASSERT(testee_->accepts(c));

    testee_->insert(c);

    c = fun_->createCompound();

    command = um_->activateCommand(c->getCommandPool());

    command->clearBeginFlag();
    command->clearEndFlag();
    command->setSequenceNumber(1);
    command->addSDU(frag2);
    command->addSDU(frag3);

    CPPUNIT_ASSERT(testee_->accepts(c));

    testee_->insert(c);

    ReassemblyBuffer::SegmentContainer sdus = testee_->getReassembledSegments(counter);

    CPPUNIT_ASSERT_EQUAL((size_t) 1, sdus.size());
    CPPUNIT_ASSERT_EQUAL((int) 1, counter);
    CPPUNIT_ASSERT_EQUAL(frag1->getBirthmark(), sdus.front()->getBirthmark() );
}

void
ReassemblyBufferTest::testTwoSegmentsTypeA()
{
    // SDU 1                         SDU 2
    // +------------+---+            +-------------+
    // | 1          | 2 |            | 3           |
    // +------------+---+'           +-------------+
    //              .   .
    // PDU 1        .    PDU 2
    // +---+--------+    +---+----+-----------+
    // | B | 1      |    | E | 2  | 3         |
    // +---+--------+    +---+----+-----------+

    int counter = 0;
    wns::ldk::CompoundPtr frag1 = fun_->createCompound();
    wns::ldk::CompoundPtr frag2 = fun_->createCompound();
    wns::ldk::CompoundPtr frag3 = fun_->createCompound();
    wns::ldk::CompoundPtr c = fun_->createCompound();
    SegAndConcatCommand* command = um_->activateCommand(c->getCommandPool());

    command->setBeginFlag();
    command->setSequenceNumber(0);
    command->addSDU(frag1);

    CPPUNIT_ASSERT(testee_->accepts(c));

    testee_->insert(c);

    CPPUNIT_ASSERT_EQUAL((size_t) 0, testee_->getReassembledSegments(counter).size());
    CPPUNIT_ASSERT_EQUAL((int) 0, counter);

    c = fun_->createCompound();

    command = um_->activateCommand(c->getCommandPool());

    command->clearBeginFlag();
    command->setEndFlag();
    command->setSequenceNumber(1);
    command->addSDU(frag2);
    command->addSDU(frag3);

    CPPUNIT_ASSERT(testee_->accepts(c));

    testee_->insert(c);

    ReassemblyBuffer::SegmentContainer sdus = testee_->getReassembledSegments(counter);
    CPPUNIT_ASSERT_EQUAL((int) 2, counter);
    // Nothing more to reassembly
    CPPUNIT_ASSERT_EQUAL((size_t) 0 ,testee_->getReassembledSegments(counter).size());
    CPPUNIT_ASSERT_EQUAL((int) 0, counter);
    // Two SDUs should be available
    CPPUNIT_ASSERT_EQUAL((size_t) 2, sdus.size());

    // The first fragment should be returned when
    // a segmented PDU is within the buffer
    CPPUNIT_ASSERT_EQUAL(frag1->getBirthmark(), sdus.front()->getBirthmark() );

    sdus.pop_front();

    CPPUNIT_ASSERT_EQUAL((size_t) 1, sdus.size());

    wns::ldk::CompoundPtr cmp = sdus.front();

    CPPUNIT_ASSERT_EQUAL(frag3->getBirthmark(), cmp->getBirthmark());
}

void
ReassemblyBufferTest::testTwoSegmentsTypeB()
{
    // SDU 1                         SDU 2              SDU 3
    // +------------+---+            +----------+--+    +------------+
    // | 1          | 2 |            | 3        | 4|    | 5          |
    // +------------+---+'           +----------+--+    +------------+
    //              .   .
    // PDU 1        .    PDU 2                    PDU 3
    // +---+--------+    +---+----+-----------+   +---+---+----------+
    // | B | 1      |    |   | 2  | 3         |   | E | 4 | 5        |
    // +---+--------+    +---+----+-----------+   +---+---+----------+

    int counter = 0;

    wns::ldk::CompoundPtr frag1 = fun_->createCompound();
    wns::ldk::CompoundPtr frag2 = fun_->createCompound();
    wns::ldk::CompoundPtr frag3 = fun_->createCompound();
    wns::ldk::CompoundPtr frag4 = fun_->createCompound();
    wns::ldk::CompoundPtr frag5 = fun_->createCompound();

    // First PDU
    wns::ldk::CompoundPtr c = fun_->createCompound();
    SegAndConcatCommand* command = um_->activateCommand(c->getCommandPool());

    command->setBeginFlag();
    command->clearEndFlag();
    command->setSequenceNumber(0);
    command->addSDU(frag1);

    CPPUNIT_ASSERT(testee_->accepts(c));

    testee_->insert(c);

    CPPUNIT_ASSERT_EQUAL((size_t) 0, testee_->getReassembledSegments(counter).size());
    CPPUNIT_ASSERT_EQUAL((int) 0, counter);
    // Second PDU
    c = fun_->createCompound();

    command = um_->activateCommand(c->getCommandPool());

    command->clearBeginFlag();
    command->clearEndFlag();
    command->setSequenceNumber(1);
    command->addSDU(frag2);
    command->addSDU(frag3);

    CPPUNIT_ASSERT(testee_->accepts(c));

    testee_->insert(c);

    // Third PDU
    c = fun_->createCompound();

    command = um_->activateCommand(c->getCommandPool());

    command->clearBeginFlag();
    command->setEndFlag();
    command->setSequenceNumber(2);
    command->addSDU(frag4);
    command->addSDU(frag5);

    CPPUNIT_ASSERT(testee_->accepts(c));

    testee_->insert(c);

    ReassemblyBuffer::SegmentContainer sdus = testee_->getReassembledSegments(counter);
    CPPUNIT_ASSERT_EQUAL((int) 3, counter);

    // Nothing more to reassembly
    CPPUNIT_ASSERT_EQUAL((size_t) 0 ,testee_->getReassembledSegments(counter).size());
    CPPUNIT_ASSERT_EQUAL((int) 0, counter);

    // Two SDUs should be available
    CPPUNIT_ASSERT_EQUAL((size_t) 3, sdus.size());

    // The first fragment should be returned when
    // a segmented PDU is within the buffer
    CPPUNIT_ASSERT_EQUAL(frag1->getBirthmark(), sdus.front()->getBirthmark() );

    sdus.pop_front();

    CPPUNIT_ASSERT_EQUAL((size_t) 2, sdus.size());

    wns::ldk::CompoundPtr cmp = sdus.front();

    CPPUNIT_ASSERT_EQUAL(frag3->getBirthmark(), cmp->getBirthmark());

    sdus.pop_front();

    CPPUNIT_ASSERT_EQUAL((size_t) 1, sdus.size());

    cmp = sdus.front();

    CPPUNIT_ASSERT_EQUAL(frag5->getBirthmark(), cmp->getBirthmark());
}

} // tests
} // reassembly
} // sar
} // ldk
} // wns
