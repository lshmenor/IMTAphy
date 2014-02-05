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

#include <WNS/TestFixture.hpp>

#include <WNS/probe/bus/ContextProviderCollection.hpp>

namespace wns { namespace probe { namespace bus { namespace tests {


	class CopyCounter :
		public contextprovider::Constant
	{
		friend class ContextProviderCollectionTest;
		bool* copied;
		int* copycounter;
	public:
		CopyCounter(bool *_copied, int *_copycounter) :
			contextprovider::Constant("test",42),
			copied(_copied),
			copycounter(_copycounter)
		{}

		CopyCounter(const CopyCounter& other) :
			contextprovider::Constant(other),
			copied(other.copied),
			copycounter(other.copycounter)
		{
			*copied = true;
			++(*copycounter);
		}
	};

    class ContextProviderCollectionTest :
        public wns::TestFixture
    {
        CPPUNIT_TEST_SUITE( ContextProviderCollectionTest );
        CPPUNIT_TEST( constructor );
        CPPUNIT_TEST( addProvider );
        CPPUNIT_TEST( copyConstructor );
        CPPUNIT_TEST( hierarchy );
        CPPUNIT_TEST_SUITE_END();
    public:
		void prepare();
		void cleanup();

        void constructor();
		void addProvider();
		void addProviderPointer();
		void hierarchy();
		void copyConstructor();
    };

    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ContextProviderCollectionTest, wns::testsuite::Default() );

}
}
}
}

using namespace wns::probe::bus;
using namespace wns::probe::bus::tests;

void
ContextProviderCollectionTest::prepare()
{
}

void
ContextProviderCollectionTest::cleanup()
{
}

void
ContextProviderCollectionTest::constructor()
{
	ContextProviderCollection c;
}

void
ContextProviderCollectionTest::addProvider()
{
	ContextProviderCollection c;
	bool copied = false;
	int counter = 0;
	CopyCounter provider(&copied, &counter);

	c.addProvider(provider);
	CPPUNIT_ASSERT( copied == true );
	CPPUNIT_ASSERT_EQUAL( 3, counter );
}

void
ContextProviderCollectionTest::copyConstructor()
{
	ContextProviderCollection c;
	bool copied = false;
	int counter = 0;
	CopyCounter provider(&copied, &counter);

	c.addProvider(provider);
	CPPUNIT_ASSERT( copied == true );
	CPPUNIT_ASSERT_EQUAL( 3, counter );

	// make a copy of the list
	ContextProviderCollection c2(c);
	// make sure the provider is also copied
	CPPUNIT_ASSERT_EQUAL( 4, counter );
}

void
ContextProviderCollectionTest::hierarchy()
{
	ContextProviderCollection parent;
	ContextProviderCollection child(&parent);

	Context c;

	CPPUNIT_ASSERT_NO_THROW( parent.fillContext(c, wns::osi::PDUPtr()) );
	CPPUNIT_ASSERT_NO_THROW( child.fillContext(c, wns::osi::PDUPtr()) );

	bool copied = false;
	int counter = 0;
	CopyCounter provider(&copied, &counter);
	parent.addProvider(provider);

	CPPUNIT_ASSERT_NO_THROW( child.fillContext(c) );
	CPPUNIT_ASSERT( c.knows("test") );
}
