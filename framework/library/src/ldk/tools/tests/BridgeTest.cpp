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

#include "BridgeTest.hpp"

#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/pyconfig/Parser.hpp>

#include <iostream>


using namespace wns::ldk::tools;


CPPUNIT_TEST_SUITE_REGISTRATION( BridgeTest );

void
BridgeTest::setUp()
{
	layerA = new tests::LayerStub();
	funA = new fun::Main(layerA);
	layerB = new tests::LayerStub();
	funB = new fun::Main(layerB);

	pyconfig::Parser emptyConfig;
	a = new Stub(funA, emptyConfig);
	b = new Stub(funB, emptyConfig);
	bridge = new Bridge(funA, funB);
} // setUp


void
BridgeTest::tearDown()
{
	delete a;
	delete b;
	delete bridge;
	delete funA;
	delete funB;
	delete layerA;
	delete layerB;
} // tearDown


void
BridgeTest::testFromBelow()
{
	bridge->getLeft()->connect(a);
	bridge->getRight()->connect(b);

	CPPUNIT_ASSERT(a->sent.size() == 0);
	CPPUNIT_ASSERT(b->sent.size() == 0);

	a->onData(funA->createCompound());
	CPPUNIT_ASSERT(a->sent.size() == 0);
	CPPUNIT_ASSERT(b->sent.size() == 1);

	a->onData(funA->createCompound());
	CPPUNIT_ASSERT(a->sent.size() == 0);
	CPPUNIT_ASSERT(b->sent.size() == 2);

	b->onData(funB->createCompound());
	CPPUNIT_ASSERT(a->sent.size() == 1);
	CPPUNIT_ASSERT(b->sent.size() == 2);
} // testFromBelow


void
BridgeTest::testFromAbove()
{
	a->connect(bridge->getLeft());
	b->connect(bridge->getRight());

	CPPUNIT_ASSERT(a->received.size() == 0);
	CPPUNIT_ASSERT(b->received.size() == 0);

	a->sendData(funA->createCompound());
	CPPUNIT_ASSERT(a->received.size() == 0);
	CPPUNIT_ASSERT(b->received.size() == 1);

	a->sendData(funA->createCompound());
	CPPUNIT_ASSERT(a->received.size() == 0);
	CPPUNIT_ASSERT(b->received.size() == 2);

	b->sendData(funB->createCompound());
	CPPUNIT_ASSERT(a->received.size() == 1);
	CPPUNIT_ASSERT(b->received.size() == 2);
} // testFromAbove


void
BridgeTest::testLossyFromBelow()
{
	const int N = 1000;

	bridge->setLoss(0.5);
	bridge->getLeft()->connect(a);
	bridge->getRight()->connect(b);

	CPPUNIT_ASSERT(a->sent.size() == 0);
	CPPUNIT_ASSERT(b->sent.size() == 0);

	int n = 0;
	for(int i = 0; i < N; i++)
	{
		a->onData(funA->createCompound());
		if(b->sent.size() == 1)
			++n;
		b->flush();
	}

	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5, n * 1.0 / N, 0.1);
} // testFromBelow


void
BridgeTest::testLossyFromAbove()
{
	const int N = 1000;

	bridge->setLoss(0.5);
	a->connect(bridge->getLeft());
	b->connect(bridge->getRight());

	CPPUNIT_ASSERT(a->received.size() == 0);
	CPPUNIT_ASSERT(b->received.size() == 0);

	int n = 0;
	for(int i = 0; i < N; i++) {
		a->sendData(funA->createCompound());
		if(b->received.size() == 1)
			++n;
		b->flush();
	}

	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5, n * 1.0 / N, 0.1);
} // testFromAbove



