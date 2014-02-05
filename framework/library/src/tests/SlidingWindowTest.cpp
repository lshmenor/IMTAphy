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

#include <WNS/SlidingWindow.hpp>
#include <WNS/events/NoOp.hpp>
#include <WNS/CppUnit.hpp>
#include <WNS/simulator/ISimulator.hpp>


namespace wns { namespace tests {

	class SlidingWindowTest :
		public wns::TestFixture
	{
		CPPUNIT_TEST_SUITE( SlidingWindowTest );
		CPPUNIT_TEST( testNoneInContainer );
		CPPUNIT_TEST( testReset );
		CPPUNIT_TEST( testNormalPut );
		CPPUNIT_TEST( testTwoAtTheSameTime );
		CPPUNIT_TEST( testExactEndOfWindow );
		CPPUNIT_TEST( testGetPerSecond );
		CPPUNIT_TEST( testGetNumSamples );
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
		testNoneInContainer()
		{
			SlidingWindow sw(0.25);
			WNS_ASSERT_MAX_REL_ERROR( 0.0, sw.getAbsolute(), 1E-9 );
		}

		void
		testReset()
		{
			SlidingWindow sw(0.25);
			sw.put(5.0);
			WNS_ASSERT_MAX_REL_ERROR( 0.0, sw.getAbsolute(), 1E-9 );
			wns::simulator::getEventScheduler()->scheduleDelay(events::NoOp(), 0.1);
			wns::simulator::getEventScheduler()->processOneEvent();
			// simTime 0.1
			WNS_ASSERT_MAX_REL_ERROR( 5.0, sw.getAbsolute(), 1E-9 );
			sw.reset();
			WNS_ASSERT_MAX_REL_ERROR( 0.0, sw.getAbsolute(), 1E-9 );
		}

		void
		testGetNumSamples()
		{
			SlidingWindow sw(0.25);

			// simTime 0.0, insert 1@0.0
			sw.put(1.0);
			WNS_ASSERT_MAX_REL_ERROR( 0, sw.getNumSamples(), 0 );

			wns::simulator::getEventScheduler()->scheduleDelay(events::NoOp(), 0.1);
			wns::simulator::getEventScheduler()->processOneEvent();
			// simTime 0.1, 1 sample
			WNS_ASSERT_MAX_REL_ERROR( 1, sw.getNumSamples(), 0 );

			// insert 2@0.1
			sw.put(1.0);
			WNS_ASSERT_MAX_REL_ERROR( 1, sw.getNumSamples(), 0 );

			wns::simulator::getEventScheduler()->scheduleDelay(events::NoOp(), 0.1);
			wns::simulator::getEventScheduler()->processOneEvent();
			// simTime 0.2, 
			WNS_ASSERT_MAX_REL_ERROR( 2, sw.getNumSamples(), 0 );

			// insert 3@0.2
			sw.put(1.0);
			WNS_ASSERT_MAX_REL_ERROR( 2, sw.getNumSamples(), 0 );

			wns::simulator::getEventScheduler()->scheduleDelay(events::NoOp(), 0.05);
			wns::simulator::getEventScheduler()->processOneEvent();
			// simTime 0.25
			WNS_ASSERT_MAX_REL_ERROR( 3, sw.getNumSamples(), 0 );

			wns::simulator::getEventScheduler()->scheduleDelay(events::NoOp(), 0.05);
			wns::simulator::getEventScheduler()->processOneEvent();
			// simTime 0.3
			WNS_ASSERT_MAX_REL_ERROR( 2, sw.getNumSamples(), 0 );

			wns::simulator::getEventScheduler()->scheduleDelay(events::NoOp(), 0.1);
			wns::simulator::getEventScheduler()->processOneEvent();
			// simTime 0.4
			WNS_ASSERT_MAX_REL_ERROR( 1, sw.getNumSamples(), 0 );

			wns::simulator::getEventScheduler()->scheduleDelay(events::NoOp(), 0.1);
			wns::simulator::getEventScheduler()->processOneEvent();
			// simTime 0.5
			WNS_ASSERT_MAX_REL_ERROR( 0, sw.getNumSamples(), 0 );
		}

		void
		testNormalPut()
		{
			SlidingWindow sw(0.25);

			// simTime 0.0
			sw.put(1.0);
			WNS_ASSERT_MAX_REL_ERROR( 0.0, sw.getAbsolute(), 1E-9 );

			wns::simulator::getEventScheduler()->scheduleDelay(events::NoOp(), 0.1);
			wns::simulator::getEventScheduler()->processOneEvent();
			// simTime 0.1
			WNS_ASSERT_MAX_REL_ERROR( 1.0, sw.getAbsolute(), 1E-9 );

			sw.put(2.3);
			WNS_ASSERT_MAX_REL_ERROR( 1.0, sw.getAbsolute(), 1E-9 );

			wns::simulator::getEventScheduler()->scheduleDelay(events::NoOp(), 0.1);
			wns::simulator::getEventScheduler()->processOneEvent();
			// simTime 0.2
			WNS_ASSERT_MAX_REL_ERROR( 3.3, sw.getAbsolute(), 1E-9 );

			sw.put(4.2);
			WNS_ASSERT_MAX_REL_ERROR( 3.3, sw.getAbsolute(), 1E-9 );

			wns::simulator::getEventScheduler()->scheduleDelay(events::NoOp(), 0.05);
			wns::simulator::getEventScheduler()->processOneEvent();
			// simTime 0.25
			WNS_ASSERT_MAX_REL_ERROR( 7.5, sw.getAbsolute(), 1E-9 );

			wns::simulator::getEventScheduler()->scheduleDelay(events::NoOp(), 0.05);
			wns::simulator::getEventScheduler()->processOneEvent();
			// simTime 0.3
			WNS_ASSERT_MAX_REL_ERROR( 6.5, sw.getAbsolute(), 1E-9 );

			wns::simulator::getEventScheduler()->scheduleDelay(events::NoOp(), 0.1);
			wns::simulator::getEventScheduler()->processOneEvent();
			// simTime 0.4
			WNS_ASSERT_MAX_REL_ERROR( 4.2, sw.getAbsolute(), 1E-9 );

			wns::simulator::getEventScheduler()->scheduleDelay(events::NoOp(), 0.1);
			wns::simulator::getEventScheduler()->processOneEvent();
			// simTime 0.5
			WNS_ASSERT_MAX_REL_ERROR( 0.0, sw.getAbsolute(), 1E-9 );
		}

		void
		testTwoAtTheSameTime()
		{
			SlidingWindow sw(0.25);
			// simTime 0.0
			sw.put(2.3);
			sw.put(4.2);

			wns::simulator::getEventScheduler()->scheduleDelay(events::NoOp(), 0.1);
			wns::simulator::getEventScheduler()->processOneEvent();
			// simTime 0.1
			WNS_ASSERT_MAX_REL_ERROR( 6.5, sw.getAbsolute(), 1E-9 );

			wns::simulator::getEventScheduler()->scheduleDelay(events::NoOp(), 0.2);
			wns::simulator::getEventScheduler()->processOneEvent();
			// simTime 0.3
			WNS_ASSERT_MAX_REL_ERROR( 0.0, sw.getAbsolute(), 1E-9 );
		}


		void
		testExactEndOfWindow()
		{
			SlidingWindow sw(0.25);
			// simTime 0.0
			sw.put(2.3);
			WNS_ASSERT_MAX_REL_ERROR( 0.0, sw.getAbsolute(), 1E-9 );

			wns::simulator::getEventScheduler()->scheduleDelay(events::NoOp(), 0.25);
			wns::simulator::getEventScheduler()->processOneEvent();
			// simTime 0.25
			WNS_ASSERT_MAX_REL_ERROR( 2.3, sw.getAbsolute(), 1E-9 );
		}

		void
		testGetPerSecond()
		{
			SlidingWindow sw(0.25);
			// simTime 0.0
			sw.put(2.3);

			wns::simulator::getEventScheduler()->scheduleDelay(events::NoOp(), 0.1);
			wns::simulator::getEventScheduler()->processOneEvent();
			// simTime 0.1
			WNS_ASSERT_MAX_REL_ERROR( 2.3/0.25, sw.getPerSecond(), 1E-9 );
		}
		
		void testGetNumSamplesIncludingNow()
		{
			SlidingWindow sw(0.25, true);
			WNS_ASSERT_MAX_REL_ERROR( 0, sw.getNumSamples(), 0 );
			// simTime 0.0, insert 1@0.0
			sw.put(1.0);
			WNS_ASSERT_MAX_REL_ERROR( 1, sw.getNumSamples(), 0 );

			wns::simulator::getEventScheduler()->scheduleDelay(events::NoOp(), 0.1);
			wns::simulator::getEventScheduler()->processOneEvent();
			// simTime 0.1, still 1 sample
			WNS_ASSERT_MAX_REL_ERROR( 1, sw.getNumSamples(), 0 );

			// insert 2@0.1
			sw.put(1.0);
			WNS_ASSERT_MAX_REL_ERROR( 2, sw.getNumSamples(), 0 );

			wns::simulator::getEventScheduler()->scheduleDelay(events::NoOp(), 0.1);
			wns::simulator::getEventScheduler()->processOneEvent();
			// simTime 0.2, 
			WNS_ASSERT_MAX_REL_ERROR( 2, sw.getNumSamples(), 0 );

			// insert 3@0.2
			sw.put(1.0);
			WNS_ASSERT_MAX_REL_ERROR( 3, sw.getNumSamples(), 0 );

			wns::simulator::getEventScheduler()->scheduleDelay(events::NoOp(), 0.05);
			wns::simulator::getEventScheduler()->processOneEvent();
			// simTime 0.25
			WNS_ASSERT_MAX_REL_ERROR( 3, sw.getNumSamples(), 0 );

			wns::simulator::getEventScheduler()->scheduleDelay(events::NoOp(), 0.05);
			wns::simulator::getEventScheduler()->processOneEvent();
			// simTime 0.3
			WNS_ASSERT_MAX_REL_ERROR( 2, sw.getNumSamples(), 0 );

			wns::simulator::getEventScheduler()->scheduleDelay(events::NoOp(), 0.1);
			wns::simulator::getEventScheduler()->processOneEvent();
			// simTime 0.4
			WNS_ASSERT_MAX_REL_ERROR( 1, sw.getNumSamples(), 0 );

			wns::simulator::getEventScheduler()->scheduleDelay(events::NoOp(), 0.1);
			wns::simulator::getEventScheduler()->processOneEvent();
			// simTime 0.5
			WNS_ASSERT_MAX_REL_ERROR( 0, sw.getNumSamples(), 0 );
		}
			
		void testNormalPutIncludingNow()
		{
			SlidingWindow sw(0.25, true);
			WNS_ASSERT_MAX_REL_ERROR( 0.0, sw.getAbsolute(), 1E-9 );
			
			// simTime 0.0
			sw.put(1.0);
			WNS_ASSERT_MAX_REL_ERROR( 1.0, sw.getAbsolute(), 1E-9 );

			wns::simulator::getEventScheduler()->scheduleDelay(events::NoOp(), 0.1);
			wns::simulator::getEventScheduler()->processOneEvent();
			// simTime 0.1
			WNS_ASSERT_MAX_REL_ERROR( 1.0, sw.getAbsolute(), 1E-9 );

			sw.put(2.3);
			WNS_ASSERT_MAX_REL_ERROR( 3.3, sw.getAbsolute(), 1E-9 );

			wns::simulator::getEventScheduler()->scheduleDelay(events::NoOp(), 0.1);
			wns::simulator::getEventScheduler()->processOneEvent();
			// simTime 0.2
			WNS_ASSERT_MAX_REL_ERROR( 3.3, sw.getAbsolute(), 1E-9 );

			sw.put(4.2);
			WNS_ASSERT_MAX_REL_ERROR( 7.5, sw.getAbsolute(), 1E-9 );

			wns::simulator::getEventScheduler()->scheduleDelay(events::NoOp(), 0.05);
			wns::simulator::getEventScheduler()->processOneEvent();
			// simTime 0.25
			WNS_ASSERT_MAX_REL_ERROR( 7.5, sw.getAbsolute(), 1E-9 );

			wns::simulator::getEventScheduler()->scheduleDelay(events::NoOp(), 0.05);
			wns::simulator::getEventScheduler()->processOneEvent();
			// simTime 0.3
			WNS_ASSERT_MAX_REL_ERROR( 6.5, sw.getAbsolute(), 1E-9 );

			wns::simulator::getEventScheduler()->scheduleDelay(events::NoOp(), 0.1);
			wns::simulator::getEventScheduler()->processOneEvent();
			// simTime 0.4
			WNS_ASSERT_MAX_REL_ERROR( 4.2, sw.getAbsolute(), 1E-9 );

			wns::simulator::getEventScheduler()->scheduleDelay(events::NoOp(), 0.1);
			wns::simulator::getEventScheduler()->processOneEvent();
			// simTime 0.5
			WNS_ASSERT_MAX_REL_ERROR( 0.0, sw.getAbsolute(), 1E-9 );
		}

	};

	CPPUNIT_TEST_SUITE_REGISTRATION( SlidingWindowTest );

} // tests
} // wns


