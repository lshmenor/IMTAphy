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

#ifndef WNS_LDK_FLOWSEPARATOR_TEST_HPP
#define WNS_LDK_FLOWSEPARATOR_TEST_HPP

#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/ldk/FlowSeparator.hpp>
#include <WNS/ldk/tools/Stub.hpp>
#include <WNS/ldk/Delayed.hpp>
#include <WNS/Cloneable.hpp>
#include <WNS/pyconfig/Parser.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <stdexcept>

namespace wns { namespace ldk {

	namespace flowseparatortest {
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
			MyFunctionalUnit(fun::FUN* fuNet, wns::pyconfig::View& _config) :
					CommandTypeSpecifier<MyCommand>(fuNet),
					HasReceptor<>(),
					HasConnector<>(),
					HasDeliverer<>(),
					Delayed<MyFunctionalUnit>(),
					Cloneable<MyFunctionalUnit>(),

					config(_config),
					ind(0),
					req(0),
					buffer(CompoundPtr())
			{}

			virtual void processIncoming(const CompoundPtr& compound);
			virtual void processOutgoing(const CompoundPtr&);
			virtual bool hasCapacity() const;
			virtual const CompoundPtr hasSomethingToSend() const;
			virtual CompoundPtr getSomethingToSend();

			pyconfig::View config;
			int ind;
			int req;

		private:
			CompoundPtr buffer;
		};


		class MyKeyBuilder;
		class MyKey :
			public Key
		{
		public:
			MyKey(const MyKeyBuilder* factory, const CompoundPtr& compound, int direction = 4711 /* not used */);
			MyKey(int _flow) :
					flow(_flow)
			{}

			bool operator<(const Key& _other) const
			{
				assure(dynamic_cast<const MyKey*>(&_other), "Comparing Keys of different types.");

				const MyKey* other = static_cast<const MyKey*>(&_other);
				return flow < other->flow;
			} // <

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
			MyKeyBuilder(const fun::FUN* fuNet, const pyconfig::View& config);
			void onFUNCreated();

			virtual ConstKeyPtr
			operator() (const CompoundPtr& compound, int direction = 4711 /* not used */) const
			{
				return KeyPtr(new MyKey(this, compound, direction));
			}

			const MyLayer* layer;
			struct Friends {
				MyFunctionalUnit* prototype;
			} friends;
		};


		class MyFlowInfoProvider :
			public wns::ldk::ControlService,
			virtual public wns::ldk::flowseparator::FlowInfoProvider
		{
			bool valid;
		public:
			MyFlowInfoProvider(wns::ldk::ControlServiceRegistry* csr) :
				wns::ldk::ControlService(csr),
				valid(false)
			{}

			void onCSRCreated()
			{}

			bool isValidFlow(const ConstKeyPtr& /*key*/) const
			{ return valid; }

			void setValid(const bool v)
			{ valid = v; }
		};
	}

	class FlowSeparatorTest : public CppUnit::TestFixture  {
		CPPUNIT_TEST_SUITE( FlowSeparatorTest );
		CPPUNIT_TEST( testIncoming );
		CPPUNIT_TEST( testOutgoing );
		CPPUNIT_TEST( testMixed );
		CPPUNIT_TEST( testGetInstancePerKey );
		CPPUNIT_TEST( testGetInstancePerCompound );
 		CPPUNIT_TEST( testAddInstance );
		CPPUNIT_TEST( testRemoveInstance );
		CPPUNIT_TEST( testSingleInstanceFlowControl );
		CPPUNIT_TEST( testMultiInstanceFlowControl );
		CPPUNIT_TEST( testLayerConfig );
		CPPUNIT_TEST( testPurePython );
		CPPUNIT_TEST( testComplain );
		CPPUNIT_TEST( testComplainComplain );
		CPPUNIT_TEST( testCreateOnValidFlow );
		CPPUNIT_TEST_SUITE_END();
	public:
		void setUp();
		void tearDown();

		void testIncoming();
		void testOutgoing();
		void testMixed();
		void testGetInstancePerKey();
		void testGetInstancePerCompound();
 		void testAddInstance();
		void testRemoveInstance();
		void testSingleInstanceFlowControl();
		void testMultiInstanceFlowControl();
		void testLayerConfig();
		void testPurePython();
		void testComplain();
		void testComplainComplain();
		void testCreateOnValidFlow();
	private:
		flowseparatortest::MyLayer* layer;
		fun::FUN* fuNet;

		flowseparatortest::MyKeyBuilder* factory;
		tools::Stub* upper;
		flowseparator::CreatorStrategy* creator;
		FlowSeparator* flowSeparator;
		tools::Stub* lower;
		tools::Stub* lower2;
		flowseparatortest::MyFlowInfoProvider* fip;
	};

}}


#endif // NOT defined WNS_LDK_FLOWSEPARATOR_TEST_HPP


