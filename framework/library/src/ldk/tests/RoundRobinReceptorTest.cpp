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

#include <WNS/CppUnit.hpp>
#include <WNS/ldk/RoundRobinReceptor.hpp>
#include <WNS/ldk/tools/FakeFU.hpp>

namespace wns { namespace ldk { namespace tests {
	class RoundRobinReceptorTest :
		public wns::TestFixture
	{
		CPPUNIT_TEST_SUITE( RoundRobinReceptorTest );
		CPPUNIT_TEST( testWakeup );
		CPPUNIT_TEST( testWakeupTwoRounds );
		CPPUNIT_TEST( testWakeupNewFU );
		CPPUNIT_TEST( testWakeupWithoutFUs );
		CPPUNIT_TEST_SUITE_END();

		class TestFU :
			public tools::FakeFU
		{
		public:
			TestFU() :
				tools::FakeFU(),
				wakeupCalled(0)
			{
			}

			virtual void
			wakeupForwarded()
			{
				wakeupCalled = ++staticWakeupCalled;
			}

			int wakeupCalled;
			static int staticWakeupCalled;

		};

		void
		prepare()
		{
			TestFU::staticWakeupCalled = 0;
		}

		void
		cleanup()
		{

		}

		void
		testWakeup()
		{
			RoundRobinReceptor rrr;
			TestFU fu;
			TestFU fu2;
			TestFU fu3;
			rrr.add(&fu);
			rrr.add(&fu2);
			rrr.add(&fu3);
			rrr.wakeup();
			CPPUNIT_ASSERT_EQUAL( 1, fu.wakeupCalled );
			CPPUNIT_ASSERT_EQUAL( 2, fu2.wakeupCalled );
			CPPUNIT_ASSERT_EQUAL( 3, fu3.wakeupCalled );
		}

		void
		testWakeupTwoRounds()
		{
 			RoundRobinReceptor rrr;
			TestFU fu;
			TestFU fu2;
			TestFU fu3;
			rrr.add(&fu);
			rrr.add(&fu2);
			rrr.add(&fu3);
			rrr.wakeup();
			rrr.wakeup();
			CPPUNIT_ASSERT_EQUAL( 4, fu.wakeupCalled );
			CPPUNIT_ASSERT_EQUAL( 5, fu2.wakeupCalled );
			CPPUNIT_ASSERT_EQUAL( 6, fu3.wakeupCalled );
		}


		void
		testWakeupNewFU()
		{
 			RoundRobinReceptor rrr;
			TestFU fu;
			TestFU fu2;
			TestFU fu3;
			TestFU fu4;
			rrr.add(&fu);
			rrr.add(&fu2);
			rrr.add(&fu3);
			rrr.wakeup();
			CPPUNIT_ASSERT_EQUAL( 1, fu.wakeupCalled );
			CPPUNIT_ASSERT_EQUAL( 2, fu2.wakeupCalled );
			CPPUNIT_ASSERT_EQUAL( 3, fu3.wakeupCalled );
			CPPUNIT_ASSERT_EQUAL( 0, fu4.wakeupCalled );
                        Link<IReceptorReceptacle>::ExchangeContainer exchange;
			exchange = rrr.get();
			exchange[1] = &fu4;
			rrr.set(exchange);
			rrr.wakeup();
			CPPUNIT_ASSERT_EQUAL( 4, fu.wakeupCalled );
			CPPUNIT_ASSERT_EQUAL( 2, fu2.wakeupCalled );
			CPPUNIT_ASSERT_EQUAL( 6, fu3.wakeupCalled );
			CPPUNIT_ASSERT_EQUAL( 5, fu4.wakeupCalled );
		}

		void
		testWakeupWithoutFUs()
		{
			// Just check that we don't get an exception
			RoundRobinReceptor rrr;
			rrr.wakeup();
		}
	};

	int RoundRobinReceptorTest::TestFU::staticWakeupCalled = 0;

	CPPUNIT_TEST_SUITE_REGISTRATION( RoundRobinReceptorTest );

} // tests
} // ldk
} // wns




