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

#include <WNS/ldk/sar/reassembly/ReorderingWindow.hpp>

#include <WNS/TestFixture.hpp>
#include <WNS/ldk/helper/FakePDU.hpp>
#include <WNS/pyconfig/Parser.hpp>
#include <WNS/pyconfig/View.hpp>

#include <boost/bind.hpp>
#include <cppunit/extensions/HelperMacros.h>

namespace wns { namespace ldk { namespace sar { namespace reassembly { namespace tests {

class ReorderingWindowTest:
    public wns::TestFixture
{
    CPPUNIT_TEST_SUITE( ReorderingWindowTest );
    CPPUNIT_TEST( testInSequence );
    CPPUNIT_TEST( testOutOfSequence );
    CPPUNIT_TEST( testLossDetection );
    CPPUNIT_TEST( testDuplicates );
    CPPUNIT_TEST_SUITE_END();

    ReorderingWindow* rw;

    std::deque<long> receivedSNs_;

public:

    void
    prepare();

    void
    cleanup();

    void
    reassemble(long, wns::ldk::CompoundPtr);

    void
    testInSequence();

    void
    testOutOfSequence();

    void
    testLossDetection();

    void
    testDuplicates();
};

} // tests
} // reassembly
} // sar
} // ldk
} // wns

using namespace wns::ldk::sar::reassembly::tests;

CPPUNIT_TEST_SUITE_REGISTRATION( ReorderingWindowTest );

void ReorderingWindowTest::prepare()
{
    receivedSNs_.clear();
    wns::pyconfig::Parser config;
    config.loadString(
        "from openwns.SAR import ReorderingWindow\n"
        "testee = ReorderingWindow(snFieldLength=5)\n"
        "testee.logger.level = 3\n"
    );

    wns::pyconfig::View configView(config, "testee");
    rw = new ReorderingWindow(configView);
    rw->connectToReassemblySignal(boost::bind(&ReorderingWindowTest::reassemble, this, _1, _2));
}

void
ReorderingWindowTest::cleanup()
{
    delete rw;
}

void
ReorderingWindowTest::reassemble(long sn, wns::ldk::CompoundPtr)
{
    receivedSNs_.push_back(sn);
}
void
ReorderingWindowTest::testInSequence()
{
    rw->onSegment(0, wns::ldk::CompoundPtr());
    rw->onSegment(1, wns::ldk::CompoundPtr());
    rw->onSegment(2, wns::ldk::CompoundPtr());
    rw->onSegment(3, wns::ldk::CompoundPtr());
    rw->onSegment(4, wns::ldk::CompoundPtr());
    rw->onSegment(5, wns::ldk::CompoundPtr());
    rw->onSegment(6, wns::ldk::CompoundPtr());
    rw->onSegment(7, wns::ldk::CompoundPtr());
    rw->onSegment(8, wns::ldk::CompoundPtr());
    rw->onSegment(9, wns::ldk::CompoundPtr());
    rw->onSegment(10, wns::ldk::CompoundPtr());
    rw->onSegment(11, wns::ldk::CompoundPtr());
    rw->onSegment(12, wns::ldk::CompoundPtr());
    rw->onSegment(13, wns::ldk::CompoundPtr());
    rw->onSegment(14, wns::ldk::CompoundPtr());
    rw->onSegment(15, wns::ldk::CompoundPtr());
    rw->onSegment(16, wns::ldk::CompoundPtr());
    rw->onSegment(17, wns::ldk::CompoundPtr());
    rw->onSegment(18, wns::ldk::CompoundPtr());
    rw->onSegment(19, wns::ldk::CompoundPtr());
    rw->onSegment(20, wns::ldk::CompoundPtr());
    rw->onSegment(21, wns::ldk::CompoundPtr());

    CPPUNIT_ASSERT_EQUAL((size_t) 22, receivedSNs_.size());

    for (long ii = 0; ii <= 21; ++ii)
    {
        CPPUNIT_ASSERT_EQUAL(ii, receivedSNs_[ii]);
    }
}

void
ReorderingWindowTest::testOutOfSequence()
{
    rw->onSegment(1, wns::ldk::CompoundPtr());
    rw->onSegment(3, wns::ldk::CompoundPtr());
    rw->onSegment(0, wns::ldk::CompoundPtr());
    rw->onSegment(2, wns::ldk::CompoundPtr());
    rw->onSegment(4, wns::ldk::CompoundPtr());
    rw->onSegment(5, wns::ldk::CompoundPtr());
    rw->onSegment(6, wns::ldk::CompoundPtr());
    rw->onSegment(7, wns::ldk::CompoundPtr());
    rw->onSegment(8, wns::ldk::CompoundPtr());
    rw->onSegment(10, wns::ldk::CompoundPtr());
    rw->onSegment(9, wns::ldk::CompoundPtr());
    rw->onSegment(11, wns::ldk::CompoundPtr());
    rw->onSegment(20, wns::ldk::CompoundPtr());
    rw->onSegment(13, wns::ldk::CompoundPtr());
    rw->onSegment(14, wns::ldk::CompoundPtr());
    rw->onSegment(15, wns::ldk::CompoundPtr());
    rw->onSegment(16, wns::ldk::CompoundPtr());
    rw->onSegment(17, wns::ldk::CompoundPtr());
    rw->onSegment(18, wns::ldk::CompoundPtr());
    rw->onSegment(19, wns::ldk::CompoundPtr());
    rw->onSegment(12, wns::ldk::CompoundPtr());
    rw->onSegment(21, wns::ldk::CompoundPtr());

    CPPUNIT_ASSERT_EQUAL((size_t) 22, receivedSNs_.size());

    for (long ii = 0; ii <= 21; ++ii)
    {
        CPPUNIT_ASSERT_EQUAL(ii, receivedSNs_[ii]);
    }
}

void
ReorderingWindowTest::testLossDetection()
{
    rw->onSegment(0, wns::ldk::CompoundPtr());
    rw->onSegment(2, wns::ldk::CompoundPtr()); // Starts Timeout

    // Fire the timeout
    wns::simulator::getEventScheduler()->processOneEvent();

    rw->onSegment(3, wns::ldk::CompoundPtr());

    CPPUNIT_ASSERT_EQUAL((size_t) 3, receivedSNs_.size());

    CPPUNIT_ASSERT_EQUAL((long) 0, receivedSNs_[0]);
    CPPUNIT_ASSERT_EQUAL((long) 2, receivedSNs_[1]);
    CPPUNIT_ASSERT_EQUAL((long) 3, receivedSNs_[2]);
}

void
ReorderingWindowTest::testDuplicates()
{
    rw->onSegment((long) 0, wns::ldk::CompoundPtr());
    rw->onSegment((long) 1, wns::ldk::CompoundPtr());
    rw->onSegment((long) 1, wns::ldk::CompoundPtr());
    rw->onSegment((long) 3, wns::ldk::CompoundPtr());
    rw->onSegment((long) 3, wns::ldk::CompoundPtr());
    rw->onSegment((long) 2, wns::ldk::CompoundPtr());

    CPPUNIT_ASSERT_EQUAL((size_t) 4, receivedSNs_.size());

    CPPUNIT_ASSERT_EQUAL((long) 0, receivedSNs_[0]);
    CPPUNIT_ASSERT_EQUAL((long) 1, receivedSNs_[1]);
    CPPUNIT_ASSERT_EQUAL((long) 2, receivedSNs_[2]);
    CPPUNIT_ASSERT_EQUAL((long) 3, receivedSNs_[3]);
}
