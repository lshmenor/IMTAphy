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
#include <WNS/scheduler/queue/SegmentingQueue.hpp>
#include <WNS/TestFixture.hpp>
#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/ldk/tools/Stub.hpp>
#include <WNS/scheduler/tests/RegistryProxyStub.hpp>
#include <WNS/ldk/helper/FakePDU.hpp>
#include <WNS/pyconfig/Parser.hpp>
#include <WNS/events/NoOp.hpp>

#include <cppunit/extensions/HelperMacros.h>


#define NEWPDU wns::ldk::helper::FakePDUPtr(new wns::ldk::helper::FakePDU())

namespace wns { namespace ldk { namespace sar { namespace tests {
class SACSegmentingQueueIntegrationTest:
    public wns::TestFixture
{
    CPPUNIT_TEST_SUITE( SACSegmentingQueueIntegrationTest );
    CPPUNIT_TEST( testSACSegmentingDisabled );
    CPPUNIT_TEST( testIncomingOneSegment );
    CPPUNIT_TEST( testIncomingTwoSegments );
    CPPUNIT_TEST( testIncomingFourSegments );
    CPPUNIT_TEST( testFirstSegmentLost );
    CPPUNIT_TEST( testSecondSegmentLost );
    CPPUNIT_TEST( testThirdSegmentLost );
    CPPUNIT_TEST( testClearingTReordering );
    CPPUNIT_TEST_SUITE_END();

    wns::ldk::ILayer* layer_;
    wns::ldk::fun::FUN* fuNet_;

    wns::ldk::tools::Stub* upper_;
    SegAndConcat* testee_;
    wns::scheduler::queue::SegmentingQueue* segmentingQueue_;
    wns::ldk::tools::Stub* lower_;
    wns::scheduler::tests::RegistryProxyStub* registryProxy_;

    wns::ldk::CompoundPtr compoundA;
    wns::ldk::CompoundPtr compoundB;
    wns::ldk::CompoundPtr compoundC;
    wns::ldk::CompoundPtr compoundD;


public:

    void
    prepare();

    void
    latePrepare();

    void
    cleanup();

    void
    testSACSegmentingDisabled();

    void
    testIncomingOneSegment();

    void
    testIncomingTwoSegments();

    void
    testIncomingFourSegments();

    void
    prepareFourCompounds();

    void
    testFirstSegmentLost();

    void
    testSecondSegmentLost();

    void
    testThirdSegmentLost();

    void
    testClearingTReordering();

    std::string alternateConfiguration;
};
} // tests
} // sar
} // ldk
} // wns

using namespace wns::ldk::sar::tests;

CPPUNIT_TEST_SUITE_REGISTRATION( SACSegmentingQueueIntegrationTest );

void SACSegmentingQueueIntegrationTest::prepare()
{
}

void SACSegmentingQueueIntegrationTest::latePrepare()
{
    wns::pyconfig::Parser emptyConfig;

    layer_ = new wns::ldk::tests::LayerStub();
    fuNet_ = new wns::ldk::fun::Main(layer_);

    upper_ = new wns::ldk::tools::Stub(fuNet_, emptyConfig);
    lower_ = new wns::ldk::tools::Stub(fuNet_, emptyConfig);

    if (alternateConfiguration == "")
    {
        wns::pyconfig::Parser config;
        config.loadString(
            "from openwns.SAR import SegAndConcat\n"
            "testee = SegAndConcat(segmentSize=24, headerSize=16, commandName=\"testee\")\n"
            "testee.logger.level = 3\n"
            "testee.isSegmenting = False\n"
            "testee.reorderingWindow.tReordering = 0.015"
        );

        wns::pyconfig::View configView(config, "testee");
        testee_ = new SegAndConcat(fuNet_, configView);
    }
    else
    {
        wns::pyconfig::Parser config;
        config.loadString(alternateConfiguration);

        wns::pyconfig::View configView(config, "testee");
        testee_ = new SegAndConcat(fuNet_, configView);
    }

    fuNet_->addFunctionalUnit("upper", upper_);
    fuNet_->addFunctionalUnit("testee", testee_);
    fuNet_->addFunctionalUnit("lower", lower_);

    upper_->connect(testee_)->connect(lower_);

    testee_->onFUNCreated();

    {
        wns::pyconfig::Parser config;
        config.loadString(
           "from openwns.Scheduler import SegmentingQueue\n"
           "testee = SegmentingQueue(\"testee\", \"testee\")\n"
            "testee.fixedHeaderSize = 16\n"
            "testee.extensionHeaderSize = 8\n"
        );
        registryProxy_ = new wns::scheduler::tests::RegistryProxyStub();

        wns::pyconfig::View configView(config, "testee");
        segmentingQueue_ = new wns::scheduler::queue::SegmentingQueue(NULL, configView);
        segmentingQueue_->setFUN(fuNet_);
        segmentingQueue_->setColleagues(registryProxy_);
    }
}

