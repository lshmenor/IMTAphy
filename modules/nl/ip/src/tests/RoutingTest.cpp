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

#include <IP/Routing.hpp>
#include <IP/tests/AddressingProviderStub.hpp>
#include <IP/tests/DLLStub.hpp>

#include <WNS/node/Node.hpp>
#include <WNS/ldk/helper/FakePDU.hpp>
#include <WNS/pyconfig/helper/Functions.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/ldk/tools/Stub.hpp>
#include <WNS/CppUnit.hpp>
#include <WNS/Assure.hpp>
#include <cppunit/extensions/HelperMacros.h>

namespace ip { namespace tests {
	class RoutingTest
		: public CppUnit::TestFixture
	{
		CPPUNIT_TEST_SUITE( RoutingTest );
		CPPUNIT_TEST( construction );
		CPPUNIT_TEST( viaNeighbourCacheDLL1 );
		CPPUNIT_TEST( viaNeighbourCacheDLL2 );
		CPPUNIT_TEST( viaRoutingTableDLL1 );
		CPPUNIT_TEST( viaRoutingTableDLL2 );
		CPPUNIT_TEST( assertionNoRouteToHost );
/*		CPPUNIT_TEST( TTLInitialization );
		CPPUNIT_TEST( passOnNextHop );
		CPPUNIT_TEST( dropNoForward );
		CPPUNIT_TEST( dropTTLExceeded );
		CPPUNIT_TEST( deliver );
		CPPUNIT_TEST( deliverTTLExceeded );
		CPPUNIT_TEST( noAPCMutation );*/
		CPPUNIT_TEST_SUITE_END();
	public:
		void setUp();
		void tearDown();
		void setupRouting();
		void setupAddressingProvider(std::string sourceAddress, std::string destinationAddress);
		void construction();
		void viaNeighbourCacheDLL1();
		void viaNeighbourCacheDLL2();
		void viaRoutingTableDLL1();
		void viaRoutingTableDLL2();
		void assertionNoRouteToHost();
/*		void TTLInitialization();
		void passOnNextHop();
		void dropNoForward();
		void dropTTLExceeded();
		void deliver();
		void deliverTTLExceeded();
		void noAPCMutation();*/
	private:
		wns::ldk::ILayer* layer;

		wns::ldk::fun::Main* fuNet;

		AddressingProviderStub* aps;

		Routing* routing;

		DLLStub* myDLL1;

		DLLStub* myDLL2;

		wns::ldk::tools::Stub* lower;
	};

} // tests
} // ip



using namespace ip::tests;

CPPUNIT_TEST_SUITE_REGISTRATION( RoutingTest );

void RoutingTest::setUp()
{
	wns::pyconfig::Parser emptyConfig;
	layer = new wns::ldk::tests::LayerStub();
	fuNet = new fun::Main(layer);

	lower = new wns::ldk::tools::Stub(fuNet, emptyConfig);
	// Please note: In an indiviudal test you still need to call
	// setupAddressing and setupForwarding
	// This is for your convience to easily set internal parameters
	// of these FUs for testing.

	// The setup for this test looks like this:
	// +----------------------------+
	// | AddressingProviderStub     |
	// +----------------------------+
	//                |
	// +----------------------------+
	// | Routing    (FU under test) |
	// +----------------------------+
	//                |
	// +----------------------------+
	// | wns::ldk::tools::Stub      |
	// +----------------------------+
}

void  RoutingTest::tearDown()
{
	delete fuNet;
	delete layer;
}

void RoutingTest::setupAddressingProvider(std::string sourceAddress, std::string destinationAddress)
{
	// This emulates the upperConvergence to TL
	// Give source and destination address which will be stamped into
	// the AddressingProviderInterface by the AdressingProviderStub
	// Forwarding will access the command via the AddressingProviderInterface

	wns::pyconfig::Parser emptyConfig;
	aps = new AddressingProviderStub(fuNet, emptyConfig);
	aps->setSourceAddress(wns::service::nl::Address(sourceAddress));
	aps->setDestinationAddress(wns::service::nl::Address(destinationAddress));
}


