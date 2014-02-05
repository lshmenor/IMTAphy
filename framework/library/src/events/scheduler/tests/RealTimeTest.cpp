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

#include <WNS/events/scheduler/tests/InterfaceTest.hpp>
#include <WNS/events/scheduler/RealTime.hpp>
#include <WNS/TestFixture.hpp>
#include <WNS/StopWatch.hpp>

namespace wns { namespace events { namespace scheduler { namespace tests {

    class RealTimeTest :
        public wns::TestFixture
    {
        CPPUNIT_TEST_SUITE( RealTimeTest );
        CPPUNIT_TEST( testRealTimeBehavior );
        CPPUNIT_TEST_SUITE_END();

    private:
        virtual void
        prepare()
        {
        }

        virtual void
        cleanup()
        {
        }

        static void
        noOp()
        {
        }

        void
        testRealTimeBehavior()
        {
            // NOTE: If this test fails it might be because your
            // computer is too slow (or heavy loaded at the moment).
            // In this case, the test didn't really _fail_. Try to
            // run the test on a faster computer again ...
            RealTime scheduler;

            double expectedDuration = 2.0;
            double eventIAT = 0.001;

            for(int ii = 0; ii < expectedDuration/eventIAT; ++ii)
            {
                scheduler.schedule(&RealTimeTest::noOp, eventIAT*ii);
            }

            StopWatch sw;
            sw.start();
            scheduler.start();
            sw.stop();

            double duration = sw.getInSeconds();

            // should be at maximum 10 ms behind (see RealTime.hpp
            // why)
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedDuration, duration, 10E-3);
        }
    };

    /**
       @todo (msg) Disabled since it does not work under high system
       load. Reason: The scheduler can't schedule the events in
       realtime under high system load since the internal timing
       function of the scheduler can not return in time. This
       problem (erratic test) can only be solved by providing a
       clock mock (which obviously does not depend on system load)
       or by running the test in a statistical manner: The test can
       be run multiple times and if it is ok in 80% of all runs the test is
       marked ok.
    */
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( RealTimeTest, wns::testsuite::Disabled() );

} // tests
} // scheduler
} // events
} // wns
