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

#include <WNS/probe/bus/ContextFilterProbeBus.hpp>
#include <WNS/probe/bus/tests/ProbeBusStub.hpp>
#include <WNS/TestFixture.hpp>

#include <WNS/pyconfig/Parser.hpp>
#include <sstream>

namespace wns { namespace probe { namespace bus { namespace tests {

    /**
     * @brief Tests for the ContextFilterProbeBus
     * @author Ralf Pabst <pab@comnets.rwth-aachen.de>
     */
    class ContextFilterProbeBusTest : public wns::TestFixture  {
        CPPUNIT_TEST_SUITE( ContextFilterProbeBusTest );
        CPPUNIT_TEST( test );
        CPPUNIT_TEST_SUITE_END();

    public:
        void prepare();
        void cleanup();

        void test();
        void testRegistry();

    private:
        ProbeBus* theContextFilterProbeBus;
    };
}}}}

using namespace wns::probe::bus::tests;

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ContextFilterProbeBusTest, wns::testsuite::Default() );

void
ContextFilterProbeBusTest::prepare()
{
}

void
ContextFilterProbeBusTest::cleanup()
{
}

void
ContextFilterProbeBusTest::test()
{
    wns::pyconfig::View config =
        wns::pyconfig::Parser::fromString("import openwns.probebus; a = openwns.probebus.ContextFilterProbeBus('Foo', [ 42,43 ])\n");

    theContextFilterProbeBus = new ContextFilterProbeBus(config.get("a"));

    ProbeBusStub listener;

    listener.startObserving(theContextFilterProbeBus);

    {
        // Correct Key and Number --> forwards the measurement
        wns::probe::bus::Context tmp;
        tmp.insertInt("Foo", 42);
        this->theContextFilterProbeBus->forwardMeasurement(1.3, 2.0, tmp);
        CPPUNIT_ASSERT(listener.receivedCounter == 1);
        CPPUNIT_ASSERT(listener.receivedValues[0] == 2.0);
    }

    {
        // Correct Key, wrong Number --> does not forward the measurement
        wns::probe::bus::Context tmp;
        tmp.insertInt("Foo", 41);
        this->theContextFilterProbeBus->forwardMeasurement(1.4, 2.0, tmp);
        CPPUNIT_ASSERT(listener.receivedCounter == 1);
        CPPUNIT_ASSERT(listener.receivedValues[0] == 2.0);
    }

    {
        // Wrong Key, correct Number --> does not forward the measurement
        wns::probe::bus::Context tmp;
        tmp.insertInt("Bar", 42);
        this->theContextFilterProbeBus->forwardMeasurement(1.5, 2.0, tmp);
        CPPUNIT_ASSERT(listener.receivedCounter == 1);
        CPPUNIT_ASSERT(listener.receivedValues[0] == 2.0);
    }

    {
        // Correct Key, another correct Number --> forwards the measurement
        wns::probe::bus::Context tmp;
        tmp.insertInt("Foo", 43);
        this->theContextFilterProbeBus->forwardMeasurement(1.6, 42.0, tmp);
        CPPUNIT_ASSERT(listener.receivedCounter == 2);
        CPPUNIT_ASSERT(listener.receivedValues[1] == 42.0);
    }

    delete theContextFilterProbeBus;
}
