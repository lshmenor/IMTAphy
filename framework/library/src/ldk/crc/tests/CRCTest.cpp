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

#include "CRCTest.hpp"

// begin example "wns.ldk.crc.tests.CRCTest.headers.example"
#include <WNS/pyconfig/Parser.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/ldk/fun/Main.hpp>
// end example
#include <WNS/ldk/tools/Bridge.hpp>
#include <WNS/ldk/tools/Consumer.hpp>
#include <WNS/ldk/tools/Producer.hpp>
#include <WNS/ldk/helper/FakePDU.hpp>

#include <vector>


using namespace wns::ldk::crc;
using namespace wns::ldk::tools;


CPPUNIT_TEST_SUITE_REGISTRATION( CRCTest );

const int
CRCTest::checkSumSize = 8;

// begin example "wns.ldk.crc.tests.CRCTest.prepare.example"
void
CRCTest::setUp()
{
    layer = new wns::ldk::tests::LayerStub();
    fuNet = new fun::Main(layer);

    wns::pyconfig::Parser emptyConfig;
    upper = new tools::Stub(fuNet, emptyConfig);

    //:BEWARE: Construction of this test's environment is not complete yet
} // setUp
// end example

// begin example "wns.ldk.crc.tests.CRCTest.setupCRC.example"
void
CRCTest::setUpCRC(const int _checkSumSize, const bool _Dropping)
{
    // Construct CRC FU
    std::stringstream ss;
    ss << "from openwns.CRC import CRC\n"
       << "crc = CRC(\"PERstub\",\n"
       << "  CRCsize = " << _checkSumSize << ",\n"
       << "  isDropping = "<< (_Dropping ? "True" : "False") << ")\n";

    wns::pyconfig::Parser all;
    all.loadString(ss.str());

    wns::pyconfig::View config(all, "crc");

    crc = new CRC(fuNet, config);
}
// end example

// begin example "wns.ldk.crc.tests.CRCTest.setupPERProvider.example"
void
CRCTest::setUpPERProvider(const double _PER)
{
    // Construct fixed PER stub
    std::stringstream ss;
    ss << "fixedPER = "<< _PER <<"\n";

    wns::pyconfig::Parser all;
    all.loadString(ss.str());

    lower = new PERProviderStub(fuNet, all);

    upper
        ->connect(crc)
        ->connect(lower);

    fuNet->addFunctionalUnit("upper",upper);
    fuNet->addFunctionalUnit("myCRC",crc);
    fuNet->addFunctionalUnit("PERstub",lower);
    fuNet->onFUNCreated();

    wns::simulator::getEventScheduler()->reset();

}
// end example

void
CRCTest::tearDown()
{
    delete fuNet;
    delete layer;
} // tearDown

// begin example "wns.ldk.crc.tests.CRCTest.testNoErrors.example"
void
CRCTest::testNoErrors()
{
    setUpCRC(checkSumSize, true);
    setUpPERProvider(0.0);

    upper->sendData(fuNet->createCompound());

    CPPUNIT_ASSERT_EQUAL( size_t(1), lower->sent.size() );

    lower->onData(lower->sent[0]);

    CPPUNIT_ASSERT_EQUAL( size_t(1), upper->received.size() );

} // testNoErrors
// end example

void
CRCTest::testErrors()
{
    setUpCRC(checkSumSize, true);
    setUpPERProvider(1.0);
    CPPUNIT_ASSERT(!crc->isMarking());

    upper->sendData(fuNet->createCompound());

    CPPUNIT_ASSERT_EQUAL( size_t(1), lower->sent.size() );

    lower->onData(lower->sent[0]);

    CPPUNIT_ASSERT_EQUAL( size_t(0), upper->received.size() );

} // testErrors

void
CRCTest::testMarking()
{
    setUpCRC(checkSumSize, false);
    setUpPERProvider(1.0);
    CPPUNIT_ASSERT(crc->isMarking());

    upper->sendData(fuNet->createCompound());

    CPPUNIT_ASSERT_EQUAL( size_t(1), lower->sent.size() );

    lower->onData(lower->sent[0]);

    CPPUNIT_ASSERT_EQUAL( size_t(1), upper->received.size() );

} // testMarking

void
CRCTest::testSize()
{
    setUpCRC(checkSumSize, false);
    setUpPERProvider(1.0);

    helper::FakePDUPtr innerPDU(new helper::FakePDU());
    CommandPool* commandPool = fuNet->createCommandPool();

    innerPDU->setLengthInBits(42);

    CompoundPtr aPDU(new Compound(commandPool, innerPDU));

    CPPUNIT_ASSERT_EQUAL(Bit(42), aPDU->getLengthInBits());

    upper->sendData(aPDU);

    CPPUNIT_ASSERT_EQUAL( size_t(1), lower->sent.size() );
    CPPUNIT_ASSERT_EQUAL( Bit(42+checkSumSize), lower->sent[0]->getLengthInBits() );

} // testSize


void
CRCTest::emptyBuffers()
{
    while (!lower->sent.empty())
    {
        lower->sent.pop_front();
    }
    while (!upper->received.empty())
    {
        upper->received.pop_front();
    }
}


