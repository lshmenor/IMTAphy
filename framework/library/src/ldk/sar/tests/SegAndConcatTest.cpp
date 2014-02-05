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

#include <WNS/TestFixture.hpp>
#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/tools/Stub.hpp>
#include <WNS/ldk/helper/FakePDU.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/pyconfig/Parser.hpp>

#include <cppunit/extensions/HelperMacros.h>

#define NEWPDU wns::ldk::helper::FakePDUPtr(new wns::ldk::helper::FakePDU())

namespace wns { namespace ldk { namespace sar { namespace tests {
class SegAndConcatTest:
    public wns::TestFixture
{
    CPPUNIT_TEST_SUITE( SegAndConcatTest );
    CPPUNIT_TEST( testOutgoingOneSegment );
    CPPUNIT_TEST( testOutgoingTwoSegments );
    CPPUNIT_TEST( testOutgoingThreeSegments );
    CPPUNIT_TEST( testIncoming );
    CPPUNIT_TEST( testReassembly );
    CPPUNIT_TEST( testMissingSegments );
    CPPUNIT_TEST( testMissingSegmentsTimeout );
    CPPUNIT_TEST_SUITE_END();

    wns::ldk::ILayer* layer_;
    wns::ldk::fun::FUN* fuNet_;

    wns::ldk::tools::Stub* upper_;
    wns::ldk::sar::SegAndConcat* testee_;
    wns::ldk::tools::Stub* lower_;

public:

    void
    prepare();

    void
    cleanup();

    void
    testOutgoingOneSegment();

    void
    testOutgoingTwoSegments();

    void
    testOutgoingThreeSegments();

    void
    testIncoming();

    void
    testReassembly();

    void
    testMissingSegments();

    void
    testMissingSegmentsTimeout();

};
} // tests
} // sar
} // ldk
} // wns

using namespace wns::ldk::sar::tests;

CPPUNIT_TEST_SUITE_REGISTRATION( SegAndConcatTest );

void SegAndConcatTest::prepare()
{
    wns::pyconfig::Parser emptyConfig;

    layer_ = new wns::ldk::tests::LayerStub();
    fuNet_ = new wns::ldk::fun::Main(layer_);

    upper_ = new wns::ldk::tools::Stub(fuNet_, emptyConfig);
    lower_ = new wns::ldk::tools::Stub(fuNet_, emptyConfig);

    {
        wns::pyconfig::Parser config;
        config.loadString(
            "from openwns.SAR import SegAndConcat, ReorderingWindow\n"
            "testee = SegAndConcat(segmentSize=24, headerSize=16, commandName=\"testee\")\n"
            "testee.reorderingWindow = ReorderingWindow(snFieldLength = 5, parentLogger = testee.logger)\n"
            "testee.isSegmenting = True\n"
            "testee.logger.level = 3\n"
        );

        wns::pyconfig::View configView(config, "testee");
        testee_ = new SegAndConcat(fuNet_, configView);
    }

    fuNet_->addFunctionalUnit("upper", upper_);
    fuNet_->addFunctionalUnit("testee", testee_);
    fuNet_->addFunctionalUnit("lower", lower_);

    upper_->connect(testee_)->connect(lower_);

    testee_->onFUNCreated();
}

void
SegAndConcatTest::cleanup()
{
    delete layer_;
    delete fuNet_;
}

void
SegAndConcatTest::testOutgoingOneSegment()
{
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), lower_->sent.size());

    wns::ldk::helper::FakePDUPtr innerPDU = NEWPDU;
    innerPDU->setLengthInBits(24);
    wns::ldk::CompoundPtr compound(new wns::ldk::Compound(fuNet_->createCommandPool(), innerPDU));
    upper_->sendData(compound);

    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), lower_->sent.size());

    // Full segment + 16 bit header = 40 bit
    CPPUNIT_ASSERT_EQUAL(Bit(40), lower_->sent[0]->getLengthInBits());
    SegAndConcatCommand* command = testee_->getCommand(lower_->sent[0]);

    CPPUNIT_ASSERT_EQUAL(Bit(24), command->peer.dataSize_);
    CPPUNIT_ASSERT_EQUAL(true, command->getBeginFlag());
    CPPUNIT_ASSERT_EQUAL(true, command->getEndFlag());
}

