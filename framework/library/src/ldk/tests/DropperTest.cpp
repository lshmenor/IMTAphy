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

#include "DropperTest.hpp"

#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/pyconfig/Parser.hpp>

#include <iostream>

using namespace wns::ldk;

CPPUNIT_TEST_SUITE_REGISTRATION( DropperTest );

void
DropperTest::setUp()
{
	layer = new tests::LayerStub();
	fuNet = new fun::Main(layer);

	wns::pyconfig::Parser emptyConfig;
	upper = new tools::Stub(fuNet, emptyConfig);
	dropper = new droppertest::DropItLikeItsHot(fuNet);
	lower = new tools::Stub(fuNet, emptyConfig);

	upper
		->connect(dropper)
		->connect(lower);
} // setUp


void
DropperTest::tearDown()
{
	delete upper;
	delete dropper;
	delete lower;

	delete fuNet;
	delete layer;
} // tearDown


void
DropperTest::testDownlink()
{
	CompoundPtr compound(fuNet->createCompound());
	upper->sendData(compound);
	CPPUNIT_ASSERT(lower->sent.size() == 1);
	CPPUNIT_ASSERT(lower->sent[0] == compound);

	CPPUNIT_ASSERT(dropper->ind == 0);
	CPPUNIT_ASSERT(dropper->req == 1);
	CPPUNIT_ASSERT(dropper->drop == 0);
} // testDownlink


void
DropperTest::testUplink()
{
	CompoundPtr compound(fuNet->createCompound());
	lower->onData(compound);
	CPPUNIT_ASSERT(upper->received.size() == 1);
	CPPUNIT_ASSERT(upper->received[0] == compound);

	CPPUNIT_ASSERT(dropper->ind == 1);
	CPPUNIT_ASSERT(dropper->req == 0);
	CPPUNIT_ASSERT(dropper->drop == 0);
} // testUplink


void
DropperTest::testDrop()
{
	dropper->setDropping(true);

	CompoundPtr compound(fuNet->createCompound());
	upper->sendData(compound);
	CPPUNIT_ASSERT(lower->sent.size() == 0);

	CPPUNIT_ASSERT(dropper->ind == 0);
	CPPUNIT_ASSERT(dropper->req == 0);
	CPPUNIT_ASSERT(dropper->drop == 1);
} // testDrop