void
SACSegmentingQueueIntegrationTest::cleanup()
{
    delete segmentingQueue_;
    delete registryProxy_;
    delete layer_;
    delete fuNet_;
}

void
SACSegmentingQueueIntegrationTest::testSACSegmentingDisabled()
{
    alternateConfiguration = "from openwns.SAR import SegAndConcat\n"
        "testee = SegAndConcat(segmentSize=24, headerSize=16, commandName=\"testee\")\n"
        "testee.logger.level = 3\n"
        "testee.isSegmenting = False\n";

    latePrepare();

    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), lower_->sent.size());

    wns::ldk::helper::FakePDUPtr innerPDU = NEWPDU;
    innerPDU->setLengthInBits(24);
    wns::ldk::CompoundPtr compound(new wns::ldk::Compound(fuNet_->createCommandPool(), innerPDU));

    upper_->sendData(compound);

    CPPUNIT_ASSERT_EQUAL((size_t) 1, lower_->sent.size());

    CPPUNIT_ASSERT_EQUAL(compound, lower_->sent[0]);
}

void
SACSegmentingQueueIntegrationTest::testIncomingOneSegment()
{
    latePrepare();

    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), lower_->sent.size());

    wns::ldk::helper::FakePDUPtr innerPDU = NEWPDU;
    innerPDU->setLengthInBits(24);
    wns::ldk::CompoundPtr compound(new wns::ldk::Compound(fuNet_->createCommandPool(), innerPDU));

    registryProxy_->setCIDforPDU(compound, 1);
    segmentingQueue_->put(compound);

    CPPUNIT_ASSERT(segmentingQueue_->queueHasPDUs(1));

    wns::ldk::CompoundPtr c = segmentingQueue_->getHeadOfLinePDUSegment(1, 40);

    CPPUNIT_ASSERT_EQUAL((Bit) 40, c->getLengthInBits());

    lower_->onData(c);

    CPPUNIT_ASSERT_EQUAL((size_t) 1, upper_->received.size());
}

void
SACSegmentingQueueIntegrationTest::testIncomingTwoSegments()
{
    latePrepare();

    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), lower_->sent.size());

    wns::ldk::helper::FakePDUPtr innerPDU = NEWPDU;
    innerPDU->setLengthInBits(24);
    wns::ldk::CompoundPtr compound(new wns::ldk::Compound(fuNet_->createCommandPool(), innerPDU));

    registryProxy_->setCIDforPDU(compound, 1);
    segmentingQueue_->put(compound);

    CPPUNIT_ASSERT(segmentingQueue_->queueHasPDUs(1));

    wns::ldk::CompoundPtr c = segmentingQueue_->getHeadOfLinePDUSegment(1, 28);

    CPPUNIT_ASSERT_EQUAL((Bit) 28, c->getLengthInBits());

    lower_->onData(c);

    CPPUNIT_ASSERT_EQUAL((size_t) 0, upper_->received.size());

    c = segmentingQueue_->getHeadOfLinePDUSegment(1, 12);

    CPPUNIT_ASSERT_EQUAL((Bit) 12, c->getLengthInBits());

    lower_->onData(c);

    CPPUNIT_ASSERT_EQUAL((size_t) 1, upper_->received.size());
}

