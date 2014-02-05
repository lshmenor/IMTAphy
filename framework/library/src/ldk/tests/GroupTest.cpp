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

#include <WNS/ldk/Group.hpp>

#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/ldk/fun/Main.hpp>

#include <WNS/ldk/tools/Stub.hpp>

#include <WNS/pyconfig/Parser.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <stdexcept>

namespace wns { namespace ldk { namespace tests	{

	class GroupTest : public CppUnit::TestFixture  {
		CPPUNIT_TEST_SUITE( GroupTest );
		CPPUNIT_TEST( testOutgoing );
		CPPUNIT_TEST( testIncoming );
		CPPUNIT_TEST_SUITE_END();

		ILayer* layer;
		fun::FUN* fuNet;

		tools::Stub* upper;
		Group* group;
		tools::Stub* lower;

		tools::Stub* inner1;
		tools::Stub* inner2;

	public:
		void setUp()
		{
			pyconfig::Parser emptyConfig;

			layer = new LayerStub();
			fuNet = new fun::Main(layer);

			upper = new tools::Stub(fuNet, emptyConfig);
			lower = new tools::Stub(fuNet, emptyConfig);

			{
				pyconfig::Parser groupConfig;
				groupConfig.loadString(
					"from openwns.FUN import FUN, Node\n"
					"from openwns.Group import Group\n"
					"from openwns.Tools import Stub\n"
					"fun = FUN()\n"
					"a = Node('inner1', Stub())\n"
					"b = Node('inner2', Stub())\n"
					"fun.add(a)\n"
					"fun.add(b)\n"
					"a.connect(b)\n"
					"group = Group(fun, 'inner1', 'inner2')\n"
					);

				pyconfig::View groupView(groupConfig, "group");
				group = new Group(fuNet, groupView);
			}

			fuNet->addFunctionalUnit("upper", upper);
			fuNet->addFunctionalUnit("group", group);
			fuNet->addFunctionalUnit("lower", lower);

			upper
				->connect(group)
				->connect(lower);

			inner1 = group->getSubFUN()->findFriend<tools::Stub*>("inner1");
			inner2 = group->getSubFUN()->findFriend<tools::Stub*>("inner2");
		}

		void tearDown()
		{
			delete layer;
			delete fuNet;
		}

		void testOutgoing()
		{
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), inner1->sent.size());
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), inner2->sent.size());
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), lower->sent.size());

			CompoundPtr compound(fuNet->createCompound());
			upper->sendData(compound);

			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), inner1->sent.size());
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), inner2->sent.size());
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), lower->sent.size());
		}

		void testIncoming()
		{
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), upper->received.size());
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), inner1->received.size());
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), inner2->received.size());

			CompoundPtr compound(fuNet->createCompound());
			lower->onData(compound);

			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), upper->received.size());
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), inner1->received.size());
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), inner2->received.size());
		}
	};


	CPPUNIT_TEST_SUITE_REGISTRATION( GroupTest );

} // wns
} // ldk
} // tests



