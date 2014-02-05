/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 16, D-52074 Aachen, Germany
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

#include <WNS/TestFixture.hpp>
#include <WNS/simulator/ISimulator.hpp>

#include <WNS/probe/bus/TimeWindowProbeBus.hpp>
#include <WNS/probe/bus/tests/ProbeBusStub.hpp>
#include <WNS/pyconfig/Parser.hpp>

#include <sstream>

namespace wns { namespace probe { namespace bus { namespace tests {

    /**
     * @brief Test for the TimeWindowProbeBus
     * @author Daniel Bültmann <me@daniel-bueltmann.de>
     */
    class TimeWindowProbeBusTest:
        public wns::TestFixture
    {
        CPPUNIT_TEST_SUITE( TimeWindowProbeBusTest );
        CPPUNIT_TEST( testNotAttached );
        CPPUNIT_TEST( testAttached );
        CPPUNIT_TEST( testDetached );
        CPPUNIT_TEST_SUITE_END();

    public:
        void prepare();
        void cleanup();

        void testNotAttached();
        void testAttached();
        void testDetached();

    private:
        ProbeBusStub* master_;
        ProbeBus* testee_;
        ProbeBusStub* listener_;

    };
} // tests
} // bus
} // probe
} // wns

using namespace wns::probe::bus::tests;

CPPUNIT_TEST_SUITE_REGISTRATION( TimeWindowProbeBusTest );

void
TimeWindowProbeBusTest::prepare()
{
    std::string config = "import openwns.probebus\n"
        "timewindow = openwns.probebus.TimeWindowProbeBus(start=0.1, end=100.12)\n";
    wns::pyconfig::Parser p;
    p.loadString(config);
    master_ = new ProbeBusStub();
    testee_ = new TimeWindowProbeBus(p.get<wns::pyconfig::View>("timewindow"));
    listener_ = new ProbeBusStub();
    // This is delayed by the TimeWindowProbeBus
    testee_->startObserving(master_);
    // The listener immediately connects to the testee
    listener_->startObserving(testee_);
}

void
TimeWindowProbeBusTest::cleanup()
{
    delete master_;
    delete testee_;
    delete listener_;
}

void
TimeWindowProbeBusTest::testNotAttached()
{
    wns::probe::bus::Context reg;
    master_->forwardMeasurement(0.001, 1.345, reg);
    // No time has passed, we should not be attached
    CPPUNIT_ASSERT(listener_->receivedCounter == 0);
}

void
TimeWindowProbeBusTest::testAttached()
{
    wns::probe::bus::Context reg;

    master_->forwardMeasurement(0.001, 6.325, reg);
    // No time has passed, we should not be attached
    CPPUNIT_ASSERT(listener_->receivedCounter == 0);

    CPPUNIT_ASSERT(wns::simulator::getEventScheduler()->processOneEvent());

    CPPUNIT_ASSERT(wns::simulator::getEventScheduler()->getTime() == 0.1);
    master_->forwardMeasurement(0.1, 12.998, reg);

    CPPUNIT_ASSERT(listener_->receivedCounter == 1);
    CPPUNIT_ASSERT(listener_->receivedValues[0] == 12.998);
}

void
TimeWindowProbeBusTest::testDetached()
{
    wns::probe::bus::Context reg;

    master_->forwardMeasurement(0.001, 13.13, reg);
    // No time has passed, we should not be attached
    CPPUNIT_ASSERT(listener_->receivedCounter == 0);

    CPPUNIT_ASSERT(wns::simulator::getEventScheduler()->processOneEvent());
    CPPUNIT_ASSERT(wns::simulator::getEventScheduler()->processOneEvent());

    CPPUNIT_ASSERT(wns::simulator::getEventScheduler()->getTime() == 100.12);

    master_->forwardMeasurement(100.12, 99.0, reg);

    CPPUNIT_ASSERT(listener_->receivedCounter == 0);
}