void
SACSegmentingQueueIntegrationTest::testIncomingFourSegments()
{
    latePrepare();

    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), lower_->sent.size());

    wns::ldk::helper::FakePDUPtr innerPDU = NEWPDU;
    innerPDU->setLengthInBits(24);
    wns::ldk::CompoundPtr compound(new wns::ldk::Compound(fuNet_->createCommandPool(), innerPDU));

    registryProxy_->setCIDforPDU(compound, 1);
    segmentingQueue_->put(compound);

    CPPUNIT_ASSERT(segmentingQueue_->queueHasPDUs(1));

    wns::ldk::CompoundPtr c = segmentingQueue_->getHeadOfLinePDUSegment(1, 22);

    CPPUNIT_ASSERT_EQUAL((Bit) 22, c->getLengthInBits());

    lower_->onData(c);

    CPPUNIT_ASSERT_EQUAL((size_t) 0, upper_->received.size());

    c = segmentingQueue_->getHeadOfLinePDUSegment(1, 6);

    CPPUNIT_ASSERT_EQUAL((Bit) 6, c->getLengthInBits());

    lower_->onData(c);

    CPPUNIT_ASSERT_EQUAL((size_t) 0, upper_->received.size());

    c = segmentingQueue_->getHeadOfLinePDUSegment(1, 6);

    CPPUNIT_ASSERT_EQUAL((Bit) 6, c->getLengthInBits());

    lower_->onData(c);

    CPPUNIT_ASSERT_EQUAL((size_t) 0, upper_->received.size());

    c = segmentingQueue_->getHeadOfLinePDUSegment(1, 6);

    CPPUNIT_ASSERT_EQUAL((Bit) 6, c->getLengthInBits());

    lower_->onData(c);

    CPPUNIT_ASSERT_EQUAL((size_t) 1, upper_->received.size());
}



/*

  We add 4 PDUs A (70 bits), B (80 bits), C (90 bits), D (50 bits) to
  the queue and set the fixed header size to 16 and the extension
  header size to 8 bits.

  What the queue shows is the "brutto" bits and they are computed
  taking one full header and 3 extension headers: (16 + 70) + (8 + 80) +
  (8 + 90) + (8 + 50) = 330 bits

  When we take segments out, the first SDU is supposed to have a 16bit
  header and subsequent SDUs (fragments) in the same segment take an
  additional 8 bits.

  Now we ask for the first 126 bits so we get (16 + 70) + (8 + 32) =
  126 which is the complete 70 bits of PDU A and 32 bits of PDU B with
  a fixed header and an extension header.

  Since we did not give the frameStart() trigger it is assumed that
  the fixed header is included in the transport block and therfore if we
  now ask for the next 126 bits and get (48) + (8 + 70) = 126
  which is the remaining 48 bits of PDU B and the first 70 bits of PDU C.
  There is no fixed header, but an extension header indicating the
  start of PDU C.

  Now we ask for the next 126 bits and get (20) + (8 + 50) = 78
  bits which is the rest of PDU C and the complete PDU D of only 50 bits
  again only with an extension header.


 Segment 1    Segment 2      Segment 3
+----------+ +------------+ +----------+
| A    | B1| | B2  | C1   | | C2 |  D  |
+----------* +------------+ +----------+

If segment 1 is lost, we want to be able to retrieve C and D. If
segment 2 is lost, we want to see A and D and if segement 3 is lost,
we expect A nd B.

*/

void
SACSegmentingQueueIntegrationTest::prepareFourCompounds()
{
    // compound A
    wns::ldk::helper::FakePDUPtr innerPDUA = NEWPDU;
    innerPDUA->setLengthInBits(70);
    compoundA = wns::ldk::CompoundPtr(new wns::ldk::Compound(fuNet_->createCommandPool(), innerPDUA));
    registryProxy_->setCIDforPDU(compoundA, 1);
    segmentingQueue_->put(compoundA);

     // compound B
    wns::ldk::helper::FakePDUPtr innerPDUB = NEWPDU;
    innerPDUB->setLengthInBits(80);
    compoundB =  wns::ldk::CompoundPtr(new wns::ldk::Compound(fuNet_->createCommandPool(), innerPDUB));
    registryProxy_->setCIDforPDU(compoundB, 1);
    segmentingQueue_->put(compoundB);

     // compound C
    wns::ldk::helper::FakePDUPtr innerPDUC = NEWPDU;
    innerPDUC->setLengthInBits(90);
    compoundC =  wns::ldk::CompoundPtr(new wns::ldk::Compound(fuNet_->createCommandPool(), innerPDUC));
    registryProxy_->setCIDforPDU(compoundC, 1);
    segmentingQueue_->put(compoundC);

     // compound D
    wns::ldk::helper::FakePDUPtr innerPDUD = NEWPDU;
    innerPDUD->setLengthInBits(50);
    compoundD =  wns::ldk::CompoundPtr(new wns::ldk::Compound(fuNet_->createCommandPool(), innerPDUD));
    registryProxy_->setCIDforPDU(compoundD, 1);
    segmentingQueue_->put(compoundD);
}

