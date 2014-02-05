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

#include "FlowSeparatorTest.hpp"

#include <WNS/ldk/arq/StopAndWait.hpp>

#include <WNS/ldk/fun/Main.hpp>

#include <iostream>

using namespace wns::ldk;
using namespace flowseparatortest;


CPPUNIT_TEST_SUITE_REGISTRATION( FlowSeparatorTest );

//
// MyKeyBuilder
//


STATIC_FACTORY_REGISTER_WITH_CREATOR(MyKeyBuilder, KeyBuilder, "flowseparatortest.MyKeyBuilder", FUNConfigCreator);

MyKeyBuilder::MyKeyBuilder(const fun::FUN* fuNet, const pyconfig::View& /* config */)
{
	assure(dynamic_cast<const MyLayer*>(fuNet->getLayer()), "wrong layer type given in MyKey constructor");
	layer = static_cast<const MyLayer*>(fuNet->getLayer());
} // MyKeyBuilder


void
MyKeyBuilder::onFUNCreated()
{
	friends.prototype = layer->prototype;
} // onFUNCreated


//
// MyKey
//

MyKey::MyKey(const MyKeyBuilder* factory, const CompoundPtr& compound, int /* direction */)
{
	CommandPool* commandPool = compound->getCommandPool();
	MyCommand* command = factory->friends.prototype->getCommand(commandPool);
	flow = command->local.flow;
} // MyKey


//
// MyFunctionalUnit
//

void
MyFunctionalUnit::processIncoming(const CompoundPtr& compound)
{
	++ind;

	getDeliverer()->getAcceptor(compound)->onData(compound);
} // processIncoming


void
MyFunctionalUnit::processOutgoing(const CompoundPtr& compound)
{
	assure(hasCapacity(), "received PDU although not accepting.");

	++req;

	buffer = compound;
} // processOutgoing


bool
MyFunctionalUnit::hasCapacity() const
{
	return buffer == CompoundPtr();
} // hasCapacity


const CompoundPtr
MyFunctionalUnit::hasSomethingToSend() const
{
	return buffer;
} // hasSomethingToSend


CompoundPtr
MyFunctionalUnit::getSomethingToSend()
{
	CompoundPtr it;
	it = buffer;
	buffer = CompoundPtr();
	return it;
} // getSomethingToSend


//
// FlowSeparatorTest
//

void
FlowSeparatorTest::setUp()
{
	wns::ldk::CommandProxy::clearRegistries();

	wns::pyconfig::Parser emptyConfig;
	wns::pyconfig::Parser config;
	config.loadString(
		"import openwns.logger\n"
		"logger = openwns.logger.Logger(\n"
		"    'WNS',\n"
		"    'FlowSeparator',\n"
		"    True\n"
		"    )\n"
		);

	layer = new MyLayer();
	fuNet = new fun::Main(layer);

	upper = new tools::Stub(fuNet, emptyConfig);

	layer->prototype = new MyFunctionalUnit(fuNet, emptyConfig);
	this->factory = new flowseparatortest::MyKeyBuilder(fuNet, emptyConfig);

	// will be deleted by FlowSeparator
	std::auto_ptr<wns::ldk::KeyBuilder> keyBuilder(this->factory);

	creator =
		new flowseparator::PrototypeCreator(fuNet, "ernie", layer->prototype);


	flowSeparator = new FlowSeparator(fuNet,
					  config,
					  keyBuilder,
					  std::auto_ptr<flowseparator::NotFoundStrategy>(new flowseparator::CreateOnFirstCompound(creator)));

	lower = new tools::Stub(fuNet, emptyConfig);
	lower2 = new tools::Stub(fuNet, emptyConfig);

	upper
		->connect(flowSeparator)
		->connect(lower);

	fip = new MyFlowInfoProvider(layer->getCSR());
	layer->addControlService("fip", fip);

	flowSeparator->onFUNCreated();
} // setUp


void
FlowSeparatorTest::tearDown()
{
	delete upper;
	delete flowSeparator;
	delete lower;

	delete layer;
} // tearDown


void
FlowSeparatorTest::testIncoming()
{
	CompoundPtr compound(fuNet->createCompound());
	MyCommand* command = layer->prototype->activateCommand(compound->getCommandPool());
	command->local.flow = 23;

	CPPUNIT_ASSERT(flowSeparator->size() == 0);
	lower->onData((compound));
	CPPUNIT_ASSERT(flowSeparator->size() == 1);
	CPPUNIT_ASSERT(upper->received.size() == 1);

	lower->onData((compound));
	CPPUNIT_ASSERT(flowSeparator->size() == 1);
	CPPUNIT_ASSERT(upper->received.size() == 2);

	command->local.flow = 42;
	lower->onData((compound));
	CPPUNIT_ASSERT(flowSeparator->size() == 2);
	CPPUNIT_ASSERT(upper->received.size() == 3);
} // testIncoming


