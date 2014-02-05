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

#include "RoundRobinConnectorTest.hpp"

#include <WNS/ldk/tools/Stub.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>

#include <WNS/pyconfig/Parser.hpp>

using namespace wns::ldk;
using namespace wns::ldk::tests;

CPPUNIT_TEST_SUITE_REGISTRATION( RoundRobinConnectorTest );

void
RoundRobinConnectorTest::prepare()
{
	pyconfig::Parser emptyConfig;

	l = new wns::ldk::tests::LayerStub();
	fuNet = new wns::ldk::fun::Main(l);
	s1 = new tools::Stub(fuNet, emptyConfig);
	s2 = new tools::Stub(fuNet, emptyConfig);
	s3 = new tools::Stub(fuNet, emptyConfig);
} // setUp


void
RoundRobinConnectorTest::cleanup()
{
	delete s1;
	delete s2;
	delete s3;
	delete fuNet;
	delete l;
} // tearDown


void
RoundRobinConnectorTest::testFill()
{
	RoundRobinConnector rr;

	CPPUNIT_ASSERT(0 == rr.size());
	rr.add(s1);
	rr.add(s2);
	rr.add(s3);
	CPPUNIT_ASSERT(3 == rr.size());
} // testFill


void
RoundRobinConnectorTest::testRR()
{
	RoundRobinConnector rr;
	CompoundPtr compound;

	rr.add(s1);
	rr.add(s2);
	rr.add(s3);

	CPPUNIT_ASSERT(rr.hasAcceptor(compound));

	CPPUNIT_ASSERT(s1 == rr.getAcceptor(compound));
	CPPUNIT_ASSERT(s2 == rr.getAcceptor(compound));
	CPPUNIT_ASSERT(s3 == rr.getAcceptor(compound));
	CPPUNIT_ASSERT(s1 == rr.getAcceptor(compound));
} // testRR


void
RoundRobinConnectorTest::testAccepting()
{
	RoundRobinConnector rr;
	CompoundPtr compound;

	rr.add(s1);
	rr.add(s2);
	rr.add(s3);

	s2->close();
	CPPUNIT_ASSERT(s1 == rr.getAcceptor(compound));
	CPPUNIT_ASSERT(s3 == rr.getAcceptor(compound));
	CPPUNIT_ASSERT(s1 == rr.getAcceptor(compound));

	s2->open();
	s3->close();
	CPPUNIT_ASSERT(s2 == rr.getAcceptor(compound));
	CPPUNIT_ASSERT(s1 == rr.getAcceptor(compound));
	CPPUNIT_ASSERT(s2 == rr.getAcceptor(compound));
} // testAccepting



