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


#include <IP/container/NeighbourCache.hpp>
#include <IP/tests/DLLStub.hpp>

#include <WNS/node/component/tests/TCP.hpp>
#include <WNS/service/dll/Address.hpp>
#include <WNS/pyconfig/helper/Functions.hpp>
#include <WNS/pyconfig/View.hpp>

#include <cppunit/extensions/HelperMacros.h>


namespace ip { namespace container { namespace tests {
	class NeighbourCacheTest
		: public CppUnit::TestFixture
	{
		CPPUNIT_TEST_SUITE( NeighbourCacheTest );
		CPPUNIT_TEST( defaultConstructor );
		CPPUNIT_TEST( assignment );
		CPPUNIT_TEST( contains );
		CPPUNIT_TEST( remove );
		CPPUNIT_TEST( setAndGet );
		CPPUNIT_TEST_SUITE_END();
	public:
		void setUp();
		void tearDown();
		void defaultConstructor();
		void assignment();
		void contains();
		void remove();
		void setAndGet();
	};
} // tests
} // container
} // ip


using namespace ip::container::tests;

CPPUNIT_TEST_SUITE_REGISTRATION( NeighbourCacheTest );

void NeighbourCacheTest::setUp()
{
}

void NeighbourCacheTest::tearDown()
{
}


void NeighbourCacheTest::defaultConstructor()
{
	NeighbourCacheEntry nc;

	CPPUNIT_ASSERT(nc.getSourceDLL() == NULL);
}

void NeighbourCacheTest::assignment()
{
	ip::tests::DLLStub* dll = new ip::tests::DLLStub();

	NeighbourCacheEntry nce1;
	NeighbourCacheEntry nce2;

	nce2.setSourceDLL(dll);
	nce2.setDestinationAddress(wns::service::dll::UnicastAddress(127));

	nce1 = nce2;

	CPPUNIT_ASSERT(nce1.getSourceDLL() == dll);
	CPPUNIT_ASSERT(nce1.getDestinationAddress() == nce2.getDestinationAddress());

	delete dll;
}

void NeighbourCacheTest::contains()
{
	NeighbourCacheEntry ne;
	wns::service::nl::Address a = wns::service::nl::Address("127.0.0.1");

	NeighbourCache *nc = new NeighbourCache("NeighbourCacheTest_contains");
	CPPUNIT_ASSERT(!nc->knows(a));
	nc->insert(a, ne);
	CPPUNIT_ASSERT(nc->knows(a));

	delete nc;
}

void NeighbourCacheTest::remove()
{
	NeighbourCacheEntry ne;
	wns::service::nl::Address a = wns::service::nl::Address("127.0.0.1");

	NeighbourCache *nc = new NeighbourCache("NeighbourCacheTest_remove");
	nc->insert(a, ne);
	CPPUNIT_ASSERT(nc->knows(a));
	nc->erase(a);
	CPPUNIT_ASSERT(!nc->knows(a));

	delete nc;
}

void NeighbourCacheTest::setAndGet()
{
	ip::tests::DLLStub* dll = new ip::tests::DLLStub();

	NeighbourCacheEntry ne;

	ne.setSourceDLL(dll);
	CPPUNIT_ASSERT(ne.getSourceDLL() == dll);

	wns::service::dll::UnicastAddress unicastAddress(127);
	ne.setDestinationAddress(unicastAddress);
	CPPUNIT_ASSERT(ne.getDestinationAddress() == unicastAddress);

	delete dll;
}


