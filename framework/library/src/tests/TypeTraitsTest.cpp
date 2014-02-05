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

#include <WNS/TypeTraits.hpp>
#include <WNS/TestFixture.hpp>

namespace wns { namespace tests {

	class TypeTraitsTest :
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

		CPPUNIT_TEST_SUITE( TypeTraitsTest );
		CPPUNIT_TEST( traits );
		CPPUNIT_TEST_SUITE_END();
	public:
		void prepare();
		void cleanup();
		void traits();

	private:
		A a;
		A a2;
		B b;
		C c;
	};

	CPPUNIT_TEST_SUITE_REGISTRATION( TypeTraitsTest );

}
}

using namespace wns::tests;

void
TypeTraitsTest::prepare()
{
	A a = A();
	A a2 = A();
	B b = B();
	C c = C();
}

void
TypeTraitsTest::cleanup()
{
}


void
TypeTraitsTest::traits()
{
	CPPUNIT_ASSERT(!TypeTraits<A>::isPointer);
	CPPUNIT_ASSERT(!TypeTraits<A>::isReference);
	CPPUNIT_ASSERT(!TypeTraits<A>::isConst);
	CPPUNIT_ASSERT(TypeTraits<A>::propNumber == 0);

	CPPUNIT_ASSERT(!TypeTraits<const A>::isPointer);
	CPPUNIT_ASSERT(!TypeTraits<const A>::isReference);
	CPPUNIT_ASSERT(TypeTraits<const A>::isConst);
	CPPUNIT_ASSERT(TypeTraits<const A>::propNumber == 1);

	CPPUNIT_ASSERT(!TypeTraits<A&>::isPointer);
	CPPUNIT_ASSERT(TypeTraits<A&>::isReference);
	CPPUNIT_ASSERT(!TypeTraits<A&>::isConst);
	CPPUNIT_ASSERT(TypeTraits<A&>::propNumber == 2);

	CPPUNIT_ASSERT(!TypeTraits<const A&>::isPointer);
	CPPUNIT_ASSERT(TypeTraits<const A&>::isReference);
	CPPUNIT_ASSERT(TypeTraits<const A&>::isConst);
	CPPUNIT_ASSERT(TypeTraits<const A&>::propNumber == 3);

	CPPUNIT_ASSERT(TypeTraits<A*>::isPointer);
	CPPUNIT_ASSERT(!TypeTraits<A*>::isReference);
	CPPUNIT_ASSERT(!TypeTraits<A*>::isConst);
	CPPUNIT_ASSERT(TypeTraits<A*>::propNumber == 4);

	CPPUNIT_ASSERT(TypeTraits<const A*>::isPointer);
	CPPUNIT_ASSERT(!TypeTraits<const A*>::isReference);
	CPPUNIT_ASSERT(TypeTraits<const A*>::isConst);
	CPPUNIT_ASSERT(TypeTraits<const A*>::propNumber == 5);
}


