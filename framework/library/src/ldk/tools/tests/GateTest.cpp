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

#include "GateTest.hpp"

#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/buffer/Bounded.hpp>

#include <WNS/pyconfig/Parser.hpp>

#include <iostream>

using namespace wns::ldk::tools;

CPPUNIT_TEST_SUITE_REGISTRATION( GateTest );

void
GateTest::setUp()
{
	layer = new tests::LayerStub();
	fuNet = new fun::Main(layer);

	pyconfig::Parser emptyConfig;
	upper = new Stub(fuNet, emptyConfig);

	{
		wns::pyconfig::Parser pyco;
		pyco.loadString("from openwns.Buffer import Bounded\n"
						"buffer = Bounded(size = 100)\n"
						);
		wns::pyconfig::View view(pyco, "buffer");
		buffer = new buffer::Bounded(fuNet, view);
	}

	{
		pyconfig::Parser all;
		all.loadString(
			"from openwns.Tools import Gate\n"
			"gate = Gate()\n"
			);

		pyconfig::View config(all, "gate");
		gate = new Gate(fuNet, config);
	}
	lower = new Stub(fuNet, emptyConfig);

	fuNet->addFunctionalUnit("ernie", gate);

	upper
		->connect(buffer)
		->connect(gate)
		->connect(lower);
} // setUp


void
GateTest::tearDown()
{
	delete upper;
	delete buffer;
	delete lower;

	delete fuNet;
	delete layer;
} // tearDown


void
GateTest::testOpenOpen()
{
	gate->setOutgoingState(Gate::OPEN);
	gate->setIncomingState(Gate::OPEN);

	CompoundPtr compound(fuNet->createCompound());
	CPPUNIT_ASSERT(gate->isAccepting(compound));

	upper->sendData(compound);
	CPPUNIT_ASSERT_EQUAL(size_t(1), lower->sent.size());
	CPPUNIT_ASSERT(lower->sent[0] == compound);

	lower->onData((compound));
	CPPUNIT_ASSERT_EQUAL(size_t(1), upper->received.size());
	CPPUNIT_ASSERT(upper->received[0] == compound);
} // testOpenOpen


void
GateTest::testOpenClosed()
{
	gate->setOutgoingState(Gate::OPEN);
	gate->setIncomingState(Gate::CLOSED);

	CompoundPtr compound(fuNet->createCompound());
	CPPUNIT_ASSERT(gate->isAccepting(compound));

	upper->sendData(compound);
	CPPUNIT_ASSERT_EQUAL(size_t(1), lower->sent.size());
	CPPUNIT_ASSERT(lower->sent[0] == compound);

	lower->onData((compound));
	CPPUNIT_ASSERT_EQUAL(size_t(0), upper->received.size());
} // testOpenClosed


void
GateTest::testClosedOpen()
{
	gate->setOutgoingState(Gate::CLOSED);
	gate->setIncomingState(Gate::OPEN);

	CompoundPtr compound(fuNet->createCompound());
	CPPUNIT_ASSERT(!gate->isAccepting(compound));

	lower->onData((compound));
	CPPUNIT_ASSERT_EQUAL(size_t(1), upper->received.size());
	CPPUNIT_ASSERT(upper->received[0] == compound);
} // testClosedOpen


void
GateTest::testClosedClosed()
{
	gate->setOutgoingState(Gate::CLOSED);
	gate->setIncomingState(Gate::CLOSED);

	CompoundPtr compound(fuNet->createCompound());
	CPPUNIT_ASSERT(!gate->isAccepting(compound));

	lower->onData((compound));
	CPPUNIT_ASSERT_EQUAL(size_t(0), upper->received.size());
} // testClosedClosed


void
GateTest::testOpening()
{
	gate->setOutgoingState(Gate::CLOSED);
	gate->setIncomingState(Gate::CLOSED);

	CompoundPtr compound(fuNet->createCompound());
	CPPUNIT_ASSERT(!gate->isAccepting(compound));

	upper->sendData(compound);
	CPPUNIT_ASSERT_EQUAL(size_t(0), lower->sent.size());

	gate->setOutgoingState(Gate::OPEN);
	CPPUNIT_ASSERT_EQUAL(size_t(1), lower->sent.size());
	CPPUNIT_ASSERT(lower->sent[0] == compound);
} // testOpening





