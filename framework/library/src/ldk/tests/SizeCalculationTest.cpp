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


#include "SizeCalculationTest.hpp"

#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/helper/FakePDU.hpp>
#include <WNS/pyconfig/Parser.hpp>

#include <iostream>

using namespace wns::ldk;

CPPUNIT_TEST_SUITE_REGISTRATION( SizeCalculationTest );

void
SizeCalculationTest::setUp()
{
	layer = new tests::LayerStub();
	fuNet = new fun::Main(layer);

	pyconfig::Parser emptyConfig;
	upper = new tools::Stub(fuNet, emptyConfig);
	lower = new tools::Stub(fuNet, emptyConfig);
} // setUp


void
SizeCalculationTest::tearDown()
{
	delete lower;
	delete upper;

	delete fuNet;
	delete layer;
} // tearDown


void
SizeCalculationTest::testEmpty()
{
	Bit commandPoolSize;
	Bit dataSize;

	CompoundPtr compound(fuNet->createCompound());
	CommandPool* commandPool = compound->getCommandPool();

	commandPool->calculateSizes(commandPoolSize, dataSize);
	CPPUNIT_ASSERT(commandPoolSize == 0);
	CPPUNIT_ASSERT(dataSize == 0);
} // testEmpty


void
SizeCalculationTest::testVanilla()
{
	Bit commandPoolSize;
	Bit dataSize;

	helper::FakePDUPtr inner(new  helper::FakePDU(42));

	CompoundPtr compound(fuNet->createCompound(inner));
	CommandPool* commandPool = compound->getCommandPool();

	commandPool->calculateSizes(commandPoolSize, dataSize);
	CPPUNIT_ASSERT(commandPoolSize == 0);
	CPPUNIT_ASSERT(dataSize == 42);
} // testVanilla


void
SizeCalculationTest::testInPath()
{
	Bit commandPoolSize;
	Bit dataSize;

	helper::FakePDUPtr inner(new helper::FakePDU(42));

	CompoundPtr compound(fuNet->createCompound(inner));
	CommandPool* commandPool = compound->getCommandPool();

	// this should raise an exception, since upper is not in the path
	commandPool->calculateSizes(commandPoolSize, dataSize, upper);
} // testInPath



