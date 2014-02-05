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

#include <WNS/ldk/CommandProxy.hpp>
#include <WNS/ldk/Command.hpp>
#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/Cloneable.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/ldk/tools/FakeFU.hpp>
#include <WNS/pyconfig/Parser.hpp>
#include <WNS/CppUnit.hpp>

namespace wns { namespace ldk { namespace tests {

	struct ACommand :
		public wns::ldk::Command
	{
		int local;
		int peer;
		int magic;
	};

	struct A :
		public virtual FunctionalUnit,
		public HasReceptor<>,
		public HasConnector<>,
		public HasDeliverer<>,
		public Cloneable<A>,
		public wns::ldk::CommandTypeSpecifier<ACommand>
	{
		A(wns::ldk::fun::FUN* fun) : wns::ldk::CommandTypeSpecifier<ACommand>(fun) {};
		void doOnData(const CompoundPtr&){}
		void doSendData(const CompoundPtr&){}
		bool doIsAccepting(const CompoundPtr&) const { return true; }
		void doWakeup(){}
	};

	struct BCommand :
		public wns::ldk::Command
	{
		int local;
		int peer;
		int magic;
	};

	struct B :
		public virtual FunctionalUnit,
		public HasReceptor<>,
		public HasConnector<>,
		public HasDeliverer<>,
		public Cloneable<B>,
		public wns::ldk::CommandTypeSpecifier<BCommand>
	{
		B(wns::ldk::fun::FUN* fun) : wns::ldk::CommandTypeSpecifier<BCommand>(fun) {};
		void doOnData(const CompoundPtr&){}
		void doSendData(const CompoundPtr&){}
		bool doIsAccepting(const CompoundPtr&) const { return true; }
		void doWakeup(){}
	};


	class CommandProxyTest :
		public wns::TestFixture
	{
		CPPUNIT_TEST_SUITE( CommandProxyTest );
		CPPUNIT_TEST( add );
		CPPUNIT_TEST( activate );
		CPPUNIT_TEST( commandReader );
		CPPUNIT_TEST( copy );
		CPPUNIT_TEST( partialCopy );
		CPPUNIT_TEST_SUITE_END();
	public:
		void prepare();
		void cleanup();

		void add();
		void activate();
		void commandReader();
		void copy();
		void partialCopy();
	private:

		fun::FUN* fun;
		CommandProxy* proxy;

		A* a;
		B* b;
	};

	CPPUNIT_TEST_SUITE_REGISTRATION( CommandProxyTest );

}
}
}
using namespace wns::ldk::tests;

void
CommandProxyTest::prepare()
{
	CommandProxy::clearRegistries();

	wns::ldk::ILayer* layer = new wns::ldk::tests::LayerStub();
	wns::pyconfig::Parser FUNPyCo;
	FUNPyCo.loadString("import openwns.logger\n"
			   "class LinkHandler:\n"
			   "  type = \"wns.ldk.SimpleLinkHandler\"\n"
			   "  isAcceptingLogger = openwns.logger.Logger(\"W-NS\", \"LinkHandler\", True)\n"
			   "  sendDataLogger = openwns.logger.Logger(\"W-NS\", \"LinkHandler\", True)\n"
			   "  wakeupLogger = openwns.logger.Logger(\"W-NS\", \"LinkHandler\", True)\n"
			   "  onDataLogger = openwns.logger.Logger(\"W-NS\", \"LinkHandler\", True)\n"
			   "  traceCompoundJourney = True\n"
			   "linkHandler = LinkHandler()\n"
			   "class FUNConfig:\n"
			   "  logger = openwns.logger.Logger(\"W-NS\",\"TestFUN\",True)\n"
			   "  commandProxy = openwns.FUN.CommandProxy(logger)\n"
			   "fun = FUNConfig()");
	fun = new wns::ldk::fun::Main(layer, FUNPyCo);
	proxy = fun->getProxy();
	a = new A(fun);
	b = new B(fun);
}

void
CommandProxyTest::cleanup()
{
	delete fun;
	fun = NULL;
	proxy = NULL;
	delete a;
	a = NULL;
	delete b;
	b = NULL;
}

void
CommandProxyTest::add()
{
	// Add "a" once
	proxy->addFunctionalUnit("roleA", a);
	// Try adding "a" gain --> oops
	CPPUNIT_ASSERT_THROW( proxy->addFunctionalUnit("roleA", a), wns::Exception );
	// Remove "a"
	CPPUNIT_ASSERT_NO_THROW( proxy->removeFunctionalUnit("roleA") );
	CommandReaderInterface* reader = NULL;
	// The commandReader should still be accessible
	CPPUNIT_ASSERT_NO_THROW( reader = proxy->getCommandReader("roleA") );
	CPPUNIT_ASSERT( reader );
	// We should not be able to add b under the former commandName of a
	CPPUNIT_ASSERT_THROW( proxy->addFunctionalUnit("roleA", b), wns::Exception );
	// but we should be able to add another instance of A
	A* other = new A(fun);
	CPPUNIT_ASSERT_NO_THROW( proxy->addFunctionalUnit("roleA", other) );
	delete other;
}

