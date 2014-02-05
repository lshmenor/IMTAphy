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

#include "GroupFlowSeparatorTest.hpp"
#include "LayerStub.hpp"

#include <WNS/ldk/fun/Main.hpp>

using namespace wns::ldk;
using namespace wns::ldk::tests;


CPPUNIT_TEST_SUITE_REGISTRATION( GroupFlowSeparatorTest );


void
GroupFlowSeparatorTest::setUp()
{
	wns::ldk::CommandProxy::clearRegistries();

	wns::pyconfig::Parser emptyConfig;

	layer = new LayerStub();
	fuNet = new fun::Main(layer);

	upper = 0;
	flowSeparator = 0;
	lower = 0;

	{
		pyconfig::Parser opcodeConfig;
		opcodeConfig.loadString(
			"from openwns.Multiplexer import OpcodeProvider\n"
			"foo = OpcodeProvider(1)\n"
			);
		pyconfig::View opcodeView(opcodeConfig, "foo");

		opcode = new multiplexer::OpcodeProvider(fuNet, opcodeView);
		fuNet->addFunctionalUnit("ernie", opcode);
	}

	upper = new tools::Stub(fuNet, emptyConfig);

	pyconfig::Parser groupConfig;
	groupConfig.loadString(
		"from openwns.FUN import FUN, Node\n"
		"from openwns.Group import Group\n"
		"from openwns.Tools import Stub\n"
		"from openwns.FlowSeparator import FlowSeparator\n"
		"fun = FUN()\n"
		"a = Node('inner1', Stub())\n"
		"b = Node('inner2', Stub())\n"
		"fun.add(a)\n"
		"fun.add(b)\n"
		"a.connect(b)\n"
		"group = Group(fun, 'inner1', 'inner2')\n"
		"flowSeparator = FlowSeparator(None, None)"
		);

	pyconfig::View groupView(groupConfig, "group");
	prototype = new Group(fuNet, groupView);

	pyconfig::Parser keyBuilderConfig;
	keyBuilderConfig.loadString(
		"opcode = 'ernie'\n"
		);

	std::auto_ptr<wns::ldk::KeyBuilder> factory(new multiplexer::OpcodeKeyBuilder(fuNet, keyBuilderConfig));

	flowseparator::CreatorStrategy* notFound =
		new flowseparator::PrototypeCreator(fuNet, "olga", prototype);

	std::auto_ptr<wns::ldk::KeyBuilder> keyBuilder();

	flowSeparator = new FlowSeparator(
		fuNet,
		groupConfig.get("flowSeparator"),
		factory,
		std::auto_ptr<flowseparator::NotFoundStrategy>(new flowseparator::CreateOnFirstCompound(notFound)));

	lower = new tools::Stub(fuNet, emptyConfig);

	upper
		->connect(flowSeparator)
		->connect(lower);

	flowSeparator->onFUNCreated();
} // setUp


void
GroupFlowSeparatorTest::tearDown()
{
	delete upper;
	delete flowSeparator;
	delete lower;

	delete layer;
} // tearDown


void
GroupFlowSeparatorTest::testIncoming()
{
	CompoundPtr compound(fuNet->createCompound());
	multiplexer::OpcodeCommand* command = opcode->activateCommand(compound->getCommandPool());
	command->peer.opcode = 23;

	CPPUNIT_ASSERT(flowSeparator->size() == 0);
	lower->onData((compound));
	CPPUNIT_ASSERT(flowSeparator->size() == 1);
	CPPUNIT_ASSERT(upper->received.size() == 1);

	lower->onData((compound));
	CPPUNIT_ASSERT(flowSeparator->size() == 1);
	CPPUNIT_ASSERT(upper->received.size() == 2);

	command->peer.opcode = 42;
	lower->onData((compound));
	CPPUNIT_ASSERT(flowSeparator->size() == 2);
	CPPUNIT_ASSERT(upper->received.size() == 3);
} // testIncoming


void
GroupFlowSeparatorTest::testOutgoing()
{
	CompoundPtr compound(fuNet->createCompound());
	multiplexer::OpcodeCommand* command = opcode->activateCommand(compound->getCommandPool());
	command->peer.opcode = 23;

	CPPUNIT_ASSERT(flowSeparator->size() == 0);
	upper->sendData((compound));
	CPPUNIT_ASSERT(flowSeparator->size() == 1);
	CPPUNIT_ASSERT(lower->sent.size() == 1);

	compound = fuNet->createCompound();
	command = opcode->activateCommand(compound->getCommandPool());
	command->peer.opcode = 23;
	upper->sendData((compound));
	CPPUNIT_ASSERT(flowSeparator->size() == 1);
	CPPUNIT_ASSERT(lower->sent.size() == 2);

	compound = fuNet->createCompound();
	command = opcode->activateCommand(compound->getCommandPool());
	command->peer.opcode = 42;
	upper->sendData((compound));
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), flowSeparator->size());
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), lower->sent.size());
} // testOutgoing



