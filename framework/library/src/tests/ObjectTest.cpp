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

#include <WNS/Object.hpp>
#include <WNS/TestFixture.hpp>

namespace wns { namespace tests {

	class ObjectTest :
		public wns::TestFixture
	{
		CPPUNIT_TEST_SUITE( ObjectTest );
		CPPUNIT_TEST( getClassNameTest );
		CPPUNIT_TEST( dynamicCastTest );
		CPPUNIT_TEST_SUITE_END();

		class A :
			public virtual wns::Object
		{

		};

		class B :
			public A
		{
		};

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
		getClassNameTest()
		{
			wns::Object* a = new A;
			wns::Object* b = new B;

			CPPUNIT_ASSERT_MESSAGE( a->getClassName(), "wns::tests::ObjectTest::A" == a->getClassName() );
			CPPUNIT_ASSERT_MESSAGE( b->getClassName(), "wns::tests::ObjectTest::B" == b->getClassName() );

			delete a;
			delete b;
		}

		void
		dynamicCastTest()
		{
			wns::Object* a = new A;

			CPPUNIT_ASSERT( dynamic_cast<A*>(a) != NULL );
			CPPUNIT_ASSERT( dynamic_cast<B*>(a) == NULL );

			delete a;
		}
	};

	CPPUNIT_TEST_SUITE_REGISTRATION( ObjectTest );

} // tests
} // wns

/*
  Local Variables:
  mode: c++
  fill-column: 80
  c-basic-offset: 8
  c-tab-always-indent: t
  indent-tabs-mode: t
  tab-width: 8
  End:
*/
