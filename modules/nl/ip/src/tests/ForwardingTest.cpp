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

#include <IP/Forwarding.hpp>
#include <IP/tests/AddressingProviderStub.hpp>

#include <WNS/node/Node.hpp>
#include <WNS/ldk/helper/FakePDU.hpp>
#include <WNS/pyconfig/helper/Functions.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/ldk/tools/Stub.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace ip { namespace tests {
	class ForwardingTest
		: public CppUnit::TestFixture
	{
		CPPUNIT_TEST_SUITE( ForwardingTest );
		CPPUNIT_TEST( construction );
		CPPUNIT_TEST( TTLInitialization );
		CPPUNIT_TEST( passOnNextHop );
		CPPUNIT_TEST( dropNoForward );
		CPPUNIT_TEST( dropTTLExceeded );
		CPPUNIT_TEST( deliver );
		CPPUNIT_TEST( deliverTTLExceeded );
		CPPUNIT_TEST( noAPCMutation );
		CPPUNIT_TEST_SUITE_END();
	public:
		void setUp();
		void tearDown();
		void setupForwarding(bool isForwarding, std::string listeningAddress, int ttl);
		void setupAddressingProvider(std::string sourceAddress, std::string destinationAddress);
		void construction();
		void TTLInitialization();
		void passOnNextHop();
		void dropNoForward();
		void dropTTLExceeded();
		void deliver();
		void deliverTTLExceeded();
		void noAPCMutation();
	private:
		wns::ldk::ILayer* layer;

		wns::ldk::fun::Main* fuNet;

		AddressingProviderStub* aps;

		Forwarding* forwarding;

		wns::ldk::tools::Stub* lower;
	};

} // tests
} // ip



using namespace ip::tests;

CPPUNIT_TEST_SUITE_REGISTRATION( ForwardingTest );

void ForwardingTest::setUp()
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
	// | Forwarding (FU under test) |
	// +----------------------------+
	//                |
	// +----------------------------+
	// | wns::ldk::tools::Stub      |
	// +----------------------------+
}

void ForwardingTest::tearDown()
{
	delete fuNet;
	delete layer;
}

void ForwardingTest::setupAddressingProvider(std::string sourceAddress, std::string destinationAddress)
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


void ForwardingTest::setupForwarding(bool isForwarding, std::string listeningAddress, int ttl)
{
	// Configure the Forwarding block as needed
	// Parameters are
	//
	//   isForwarding : boolean
	//       do we forward packets on another hop, e.g. are we behaving like a router
	//   listeningAddress : string
	//       this is the IP address for which the Forwarding FU accepts data and passes
	//       it to the TL layer
	//   ttl : int
	//       Time To Live. IP datagrams that have a TTL of zero are not forwarded on
	//       another hop, even if we behave like a router.
	std::stringstream ss;
	ss << "from ip.Component import Forwarding\n"
	   << "import wns.Logger\n"
	   << "forwarding = Forwarding()\n"
	   << "forwarding.addressingProvider = \"aps\"\n"
	   << "forwarding.ttl = " << ttl << "\n"
	   << "forwarding.isForwarding = " << (isForwarding ? "True" : "False") << "\n";

	wns::pyconfig::Parser all;
	all.loadString(ss.str());

	wns::pyconfig::View config(all, "forwarding");

	forwarding = new Forwarding(fuNet, config);
	forwarding->setListeningAddress(wns::service::nl::Address(listeningAddress));
	aps->connect(forwarding)->connect(lower);
	fuNet->addFunctionalUnit("aps", aps);
	fuNet->addFunctionalUnit("myForwarding", forwarding);
	fuNet->addFunctionalUnit("lower", lower);
	fuNet->onFUNCreated();

	wns::simulator::getEventScheduler()->reset();
}

void ForwardingTest::construction()
{
	// Try simple construction
	setupAddressingProvider("127.0.0.1", "127.0.0.2");
	setupForwarding(false, "127.0.0.1", 255);
}

void ForwardingTest::TTLInitialization()
{
	// Is the initial ttl value put correctly to the ForwardingCommand?
	setupAddressingProvider("127.0.0.1", "127.0.0.2");
	setupForwarding(false, "127.0.0.1", 255);

	aps->sendData(fuNet->createCompound());

	CPPUNIT_ASSERT_EQUAL( size_t(1), lower->sent.size() );

	ForwardingCommand* fc = forwarding->getCommand(lower->sent[0]->getCommandPool());

	CPPUNIT_ASSERT(fc->peer.TTL == 255);
}