void
FlowSeparatorTest::testOutgoing()
{
	CompoundPtr compound(fuNet->createCompound());
	MyCommand* command = layer->prototype->activateCommand(compound->getCommandPool());
	command->local.flow = 23;

	CPPUNIT_ASSERT(flowSeparator->size() == 0);
	upper->sendData((compound));
	CPPUNIT_ASSERT(flowSeparator->size() == 1);
	CPPUNIT_ASSERT(lower->sent.size() == 1);

	upper->sendData((compound));
	CPPUNIT_ASSERT(flowSeparator->size() == 1);
	CPPUNIT_ASSERT(lower->sent.size() == 2);

	command->local.flow = 42;
	upper->sendData((compound));
	CPPUNIT_ASSERT(flowSeparator->size() == 2);
	CPPUNIT_ASSERT(lower->sent.size() == 3);
} // testOutgoing


void
FlowSeparatorTest::testMixed()
{
	CompoundPtr compound(fuNet->createCompound());
	MyCommand* command = layer->prototype->activateCommand(compound->getCommandPool());
	command->local.flow = 23;

	CPPUNIT_ASSERT(flowSeparator->size() == 0);
	upper->sendData((compound));
	CPPUNIT_ASSERT(flowSeparator->size() == 1);
	CPPUNIT_ASSERT(lower->sent.size() == 1);
	CPPUNIT_ASSERT(upper->received.size() == 0);

	lower->onData((compound));
	CPPUNIT_ASSERT(flowSeparator->size() == 1);
	CPPUNIT_ASSERT(lower->sent.size() == 1);
	CPPUNIT_ASSERT(upper->received.size() == 1);

	command->local.flow = 42;
	upper->sendData((compound));
	CPPUNIT_ASSERT(flowSeparator->size() == 2);
	CPPUNIT_ASSERT(lower->sent.size() == 2);
	CPPUNIT_ASSERT(upper->received.size() == 1);

	lower->onData((compound));
	CPPUNIT_ASSERT(flowSeparator->size() == 2);
	CPPUNIT_ASSERT(lower->sent.size() == 2);
	CPPUNIT_ASSERT(upper->received.size() == 2);
} // testMixed


void
FlowSeparatorTest::testGetInstancePerKey()
{
	CompoundPtr compound(fuNet->createCompound());
	MyCommand* command = layer->prototype->activateCommand(compound->getCommandPool());
	command->local.flow = 23;

	KeyPtr key(new MyKey(factory, compound, 0));
	CPPUNIT_ASSERT(flowSeparator->getInstance(key) == NULL);

	upper->sendData((compound));
	CPPUNIT_ASSERT(flowSeparator->getInstance(key) != NULL);
	CPPUNIT_ASSERT(dynamic_cast<MyFunctionalUnit*>(flowSeparator->getInstance(key))->ind == 0);
	CPPUNIT_ASSERT(dynamic_cast<MyFunctionalUnit*>(flowSeparator->getInstance(key))->req == 1);

	lower->onData((compound));
	CPPUNIT_ASSERT(dynamic_cast<MyFunctionalUnit*>(flowSeparator->getInstance(key))->ind == 1);
	CPPUNIT_ASSERT(dynamic_cast<MyFunctionalUnit*>(flowSeparator->getInstance(key))->req == 1);

	command->local.flow = 42;
	key = KeyPtr(new MyKey(factory, compound));
	CPPUNIT_ASSERT(flowSeparator->getInstance(key) == NULL);

	upper->sendData((compound));
	CPPUNIT_ASSERT(flowSeparator->getInstance(key) != NULL);
	CPPUNIT_ASSERT(dynamic_cast<MyFunctionalUnit*>(flowSeparator->getInstance(key))->ind == 0);
	CPPUNIT_ASSERT(dynamic_cast<MyFunctionalUnit*>(flowSeparator->getInstance(key))->req == 1);

	lower->onData((compound));
	CPPUNIT_ASSERT(dynamic_cast<MyFunctionalUnit*>(flowSeparator->getInstance(key))->ind == 1);
	CPPUNIT_ASSERT(dynamic_cast<MyFunctionalUnit*>(flowSeparator->getInstance(key))->req == 1);
} // testGetInstance

