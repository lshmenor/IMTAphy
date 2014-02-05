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

#include <WNS/ldk/tests/FUTestBase.hpp>
#include <WNS/ldk/buffer/Bounded.hpp>
#include <WNS/ldk/sar/DynamicSAR.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>

#include <WNS/pyconfig/Parser.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace wns { namespace ldk { namespace sar { namespace tests {

                class DynamicSARTest
                    : public wns::ldk::tests::FUTestBase
                {
                    CPPUNIT_TEST_SUITE( DynamicSARTest );
                    CPPUNIT_TEST( testSendSmallCompound );
                    CPPUNIT_TEST( testSendLargeCompound );
                    CPPUNIT_TEST( testSendDifferentAcceptanceSizes );
                    CPPUNIT_TEST( testReceiveSmallCompound );
                    CPPUNIT_TEST( testReceiveLargeCompound );
                    CPPUNIT_TEST( testReceiveDifferentAcceptanceSizes );
                    CPPUNIT_TEST( testReceiveUnorderedSegmentsOneCompound );
                    CPPUNIT_TEST( testReceiveUnorderedSegmentsTwoCompounds );
                    CPPUNIT_TEST( testReceiveDuplicatedSegment );
                    CPPUNIT_TEST_SUITE_END();
                public:
                    void
                    testSendSmallCompound();

                    void
                    testSendLargeCompound();

                    void
                    testSendDifferentAcceptanceSizes();

                    void
                    testReceiveSmallCompound();

                    void
                    testReceiveLargeCompound();

                    void
                    testReceiveDifferentAcceptanceSizes();

                    void
                    testReceiveUnorderedSegmentsOneCompound();

                    void
                    testReceiveUnorderedSegmentsTwoCompounds();

                    void
                    testReceiveDuplicatedSegment();

                private:
                    virtual void
                    setUpTestFUs();

                    virtual void
                    tearDownTestFUs()
                    {}

                    virtual FunctionalUnit*
                    getUpperTestFU() const;

                    virtual FunctionalUnit*
                    getLowerTestFU() const;

                    buffer::Bounded* buffer;
                    DynamicSAR* dynamicSAR;
                };

                CPPUNIT_TEST_SUITE_REGISTRATION( DynamicSARTest );

                void
                DynamicSARTest::setUpTestFUs()
                {
                    wns::pyconfig::Parser pyco;
                    pyco.loadString("from openwns.Buffer import Bounded\n"
                                    "buffer = Bounded(size = 100, probingEnabled = False)\n");
                    wns::pyconfig::View view(pyco, "buffer");
                    buffer = new buffer::Bounded(getFUN(), view);
                    getFUN()->addFunctionalUnit("buffer", buffer);

                    std::stringstream ss;
                    ss << "from openwns.SAR import DynamicSAR\n"
                       << "sar = DynamicSAR(\n"
                       << "  maxSegmentSize = 1024\n"
                       << ")\n";
                    wns::pyconfig::Parser pycoSAR;
                    pycoSAR.loadString(ss.str());
                    wns::pyconfig::View viewSAR(pycoSAR, "sar");
                    dynamicSAR = new DynamicSAR(getFUN(), viewSAR);
                    getFUN()->addFunctionalUnit("dynamicSAR", dynamicSAR);

                    buffer->connect(dynamicSAR);
                } // setUpTestFUs

                wns::ldk::FunctionalUnit*
                DynamicSARTest::getUpperTestFU() const
                {
                    return buffer;
                } // getUpperTestFU

                wns::ldk::FunctionalUnit*
                DynamicSARTest::getLowerTestFU() const
                {
                    return dynamicSAR;
                } // getLowerTestFU

                void
                DynamicSARTest::testSendSmallCompound()
                {
                    getUpperStub()->setSizes(64, 128);
                    getUpperStub()->sendData(newFakeCompound());
                    getUpperStub()->sendData(newFakeCompound());

                    Bit commandPoolSize = 0;
                    Bit dataSize = 0;
                    getLowerStub()->calculateSizes(getLowerStub()->sent[0]
                                                   ->getCommandPool(), commandPoolSize, dataSize);
                    CPPUNIT_ASSERT_EQUAL(Bit(0), commandPoolSize);
                    CPPUNIT_ASSERT_EQUAL(Bit(192), dataSize);

                    commandPoolSize = 0;
                    dataSize = 0;
                    getLowerStub()->calculateSizes(getLowerStub()->sent[1]
                                                   ->getCommandPool(), commandPoolSize, dataSize);
                    CPPUNIT_ASSERT_EQUAL(Bit(0), commandPoolSize);
                    CPPUNIT_ASSERT_EQUAL(Bit(192), dataSize);

                    CPPUNIT_ASSERT_EQUAL(size_t(2), getLowerStub()->sent.size());
                } // testSendSmallCompound

                void
                DynamicSARTest::testSendLargeCompound()
                {
                    getUpperStub()->setSizes(64, 1536);
                    getUpperStub()->sendData(newFakeCompound());
                    getUpperStub()->sendData(newFakeCompound());

                    Bit commandPoolSize = 0;
                    Bit dataSize = 0;
                    getLowerStub()->calculateSizes(getLowerStub()->sent[0]
                                                   ->getCommandPool(), commandPoolSize, dataSize);
                    CPPUNIT_ASSERT_EQUAL(Bit(0), commandPoolSize);
                    CPPUNIT_ASSERT_EQUAL(Bit(1024), dataSize);

                    commandPoolSize = 0;
                    dataSize = 0;
                    getLowerStub()->calculateSizes(getLowerStub()->sent[1]
                                                   ->getCommandPool(), commandPoolSize, dataSize);
                    CPPUNIT_ASSERT_EQUAL(Bit(0), commandPoolSize);
                    CPPUNIT_ASSERT_EQUAL(Bit(576), dataSize);

                    commandPoolSize = 0;
                    dataSize = 0;
                    getLowerStub()->calculateSizes(getLowerStub()->sent[2]
                                                   ->getCommandPool(), commandPoolSize, dataSize);
                    CPPUNIT_ASSERT_EQUAL(Bit(0), commandPoolSize);
                    CPPUNIT_ASSERT_EQUAL(Bit(1024), dataSize);

                    commandPoolSize = 0;
                    dataSize = 0;
                    getLowerStub()->calculateSizes(getLowerStub()->sent[3]
                                                   ->getCommandPool(), commandPoolSize, dataSize);
                    CPPUNIT_ASSERT_EQUAL(Bit(0), commandPoolSize);
                    CPPUNIT_ASSERT_EQUAL(Bit(576), dataSize);

                    CPPUNIT_ASSERT_EQUAL(size_t(4), getLowerStub()->sent.size());
                } // testSendLargeCompound

                void
                DynamicSARTest::testSendDifferentAcceptanceSizes()
                {
                    getUpperStub()->setSizes(64, 1536);
                    getLowerStub()->close();
                    getUpperStub()->sendData(newFakeCompound());
                    getUpperStub()->sendData(newFakeCompound());

                    for (int number = 1; number < 82; number++)
                    {
                        getLowerStub()->setAcceptanceSize(number);
                        getLowerStub()->step();
                    }

                    for (int number = 1; number < 82; number++)
                    {
                        Bit commandPoolSize = 0;
                        Bit dataSize = 0;
                        getLowerStub()->calculateSizes(getLowerStub()->sent[number-1]
                                                       ->getCommandPool(), commandPoolSize, dataSize);
                        CPPUNIT_ASSERT_EQUAL(Bit(0), commandPoolSize);
                        if (number == 57)
                            // last segment of the first compound
                            CPPUNIT_ASSERT_EQUAL(Bit(4), dataSize);
                        else if (number == 81)
                            // last segment of the second compound
                            CPPUNIT_ASSERT_EQUAL(Bit(13), dataSize);
                        else
                            CPPUNIT_ASSERT_EQUAL(Bit(number), dataSize);
                    }

                    CPPUNIT_ASSERT_EQUAL(size_t(81), getLowerStub()->sent.size());
                } // testSendDifferentAcceptanceSizes

                void
                DynamicSARTest::testReceiveSmallCompound()
                {
                    getUpperStub()->setSizes(64, 128);
                    getUpperStub()->sendData(newFakeCompound());
                    getUpperStub()->sendData(newFakeCompound());
                    getLowerStub()->onData(getLowerStub()->sent[0]);
                    getLowerStub()->onData(getLowerStub()->sent[1]);

                    Bit commandPoolSize = 0;
                    Bit dataSize = 0;
                    getUpperStub()->calculateSizes(getUpperStub()->received[0]
                                                   ->getCommandPool(), commandPoolSize, dataSize);
                    CPPUNIT_ASSERT_EQUAL(Bit(64), commandPoolSize);
                    CPPUNIT_ASSERT_EQUAL(Bit(128), dataSize);
                    CPPUNIT_ASSERT_EQUAL(0, getUpperStub()->getCommand(
                                             getUpperStub()->received[0])->magic.sequenceNumber);

                    commandPoolSize = 0;
                    dataSize = 0;
                    getUpperStub()->calculateSizes(getUpperStub()->received[1]
                                                   ->getCommandPool(), commandPoolSize, dataSize);
                    CPPUNIT_ASSERT_EQUAL(Bit(64), commandPoolSize);
                    CPPUNIT_ASSERT_EQUAL(Bit(128), dataSize);
                    CPPUNIT_ASSERT_EQUAL(1, getUpperStub()->getCommand(
                                             getUpperStub()->received[1])->magic.sequenceNumber);

                    CPPUNIT_ASSERT_EQUAL(size_t(2), getUpperStub()->received.size());
                } // testReceiveSmallCompound

                void
                DynamicSARTest::testReceiveLargeCompound()
                {
                    getUpperStub()->setSizes(64, 1536);
                    getUpperStub()->sendData(newFakeCompound());
                    getUpperStub()->sendData(newFakeCompound());
                    getLowerStub()->onData(getLowerStub()->sent[0]);
                    getLowerStub()->onData(getLowerStub()->sent[1]);
                    getLowerStub()->onData(getLowerStub()->sent[2]);
                    getLowerStub()->onData(getLowerStub()->sent[3]);

                    Bit commandPoolSize = 0;
                    Bit dataSize = 0;
                    getUpperStub()->calculateSizes(getUpperStub()->received[0]
                                                   ->getCommandPool(), commandPoolSize, dataSize);
                    CPPUNIT_ASSERT_EQUAL(Bit(64), commandPoolSize);
                    CPPUNIT_ASSERT_EQUAL(Bit(1536), dataSize);
                    CPPUNIT_ASSERT_EQUAL(0, getUpperStub()->getCommand(
                                             getUpperStub()->received[0])->magic.sequenceNumber);

                    commandPoolSize = 0;
                    dataSize = 0;
                    getUpperStub()->calculateSizes(getUpperStub()->received[1]
                                                   ->getCommandPool(), commandPoolSize, dataSize);
                    CPPUNIT_ASSERT_EQUAL(Bit(64), commandPoolSize);
                    CPPUNIT_ASSERT_EQUAL(Bit(1536), dataSize);
                    CPPUNIT_ASSERT_EQUAL(1, getUpperStub()->getCommand(
                                             getUpperStub()->received[1])->magic.sequenceNumber);

                    CPPUNIT_ASSERT_EQUAL(size_t(2), getUpperStub()->received.size());
                } // testReceiveLargeCompound

                void
                DynamicSARTest::testReceiveDifferentAcceptanceSizes()
                {
                    getUpperStub()->setSizes(64, 1536);
                    getLowerStub()->close();
                    getUpperStub()->sendData(newFakeCompound());
                    getUpperStub()->sendData(newFakeCompound());

                    for (int number = 1; number < 82; number++)
                    {
                        getLowerStub()->setAcceptanceSize(number);
                        getLowerStub()->step();
                        getLowerStub()->onData(getLowerStub()->sent[number-1]);
                    }

                    Bit commandPoolSize = 0;
                    Bit dataSize = 0;
                    getUpperStub()->calculateSizes(getUpperStub()->received[0]
                                                   ->getCommandPool(), commandPoolSize, dataSize);
                    CPPUNIT_ASSERT_EQUAL(Bit(64), commandPoolSize);
                    CPPUNIT_ASSERT_EQUAL(Bit(1536), dataSize);
                    CPPUNIT_ASSERT_EQUAL(0, getUpperStub()->getCommand(
                                             getUpperStub()->received[0])->magic.sequenceNumber);

                    commandPoolSize = 0;
                    dataSize = 0;
                    getUpperStub()->calculateSizes(getUpperStub()->received[1]
                                                   ->getCommandPool(), commandPoolSize, dataSize);
                    CPPUNIT_ASSERT_EQUAL(Bit(64), commandPoolSize);
                    CPPUNIT_ASSERT_EQUAL(Bit(1536), dataSize);
                    CPPUNIT_ASSERT_EQUAL(1, getUpperStub()->getCommand(
                                             getUpperStub()->received[1])->magic.sequenceNumber);

                    CPPUNIT_ASSERT_EQUAL(size_t(2), getUpperStub()->received.size());
                } // testReceiveDifferentAcceptanceSizes

                void
                DynamicSARTest::testReceiveUnorderedSegmentsOneCompound()
                {
                    getUpperStub()->setSizes(64, 1536);
                    getLowerStub()->close();
                    getUpperStub()->sendData(newFakeCompound());
                    getLowerStub()->setAcceptanceSize(768);
                    getLowerStub()->step();
                    getLowerStub()->setAcceptanceSize(512);
                    getLowerStub()->step();
                    getLowerStub()->setAcceptanceSize(1024);
                    getLowerStub()->step();

                    getLowerStub()->onData(getLowerStub()->sent[1]);
                    getLowerStub()->onData(getLowerStub()->sent[2]);
                    getLowerStub()->onData(getLowerStub()->sent[0]);

                    Bit commandPoolSize = 0;
                    Bit dataSize = 0;
                    getUpperStub()->calculateSizes(getUpperStub()->received[0]
                                                   ->getCommandPool(), commandPoolSize, dataSize);
                    CPPUNIT_ASSERT_EQUAL(Bit(64), commandPoolSize);
                    CPPUNIT_ASSERT_EQUAL(Bit(1536), dataSize);
                    CPPUNIT_ASSERT_EQUAL(0, getUpperStub()->getCommand(
                                             getUpperStub()->received[0])->magic.sequenceNumber);

                    CPPUNIT_ASSERT_EQUAL(size_t(1), getUpperStub()->received.size());
                } // testReceiveUnorderedSegmentsOneCompound

                void
                DynamicSARTest::testReceiveUnorderedSegmentsTwoCompounds()
                {
                    getUpperStub()->setSizes(64, 1536);
                    getLowerStub()->close();
                    getUpperStub()->sendData(newFakeCompound());
                    getUpperStub()->sendData(newFakeCompound());
                    getLowerStub()->setAcceptanceSize(768);
                    getLowerStub()->step();
                    getLowerStub()->setAcceptanceSize(512);
                    getLowerStub()->step();
                    getLowerStub()->setAcceptanceSize(1024);
                    getLowerStub()->step();
                    getLowerStub()->setAcceptanceSize(256);
                    getLowerStub()->step();
                    getLowerStub()->setAcceptanceSize(384);
                    getLowerStub()->step();
                    getLowerStub()->setAcceptanceSize(128);
                    getLowerStub()->step();
                    getLowerStub()->setAcceptanceSize(1024);
                    getLowerStub()->step();

                    getLowerStub()->onData(getLowerStub()->sent[3]);
                    getLowerStub()->onData(getLowerStub()->sent[5]);
                    getLowerStub()->onData(getLowerStub()->sent[0]);
                    getLowerStub()->onData(getLowerStub()->sent[2]);
                    getLowerStub()->onData(getLowerStub()->sent[4]);
                    getLowerStub()->onData(getLowerStub()->sent[6]);
                    getLowerStub()->onData(getLowerStub()->sent[1]);

                    Bit commandPoolSize = 0;
                    Bit dataSize = 0;
                    getUpperStub()->calculateSizes(getUpperStub()->received[0]
                                                   ->getCommandPool(), commandPoolSize, dataSize);
                    CPPUNIT_ASSERT_EQUAL(Bit(64), commandPoolSize);
                    CPPUNIT_ASSERT_EQUAL(Bit(1536), dataSize);
                    CPPUNIT_ASSERT_EQUAL(1, getUpperStub()->getCommand(
                                             getUpperStub()->received[0])->magic.sequenceNumber);

                    commandPoolSize = 0;
                    dataSize = 0;
                    getUpperStub()->calculateSizes(getUpperStub()->received[1]
                                                   ->getCommandPool(), commandPoolSize, dataSize);
                    CPPUNIT_ASSERT_EQUAL(Bit(64), commandPoolSize);
                    CPPUNIT_ASSERT_EQUAL(Bit(1536), dataSize);
                    CPPUNIT_ASSERT_EQUAL(0, getUpperStub()->getCommand(
                                             getUpperStub()->received[1])->magic.sequenceNumber);

                    CPPUNIT_ASSERT_EQUAL(size_t(2), getUpperStub()->received.size());
                } // testReceiveUnorderedSegmentsTwoCompounds

                void
                DynamicSARTest::testReceiveDuplicatedSegment()
                {
                    getUpperStub()->setSizes(64, 1536);
                    getUpperStub()->sendData(newFakeCompound());
                    getLowerStub()->onData(getLowerStub()->sent[0]);
                    getLowerStub()->onData(getLowerStub()->sent[1]->copy());
                    CPPUNIT_ASSERT_THROW(getLowerStub()->onData(getLowerStub()->sent[1]->copy()),
                                         wns::Exception);
                } // testReceiveDuplicatedSegment

             } // tests
        } // sar
    } // ldk
} // wns
