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
#include <WNS/ldk/SingleReceptor.hpp>
#include <WNS/ldk/tools/FakeFU.hpp>

namespace wns { namespace ldk { namespace tests {
	class SingleReceptorTest :
		public wns::TestFixture
	{
		CPPUNIT_TEST_SUITE( SingleReceptorTest );
		CPPUNIT_TEST( testWakeup );
		CPPUNIT_TEST( testWakeupWithoutFU );
		CPPUNIT_TEST_SUITE_END();

		class TestFU :
			public tools::FakeFU
		{
		public:
			TestFU() :
				FakeFU(),
				wakeupCalled(0)
			{
			}

			virtual void
			wakeupForwarded()
			{
				++wakeupCalled;
			}

			int wakeupCalled;

		};


		void
		prepare()
		{
		}

		void
		cleanup()
		{
		}

		void
		testWakeup()
		{
			SingleReceptor sr;
			TestFU fu;
			sr.add(&fu);
			sr.wakeup();
			CPPUNIT_ASSERT_EQUAL( 1, fu.wakeupCalled );
			sr.wakeup();
			CPPUNIT_ASSERT_EQUAL( 2, fu.wakeupCalled );
		}

		void
		testWakeupWithoutFU()
		{
			// we check only that we don't get an exception ...
			SingleReceptor sr;
			sr.wakeup();
		}
	};

	CPPUNIT_TEST_SUITE_REGISTRATION( SingleReceptorTest );

} // tests
} // ldk
} // wns