void
CommandProxyTest::activate()
{
	// Add "a" once
	proxy->addFunctionalUnit("roleA", a);
	// Add "b" once
	proxy->addFunctionalUnit("roleB", b);
	// create vanilla command Pool
	CommandPool* pool = proxy->createCommandPool();
	// try to obtain un-activated command --> oops
	CPPUNIT_ASSERT( ! proxy->commandIsActivated(pool, a));
	CPPUNIT_ASSERT_THROW( proxy->getCommand(pool, a), wns::Exception );
	// activate a's command
	proxy->activateCommand(pool, a);
	// not again --> oops
	CPPUNIT_ASSERT_THROW( proxy->activateCommand(pool, a), wns::Exception);
	// try to obtain activated command --> works
	CPPUNIT_ASSERT( proxy->commandIsActivated(pool, a));
	CPPUNIT_ASSERT_NO_THROW( proxy->getCommand(pool, a) );
	delete pool;
}

void
CommandProxyTest::commandReader()
{
	// Add "a" once
	proxy->addFunctionalUnit("roleA", a);
	// create vanilla command Pool
	CommandPool* pool = proxy->createCommandPool();
	// activate a's command
	proxy->activateCommand(pool, a);
	// remove "a", accessing the command must still be possible
	proxy->removeFunctionalUnit("roleA");

	CommandReaderInterface* reader = NULL;
	ACommand* ac = NULL;
	// get the command reader
	CPPUNIT_ASSERT_NO_THROW( reader = proxy->getCommandReader("roleA") );
	CPPUNIT_ASSERT( reader );
	// and read the command
	CPPUNIT_ASSERT_NO_THROW( ac = reader->readCommand<ACommand>(pool) );
	delete pool;
}

void
CommandProxyTest::copy()
{
	// Add "a" once
	proxy->addFunctionalUnit("roleA", a);
	// Add "b" once
	proxy->addFunctionalUnit("roleB", b);
	// create vanilla command Pool
	CommandPool* pool = proxy->createCommandPool();

	// All this must be possible without the FUs actually being known in the proxy
	proxy->removeFunctionalUnit("roleA");
	proxy->removeFunctionalUnit("roleB");

	// activate a's command
	proxy->activateCommand(pool, a);
	// Make first copy
	CommandPool* copy1 = proxy->createCommandPool();
	proxy->copy(copy1, pool);
	CPPUNIT_ASSERT( copy1->knowsSameCommandsAs(*pool) );
	// activate b's command also
	proxy->activateCommand(pool, b);
	// Make second copy
	CommandPool* copy2 = proxy->createCommandPool();
	proxy->copy(copy2, pool);
	CPPUNIT_ASSERT( copy2->knowsSameCommandsAs(*pool) );

	CPPUNIT_ASSERT(  proxy->commandIsActivated(copy1, a) );
	CPPUNIT_ASSERT( !proxy->commandIsActivated(copy1, b) );

	CPPUNIT_ASSERT( proxy->commandIsActivated(copy2, a) );
	CPPUNIT_ASSERT( proxy->commandIsActivated(copy2, b) );

	delete pool;
	delete copy1;
	delete copy2;
}

void
CommandProxyTest::partialCopy()
{
	// Add "a" once
	proxy->addFunctionalUnit("roleA", a);
	// Add "b" once
	proxy->addFunctionalUnit("roleB", b);
	// create 2 vanilla command Pools
	CommandPool* pool1 = proxy->createCommandPool();
	CommandPool* pool2 = proxy->createCommandPool();

	// All this must be possible without the FUs actually being known in the proxy
	proxy->removeFunctionalUnit("roleA");
	proxy->removeFunctionalUnit("roleB");

	// pool1: activate a's and b's  command in that order
	proxy->activateCommand(pool1, a);
	proxy->activateCommand(pool1, b);

	// pool2: activate b's and a's  command (reverse order)
	proxy->activateCommand(pool2, b);
	proxy->activateCommand(pool2, a);

	// Make partial copy of pool1 from viewpoint of b --> ACommand gets copied
	CommandPool* copy = proxy->createCommandPool();
	proxy->partialCopy(b, copy, pool1);
	CPPUNIT_ASSERT(  proxy->commandIsActivated(copy, a) );
	CPPUNIT_ASSERT( !proxy->commandIsActivated(copy, b) );
	delete copy;
	// Make partial copy of pool1 from viewpoint of a --> No Command gets copied
	copy = proxy->createCommandPool();
	proxy->partialCopy(a, copy, pool1);
	CPPUNIT_ASSERT( !proxy->commandIsActivated(copy, a) );
	CPPUNIT_ASSERT( !proxy->commandIsActivated(copy, b) );
	delete copy;
	// Make partial copy of pool2 from viewpoint of a --> BCommand gets copied
	copy = proxy->createCommandPool();
	proxy->partialCopy(a, copy, pool2);
	CPPUNIT_ASSERT( !proxy->commandIsActivated(copy, a) );
	CPPUNIT_ASSERT(  proxy->commandIsActivated(copy, b) );
	delete copy;
	// Make partial copy of pool2 from viewpoint of b --> No Command gets copied
	copy = proxy->createCommandPool();
	proxy->partialCopy(b, copy, pool2);
	CPPUNIT_ASSERT( !proxy->commandIsActivated(copy, a) );
	CPPUNIT_ASSERT( !proxy->commandIsActivated(copy, b) );
	delete copy;

	delete pool1;
	delete pool2;
}