void
SegAndConcatTest::testOutgoingTwoSegments()
{
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), lower_->sent.size());

    wns::ldk::helper::FakePDUPtr innerPDU = NEWPDU;
    innerPDU->setLengthInBits(46);
    wns::ldk::CompoundPtr compound(new wns::ldk::Compound(fuNet_->createCommandPool(), innerPDU));
    upper_->sendData(compound);

    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), lower_->sent.size());

    // Full segment + 16 bit header = 40 bit
    CPPUNIT_ASSERT_EQUAL(Bit(40), lower_->sent[0]->getLengthInBits());
    SegAndConcatCommand* command = testee_->getCommand(lower_->sent[0]);

    CPPUNIT_ASSERT_EQUAL(Bit(24), command->peer.dataSize_);
    CPPUNIT_ASSERT_EQUAL(true, command->getBeginFlag());
    CPPUNIT_ASSERT_EQUAL(false, command->getEndFlag());

    // Last segment of size 22 bit + 16 bit header = 38 bit
    CPPUNIT_ASSERT_EQUAL(Bit(38), lower_->sent[1]->getLengthInBits());
    command = testee_->getCommand(lower_->sent[1]);

    CPPUNIT_ASSERT_EQUAL(Bit(22), command->peer.dataSize_);
    CPPUNIT_ASSERT_EQUAL(false, command->getBeginFlag());
    CPPUNIT_ASSERT_EQUAL(true, command->getEndFlag());
}

void
SegAndConcatTest::testOutgoingThreeSegments()
{
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), lower_->sent.size());

    wns::ldk::helper::FakePDUPtr innerPDU = NEWPDU;
    innerPDU->setLengthInBits(70);
    wns::ldk::CompoundPtr compound(new wns::ldk::Compound(fuNet_->createCommandPool(), innerPDU));
    upper_->sendData(compound);

    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), lower_->sent.size());

    // Full segment + 16 bit header = 40 bit
    CPPUNIT_ASSERT_EQUAL(Bit(40), lower_->sent[0]->getLengthInBits());
    SegAndConcatCommand* command = testee_->getCommand(lower_->sent[0]);

    CPPUNIT_ASSERT_EQUAL(Bit(24), command->peer.dataSize_);
    CPPUNIT_ASSERT_EQUAL(true, command->getBeginFlag());
    CPPUNIT_ASSERT_EQUAL(false, command->getEndFlag());

    // Full segment + 16 bit header = 40 bit
    CPPUNIT_ASSERT_EQUAL(Bit(40), lower_->sent[1]->getLengthInBits());
    command = testee_->getCommand(lower_->sent[1]);

    CPPUNIT_ASSERT_EQUAL(Bit(24), command->peer.dataSize_);
    CPPUNIT_ASSERT_EQUAL(false, command->getBeginFlag());
    CPPUNIT_ASSERT_EQUAL(false, command->getEndFlag());

    // Last segment of size 22 bit + 16 bit header = 38 bit
    CPPUNIT_ASSERT_EQUAL(Bit(38), lower_->sent[2]->getLengthInBits());
    command = testee_->getCommand(lower_->sent[2]);

    CPPUNIT_ASSERT_EQUAL(Bit(22), command->peer.dataSize_);
    CPPUNIT_ASSERT_EQUAL(false, command->getBeginFlag());
    CPPUNIT_ASSERT_EQUAL(true, command->getEndFlag());
}

void
SegAndConcatTest::testIncoming()
{
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), upper_->received.size());

    wns::ldk::CompoundPtr compound(fuNet_->createCompound());
    SegAndConcatCommand* command = testee_->activateCommand(compound->getCommandPool());
    command->peer.sn_ = 0;
    command->peer.isBegin_ = true;
    command->peer.isEnd_ = true;

    wns::ldk::CompoundPtr inner(fuNet_->createCompound());
    command->addSDU(inner);

    lower_->onData(compound);

    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), upper_->received.size());
}

void
SegAndConcatTest::testReassembly()
{
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), lower_->sent.size());

    wns::ldk::helper::FakePDUPtr innerPDU = NEWPDU;
    innerPDU->setLengthInBits(70);
    wns::ldk::CompoundPtr compound(new wns::ldk::Compound(fuNet_->createCommandPool(), innerPDU));
    upper_->sendData(compound);

    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), lower_->sent.size());

    CPPUNIT_ASSERT_EQUAL(static_cast<Bit>(38), lower_->sent[2]->getLengthInBits());
    lower_->onData(lower_->sent[2]);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), upper_->received.size());

    CPPUNIT_ASSERT_EQUAL(static_cast<Bit>(40), lower_->sent[1]->getLengthInBits());
    lower_->onData(lower_->sent[1]);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), upper_->received.size());

    CPPUNIT_ASSERT_EQUAL(static_cast<Bit>(40), lower_->sent[0]->getLengthInBits());
    lower_->onData(lower_->sent[0]);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), upper_->received.size());

    CPPUNIT_ASSERT_EQUAL(Bit(70), upper_->received[0]->getLengthInBits());
}

