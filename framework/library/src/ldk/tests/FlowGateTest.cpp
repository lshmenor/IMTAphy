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

#include <WNS/CppUnit.hpp>
#include <WNS/ldk/FlowGate.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/ldk/tools/FakeFU.hpp>
#include <WNS/ldk/tools/Stub.hpp>
#include <WNS/ldk/Delayed.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/Cloneable.hpp>
#include <WNS/ldk/tools/Stub.hpp>
#include <WNS/pyconfig/Parser.hpp>

namespace wns { namespace ldk { namespace tests {

	namespace flowgatetest {

		class MyCommand :
			public Command
		{
		public:
			struct {
				int flow;
			} local;
			struct {} peer;
			struct {} magic;
		};

		class MyFunctionalUnit;
		class MyLayer :
			public wns::ldk::tests::LayerStub
		{
		public:
			MyLayer() :
				wns::ldk::tests::LayerStub()
			{}

			MyFunctionalUnit* prototype;
		};

		class MyFunctionalUnit :
			public CommandTypeSpecifier<MyCommand>,
			public HasReceptor<>,
			public HasConnector<>,
			public HasDeliverer<>,
			public Delayed<MyFunctionalUnit>,
			public Cloneable<MyFunctionalUnit>
		{
		public:
			MyFunctionalUnit(fun::FUN* fuNet, wns::pyconfig::View& /* _config*/) :
					CommandTypeSpecifier<MyCommand>(fuNet),
					HasReceptor<>(),
					HasConnector<>(),
					HasDeliverer<>(),
					Delayed<MyFunctionalUnit>(),
					Cloneable<MyFunctionalUnit>()
			{}

			virtual void processIncoming(const CompoundPtr&){}
			virtual void processOutgoing(const CompoundPtr&){}
			virtual bool hasCapacity() const { return false; }
			virtual const CompoundPtr hasSomethingToSend() const { return wns::ldk::CompoundPtr(); }
			virtual CompoundPtr getSomethingToSend() { return wns::ldk::CompoundPtr(); }
		};

		class MyKeyBuilder;
		class MyKey :
			public Key
		{
		public:
			MyKey(const MyKeyBuilder* factory, const CompoundPtr& compound, int/* not used */);

			MyKey(int _flow) :
					flow(_flow)
			{}

			bool operator<(const Key& _other) const
			{
				assure(dynamic_cast<const MyKey*>(&_other), "Comparing Keys of different types.");

				const MyKey* other = static_cast<const MyKey*>(&_other);
				return flow < other->flow;
			}

			std::string str() const	// FIXME(fds) should be streaming
			{
				std::stringstream ss;
				ss << "flow:" << flow;
				return ss.str();
			}

			int flow;
		};

		class MyKeyBuilder :
			public KeyBuilder
		{
		public:
			MyKeyBuilder(const fun::FUN* fuNet, const pyconfig::View& /*config*/)
			{
				assure(dynamic_cast<const MyLayer*>(fuNet->getLayer()), "wrong layer type given in MyKey constructor");
				layer = static_cast<const MyLayer*>(fuNet->getLayer());
			}

			void
			onFUNCreated()
			{
				friends.prototype = layer->prototype;
			}

			virtual ConstKeyPtr
			operator() (const CompoundPtr& compound, int direction = 4711 /* not used */) const
			{
				return ConstKeyPtr(new MyKey(this, compound, direction));
			}

			const MyLayer* layer;
			struct Friends {
				MyFunctionalUnit* prototype;
			} friends;
		};

		MyKey::MyKey(const MyKeyBuilder* factory, const CompoundPtr& compound, int/* not used */)
		{
			CommandPool* commandPool = compound->getCommandPool();
			MyCommand* command = factory->friends.prototype->getCommand(commandPool);
			flow = command->local.flow;
		}



	}

	using namespace wns::ldk::tests::flowgatetest;

