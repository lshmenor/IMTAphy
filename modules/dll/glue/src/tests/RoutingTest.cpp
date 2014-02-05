/******************************************************************************
 * WNS (Wireless Network Simulator)                                           *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2004-2006                                                    *
 * Chair of Communication Networks (ComNets)                                  *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnets.rwth-aachen.de                                          *
 * www: http://wns.comnets.rwth-aachen.de                                     *
 ******************************************************************************/

#include <GLUE/Routing.hpp>
#include <GLUE/convergence/Upper.hpp>

#include <WNS/ldk/Layer.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/tools/Stub.hpp>
#include <WNS/ldk/multiplexer/Dispatcher.hpp>

#include <WNS/ldk/helper/FakePDU.hpp>
#include <WNS/service/dll/Address.hpp>
#include <WNS/service/dll/Handler.hpp>
#include <WNS/pyconfig/Parser.hpp>
#include <WNS/Exception.hpp>
#include <WNS/CppUnit.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace glue {

	class RoutingTest :
		public wns::TestFixture
	{
		CPPUNIT_TEST_SUITE( RoutingTest );
		CPPUNIT_TEST( testNoRouteCompoundWithoutRoutingInformation );
		CPPUNIT_TEST( testNoRouteCompoundWithRoutingInformation );
		CPPUNIT_TEST( testSetRoute );
		CPPUNIT_TEST( testSetSameRouteTwice );
		CPPUNIT_TEST( testRouteChangeNotAllowed );
		CPPUNIT_TEST( testRouteChangeAllowed );
		CPPUNIT_TEST( testIncomingCompoundsPassThrough );
		CPPUNIT_TEST( testWakeup );
		CPPUNIT_TEST( testCommandSize );
		CPPUNIT_TEST_SUITE_END();

	public:
		class DummyHandler :
			public wns::service::dll::Handler
		{
		public:
			virtual void
			onData(const wns::osi::PDUPtr&, wns::service::dll::FlowID)
			{}
		};

		void prepare();
		void cleanup();

		void testNoRouteCompoundWithoutRoutingInformation();
		void testNoRouteCompoundWithRoutingInformation();
		void testSetRoute();
		void testSetSameRouteTwice();
		void testRouteChangeNotAllowed();
		void testRouteChangeAllowed();
		void testIncomingCompoundsPassThrough();
		void testWakeup();
		void testCommandSize();

	private:
		void doCatchExceptionSetUp(wns::pyconfig::View routingView);
		void doCatchExceptionTearDown();

		wns::ldk::ILayer* layer;
		wns::ldk::fun::Main* fun;

		DummyHandler* dummyHandler;
		glue::convergence::UnicastUpper* upperConvergence;
		wns::ldk::multiplexer::Dispatcher* dispatcher;
		Routing* routing;
		wns::ldk::tools::Stub* belowUpperConvergence;
		wns::ldk::tools::Stub* aboveRouting;
		wns::ldk::tools::Stub* belowRouting;
	};

	CPPUNIT_TEST_SUITE_REGISTRATION( RoutingTest );

	using namespace wns::ldk;

	void
	RoutingTest::doCatchExceptionSetUp(wns::pyconfig::View routingView)
	{
		// Clear the commandProxy to remove remainders of previous tests
		wns::ldk::CommandProxy::clearRegistries();
		layer = new wns::ldk::tests::LayerStub();
		fun = new wns::ldk::fun::Main(layer);

		wns::pyconfig::Parser upperConvergencePyCo;
		upperConvergencePyCo.loadString("import glue.Glue\n"
						"upperConvergence = glue.Glue.UpperConvergence()\n");
		wns::pyconfig::View upperConvergenceView(upperConvergencePyCo, "upperConvergence");
		upperConvergence = new glue::convergence::UnicastUpper(fun, upperConvergenceView);

		wns::service::dll::UnicastAddress sourceAddress(1);
		upperConvergence->setMACAddress(sourceAddress);
		dummyHandler = new DummyHandler();
		upperConvergence->registerHandler(wns::service::dll::TESTING, dummyHandler);

		wns::pyconfig::Parser dispatcherPyCo;
		dispatcherPyCo.loadString("import openwns.Multiplexer\n"
					  "dispatcher = openwns.Multiplexer.Dispatcher(1)\n");
		wns::pyconfig::View dispatcherView(dispatcherPyCo, "dispatcher");
		dispatcher = new wns::ldk::multiplexer::Dispatcher(fun, dispatcherView);

		routing = new Routing(fun, routingView);

		wns::pyconfig::Parser emptyPyCo;
		belowUpperConvergence = new wns::ldk::tools::Stub(fun, emptyPyCo);
		aboveRouting = new wns::ldk::tools::Stub(fun, emptyPyCo);
		belowRouting = new wns::ldk::tools::Stub(fun, emptyPyCo);

		fun->addFunctionalUnit("upperConvergence", upperConvergence);
		fun->addFunctionalUnit("dispatcher", dispatcher);
		fun->addFunctionalUnit("routing", routing);
		fun->addFunctionalUnit("belowUpperConvergence", belowUpperConvergence);
		fun->addFunctionalUnit("aboveRouting", aboveRouting);
		fun->addFunctionalUnit("belowRouting", belowRouting);

		upperConvergence
			->connect(belowUpperConvergence)
			->connect(dispatcher)
			->connect(routing)
			->connect(belowRouting);

		aboveRouting
			->connect(dispatcher);

		// find friends
		fun->onFUNCreated();
	} // doCatchExceptionSetUp

	void
	RoutingTest::prepare()
	{
		wns::pyconfig::Parser routingPyCo;
		routingPyCo.loadString("import glue.Routing\n"
				       "routing = glue.Routing.Routing(\"upperConvergence\")\n");
		wns::pyconfig::View routingView(routingPyCo, "routing");
		doCatchExceptionSetUp(routingView);
	} // prepare

	void
	RoutingTest::doCatchExceptionTearDown()
	{
		delete dummyHandler;

		delete fun;
		delete layer;
	} // doCatchExceptionTearDown

	void
	RoutingTest::cleanup()
	{
		doCatchExceptionTearDown();
	} // cleanup

	void
	RoutingTest::testNoRouteCompoundWithoutRoutingInformation()
	{
		CompoundPtr compound = CompoundPtr(new Compound(fun->getProxy()->createCommandPool()));

		CPPUNIT_ASSERT(!aboveRouting->isAccepting(compound));
	} // testNoRouteCompoundWithoutRoutingInformation

	void
	RoutingTest::testNoRouteCompoundWithRoutingInformation()
	{
		wns::ldk::helper::FakePDUPtr pdu = wns::ldk::helper::FakePDUPtr(new wns::ldk::helper::FakePDU(42));
		wns::service::dll::UnicastAddress address(2);
		upperConvergence->sendData(address, pdu, wns::service::dll::TESTING, 42);

		CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), belowRouting->sent.size());

		CompoundPtr compound = belowRouting->sent[0];

		convergence::UnicastUpperCommand* upperCommand =
			dynamic_cast<convergence::UnicastUpperCommand*>(fun->getProxy()->getCommand(compound->getCommandPool(), upperConvergence));

		assure(upperCommand, "Command of Upper Convergence FU is not of type UpperCommand*");

		CPPUNIT_ASSERT_EQUAL(wns::service::dll::UnicastAddress(1), upperCommand->peer.sourceMACAddress);
		CPPUNIT_ASSERT_EQUAL(wns::service::dll::UnicastAddress(2), upperCommand->peer.targetMACAddress);

		// routing information is set using incoming compounds
		// hence, compounds without an UpperCommand shall still be discarded
		compound = CompoundPtr(new Compound(fun->getProxy()->createCommandPool()));

		CPPUNIT_ASSERT(!aboveRouting->isAccepting(compound));
	} // testNoRouteCompoundWithRoutingInformation

	void
	RoutingTest::testSetRoute()
	{
		wns::ldk::helper::FakePDUPtr pdu = wns::ldk::helper::FakePDUPtr(new wns::ldk::helper::FakePDU(42));
		wns::service::dll::UnicastAddress address(2);
		upperConvergence->sendData(address, pdu, wns::service::dll::TESTING, 42);

		// setting routing information
		CompoundPtr compound = belowRouting->sent[0];
		convergence::UnicastUpperCommand* upperCommand =
			dynamic_cast<convergence::UnicastUpperCommand*>(fun->getProxy()->getCommand(compound->getCommandPool(), upperConvergence));

		assure(upperCommand, "Command of Upper Convergence FU is not of type UpperCommand*");

		wns::service::dll::UnicastAddress helpAddress;
		helpAddress = upperCommand->peer.sourceMACAddress;
		upperCommand->peer.sourceMACAddress = upperCommand->peer.targetMACAddress;
		upperCommand->peer.targetMACAddress = helpAddress;
		belowRouting->onData(compound);

		// test routing information
		compound = CompoundPtr(new Compound(fun->getProxy()->createCommandPool()));

		CPPUNIT_ASSERT(aboveRouting->isAccepting(compound));
		aboveRouting->sendData(compound);
		CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), belowRouting->sent.size());

		upperCommand =
			dynamic_cast<convergence::UnicastUpperCommand*>(fun->getProxy()->getCommand(compound->getCommandPool(), upperConvergence));

		assure(upperCommand, "Command of Upper Convergence FU is not of type UpperCommand*");

		CPPUNIT_ASSERT_EQUAL(wns::service::dll::UnicastAddress(1), upperCommand->peer.sourceMACAddress);
		CPPUNIT_ASSERT_EQUAL(wns::service::dll::UnicastAddress(2), upperCommand->peer.targetMACAddress);

		// routing information of outgoing compounds with an
		// UpperCommand shall not be changed
		pdu = wns::ldk::helper::FakePDUPtr(new wns::ldk::helper::FakePDU(42));
		address = wns::service::dll::UnicastAddress(3);
		upperConvergence->sendData(address, pdu, wns::service::dll::TESTING, 42);

		CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), belowRouting->sent.size());

		compound = belowRouting->sent[2];

		upperCommand =
			dynamic_cast<convergence::UnicastUpperCommand*>(fun->getProxy()->getCommand(compound->getCommandPool(), upperConvergence));

		assure(upperCommand, "Command of Upper Convergence FU is not of type UpperCommand*");

		CPPUNIT_ASSERT_EQUAL(wns::service::dll::UnicastAddress(1), upperCommand->peer.sourceMACAddress);
		CPPUNIT_ASSERT_EQUAL(wns::service::dll::UnicastAddress(3), upperCommand->peer.targetMACAddress);
	} // testSetRoute

	void
	RoutingTest::testSetSameRouteTwice()
	{
		wns::ldk::helper::FakePDUPtr pdu = wns::ldk::helper::FakePDUPtr(new wns::ldk::helper::FakePDU(42));
		wns::service::dll::UnicastAddress address(2);
		upperConvergence->sendData(address, pdu, wns::service::dll::TESTING, 42);

		// setting routing information
		CompoundPtr compound = belowRouting->sent[0];
		convergence::UnicastUpperCommand* upperCommand =
			dynamic_cast<convergence::UnicastUpperCommand*>(fun->getProxy()->getCommand(compound->getCommandPool(), upperConvergence));

		assure(upperCommand, "Command of Upper Convergence FU is not of type UpperCommand*");

		wns::service::dll::UnicastAddress helpAddress;
		helpAddress = upperCommand->peer.sourceMACAddress;
		upperCommand->peer.sourceMACAddress = upperCommand->peer.targetMACAddress;
		upperCommand->peer.targetMACAddress = helpAddress;
		belowRouting->onData(compound);


		pdu = wns::ldk::helper::FakePDUPtr(new wns::ldk::helper::FakePDU(42));
		address = wns::service::dll::UnicastAddress(2);
		upperConvergence->sendData(address, pdu, wns::service::dll::TESTING, 42);

		// setting routing information
		compound = belowRouting->sent[1];
		upperCommand =
			dynamic_cast<convergence::UnicastUpperCommand*>(fun->getProxy()->getCommand(compound->getCommandPool(), upperConvergence));

		assure(upperCommand, "Command of Upper Convergence FU is not of type UpperCommand*");

		helpAddress = upperCommand->peer.sourceMACAddress;
		upperCommand->peer.sourceMACAddress = upperCommand->peer.targetMACAddress;
		upperCommand->peer.targetMACAddress = helpAddress;
		belowRouting->onData(compound);
	} // testSetSameRouteTwice

	void
	RoutingTest::testRouteChangeNotAllowed()
	{
		wns::ldk::helper::FakePDUPtr pdu = wns::ldk::helper::FakePDUPtr(new wns::ldk::helper::FakePDU(42));
		wns::service::dll::UnicastAddress address(2);
		upperConvergence->sendData(address, pdu, wns::service::dll::TESTING, 42);

		// setting routing information
		CompoundPtr compound = belowRouting->sent[0];
		convergence::UnicastUpperCommand* upperCommand =
			dynamic_cast<convergence::UnicastUpperCommand*>(fun->getProxy()->getCommand(compound->getCommandPool(), upperConvergence));

		assure(upperCommand, "Command of Upper Convergence FU is not of type UpperCommand*");

		wns::service::dll::UnicastAddress helpAddress;
		helpAddress = upperCommand->peer.sourceMACAddress;
		upperCommand->peer.sourceMACAddress = upperCommand->peer.targetMACAddress;
		upperCommand->peer.targetMACAddress = helpAddress;
		belowRouting->onData(compound);


		pdu = wns::ldk::helper::FakePDUPtr(new wns::ldk::helper::FakePDU(42));
		address = wns::service::dll::UnicastAddress(3);
		upperConvergence->sendData(address, pdu, wns::service::dll::TESTING, 42);

		// setting routing information
		compound = belowRouting->sent[1];
		upperCommand =
			dynamic_cast<convergence::UnicastUpperCommand*>(fun->getProxy()->getCommand(compound->getCommandPool(), upperConvergence));

		assure(upperCommand, "Command of Upper Convergence FU is not of type UpperCommand*");

		helpAddress = upperCommand->peer.sourceMACAddress;
		upperCommand->peer.sourceMACAddress = upperCommand->peer.targetMACAddress;
		upperCommand->peer.targetMACAddress = helpAddress;
		belowRouting->onData(compound);

		// test routing information
		compound = CompoundPtr(new Compound(fun->getProxy()->createCommandPool()));

		CPPUNIT_ASSERT(aboveRouting->isAccepting(compound));
		aboveRouting->sendData(compound);
		CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), belowRouting->sent.size());

		upperCommand =
			dynamic_cast<convergence::UnicastUpperCommand*>(fun->getProxy()->getCommand(compound->getCommandPool(), upperConvergence));

		assure(upperCommand, "Command of Upper Convergence FU is not of type UpperCommand*");

		CPPUNIT_ASSERT_EQUAL(wns::service::dll::UnicastAddress(1), upperCommand->peer.sourceMACAddress);
		CPPUNIT_ASSERT_EQUAL(wns::service::dll::UnicastAddress(2), upperCommand->peer.targetMACAddress);
	} // testRouteChangeNotAllowed

	void
	RoutingTest::testRouteChangeAllowed()
	{
		doCatchExceptionTearDown();

		wns::pyconfig::Parser routingPyCo;
		routingPyCo.loadString("import glue.Routing\n"
				       "routing = glue.Routing.Routing(\"upperConvergence\", allowRouteChange = True)\n");
		wns::pyconfig::View routingView(routingPyCo, "routing");
		doCatchExceptionSetUp(routingView);

		wns::ldk::helper::FakePDUPtr pdu = wns::ldk::helper::FakePDUPtr(new wns::ldk::helper::FakePDU(42));
		wns::service::dll::UnicastAddress address(2);
		upperConvergence->sendData(address, pdu, wns::service::dll::TESTING, 42);

		// setting routing information
		CompoundPtr compound = belowRouting->sent[0];
		convergence::UnicastUpperCommand* upperCommand =
			dynamic_cast<convergence::UnicastUpperCommand*>(fun->getProxy()->getCommand(compound->getCommandPool(), upperConvergence));

		assure(upperCommand, "Command of Upper Convergence FU is not of type UpperCommand*");

		wns::service::dll::UnicastAddress helpAddress;
		helpAddress = upperCommand->peer.sourceMACAddress;
		upperCommand->peer.sourceMACAddress = upperCommand->peer.targetMACAddress;
		upperCommand->peer.targetMACAddress = helpAddress;
		belowRouting->onData(compound);


		pdu = wns::ldk::helper::FakePDUPtr(new wns::ldk::helper::FakePDU(42));
		address = wns::service::dll::UnicastAddress(3);
		upperConvergence->sendData(address, pdu, wns::service::dll::TESTING, 42);

		// setting routing information
		compound = belowRouting->sent[1];
		upperCommand =
			dynamic_cast<convergence::UnicastUpperCommand*>(fun->getProxy()->getCommand(compound->getCommandPool(), upperConvergence));

		assure(upperCommand, "Command of Upper Convergence FU is not of type UpperCommand*");

		helpAddress = upperCommand->peer.sourceMACAddress;
		upperCommand->peer.sourceMACAddress = upperCommand->peer.targetMACAddress;
		upperCommand->peer.targetMACAddress = helpAddress;
		belowRouting->onData(compound);

		// test routing information
		compound = CompoundPtr(new Compound(fun->getProxy()->createCommandPool()));

		CPPUNIT_ASSERT(aboveRouting->isAccepting(compound));
		aboveRouting->sendData(compound);
		CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), belowRouting->sent.size());

		upperCommand =
			dynamic_cast<convergence::UnicastUpperCommand*>(fun->getProxy()->getCommand(compound->getCommandPool(), upperConvergence));

		assure(upperCommand, "Command of Upper Convergence FU is not of type UpperCommand*");

		CPPUNIT_ASSERT_EQUAL(wns::service::dll::UnicastAddress(1), upperCommand->peer.sourceMACAddress);
		CPPUNIT_ASSERT_EQUAL(wns::service::dll::UnicastAddress(3), upperCommand->peer.targetMACAddress);
	} // testRouteChangeAllowed

	void
	RoutingTest::testIncomingCompoundsPassThrough()
	{
		CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), belowUpperConvergence->received.size());

		wns::ldk::helper::FakePDUPtr pdu = wns::ldk::helper::FakePDUPtr(new wns::ldk::helper::FakePDU(42));
		wns::service::dll::UnicastAddress address(2);
		upperConvergence->sendData(address, pdu, wns::service::dll::TESTING, 42);

		CompoundPtr compound = belowRouting->sent[0];
		convergence::UnicastUpperCommand* upperCommand =
			dynamic_cast<convergence::UnicastUpperCommand*>(fun->getProxy()->getCommand(compound->getCommandPool(), upperConvergence));

		assure(upperCommand, "Command of Upper Convergence FU is not of type UpperCommand*");

		wns::service::dll::UnicastAddress helpAddress;
		helpAddress = upperCommand->peer.sourceMACAddress;
		upperCommand->peer.sourceMACAddress = upperCommand->peer.targetMACAddress;
		upperCommand->peer.targetMACAddress = helpAddress;
		belowRouting->onData(compound);

		CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), belowUpperConvergence->received.size());
		CPPUNIT_ASSERT_EQUAL(compound, belowUpperConvergence->received[0]);
	} // testIncomingCompoundsPassThrough

	void
	RoutingTest::testWakeup()
	{
		CPPUNIT_ASSERT_EQUAL(static_cast<long>(0), belowUpperConvergence->wakeupCalled);

		belowRouting->wakeup();

		CPPUNIT_ASSERT_EQUAL(static_cast<long>(1), belowUpperConvergence->wakeupCalled);
	} // testWakeup

	void
	RoutingTest::testCommandSize()
	{
		wns::ldk::helper::FakePDUPtr pdu = wns::ldk::helper::FakePDUPtr(new wns::ldk::helper::FakePDU(42));
		wns::service::dll::UnicastAddress address(2);
		upperConvergence->sendData(address, pdu, wns::service::dll::TESTING, 42);

		// setting routing information
		CompoundPtr compound = belowRouting->sent[0];
		convergence::UnicastUpperCommand* upperCommand =
			dynamic_cast<convergence::UnicastUpperCommand*>(fun->getProxy()->getCommand(compound->getCommandPool(), upperConvergence));

		assure(upperCommand, "Command of Upper Convergence FU is not of type UpperCommand*");

		wns::service::dll::UnicastAddress helpAddress;
		helpAddress = upperCommand->peer.sourceMACAddress;
		upperCommand->peer.sourceMACAddress = upperCommand->peer.targetMACAddress;
		upperCommand->peer.targetMACAddress = helpAddress;
		belowRouting->onData(compound);

		pdu = wns::ldk::helper::FakePDUPtr(new wns::ldk::helper::FakePDU(42));
		address = wns::service::dll::UnicastAddress(2);
		upperConvergence->sendData(address, pdu, wns::service::dll::TESTING, 42);

		compound = belowRouting->sent[1];
		Bit headerSize = 0;
		Bit payloadSize = 0;
		belowRouting->calculateSizes(compound->getCommandPool(),
					     headerSize,
					     payloadSize);
		CPPUNIT_ASSERT_EQUAL(static_cast<Bit>(1), headerSize);

		compound = CompoundPtr(new Compound(fun->getProxy()->createCommandPool()));

		CPPUNIT_ASSERT(aboveRouting->isAccepting(compound));
		aboveRouting->sendData(compound);

		compound = belowRouting->sent[2];
		headerSize = 0;
		payloadSize = 0;
		belowRouting->calculateSizes(compound->getCommandPool(),
					     headerSize,
					     payloadSize);
		CPPUNIT_ASSERT_EQUAL(static_cast<Bit>(1), headerSize);
	} // testCommandSize

} // glue