void
SegAndConcatTest::testMissingSegments()
{
    // Send two SDUs. Scramble the segments and make one segment of the first SDU get
    // lost. Only the second SDU should be received.
    // A third SDU is used to fill the reassembly window.
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), lower_->sent.size());

    wns::ldk::helper::FakePDUPtr innerPDU = NEWPDU;
    innerPDU->setLengthInBits(70);
    wns::ldk::CompoundPtr compound(new wns::ldk::Compound(fuNet_->createCommandPool(), innerPDU));
    upper_->sendData(compound);

    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), lower_->sent.size());

    innerPDU = NEWPDU;
    innerPDU->setLengthInBits(90);
    compound = wns::ldk::CompoundPtr(new wns::ldk::Compound(fuNet_->createCommandPool(), innerPDU));
    upper_->sendData(compound);

    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), lower_->sent.size());

    innerPDU = NEWPDU;
    innerPDU->setLengthInBits(16*24); // One full window
    compound = wns::ldk::CompoundPtr(new wns::ldk::Compound(fuNet_->createCommandPool(), innerPDU));
    upper_->sendData(compound);

    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(23), lower_->sent.size());

    // Make Segment 1 missing
    lower_->onData(lower_->sent[3]);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), upper_->received.size());

    lower_->onData(lower_->sent[0]);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), upper_->received.size());

    lower_->onData(lower_->sent[2]);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), upper_->received.size());

    lower_->onData(lower_->sent[6]);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), upper_->received.size());

    lower_->onData(lower_->sent[4]);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), upper_->received.size());

    lower_->onData(lower_->sent[5]);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), upper_->received.size());

    lower_->onData(lower_->sent[7]);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), upper_->received.size());

    lower_->onData(lower_->sent[8]);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), upper_->received.size());

    lower_->onData(lower_->sent[9]);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), upper_->received.size());

    lower_->onData(lower_->sent[10]);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), upper_->received.size());

    lower_->onData(lower_->sent[11]);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), upper_->received.size());

    lower_->onData(lower_->sent[12]);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), upper_->received.size());

    lower_->onData(lower_->sent[13]);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), upper_->received.size());

    lower_->onData(lower_->sent[14]);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), upper_->received.size());

    lower_->onData(lower_->sent[15]);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), upper_->received.size());

    lower_->onData(lower_->sent[16]);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), upper_->received.size());

    lower_->onData(lower_->sent[17]);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), upper_->received.size());

    lower_->onData(lower_->sent[18]);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), upper_->received.size());

    lower_->onData(lower_->sent[19]);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), upper_->received.size());

    CPPUNIT_ASSERT_EQUAL(Bit(90), upper_->received[0]->getLengthInBits());
}

void
SegAndConcatTest::testMissingSegmentsTimeout()
{
    // Send two SDUs. Scramble the segments and make one segment of the first SDU get
    // lost. Only the second SDU should be received.
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), lower_->sent.size());

    wns::ldk::helper::FakePDUPtr innerPDU = NEWPDU;
    innerPDU->setLengthInBits(70);
    wns::ldk::CompoundPtr compound(new wns::ldk::Compound(fuNet_->createCommandPool(), innerPDU));
    upper_->sendData(compound);

    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), lower_->sent.size());

    innerPDU = NEWPDU;
    innerPDU->setLengthInBits(90);
    compound = wns::ldk::CompoundPtr(new wns::ldk::Compound(fuNet_->createCommandPool(), innerPDU));
    upper_->sendData(compound);

    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), lower_->sent.size());
    // Make Segment 1 missing
    lower_->onData(lower_->sent[3]);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), upper_->received.size());

    lower_->onData(lower_->sent[0]);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), upper_->received.size());

    lower_->onData(lower_->sent[2]);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), upper_->received.size());

    lower_->onData(lower_->sent[6]);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), upper_->received.size());

    lower_->onData(lower_->sent[4]);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), upper_->received.size());

    // Make t-Reordering expire
    wns::simulator::getEventScheduler()->processOneEvent();

    lower_->onData(lower_->sent[5]);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), upper_->received.size());

    CPPUNIT_ASSERT_EQUAL(Bit(90), upper_->received[0]->getLengthInBits());
}
