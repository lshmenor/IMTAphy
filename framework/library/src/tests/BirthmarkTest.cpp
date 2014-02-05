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

#include <WNS/Birthmark.hpp>
#include <WNS/CppUnit.hpp>

namespace wns { namespace tests {

	class BirthmarkTest :
		public wns::TestFixture
	{
		class A :
			virtual public wns::HasBirthmark
		{
		};

		class B :
			public A
		{
		};

		class C :
			virtual public wns::HasBirthmark
		{
		};


		class D :
			public A,
			public C
		{
		public:
			void
			setBM(const Birthmark& other)
			{
				this->setBirthmark(other);
			}
		};


		CPPUNIT_TEST_SUITE( BirthmarkTest );
		CPPUNIT_TEST( testConstructor );
		CPPUNIT_TEST( testCopyConstructor );
		CPPUNIT_TEST( testSetter );
		CPPUNIT_TEST( testAssignment );
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
			A a;
			B b;
			CPPUNIT_ASSERT( a.getBirthmark() != b.getBirthmark() );
		}

		void
		testCopyConstructor()
		{
			A a;
			B b;
			D d;
			HasBirthmark birthmark;

			A a2(a);
			B b2(b);
			D d2(d);
			HasBirthmark birthmark2(birthmark);

			CPPUNIT_ASSERT_EQUAL( a.getBirthmark(), a2.getBirthmark());
			CPPUNIT_ASSERT_EQUAL( b.getBirthmark(), b2.getBirthmark());
			CPPUNIT_ASSERT_EQUAL( d.getBirthmark(), d2.getBirthmark());
			CPPUNIT_ASSERT_EQUAL( birthmark.getBirthmark(), birthmark2.getBirthmark());
		}

		void
		testAssignment()
		{
			A a;
			A a2 = a;
			B b;
			B b2 = b;
			D d;
			D d2 = d;
			HasBirthmark birthmark;
			HasBirthmark birthmark2 = birthmark;

			CPPUNIT_ASSERT_EQUAL( a.getBirthmark(), a2.getBirthmark());
			CPPUNIT_ASSERT_EQUAL( b.getBirthmark(), b2.getBirthmark());
			CPPUNIT_ASSERT_EQUAL( d.getBirthmark(), d2.getBirthmark());
			CPPUNIT_ASSERT_EQUAL( birthmark.getBirthmark(), birthmark2.getBirthmark());
		}

		void
		testSetter()
		{
			D d;
			B b;

			d.setBM(b.getBirthmark());

			CPPUNIT_ASSERT( d.getBirthmark() == b.getBirthmark() );
		}
	};

	CPPUNIT_TEST_SUITE_REGISTRATION( BirthmarkTest );

} // tests
} // wns