void
FlowSeparatorTest::testGetInstancePerCompound()
{
	CompoundPtr compound(fuNet->createCompound());
	MyCommand* command = layer->prototype->activateCommand(compound->getCommandPool());
	command->local.flow = 23;

	CPPUNIT_ASSERT(flowSeparator->getInstance(compound, Direction::INCOMING()) == NULL);

 	upper->sendData((compound));
 	CPPUNIT_ASSERT(flowSeparator->getInstance(compound, Direction::INCOMING()) != NULL);
 	CPPUNIT_ASSERT(dynamic_cast<MyFunctionalUnit*>(flowSeparator->getInstance(compound, Direction::INCOMING()))->ind == 0);
	CPPUNIT_ASSERT(dynamic_cast<MyFunctionalUnit*>(flowSeparator->getInstance(compound, Direction::INCOMING()))->req == 1);

	lower->onData((compound));
	CPPUNIT_ASSERT(dynamic_cast<MyFunctionalUnit*>(flowSeparator->getInstance(compound, Direction::INCOMING()))->ind == 1);
	CPPUNIT_ASSERT(dynamic_cast<MyFunctionalUnit*>(flowSeparator->getInstance(compound, Direction::INCOMING()))->req == 1);

	command->local.flow = 42;
	CPPUNIT_ASSERT(flowSeparator->getInstance(compound, Direction::INCOMING()) == NULL);

	upper->sendData((compound));
	CPPUNIT_ASSERT(flowSeparator->getInstance(compound, Direction::INCOMING()) != NULL);
	CPPUNIT_ASSERT(dynamic_cast<MyFunctionalUnit*>(flowSeparator->getInstance(compound, Direction::INCOMING()))->ind == 0);
	CPPUNIT_ASSERT(dynamic_cast<MyFunctionalUnit*>(flowSeparator->getInstance(compound, Direction::INCOMING()))->req == 1);

	lower->onData((compound));
	CPPUNIT_ASSERT(dynamic_cast<MyFunctionalUnit*>(flowSeparator->getInstance(compound, Direction::INCOMING()))->ind == 1);
	CPPUNIT_ASSERT(dynamic_cast<MyFunctionalUnit*>(flowSeparator->getInstance(compound, Direction::INCOMING()))->req == 1);
} // testGetInstance

void
FlowSeparatorTest::testAddInstance()
{
	CPPUNIT_ASSERT(flowSeparator->size() == 0);
	FunctionalUnit* fu = dynamic_cast<FunctionalUnit*>(layer->prototype->clone());
	KeyPtr key(new MyKey(23));

	flowSeparator->addInstance(key, fu);
	CPPUNIT_ASSERT(flowSeparator->size() == 1);
	CPPUNIT_ASSERT(flowSeparator->getInstance(key) == fu);

	CompoundPtr compound(fuNet->createCompound());
	MyCommand* command = layer->prototype->activateCommand(compound->getCommandPool());
	command->local.flow = 23;
	upper->sendData(compound);
	CPPUNIT_ASSERT(flowSeparator->size() == 1);

	command->local.flow = 42;
	upper->sendData(compound);
	CPPUNIT_ASSERT(flowSeparator->size() == 2);
} // testAddInstance


void
FlowSeparatorTest::testRemoveInstance()
{
	CPPUNIT_ASSERT(flowSeparator->size() == 0);
	FunctionalUnit* fu = dynamic_cast<FunctionalUnit*>(layer->prototype->clone());
	KeyPtr key(new MyKey(23));

	flowSeparator->addInstance(key, fu);
	CPPUNIT_ASSERT(flowSeparator->size() == 1);

	flowSeparator->removeInstance(key);
	CPPUNIT_ASSERT(flowSeparator->size() == 0);
} // testRemoveInstance


void
FlowSeparatorTest::testSingleInstanceFlowControl()
{
	CompoundPtr compound(fuNet->createCompound());
	MyCommand* command = layer->prototype->activateCommand(compound->getCommandPool());
	command->local.flow = 23;

	CPPUNIT_ASSERT(flowSeparator->isAccepting(compound));
	lower->close();
	CPPUNIT_ASSERT(flowSeparator->isAccepting(compound));

	CPPUNIT_ASSERT(upper->isAccepting(compound));
	upper->sendData((compound));
	CPPUNIT_ASSERT(!upper->isAccepting(compound));

	CPPUNIT_ASSERT(lower->sent.size() == 0);
	lower->open();
	CPPUNIT_ASSERT(lower->sent.size() == 1);

	CPPUNIT_ASSERT(upper->isAccepting(compound));
	upper->sendData((compound));
	CPPUNIT_ASSERT(lower->sent.size() == 2);
} // testSingleInstanceFlowControl()