	class FlowGateTest :
		public wns::TestFixture
	{
		flowgatetest::MyLayer* layer;
		fun::FUN* fuNet;

		tools::Stub* upper;
		tools::Stub* lower;
		flowgatetest::MyKeyBuilder* factory;
		FlowGate* flowGate;

		void
		prepare()
		{
			wns::ldk::CommandProxy::clearRegistries();

			wns::pyconfig::Parser emptyConfig;
			wns::pyconfig::Parser config;
			config.loadString(
				"import openwns.logger\n"
				"logger = openwns.logger.Logger(\n"
				"    'WNS',\n"
				"    'FlowGate',\n"
				"    True\n"
				"    )\n"
				);

			layer = new flowgatetest::MyLayer();
			fuNet = new wns::ldk::fun::Main(layer);

			upper = new tools::Stub(fuNet, emptyConfig);
			lower = new tools::Stub(fuNet, emptyConfig);

			layer->prototype = new flowgatetest::MyFunctionalUnit(fuNet, emptyConfig);
			fuNet->addFunctionalUnit("dummyCommand", "dummyFU", layer->prototype);
			factory = new flowgatetest::MyKeyBuilder(fuNet, emptyConfig);

			flowGate = new FlowGate(fuNet,
						config,
						factory);

			upper
				->connect(flowGate)
				->connect(lower);

			flowGate->onFUNCreated();
		}

		void
		cleanup()
		{
			delete upper;
			delete lower;
			delete flowGate;
			delete layer;
		}

		CPPUNIT_TEST_SUITE( FlowGateTest );
                CPPUNIT_TEST( testCreateFlow );
                CPPUNIT_TEST( testDestroyFlow );
                CPPUNIT_TEST( testOpenCloseFlow );
		CPPUNIT_TEST_SUITE_END();

	public:
		void
		testCreateFlow()
		{
			// Create Key
			KeyPtr key(new MyKey(0));
			CPPUNIT_ASSERT_NO_THROW(flowGate->createFlow(key));

			// Should not be able to create the same flow again
			CPPUNIT_ASSERT_THROW(flowGate->createFlow(key), wns::Exception);

			// Should not be able to create the same flow again, not
			// even with a new key
			KeyPtr key2(new MyKey(0));
			CPPUNIT_ASSERT_THROW(flowGate->createFlow(key2), wns::Exception);

			// Create another flow
			KeyPtr key3(new MyKey(1));
			CPPUNIT_ASSERT_NO_THROW(flowGate->createFlow(key3));
		}

		void
		testDestroyFlow()
		{
			// Create Flow
			KeyPtr key(new MyKey(0));
			CPPUNIT_ASSERT_NO_THROW(flowGate->createFlow(key));

			// Destroy Flow, key will be deleted
			CPPUNIT_ASSERT_NO_THROW(flowGate->destroyFlow(key));

			// Should not be able to destroy the same flow again
			KeyPtr key2(new MyKey(0));
			CPPUNIT_ASSERT_THROW(flowGate->destroyFlow(key), wns::Exception);

			// Re-Create Flow
			CPPUNIT_ASSERT_NO_THROW(flowGate->createFlow(key));
		}

		void
		testOpenCloseFlow()
		{
			// Create compounds for two flows
			CompoundPtr compound1(fuNet->createCompound());
			layer->prototype->activateCommand(compound1->getCommandPool())->local.flow = 1;

			CompoundPtr compound2(fuNet->createCompound());
			layer->prototype->activateCommand(compound2->getCommandPool())->local.flow = 2;

			// Create Keys and gates for the two flows
			KeyPtr key1(new MyKey(1));
			KeyPtr key2(new MyKey(2));
			CPPUNIT_ASSERT_NO_THROW(flowGate->createFlow(key1));
			CPPUNIT_ASSERT_NO_THROW(flowGate->createFlow(key2));

			// Close flow=1, leave flow=2 open
			CPPUNIT_ASSERT_NO_THROW(flowGate->closeFlow(key1));

			// flow=1 should not accept outgoing
			CPPUNIT_ASSERT_EQUAL( false, flowGate->isAccepting(compound1) );
			// flow=1 should drop incoming
			lower->onData(compound1);
			CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(0), upper->received.size() );

			// flow=2 should accept outgoing
			CPPUNIT_ASSERT_EQUAL( true, flowGate->isAccepting(compound2) );
			// flow=2 should forward incoming
			lower->onData(compound2);
			CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(1), upper->received.size() );

			// Re-open flow=1
			CPPUNIT_ASSERT_NO_THROW(flowGate->openFlow(key1));

			// flow=1 should now accept outgoing
			CPPUNIT_ASSERT_EQUAL( true, flowGate->isAccepting(compound1) );
			upper->sendData(compound1);
			CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(1), lower->sent.size() );
			// flow=1 should forward incoming
			lower->onData(compound1);
			CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(2), upper->received.size() );
		}


	};


	CPPUNIT_TEST_SUITE_REGISTRATION( FlowGateTest );

} // tests
} // ldk
} // wns