void RoutingTest::setupRouting()
{

	// Our routing FU knows two interfaces (eth0 and ath0)
	// It has IP "192.168.2.10" on the eth0 subnet and "192.168.7.10" on
	// ath0

	// We have two routing table entries
	// The first delivers all packets for the 192.168.0.0 nets and uses the
	// router "192.168.1.254" on ath0
	// The second route is the default route and forwards all packets to
	// "192.168.2.254" on eth0
	std::stringstream ss;
	ss << "from ip.Component import Routing\n"
	   << "routing = Routing()\n"
	   << "routing.addressingProvider = \"aps\"\n"
	   << "routing.addRoute(\"192.168.0.0\", \"255.255.255.0\", \"192.168.1.254\", \"ath0\")\n"
	   << "routing.addRoute(\"0.0.0.0\", \"0.0.0.0\", \"192.168.2.254\", \"eth0\")\n";
	// Now we tell the router about the source addresses it should put in
	// the IP header when a packet leaves a specfic interface. This is done
	// by setup of an entry in the output chain
	ss << "routing
	wns::pyconfig::Parser all;
	all.loadString(ss.str());

	wns::pyconfig::View config(all, "routing");

	routing = new Routing(fuNet, config);

	myDLL1 = new DLLStub();
	myDLL2 = new DLLStub();
	container::NeighbourCache* nc = new container::NeighbourCache("myNeighbourCache");
	container::NeighbourCacheEntry nce1(myDLL1, wns::service::dll::UnicastAddress(4));
	nc->insert(wns::service::nl::Address("192.168.1.2"), nce1);
	container::NeighbourCacheEntry nce2(myDLL2, wns::service::dll::UnicastAddress(5));
	nc->insert(wns::service::nl::Address("192.168.1.3"), nce2);
	routing->setNeighbourCache(nc);

	aps->connect(routing)->connect(lower);
	fuNet->addFunctionalUnit("aps", aps);
	fuNet->addFunctionalUnit("myRouting", routing);
	fuNet->addFunctionalUnit("lower", lower);
	fuNet->onFUNCreated();

	wns::simulator::getEventScheduler()->reset();
}

void RoutingTest::construction()
{
	// Try simple construction
	setupAddressingProvider("127.0.0.1", "127.0.0.2");
	setupRouting();
}

void RoutingTest::viaNeighbourCacheDLL1()
{
	setupAddressingProvider("192.168.1.1", "192.168.1.2");
	setupRouting();

	aps->sendData(fuNet->createCompound());

	CPPUNIT_ASSERT_EQUAL( size_t(1), lower->sent.size() );

	RoutingCommand* rc = routing->getCommand(lower->sent[0]->getCommandPool());

	CPPUNIT_ASSERT_EQUAL( rc->local.nextHop, wns::service::dll::UnicastAddress(4));

	CPPUNIT_ASSERT(rc->local.dll == myDLL1);
}

void RoutingTest::viaNeighbourCacheDLL2()
{
	setupAddressingProvider("192.168.1.1", "192.168.1.3");
	setupRouting();

	aps->sendData(fuNet->createCompound());

	CPPUNIT_ASSERT_EQUAL( size_t(1), lower->sent.size() );

	RoutingCommand* rc = routing->getCommand(lower->sent[0]->getCommandPool());

	CPPUNIT_ASSERT_EQUAL(  wns::service::dll::UnicastAddress(5), rc->local.nextHop);

	CPPUNIT_ASSERT(rc->local.dll == myDLL2);
}

void RoutingTest::viaRoutingTableDLL1()
{
	setupAddressingProvider("192.168.1.1", "192.168.2.2");
	setupRouting();

	aps->sendData(fuNet->createCompound());

	CPPUNIT_ASSERT_EQUAL( size_t(1), lower->sent.size() );

	RoutingCommand* rc = routing->getCommand(lower->sent[0]->getCommandPool());

	CPPUNIT_ASSERT_EQUAL( wns::service::dll::UnicastAddress(4), rc->local.nextHop);

	CPPUNIT_ASSERT(rc->local.dll == myDLL1);
}

void RoutingTest::viaRoutingTableDLL2()
{
	setupAddressingProvider("192.168.1.1", "192.168.5.2");
	setupRouting();

	aps->sendData(fuNet->createCompound());

	CPPUNIT_ASSERT_EQUAL( size_t(1), lower->sent.size() );

	RoutingCommand* rc = routing->getCommand(lower->sent[0]->getCommandPool());

	CPPUNIT_ASSERT_EQUAL( wns::service::dll::UnicastAddress(5), rc->local.nextHop);

	CPPUNIT_ASSERT(rc->local.dll == myDLL2);
}

void RoutingTest::assertionNoRouteToHost()
{
	setupAddressingProvider("192.168.1.1", "137.226.5.1");
	setupRouting();

	WNS_ASSERT_ASSURE_EXCEPTION(aps->sendData(fuNet->createCompound()));

	CPPUNIT_ASSERT_EQUAL( size_t(0), lower->sent.size() );
}


