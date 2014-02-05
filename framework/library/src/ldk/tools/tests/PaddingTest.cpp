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

#include "PaddingTest.hpp"

#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/ldk/fun/Main.hpp>

#include <WNS/pyconfig/Parser.hpp>

#include <iostream>

using namespace wns::ldk::tools;

CPPUNIT_TEST_SUITE_REGISTRATION( PaddingTest );

void
PaddingTest::setUp()
{
	layer = new tests::LayerStub();
	fuNet = new fun::Main(layer);

	pyconfig::Parser emptyConfig;
	upper = new Stub(fuNet, emptyConfig);

	pyconfig::Parser padderConfig;
	padderConfig.loadString("size = 42");
	padder = new Padding(fuNet, padderConfig);
	lower = new Stub(fuNet, emptyConfig);

	fuNet->addFunctionalUnit("la", upper);
	fuNet->addFunctionalUnit("le", padder);
	fuNet->addFunctionalUnit("lu", lower);

	upper
		->connect(padder)
		->connect(lower);
} // setUp


void
PaddingTest::tearDown()
{
	delete fuNet;
	delete layer;
} // tearDown


void
PaddingTest::testDownlink()
{
	CompoundPtr compound(fuNet->createCompound());
	upper->sendData(compound);
	CPPUNIT_ASSERT(lower->sent.size() == 1);
	CPPUNIT_ASSERT(lower->sent[0] == compound);

	CPPUNIT_ASSERT(compound->getLengthInBits() == 42);
	Bit commandPoolSize;
	Bit dataSize;
	fuNet->calculateSizes(compound->getCommandPool(), commandPoolSize, dataSize);
	CPPUNIT_ASSERT_EQUAL(Bit(0), commandPoolSize);
	CPPUNIT_ASSERT_EQUAL(Bit(42), dataSize);
} // testDownlink


void
PaddingTest::testUplink()
{
	CompoundPtr compound(fuNet->createCompound());
	lower->onData(compound);
	CPPUNIT_ASSERT(upper->received.size() == 1);
	CPPUNIT_ASSERT(upper->received[0] == compound);
	CPPUNIT_ASSERT(compound->getLengthInBits() == 0);
} // testUplink


void
PaddingTest::testPad()
{
	upper->setSizes(21, 20);

	CompoundPtr compound(fuNet->createCompound());
	upper->sendData(compound);
	CPPUNIT_ASSERT(lower->sent.size() == 1);
	CPPUNIT_ASSERT(lower->sent[0] == compound);

	CPPUNIT_ASSERT_EQUAL(Bit(42), compound->getLengthInBits());
	Bit commandPoolSize;
	Bit dataSize;
	fuNet->calculateSizes(compound->getCommandPool(), commandPoolSize, dataSize);
	CPPUNIT_ASSERT_EQUAL(Bit(21), commandPoolSize);
	CPPUNIT_ASSERT_EQUAL(Bit(21), dataSize);
} // testPad

void
PaddingTest::testNoPad()
{
	upper->setSizes(21, 22);

	CompoundPtr compound(fuNet->createCompound());
	upper->sendData(compound);
	CPPUNIT_ASSERT(lower->sent.size() == 1);
	CPPUNIT_ASSERT(lower->sent[0] == compound);

	CPPUNIT_ASSERT_EQUAL(Bit(43), compound->getLengthInBits());
	Bit commandPoolSize;
	Bit dataSize;
	fuNet->calculateSizes(compound->getCommandPool(), commandPoolSize, dataSize);
	CPPUNIT_ASSERT_EQUAL(Bit(21), commandPoolSize);
	CPPUNIT_ASSERT_EQUAL(Bit(22), dataSize);
} //testNoPad


void
PaddingTest::testExact()
{
	upper->setSizes(21, 21);

	CompoundPtr compound(fuNet->createCompound());
	upper->sendData(compound);
	CPPUNIT_ASSERT(lower->sent.size() == 1);
	CPPUNIT_ASSERT(lower->sent[0] == compound);

	CPPUNIT_ASSERT(compound->getLengthInBits() == 42);
	Bit commandPoolSize;
	Bit dataSize;
	fuNet->calculateSizes(compound->getCommandPool(), commandPoolSize, dataSize);
	CPPUNIT_ASSERT_EQUAL(Bit(21), commandPoolSize);
	CPPUNIT_ASSERT_EQUAL(Bit(21), dataSize);
} // testExact


