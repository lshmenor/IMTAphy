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

#include <WNS/distribution/TimeDependent.hpp>

#include <WNS/pyconfig/Parser.hpp>
#include <WNS/TestFixture.hpp>

namespace wns { namespace distribution { namespace tests {

    class TimeDependentTest :
        public wns::TestFixture
    {
        CPPUNIT_TEST_SUITE( TimeDependentTest );
        CPPUNIT_TEST( testOneEventAtNULL );
        CPPUNIT_TEST( testMultipleEventsAndEventAtNULL );
        CPPUNIT_TEST( testMultipleEvents );
        CPPUNIT_TEST_SUITE_END();

    public:
        void prepare();
        void cleanup();

        void testOneEventAtNULL();
        void testMultipleEventsAndEventAtNULL();
        void testMultipleEvents();
    };

    CPPUNIT_TEST_SUITE_REGISTRATION( TimeDependentTest );

    void
    TimeDependentTest::prepare()
    {
        wns::simulator::getEventScheduler()->reset();
    } // prepare

    void
    TimeDependentTest::cleanup()
    {
    } // cleanup

    void
    TimeDependentTest::testOneEventAtNULL()
    {
        wns::pyconfig::View config = wns::pyconfig::Parser::fromString(
            "import openwns.distribution\n"
            "dist = openwns.distribution.TimeDependent()\n"
            "dist.eventList.append(openwns.distribution.Event(0.0, openwns.distribution.Fixed(47)))\n"
            );
        TimeDependent t(config.get("dist"));
        WNS_ASSERT_MAX_REL_ERROR(47.0, t(), 1E-6);
    } // testOneEventAtNULL

    void
    TimeDependentTest::testMultipleEventsAndEventAtNULL()
    {
        wns::pyconfig::View config = wns::pyconfig::Parser::fromString(
            // begin example "TimeDependent::config.example" unquote
            "import openwns.distribution\n"
            "dist = openwns.distribution.TimeDependent()\n"
            "dist.eventList.append(openwns.distribution.Event(0.0, openwns.distribution.Fixed(47)))\n"
            "dist.eventList.append(openwns.distribution.Event(2.0, openwns.distribution.Fixed(11)))\n"
            "dist.eventList.append(openwns.distribution.Event(5.0, openwns.distribution.Fixed(8)))\n"
            "dist.eventList.append(openwns.distribution.Event(11.0, openwns.distribution.Fixed(15)))\n"
            // end example
            );

        // begin example "TimeDependent::usage.example"
        TimeDependent t(config.get("dist"));
        WNS_ASSERT_MAX_REL_ERROR(47.0, t(), 1E-6);

        wns::simulator::getEventScheduler()->processOneEvent();
        WNS_ASSERT_MAX_REL_ERROR(11.0, t(), 1E-6);
        WNS_ASSERT_MAX_REL_ERROR(simTimeType(2.0), wns::simulator::getEventScheduler()->getTime(), 1E-6);

        wns::simulator::getEventScheduler()->processOneEvent();
        WNS_ASSERT_MAX_REL_ERROR(8.0, t(), 1E-6);
        WNS_ASSERT_MAX_REL_ERROR(simTimeType(5.0), wns::simulator::getEventScheduler()->getTime(), 1E-6);

        wns::simulator::getEventScheduler()->processOneEvent();
        WNS_ASSERT_MAX_REL_ERROR(15.0, t(), 1E-6);
        WNS_ASSERT_MAX_REL_ERROR(simTimeType(11.0), wns::simulator::getEventScheduler()->getTime(), 1E-6);
        // end example
    } // testMultipleEventsAndEventAtNULL

    void
    TimeDependentTest::testMultipleEvents()
    {
        wns::pyconfig::View config = wns::pyconfig::Parser::fromString(
            "import openwns.distribution\n"
            "dist = openwns.distribution.TimeDependent()\n"
            "dist.eventList.append(openwns.distribution.Event(2.0, openwns.distribution.Fixed(11)))\n"
            "dist.eventList.append(openwns.distribution.Event(5.0, openwns.distribution.Fixed(8)))\n"
            "dist.eventList.append(openwns.distribution.Event(11.0, openwns.distribution.Fixed(15)))\n"
            );

        TimeDependent t(config.get("dist"));

//      WNS_ASSERT_MAX_REL_ERROR(15.0, t.getMean(), 1E-6); // the last value

        wns::simulator::getEventScheduler()->processOneEvent();
        WNS_ASSERT_MAX_REL_ERROR(11.0, t(), 1E-6);
        WNS_ASSERT_MAX_REL_ERROR(simTimeType(2.0), wns::simulator::getEventScheduler()->getTime(), 1E-6);

        wns::simulator::getEventScheduler()->processOneEvent();
        WNS_ASSERT_MAX_REL_ERROR(8.0, t(), 1E-6);
        WNS_ASSERT_MAX_REL_ERROR(simTimeType(5.0), wns::simulator::getEventScheduler()->getTime(), 1E-6);

        wns::simulator::getEventScheduler()->processOneEvent();
        WNS_ASSERT_MAX_REL_ERROR(15.0, t(), 1E-6);
        WNS_ASSERT_MAX_REL_ERROR(simTimeType(11.0), wns::simulator::getEventScheduler()->getTime(), 1E-6);

    } // testMultipleEvents

} // tests
} // distribution
} // wns

/*
  Local Variables:
  mode: c++
  fill-column: 80
  c-basic-offset: 8
  c-comment-only-line-offset: 0
  c-tab-always-indent: t
  indent-tabs-mode: t
  tab-width: 8
  End:
*/
