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

#include <WNS/ldk/tools/tests/ProducerTest.hpp>

#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/ldk/fun/Main.hpp>

#include <iostream>


using namespace wns::ldk::tools;


CPPUNIT_TEST_SUITE_REGISTRATION( ProducerTest );

void
ProducerTest::setUp()
{
	wns::pyconfig::Parser pyco;
	pyco.loadString("size = 2\n"
					  );
	layer = new tests::LayerStub();
	fuNet = new fun::Main(layer);

	wns::pyconfig::Parser emptyConfig;
	upper = new Stub(fuNet, emptyConfig);
	producer = new Producer(fuNet);
	lower = new Stub(fuNet, emptyConfig);
	lower->setStepping(true);
	upper
		->connect(producer)
		->connect(lower);
} // setUp

void
ProducerTest::tearDown()
{
	delete upper;
	delete producer;
	delete lower;

	delete fuNet;
	delete layer;
} // tearDown


void
ProducerTest::testDownlink()
{
	CPPUNIT_ASSERT(lower->sent.size() == 0);

	// to start processing, we have to wakeup someone.
	// producer is a good choice here, because we know he has
	// something to send.
	// since lower layer is in stepping mode, only one PDU
	// should make it to the lower layer...
	producer->wakeup();
	CPPUNIT_ASSERT(lower->sent.size() == 1);

	// stepping opens makes the lower layer accept another PDU...
	lower->step();
	CPPUNIT_ASSERT(lower->sent.size() == 2);

	lower->step();
	CPPUNIT_ASSERT(lower->sent.size() == 3);
} // testProxy


void
ProducerTest::testUplink()
{
	CompoundPtr compound(fuNet->createCompound());

	// Producer should forward incoming compounds.
	// as a side effect, it tries to send compounds to its lower layers
	// (successfully only once, since the lower layer is in stepping mode.)
	lower->onData((compound));
	CPPUNIT_ASSERT(upper->received.size() == 1);
	CPPUNIT_ASSERT(lower->sent.size() == 1);

	lower->onData((compound));
	CPPUNIT_ASSERT(upper->received.size() == 2);
	CPPUNIT_ASSERT(lower->sent.size() == 1);
} // testUplink