void
SACSegmentingQueueIntegrationTest::testFirstSegmentLost()
{
    latePrepare();
    prepareFourCompounds();

    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), lower_->sent.size());

    CPPUNIT_ASSERT(segmentingQueue_->queueHasPDUs(1)); // queue with CID=1

    // remove the "lost" segment from the queue
    wns::ldk::CompoundPtr lost = segmentingQueue_->getHeadOfLinePDUSegment(1, 126);
    CPPUNIT_ASSERT_EQUAL((Bit) 126, lost->getLengthInBits());
    wns::simulator::getEventScheduler()->scheduleDelay(events::NoOp(), 0.001);
    wns::simulator::getEventScheduler()->processOneEvent();

    // now at t=0.001, second segement arrives
    wns::ldk::CompoundPtr segment2 = segmentingQueue_->getHeadOfLinePDUSegment(1, 126);
    CPPUNIT_ASSERT_EQUAL((Bit) 126, segment2->getLengthInBits());
    lower_->onData(segment2);

    // no complete SDU in there, nothing to be received
    CPPUNIT_ASSERT_EQUAL((size_t) 0, upper_->received.size());

    // now at t=0.002, third segement arrives
    wns::ldk::CompoundPtr segment3 = segmentingQueue_->getHeadOfLinePDUSegment(1, 126);
    CPPUNIT_ASSERT_EQUAL((Bit) 78, segment3->getLengthInBits());
    lower_->onData(segment3);

    // make sure queue is empty:
    CPPUNIT_ASSERT(!segmentingQueue_->queueHasPDUs(1));

    // with segment 1 lost, only SDUs C and D should arrive but not until tReordering has expired
    // at t=0.002 nothing has arrived:
    CPPUNIT_ASSERT_EQUAL((size_t) 0, upper_->received.size());

    // after tReordering == 0.015 the SDUs C and D are retrieved:
    wns::simulator::getEventScheduler()->processOneEvent();
    CPPUNIT_ASSERT_DOUBLES_EQUAL((double)wns::simulator::getEventScheduler()->getTime(), (double) 0.016, 0.00001);
    CPPUNIT_ASSERT_EQUAL((size_t) 2, upper_->received.size());
    CPPUNIT_ASSERT_EQUAL(compoundC->getLengthInBits(), upper_->received.front()->getLengthInBits()); // PDU C of size 90
    CPPUNIT_ASSERT_EQUAL(compoundD->getLengthInBits(), upper_->received.back()->getLengthInBits()); // PDU D of size 50
}