void
FlowSeparatorTest::testMultiInstanceFlowControl()
{
	CompoundPtr compound1(fuNet->createCompound());
	MyCommand* command1 = layer->prototype->activateCommand(compound1->getCommandPool());
	command1->local.flow = 23;

	CompoundPtr compound2(fuNet->createCompound());
	MyCommand* command2 = layer->prototype->activateCommand(compound2->getCommandPool());
	command2->local.flow = 42;

	lower->close();
	CPPUNIT_ASSERT(flowSeparator->isAccepting(compound1));
	CPPUNIT_ASSERT(flowSeparator->isAccepting(compound2));

	upper->sendData((compound1));
	CPPUNIT_ASSERT(!flowSeparator->isAccepting(compound1));
	CPPUNIT_ASSERT(flowSeparator->isAccepting(compound2));

	upper->sendData((compound2));
	CPPUNIT_ASSERT(!flowSeparator->isAccepting(compound1));
	CPPUNIT_ASSERT(!flowSeparator->isAccepting(compound2));

	lower->open();
	CPPUNIT_ASSERT(flowSeparator->isAccepting(compound1));
	CPPUNIT_ASSERT(flowSeparator->isAccepting(compound2));
	CPPUNIT_ASSERT(lower->sent.size() == 2);
} // testMultiInstanceFlowControl()


void
FlowSeparatorTest::testLayerConfig()
{
	wns::pyconfig::Parser config;
	config.loadString(
		"import openwns.ARQ\n"
		"from openwns.logger import Logger\n"
		"logger = Logger('WNS', 'FlowSeparator', True)\n"
		"class notFound:\n"
		"    __plugin__ = 'createonfirstcompound'\n"
		"    class creator:\n"
		"        __plugin__ = 'prototypecreator'\n"
		"        prototypeConfig = openwns.ARQ.StopAndWait()\n"
		"        name = 'Helga'\n"
		"        fuName = 'Helga'\n"
		"class keyBuilder:\n"
		"    __plugin__ = 'flowseparatortest.MyKeyBuilder'\n"
		);

	FlowSeparator(fuNet, config);
	const arq::ARQ* prototype = fuNet->findFriend<arq::ARQ*>("Helga");
	CPPUNIT_ASSERT(prototype);
} // testLayerConfig


void
FlowSeparatorTest::testPurePython()
{
	wns::pyconfig::Parser config;
	config.loadString(
		"import openwns.FlowSeparator\n"
		"import openwns.ARQ\n"
		"class TestBuilder:\n"
		"    __plugin__ = 'flowseparatortest.MyKeyBuilder'\n"
		"seppel = openwns.FlowSeparator.FlowSeparator(\n"
		"    TestBuilder(),\n"
		"    openwns.FlowSeparator.PrototypeCreator(\n"
		"        'Helga',\n"
		"        openwns.ARQ.StopAndWait())\n"
		"    )\n"
		);

	wns::pyconfig::View layerConfig(config, "seppel");
	std::string layerName = layerConfig.get<std::string>("__plugin__");
	FunctionalUnit* it = FunctionalUnitFactory::creator(layerName)->create(fuNet, layerConfig);

	FlowSeparator* seppel = dynamic_cast<FlowSeparator*>(it);
	CPPUNIT_ASSERT(seppel);

	const arq::ARQ* prototype = fuNet->findFriend<arq::ARQ*>("Helga");
	CPPUNIT_ASSERT(prototype);
} // testPurePython


void
FlowSeparatorTest::testComplain()
{
	wns::pyconfig::Parser config;
	config.loadString(
		"import openwns.FlowSeparator\n"
		"class TestBuilder:\n"
		"    __plugin__ = 'flowseparatortest.MyKeyBuilder'\n"
		"seppel = openwns.FlowSeparator.FlowSeparator(\n"
		"    TestBuilder(),\n"
		"    openwns.FlowSeparator.Complain()\n"
		"    )\n"
		);

	wns::pyconfig::View layerConfig(config, "seppel");
	std::string layerName = layerConfig.get<std::string>("__plugin__");
	FunctionalUnit* it = FunctionalUnitFactory::creator(layerName)->create(fuNet, layerConfig);

	FlowSeparator* flowSeparator = dynamic_cast<FlowSeparator*>(it);
	CPPUNIT_ASSERT(flowSeparator != NULL);
	fuNet->addFunctionalUnit("sepel", flowSeparator);
	flowSeparator->onFUNCreated();

	flowSeparator->connect(lower2);

	CPPUNIT_ASSERT(flowSeparator->size() == 0);
	FunctionalUnit* fu = dynamic_cast<FunctionalUnit*>(layer->prototype->clone());
	KeyPtr key(new MyKey(23));

	flowSeparator->addInstance(key, fu);

	CompoundPtr compound(fuNet->createCompound());
	MyCommand* command = layer->prototype->activateCommand(compound->getCommandPool());
	command->local.flow = 23;
	flowSeparator->sendData(compound);
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), lower2->sent.size());
} // testComplain


