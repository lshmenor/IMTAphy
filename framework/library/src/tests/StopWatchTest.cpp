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

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// The stopwatch test is part of the openWNS developers handobook. The documentation
// references line numbers there. If you change this file pleas make sure to
// update the documentation chapter on CPP Unit tests.
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// begin example "wns.tests.StopWatchTest.example"
#include <WNS/TestFixture.hpp>
#include <WNS/StopWatch.hpp>

namespace wns { namespace tests {

	/**
	 * @brief test for wns::StopWatch
	 * @author Marc Schinnenburg <marc@schinnenburg.com>
	 */
	class StopWatchTest :
		public wns::TestFixture
	{
        CPPUNIT_TEST_SUITE( StopWatchTest );
        CPPUNIT_TEST( testConstructor );
		CPPUNIT_TEST( testGetInSeconds );
		CPPUNIT_TEST_SUITE_END();

    public:

		void
		prepare()
		{
		}

		void
		cleanup()
		{
		}

        void
        testConstructor()
        {
            StopWatch sw;
            CPPUNIT_ASSERT( 0.0 == sw.getInSeconds() );
            CPPUNIT_ASSERT_MESSAGE( sw.toString(), sw.toString() == "0 s");
        }

		void
		testGetInSeconds()
		{
            StopWatch sw;

            // 2 s
			timespec delay;
			delay.tv_sec = static_cast<time_t>(2);
			delay.tv_nsec = static_cast<long>(0*1E9);

            sw.start();
            nanosleep(&delay, NULL);
            sw.stop();

            CPPUNIT_ASSERT_EQUAL( 2, static_cast<int>(round(sw.getInSeconds())) );
        }
    };

	CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( StopWatchTest, wns::testsuite::Default() );

} // namespace tests
} // namespace wns

//end example
