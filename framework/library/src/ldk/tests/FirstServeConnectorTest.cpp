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

#include "FirstServeConnectorTest.hpp"

#include <WNS/ldk/tools/Stub.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>

#include <WNS/pyconfig/Parser.hpp>



using namespace wns::ldk;
using namespace wns::ldk::tests;



CPPUNIT_TEST_SUITE_REGISTRATION( FirstServeConnectorTest );



void
FirstServeConnectorTest::prepare()
{
	pyconfig::Parser emptyConfig;

	l = new wns::ldk::tests::LayerStub();
	fuNet = new wns::ldk::fun::Main(l);
	s1 = new tools::Stub(fuNet, emptyConfig);
	s2 = new tools::Stub(fuNet, emptyConfig);
	s3 = new tools::Stub(fuNet, emptyConfig);
} // setUp



void
FirstServeConnectorTest::cleanup()
{
	delete s1;
	delete s2;
	delete s3;
	delete fuNet;
	delete l;
} // tearDown



void
FirstServeConnectorTest::testFill()
{
	FirstServeConnector fsc;

	CPPUNIT_ASSERT(0 == fsc.size());
	fsc.add(s1);
	fsc.add(s2);
	fsc.add(s3);
	CPPUNIT_ASSERT(3 == fsc.size());
} // testFill



void
FirstServeConnectorTest::testFSC()
{
	FirstServeConnector fsc;
	CompoundPtr compound;

	WNS_ASSERT_ASSURE_EXCEPTION(fsc.getAcceptor(compound));

	fsc.add(s1);
	fsc.add(s2);
	fsc.add(s3);

	CPPUNIT_ASSERT(fsc.hasAcceptor(compound));

	CPPUNIT_ASSERT(s1 == fsc.getAcceptor(compound));
	s1->close();
	CPPUNIT_ASSERT(s2 == fsc.getAcceptor(compound));
	s2->close();
	CPPUNIT_ASSERT(s3 == fsc.getAcceptor(compound));
	s3->close();

	CPPUNIT_ASSERT(!fsc.hasAcceptor(compound));
	WNS_ASSERT_ASSURE_EXCEPTION(fsc.getAcceptor(compound));

	s2->open();
	CPPUNIT_ASSERT(fsc.hasAcceptor(compound));
	CPPUNIT_ASSERT(s2 == fsc.getAcceptor(compound));
	s1->open();
	CPPUNIT_ASSERT(s1 == fsc.getAcceptor(compound));
} // testFSC



void
FirstServeConnectorTest::testAccepting()
{
	FirstServeConnector fsc;
	CompoundPtr compound;

	fsc.add(s1);
	fsc.add(s2);
	fsc.add(s3);

	CPPUNIT_ASSERT(fsc.hasAcceptor(compound));
	s2->close();
	CPPUNIT_ASSERT(fsc.hasAcceptor(compound));
	s1->close();
	CPPUNIT_ASSERT(fsc.hasAcceptor(compound));
	s3->close();

	CPPUNIT_ASSERT(!fsc.hasAcceptor(compound));

	s2->open();
	CPPUNIT_ASSERT(fsc.hasAcceptor(compound));
	s2->close();
	s3->open();
	CPPUNIT_ASSERT(fsc.hasAcceptor(compound));

} // testAccepting