void
FlowSeparatorTest::testComplainComplain()
{
	wns::pyconfig::Parser config;
	config.loadString(
		"import openwns.FlowSeparator\n"
		"class TestBuilder:\n"
		"    __plugin__ = 'flowseparatortest.MyKeyBuilder'\n"
		"seppel = openwns.FlowSeparator.FlowSeparator(\n"
		"    TestBuilder(),\n"
		"    openwns.FlowSeparator.Complain()\n"
		"    )\n"
		);

	wns::pyconfig::View layerConfig(config, "seppel");
	std::string layerName = layerConfig.get<std::string>("__plugin__");
	FunctionalUnit* it = FunctionalUnitFactory::creator(layerName)->create(fuNet, layerConfig);

	FlowSeparator* flowSeparator = dynamic_cast<FlowSeparator*>(it);
	CPPUNIT_ASSERT(flowSeparator != NULL);
	fuNet->addFunctionalUnit("sepel", flowSeparator);
	flowSeparator->onFUNCreated();

	flowSeparator->connect(lower2);

	CompoundPtr compound(fuNet->createCompound());
	MyCommand* command = layer->prototype->activateCommand(compound->getCommandPool());
	command->local.flow = 23;

	CPPUNIT_ASSERT_THROW(flowSeparator->sendData(compound), wns::Exception); // BANG
} // testComplainComplain

void
FlowSeparatorTest::testCreateOnValidFlow()
{
	wns::pyconfig::Parser config;
	config.loadString(
		"import openwns.Tools\n"
		"import openwns.FlowSeparator\n"
		"class TestBuilder:\n"
		"    __plugin__ = 'flowseparatortest.MyKeyBuilder'\n"
		"creator = openwns.FlowSeparator.Prototype('nix',openwns.Tools.Forwarder())\n"
		"seppel = openwns.FlowSeparator.FlowSeparator(\n"
		"    TestBuilder(),\n"
		"    openwns.FlowSeparator.CreateOnValidFlow(creator,'fip')\n"
		"    )\n"
		);

	wns::pyconfig::View layerConfig(config, "seppel");
	std::string layerName = layerConfig.get<std::string>("__plugin__");
	FunctionalUnit* it = FunctionalUnitFactory::creator(layerName)->create(fuNet, layerConfig);

	FlowSeparator* flowSeparator = dynamic_cast<FlowSeparator*>(it);
	CPPUNIT_ASSERT(flowSeparator != NULL);
	fuNet->addFunctionalUnit("sepel", flowSeparator);

	flowSeparator->onFUNCreated();

	flowSeparator->connect(lower2);

	CPPUNIT_ASSERT(flowSeparator->size() == 0);

	CompoundPtr compound(fuNet->createCompound());
	MyCommand* command = layer->prototype->activateCommand(compound->getCommandPool());
	command->local.flow = 23;

	// deny all flows --> must complain
	fip->setValid(false);
	CPPUNIT_ASSERT_THROW( flowSeparator->isAccepting(compound), wns::Exception);

	// accept all flows --> create instance
	fip->setValid(true);
	CPPUNIT_ASSERT_NO_THROW( flowSeparator->isAccepting(compound));
	CPPUNIT_ASSERT_NO_THROW( flowSeparator->sendData(compound));
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), lower2->sent.size());

	CompoundPtr compound2(fuNet->createCompound());
	command = layer->prototype->activateCommand(compound2->getCommandPool());
	command->local.flow = 42;

	// set fip to deny --> must complain for new flows but admit existing ones
	fip->setValid(false);
	CPPUNIT_ASSERT_NO_THROW( flowSeparator->isAccepting(compound) );
	CPPUNIT_ASSERT_THROW( flowSeparator->isAccepting(compound2), wns::Exception);

} // testComplain


