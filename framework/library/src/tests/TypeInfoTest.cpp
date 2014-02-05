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

#include <WNS/TypeInfo.hpp>
#include <WNS/TestFixture.hpp>

#include <memory>

namespace wns { namespace tests {

	class TypeInfoTest :
		public wns::TestFixture
	{
		// For testing purpose
		class A
		{
		public:
			virtual
			~A()
			{}
		};

		class B
		{
		public:
			virtual
			~B()
			{}
		};

		class C :
			public A
		{
		public:
			virtual
			~C()
			{}
		};

		CPPUNIT_TEST_SUITE( TypeInfoTest );
		CPPUNIT_TEST( equality );
		CPPUNIT_TEST( assignment );
		CPPUNIT_TEST( ordering );
		CPPUNIT_TEST( demangled );
		CPPUNIT_TEST( stream );
		CPPUNIT_TEST_SUITE_END();
	public:
		void prepare();
		void cleanup();
		void equality();
		void assignment();
		void ordering();
		void demangled();
		void stream();

	private:
		A a;
		A a2;
		B b;
		C c;
	};

	CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( TypeInfoTest, wns::testsuite::Default() );

}
}

using namespace wns::tests;

void
TypeInfoTest::prepare()
{
	A a = A();
	A a2 = A();
	B b = B();
	C c = C();
}

void
TypeInfoTest::cleanup()
{
}

void
TypeInfoTest::equality()
{
	CPPUNIT_ASSERT( TypeInfo::create<A>() == TypeInfo::create<A>() );
	CPPUNIT_ASSERT( TypeInfo::create(a) == TypeInfo::create(a2) );
	CPPUNIT_ASSERT( TypeInfo::create(a) != TypeInfo::create(b) );
	CPPUNIT_ASSERT( !(TypeInfo::create(a) == TypeInfo::create(b)) );
	CPPUNIT_ASSERT( TypeInfo::create(a) != TypeInfo::create(c) );
}

void
TypeInfoTest::assignment()
{
	TypeInfo T_A1 = TypeInfo::create<A>();
	TypeInfo T_A2 = TypeInfo::create(a);
	CPPUNIT_ASSERT( T_A1 == T_A2 );

	T_A2 = TypeInfo::create(b);
	CPPUNIT_ASSERT( T_A1 != T_A2 );

	T_A2 = TypeInfo(T_A1);
	CPPUNIT_ASSERT( T_A1 == T_A2 );
}

void
TypeInfoTest::ordering()
{
	TypeInfo T_A1 = TypeInfo::create<A>();
	TypeInfo T_A2 = TypeInfo::create<A>();
	TypeInfo T_B = TypeInfo::create<B>();

	// This fails because the ordering is unpredictable
	// @todo is there a way to test this?
	// CPPUNIT_ASSERT( T_A1 <= T_A2 );
	// CPPUNIT_ASSERT( T_A1 >= T_A2 );
	// CPPUNIT_ASSERT( T_A1 >= T_B );
	// CPPUNIT_ASSERT( T_B <= T_A2 );
	// CPPUNIT_ASSERT( T_A1 > T_B );
	// CPPUNIT_ASSERT( T_B < T_A2 );
}

void
TypeInfoTest::demangled()
{
	CPPUNIT_ASSERT_MESSAGE( TypeInfo::create<A>().toString(), TypeInfo::create<A>().toString() == std::string("wns::tests::TypeInfoTest::A") );

	std::auto_ptr<A> c(new C);
	CPPUNIT_ASSERT_MESSAGE( TypeInfo::create(*c).toString(), TypeInfo::create(*c).toString() == std::string("wns::tests::TypeInfoTest::C") );
}

void
TypeInfoTest::stream()
{
	std::stringstream ss;
	ss << TypeInfo::create<A>();
	CPPUNIT_ASSERT_MESSAGE( ss.str(), ss.str() == std::string("wns::tests::TypeInfoTest::A") );

	std::auto_ptr<A> c(new C);
	std::stringstream ss2;
	ss2 << TypeInfo::create(*c);
	CPPUNIT_ASSERT_MESSAGE( ss2.str(), ss2.str() == std::string("wns::tests::TypeInfoTest::C") );
}