void
SACSegmentingQueueIntegrationTest::testSecondSegmentLost()
{
    latePrepare();
    prepareFourCompounds();

    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), lower_->sent.size());

    CPPUNIT_ASSERT(segmentingQueue_->queueHasPDUs(1)); // queue with CID=1

    // get the first segment from the queue
    wns::ldk::CompoundPtr segment1 = segmentingQueue_->getHeadOfLinePDUSegment(1, 126);
    CPPUNIT_ASSERT_EQUAL((Bit) 126, segment1->getLengthInBits());
    lower_->onData(segment1);

    wns::simulator::getEventScheduler()->scheduleDelay(events::NoOp(), 0.001);
    wns::simulator::getEventScheduler()->processOneEvent();

    // complete SDU A should have already arrived
    CPPUNIT_ASSERT_EQUAL((size_t) 1, upper_->received.size());
    CPPUNIT_ASSERT_EQUAL(compoundA->getLengthInBits(), upper_->received.front()->getLengthInBits()); // SDU A with 70 bits

    // now at t=0.001, take the "lost" segement out of the queue
    wns::ldk::CompoundPtr lost = segmentingQueue_->getHeadOfLinePDUSegment(1, 126);
    CPPUNIT_ASSERT_EQUAL((Bit) 126, lost->getLengthInBits());


    wns::simulator::getEventScheduler()->scheduleDelay(events::NoOp(), 0.001);
    wns::simulator::getEventScheduler()->processOneEvent();

    // now at t=0.002, third segement arrives
    wns::ldk::CompoundPtr segment3 = segmentingQueue_->getHeadOfLinePDUSegment(1, 126);
    CPPUNIT_ASSERT_EQUAL((Bit) 78, segment3->getLengthInBits());
    lower_->onData(segment3);

    // as segment 2 was lost, only SDU A should have arrived until tReorder expires
    CPPUNIT_ASSERT_EQUAL((size_t) 1, upper_->received.size());

    // after tReordering == 0.015 the SDUs D is retrieved in addition to A
    wns::simulator::getEventScheduler()->processOneEvent();
    CPPUNIT_ASSERT_DOUBLES_EQUAL((double)wns::simulator::getEventScheduler()->getTime(), (double) 0.017, 0.00001);
    CPPUNIT_ASSERT_EQUAL((size_t) 2, upper_->received.size());
    CPPUNIT_ASSERT_EQUAL(compoundA->getLengthInBits(), upper_->received.front()->getLengthInBits()); // SDU A with 70 bits
    CPPUNIT_ASSERT_EQUAL(compoundD->getLengthInBits(), upper_->received.back()->getLengthInBits()); // SDU D with 50 bits
}

void
SACSegmentingQueueIntegrationTest::testThirdSegmentLost()
{
    latePrepare();
    prepareFourCompounds();

    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), lower_->sent.size());

    CPPUNIT_ASSERT(segmentingQueue_->queueHasPDUs(1)); // queue with CID=1

    // get the first segment from the queue
    wns::ldk::CompoundPtr segment1 = segmentingQueue_->getHeadOfLinePDUSegment(1, 126);
    CPPUNIT_ASSERT_EQUAL((Bit) 126, segment1->getLengthInBits());
    lower_->onData(segment1);

    wns::simulator::getEventScheduler()->scheduleDelay(events::NoOp(), 0.001);
    wns::simulator::getEventScheduler()->processOneEvent();

    // complete SDU A should have already arrived
    CPPUNIT_ASSERT_EQUAL(compoundA->getLengthInBits(), upper_->received.front()->getLengthInBits());
    CPPUNIT_ASSERT_EQUAL((size_t) 1, upper_->received.size());

     // now at t=0.001, second segement arrives
    wns::ldk::CompoundPtr segment2 = segmentingQueue_->getHeadOfLinePDUSegment(1, 126);
    CPPUNIT_ASSERT_EQUAL((Bit) 126, segment2->getLengthInBits());
    lower_->onData(segment2);

    // now SDU A and B should be received:
    CPPUNIT_ASSERT_EQUAL((size_t) 2, upper_->received.size());
    CPPUNIT_ASSERT_EQUAL(compoundA->getLengthInBits(), upper_->received.front()->getLengthInBits()); // SDU A with 70 bits
    CPPUNIT_ASSERT_EQUAL(compoundB->getLengthInBits(), upper_->received.back()->getLengthInBits()); // SDU B with 80 bits
    wns::simulator::getEventScheduler()->scheduleDelay(events::NoOp(), 0.001);
    wns::simulator::getEventScheduler()->processOneEvent();

    // now at t=0.002, third segement gets lost
    wns::ldk::CompoundPtr lost = segmentingQueue_->getHeadOfLinePDUSegment(1, 126);
    CPPUNIT_ASSERT_EQUAL((Bit) 78, lost->getLengthInBits());

    // as segment 3 was lost, no tReorder timer was started
    CPPUNIT_ASSERT_EQUAL((size_t) 2, upper_->received.size());
}

