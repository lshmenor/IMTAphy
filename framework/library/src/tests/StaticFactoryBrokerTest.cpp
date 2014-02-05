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

#include <WNS/StaticFactoryBroker.hpp>
#include <WNS/PyConfigViewCreator.hpp>
#include <WNS/pyconfig/Parser.hpp>
#include <WNS/isClass.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace wns { namespace tests {

    class StaticFactoryBrokerTest :
	public CppUnit::TestFixture
    {
	CPPUNIT_TEST_SUITE( StaticFactoryBrokerTest );
	CPPUNIT_TEST( procureByPyco );
	CPPUNIT_TEST( procureByName );
	CPPUNIT_TEST_SUITE_END();
    public:

	void setUp() {};
	void tearDown() {};
	void procureByPyco();
	void procureByName();

	struct Base
	{
	    int foo;
	    virtual ~Base() {};
	};

	struct A :
	    public Base
	{
	    A(const pyconfig::View& pyco)
	    {
		foo = pyco.get<int>("foo");
	    }
	};

	struct B :
	    public Base
	{
	    B(const pyconfig::View& pyco)
	    {
		foo = pyco.get<int>("foo");
	    }
	};

	struct C :
	    public Base
	{
	    C()
	    {
		foo = 3;
	    }
	};

	struct D :
	    public Base
	{
	    D()
	    {
		foo = 4;
	    }
	};

    };

    CPPUNIT_TEST_SUITE_REGISTRATION( StaticFactoryBrokerTest );
    STATIC_FACTORY_BROKER_REGISTER( StaticFactoryBrokerTest::A, StaticFactoryBrokerTest::Base, "A" );
    STATIC_FACTORY_BROKER_REGISTER( StaticFactoryBrokerTest::B, StaticFactoryBrokerTest::Base, "B" );
    STATIC_FACTORY_BROKER_REGISTER_PLAIN( StaticFactoryBrokerTest::C, StaticFactoryBrokerTest::Base, "C" );
    STATIC_FACTORY_BROKER_REGISTER_PLAIN( StaticFactoryBrokerTest::D, StaticFactoryBrokerTest::Base, "D" );

    void
    StaticFactoryBrokerTest::procureByPyco()
    {
	pyconfig::Parser pyco = pyconfig::Parser();
	pyco.loadString("class A:\n"
			"  name = \"A\"\n"
			"  foo = None\n"
			"class B:\n"
			"  name = \"B\"\n"
			"  foo = None\n"
			"a = A()\n"
			"aa = a\n"
			"aa.foo = 1\n"
			"b = B()\n"
			"bb = b\n"
			"b.foo = 2\n");
	StaticFactoryBroker<StaticFactoryBrokerTest::Base, wns::PyConfigViewCreator<StaticFactoryBrokerTest::Base> > broker;
	Base* a  = broker.procure(pyco.get("a").get<std::string>("name"),  pyco.get("a"));
	Base* aa = broker.procure(pyco.get("aa").get<std::string>("name"), pyco.get("aa"));
	Base* b  = broker.procure(pyco.get("b").get<std::string>("name"),  pyco.get("b"));
	Base* bb = broker.procure(pyco.get("bb").get<std::string>("name"), pyco.get("bb"));

	CPPUNIT_ASSERT( isClass<StaticFactoryBrokerTest::A>(*a) );
	CPPUNIT_ASSERT( isClass<StaticFactoryBrokerTest::A>(*aa) );
	CPPUNIT_ASSERT( !isClass<StaticFactoryBrokerTest::B>(*a) );
	CPPUNIT_ASSERT( !isClass<StaticFactoryBrokerTest::B>(*aa) );
	CPPUNIT_ASSERT( isClass<StaticFactoryBrokerTest::B>(*b) );
	CPPUNIT_ASSERT( isClass<StaticFactoryBrokerTest::B>(*bb) );
	CPPUNIT_ASSERT( !isClass<StaticFactoryBrokerTest::A>(*b) );
	CPPUNIT_ASSERT( !isClass<StaticFactoryBrokerTest::A>(*bb) );

	CPPUNIT_ASSERT_EQUAL(1, a->foo);
	CPPUNIT_ASSERT_EQUAL(1, aa->foo);
	CPPUNIT_ASSERT_EQUAL(2, b->foo);
	CPPUNIT_ASSERT_EQUAL(2, bb->foo);
	bb->foo = 3;
	CPPUNIT_ASSERT_EQUAL(3, b->foo);
    }

    void
    StaticFactoryBrokerTest::procureByName()
    {
	pyconfig::Parser pyco = pyconfig::Parser();
	pyco.loadString("class C:\n"
					"  name = \"C\"\n"
					"class D:\n"
					"  name = \"D\"\n"
					"c = C()\n"
					"cc = C()\n"
					"d = D()\n"
					"dd = D()\n");
	// Note above that even though we create different Python Objects (e.g. for
	// 'c' and 'cc', we still expect the Broker to procure the same Object

	StaticFactoryBroker<StaticFactoryBrokerTest::Base, wns::Creator<StaticFactoryBrokerTest::Base> > broker;
	Base* c  = broker.procure(pyco.get("c").get<std::string>("name"));
	Base* cc = broker.procure(pyco.get("cc").get<std::string>("name"));
	Base* d  = broker.procure(pyco.get("d").get<std::string>("name"));
	Base* dd = broker.procure(pyco.get("dd").get<std::string>("name"));

	CPPUNIT_ASSERT( isClass<StaticFactoryBrokerTest::C>(*c) );
	CPPUNIT_ASSERT( isClass<StaticFactoryBrokerTest::C>(*cc) );
	CPPUNIT_ASSERT( !isClass<StaticFactoryBrokerTest::D>(*c) );
	CPPUNIT_ASSERT( !isClass<StaticFactoryBrokerTest::D>(*cc) );
	CPPUNIT_ASSERT( isClass<StaticFactoryBrokerTest::D>(*d) );
	CPPUNIT_ASSERT( isClass<StaticFactoryBrokerTest::D>(*dd) );
	CPPUNIT_ASSERT( !isClass<StaticFactoryBrokerTest::C>(*d) );
	CPPUNIT_ASSERT( !isClass<StaticFactoryBrokerTest::C>(*dd) );

	CPPUNIT_ASSERT_EQUAL(3, c->foo);
	CPPUNIT_ASSERT_EQUAL(3, cc->foo);
	CPPUNIT_ASSERT_EQUAL(4, d->foo);
	CPPUNIT_ASSERT_EQUAL(4, dd->foo);
	cc->foo = 5;
	CPPUNIT_ASSERT_EQUAL(5, c->foo);
    }

} // tests
} // wns

