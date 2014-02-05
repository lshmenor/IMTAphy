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

#include <WNS/probe/bus/PassThroughProbeBus.hpp>
#include <WNS/probe/bus/tests/ProbeBusStub.hpp>

#include <WNS/pyconfig/Parser.hpp>

#include <WNS/TestFixture.hpp>
#include <sstream>

namespace wns { namespace probe { namespace bus { namespace tests {

    /**
     * @brief Tests for the PassThroughProbeBus
     * @author Daniel Bültmann <me@daniel-bueltmann.de>
     */
    class PassThroughProbeBusTest : public wns::TestFixture  {
        CPPUNIT_TEST_SUITE( PassThroughProbeBusTest );
        CPPUNIT_TEST( testSingleListener );
        CPPUNIT_TEST( testMultipleListeners );
        CPPUNIT_TEST( testRegistry );
        CPPUNIT_TEST_SUITE_END();

    public:
        void prepare();
        void cleanup();

        void testSingleListener();
        void testMultipleListeners();
        void testRegistry();

    private:
        ProbeBus* thePassThroughProbeBus_;
    };
}}}}

using namespace wns::probe::bus::tests;

CPPUNIT_TEST_SUITE_REGISTRATION( PassThroughProbeBusTest );

void
PassThroughProbeBusTest::prepare()
{
    wns::pyconfig::Parser empty;
    thePassThroughProbeBus_ = new PassThroughProbeBus(empty);
}

void
PassThroughProbeBusTest::cleanup()
{
    delete thePassThroughProbeBus_;
}

void
PassThroughProbeBusTest::testSingleListener()
{
    ProbeBusStub listener;

    listener.startObserving(thePassThroughProbeBus_);

    wns::probe::bus::Context tmp;
    this->thePassThroughProbeBus_->forwardMeasurement(1.0, 2.0, tmp);

    CPPUNIT_ASSERT(listener.receivedCounter == 1);
    CPPUNIT_ASSERT(listener.receivedTimestamps[0] == 1.0);
    CPPUNIT_ASSERT(listener.receivedValues[0] == 2.0);
}

void
PassThroughProbeBusTest::testMultipleListeners()
{
    ProbeBusStub listener1;
    listener1.startObserving(thePassThroughProbeBus_);

    ProbeBusStub listener2;
    listener2.startObserving(thePassThroughProbeBus_);

    wns::probe::bus::Context tmp;
    this->thePassThroughProbeBus_->forwardMeasurement(2.0, 5.0, tmp);

    CPPUNIT_ASSERT(listener1.receivedCounter == 1);
    CPPUNIT_ASSERT(listener1.receivedTimestamps[0] == 2.0);
    CPPUNIT_ASSERT(listener1.receivedValues[0] == 5.0);

    CPPUNIT_ASSERT(listener2.receivedCounter == 1);
    CPPUNIT_ASSERT(listener2.receivedTimestamps[0] == 2.0);
    CPPUNIT_ASSERT(listener2.receivedValues[0] == 5.0);
}

void
PassThroughProbeBusTest::testRegistry()
{
    wns::probe::bus::Context reg;
    reg.insertInt("Peter", 3);
    reg.insertInt("July", 7);

    ProbeBusStub listener1;
    listener1.startObserving(thePassThroughProbeBus_);
    listener1.setFilter("Peter", 3);

    ProbeBusStub listener2;
    listener2.startObserving(thePassThroughProbeBus_);
    listener2.setFilter("July", 15);

    this->thePassThroughProbeBus_->forwardMeasurement(3.0, 17.0, reg);

    CPPUNIT_ASSERT(listener1.receivedCounter == 1);

    CPPUNIT_ASSERT(listener1.receivedTimestamps[0] == 3.0);

    CPPUNIT_ASSERT(listener1.receivedValues[0] == 17.0);

    CPPUNIT_ASSERT(listener2.receivedCounter == 0);
}