void
SACSegmentingQueueIntegrationTest::testClearingTReordering()
{
    // here we want to test if the t-reordering timer gets cleared
    // after the first expiration

    latePrepare();
    prepareFourCompounds();

    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), lower_->sent.size());

    CPPUNIT_ASSERT(segmentingQueue_->queueHasPDUs(1)); // queue with CID=1

    // get the first segment from the queue which is lost, this affects PDU A
    wns::ldk::CompoundPtr lost = segmentingQueue_->getHeadOfLinePDUSegment(1, 80);
    CPPUNIT_ASSERT_EQUAL((Bit) 80, lost->getLengthInBits());

    wns::simulator::getEventScheduler()->scheduleDelay(events::NoOp(), 0.001);
    wns::simulator::getEventScheduler()->processOneEvent();

    // nothing should have arrived
    CPPUNIT_ASSERT_EQUAL((size_t) 0, upper_->received.size());

    // now at t=0.001, second segement arrives which should trigger t-reordering timer
    // this segement contains the rest of A, the entire B and some C bits
    wns::ldk::CompoundPtr segment2 = segmentingQueue_->getHeadOfLinePDUSegment(1, 126);
    CPPUNIT_ASSERT_EQUAL((Bit) 126, segment2->getLengthInBits());
    lower_->onData(segment2);

    // nothing should have arrived
    CPPUNIT_ASSERT_EQUAL((size_t) 0, upper_->received.size());

    // make the simulation time jump to t-reorder timer expiration
    wns::simulator::getEventScheduler()->processOneEvent();
    CPPUNIT_ASSERT_DOUBLES_EQUAL((double)wns::simulator::getEventScheduler()->getTime(), (double) 0.016, 0.00001);

    // now we should see PDU B
    CPPUNIT_ASSERT_EQUAL(compoundB->getLengthInBits(), upper_->received.front()->getLengthInBits()); // SDU B with 80 bits


    // now at t=0.016, we get a third segment which contains some C bits
    wns::ldk::CompoundPtr segment3 = segmentingQueue_->getHeadOfLinePDUSegment(1, 30);
    CPPUNIT_ASSERT_EQUAL((Bit) 30, segment3->getLengthInBits());
    lower_->onData(segment3);

    // we should still only see 1 PDU:
     CPPUNIT_ASSERT_EQUAL((size_t) 1, upper_->received.size());

     wns::simulator::getEventScheduler()->scheduleDelay(events::NoOp(), 0.001);
     wns::simulator::getEventScheduler()->processOneEvent();

     // now at t=0.017, we get a fourth segment which contains some C bits that go lost
    wns::ldk::CompoundPtr lost2 = segmentingQueue_->getHeadOfLinePDUSegment(1, 30);
    CPPUNIT_ASSERT_EQUAL((Bit) 30, lost2->getLengthInBits());

    wns::simulator::getEventScheduler()->scheduleDelay(events::NoOp(), 0.001);
    wns::simulator::getEventScheduler()->processOneEvent();

    // now at t=0.018, we get a fifth segment which contains the rest of C and all D
    wns::ldk::CompoundPtr segment5 = segmentingQueue_->getHeadOfLinePDUSegment(1, 128);
    CPPUNIT_ASSERT_EQUAL((Bit) 64, segment5->getLengthInBits());
    lower_->onData(segment5);

    // this should start t-reordering again, which should expire 0.015s later::
    wns::simulator::getEventScheduler()->processOneEvent();
    CPPUNIT_ASSERT_DOUBLES_EQUAL((double)wns::simulator::getEventScheduler()->getTime(), (double) 0.033, 0.00001);

    // now SDU D should also be receibed:
    CPPUNIT_ASSERT_EQUAL((size_t) 2, upper_->received.size());
    CPPUNIT_ASSERT_EQUAL(compoundB->getLengthInBits(), upper_->received.front()->getLengthInBits()); // SDU B with 80 bits
    CPPUNIT_ASSERT_EQUAL(compoundD->getLengthInBits(), upper_->received.back()->getLengthInBits()); // SDU D with 50 bits
}
