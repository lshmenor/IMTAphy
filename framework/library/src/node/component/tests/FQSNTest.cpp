/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 16, D-52074 Aachen, Germany
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

#include <WNS/node/component/tests/FQSNTest.hpp>
#include <WNS/node/component/tests/TCP.hpp>
#include <WNS/node/component/FQSN.hpp>

#include <WNS/pyconfig/helper/Functions.hpp>

using namespace wns::node::component::tests;

CPPUNIT_TEST_SUITE_REGISTRATION( FQSNTest );

void FQSNTest::setUp()
{
}

void FQSNTest::tearDown()
{
}


void FQSNTest::construct()
{
	std::string config =
		"from openwns.node import FQSN\n"
		"class DummyNode:\n"
		"    name = 'dummyNode'\n"
		"fqsn = FQSN(DummyNode(), 'dummyService')\n";

	wns::pyconfig::View pyco = pyconfig::helper::createViewFromString(config);

	FQSN fqsn = FQSN(pyco.get<wns::pyconfig::View>("fqsn"));

	CPPUNIT_ASSERT(fqsn.getNodeName() == "dummyNode");
	CPPUNIT_ASSERT(fqsn.getServiceName() == "dummyService");
}

void FQSNTest::stream()
{
	std::string config =
		"from openwns.node import FQSN\n"
		"class DummyNode:\n"
		"    name = 'dummyNode'\n"
		"fqsn = FQSN(DummyNode(), 'dummyService')\n";

	wns::pyconfig::View pyco = pyconfig::helper::createViewFromString(config);

	FQSN fqsn = FQSN(pyco.get<wns::pyconfig::View>("fqsn"));

	std::stringstream ss;
	ss << fqsn;
	CPPUNIT_ASSERT(ss.str() == "dummyNode.dummyService");
}