void ForwardingTest::passOnNextHop()
{
	// Check if packet is forwarded to a next hop and if TTL is decreased
	setupAddressingProvider("127.0.0.1", "127.0.0.2");
	setupForwarding(true, "127.0.0.1", 128);

	aps->sendData(fuNet->createCompound());

	CPPUNIT_ASSERT_EQUAL( size_t(1), lower->sent.size() );

	ForwardingCommand* fc = forwarding->getCommand(lower->sent[0]->getCommandPool());

	CPPUNIT_ASSERT(fc->peer.TTL == 128);

	lower->onData(lower->sent[0]);

	CPPUNIT_ASSERT_EQUAL( size_t(2), lower->sent.size() );

	fc = forwarding->getCommand(lower->sent[1]->getCommandPool());

	CPPUNIT_ASSERT(fc->peer.TTL == 127);

}

void ForwardingTest::dropNoForward()
{
	// Check if packet is dropped if we do not behave like a router
	setupAddressingProvider("127.0.0.1", "127.0.0.2");
	setupForwarding(false, "127.0.0.1", 128);

	aps->sendData(fuNet->createCompound());

	CPPUNIT_ASSERT_EQUAL( size_t(1), lower->sent.size() );

	ForwardingCommand* fc = forwarding->getCommand(lower->sent[0]->getCommandPool());

	CPPUNIT_ASSERT(fc->peer.TTL == 128);

	lower->onData(lower->sent[0]);

	CPPUNIT_ASSERT_EQUAL( size_t(1), lower->sent.size() );
}

void ForwardingTest::dropTTLExceeded()
{
	// Even if we behave routerlike we must drop the packet if TTL is exceeded
	setupAddressingProvider("127.0.0.1", "127.0.0.2");
	setupForwarding(true, "127.0.0.1", 0);

	aps->sendData(fuNet->createCompound());

	CPPUNIT_ASSERT_EQUAL( size_t(1), lower->sent.size() );

	ForwardingCommand* fc = forwarding->getCommand(lower->sent[0]->getCommandPool());

	CPPUNIT_ASSERT(fc->peer.TTL == 0);

	lower->onData(lower->sent[0]);

	CPPUNIT_ASSERT_EQUAL( size_t(1), lower->sent.size() );
}

void ForwardingTest::deliver()
{
	// If packet is for our listening address it must be passed to the upper layer
	setupAddressingProvider("127.0.0.2", "127.0.0.1");
	setupForwarding(true, "127.0.0.1", 255);

	aps->sendData(fuNet->createCompound());

	CPPUNIT_ASSERT_EQUAL( size_t(1), lower->sent.size() );

	ForwardingCommand* fc = forwarding->getCommand(lower->sent[0]->getCommandPool());

	CPPUNIT_ASSERT(fc->peer.TTL == 255);

	lower->onData(lower->sent[0]);

	CPPUNIT_ASSERT_EQUAL( size_t(1), aps->received.size() );
}

void ForwardingTest::deliverTTLExceeded()
{
	// We drop the packet if TTL is smaller than zero.
	/**
	 * @todo dbn: Modify whole TTL handling that negative TTLs are asserted
	 */
	setupAddressingProvider("127.0.0.2", "127.0.0.1");
	setupForwarding(true, "127.0.0.1", -1);

	aps->sendData(fuNet->createCompound());

	CPPUNIT_ASSERT_EQUAL( size_t(1), lower->sent.size() );

	ForwardingCommand* fc = forwarding->getCommand(lower->sent[0]->getCommandPool());

	CPPUNIT_ASSERT(fc->peer.TTL == -1);

	lower->onData(lower->sent[0]);

	CPPUNIT_ASSERT_EQUAL( size_t(0), aps->received.size() );
}

void ForwardingTest::noAPCMutation()
{
	// Whatever we do, we are not allowed to mutate the AddressingCommand
	setupAddressingProvider("127.0.0.2", "127.0.0.1");
	setupForwarding(true, "127.0.0.1", 255);

	aps->sendData(fuNet->createCompound());

	CPPUNIT_ASSERT_EQUAL( size_t(1), lower->sent.size() );

	AddressingProviderInterface<wns::service::nl::Address>* api = aps->getCommand(lower->sent[0]->getCommandPool());

	CPPUNIT_ASSERT(api->getSourceAddress() == wns::service::nl::Address("127.0.0.2"));

	CPPUNIT_ASSERT(api->getDestinationAddress() == wns::service::nl::Address("127.0.0.1"));

	lower->onData(lower->sent[0]);

	CPPUNIT_ASSERT_EQUAL( size_t(1), aps->received.size() );

	api = aps->getCommand(aps->received[0]->getCommandPool());

	CPPUNIT_ASSERT(api->getSourceAddress() == wns::service::nl::Address("127.0.0.2"));

	CPPUNIT_ASSERT(api->getDestinationAddress() == wns::service::nl::Address("127.0.0.1"));
}


