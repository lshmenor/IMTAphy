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

#include "ConsumerTest.hpp"

#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/ldk/fun/Main.hpp>

using namespace wns::ldk::tools;


CPPUNIT_TEST_SUITE_REGISTRATION( ConsumerTest );

void
ConsumerTest::setUp()
{
	layer = new tests::LayerStub();
	fuNet = new fun::Main(layer);

	wns::pyconfig::Parser emptyConfig;
	upper = new Stub(fuNet, emptyConfig);
	consumer = new Consumer(fuNet);
	lower = new Stub(fuNet, emptyConfig);
	lower->setStepping(true);
	upper
		->connect(consumer)
		->connect(lower);
} // setUp

void
ConsumerTest::tearDown()
{
	delete upper;
	delete consumer;
	delete lower;

	delete fuNet;
	delete layer;
} // tearDown


void
ConsumerTest::testDownlink()
{
	CompoundPtr compound(fuNet->createCompound());

	consumer->consumeOutgoing();

	upper->sendData((compound));
	CPPUNIT_ASSERT(lower->sent.size() == 0);
	CPPUNIT_ASSERT(upper->received.size() == 0);

	lower->onData((compound));
	CPPUNIT_ASSERT(lower->sent.size() == 0);
	CPPUNIT_ASSERT(upper->received.size() == 1);
} // testProxy


void
ConsumerTest::testUplink()
{
	CompoundPtr compound(fuNet->createCompound());

	consumer->consumeIncoming();

	upper->sendData((compound));
	CPPUNIT_ASSERT(lower->sent.size() == 1);
	CPPUNIT_ASSERT(upper->received.size() == 0);

	lower->onData((compound));
	CPPUNIT_ASSERT(lower->sent.size() == 1);
	CPPUNIT_ASSERT(upper->received.size() == 0);
} // testUplink


void
ConsumerTest::testBlackHole()
{
	CompoundPtr compound(fuNet->createCompound());

	consumer->consumeIncoming();
	consumer->consumeOutgoing();

	upper->sendData((compound));
	CPPUNIT_ASSERT(lower->sent.size() == 0);
	CPPUNIT_ASSERT(upper->received.size() == 0);

	lower->onData((compound));
	CPPUNIT_ASSERT(lower->sent.size() == 0);
	CPPUNIT_ASSERT(upper->received.size() == 0);
} // testBlackHole



