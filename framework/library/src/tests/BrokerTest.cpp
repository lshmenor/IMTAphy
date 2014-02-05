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

#include <WNS/Broker.hpp>
#include <WNS/Singleton.hpp>
#include <WNS/pyconfig/Parser.hpp>
#include <cppunit/extensions/HelperMacros.h>

namespace wns { namespace tests {

	class BrokerTest :
		public CppUnit::TestFixture
	{
		class A
		{
		public:
			A(const pyconfig::View& pyco) :
				foo(pyco.get<int>("foo"))
			{
			}
			int foo;
		};

		CPPUNIT_TEST_SUITE( BrokerTest );
		CPPUNIT_TEST( procure );
		CPPUNIT_TEST( procureSingleton );
		CPPUNIT_TEST_SUITE_END();
	public:
		void setUp();
		void tearDown();
		void procure();
		void procureSingleton();
		typedef wns::Broker<A> ABroker;
		typedef wns::SingletonHolder<ABroker> GlobalABroker;

		pyconfig::Parser* pyco;
	};

	CPPUNIT_TEST_SUITE_REGISTRATION( BrokerTest );

	void
	BrokerTest::setUp()
	{
		pyco = new pyconfig::Parser();
		pyco->loadString(
			"class A:\n"
			"  foo = None\n"
			"a = A()\n"
			"a.foo = 1\n"
			"aa = a\n"
			"b = A()\n"
			"b.foo = 2\n");
	}

	void
	BrokerTest::tearDown()
	{
		delete pyco;
	}

	void
	BrokerTest::procure()
	{
		ABroker testee;
		A* a = testee.procure(pyco->get("a"));
		A* aa = testee.procure(pyco->get("aa"));
		A* b = testee.procure(pyco->get("b"));

		CPPUNIT_ASSERT_EQUAL(1, a->foo);
		CPPUNIT_ASSERT_EQUAL(1, aa->foo);
		CPPUNIT_ASSERT_EQUAL(2, b->foo);

		CPPUNIT_ASSERT( a == aa );
		CPPUNIT_ASSERT( b != a );
	}

	void
	BrokerTest::procureSingleton()
	{
		ABroker& testee = GlobalABroker::Instance();
		A* a = testee.procure(pyco->get("a"));
		A* aa = testee.procure(pyco->get("aa"));
		A* b = testee.procure(pyco->get("b"));

		CPPUNIT_ASSERT_EQUAL(1, a->foo);
		CPPUNIT_ASSERT_EQUAL(1, aa->foo);
		CPPUNIT_ASSERT_EQUAL(2, b->foo);

		CPPUNIT_ASSERT( a == aa );
		CPPUNIT_ASSERT( b != a );
	}

} // tests
} // wns



