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

#include <WNS/tests/CandITest.hpp>

using namespace wns;
using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( CandITest );

void CandITest::setUp()
{
	candi1 = CandI();
	candi2 = CandI(wns::Power::from_dBm(-32), wns::Power::from_dBm(-80));
	candi2.sdma.iIntra = wns::Power::from_dBm(-96);
}

void CandITest::tearDown()
{
}

void CandITest::testConstructors()
{
	CPPUNIT_ASSERT( candi1.C.get_mW() == 0.0 );
	CPPUNIT_ASSERT( candi1.I.get_mW() == 0.0 );
	CPPUNIT_ASSERT( candi1.sdma.iIntra.get_mW() == 0.0 );

	CPPUNIT_ASSERT_DOUBLES_EQUAL( -32.0, candi2.C.get_dBm(), 1E-5 );
	CPPUNIT_ASSERT_DOUBLES_EQUAL( -80.0, candi2.I.get_dBm(), 1E-5 );
	CPPUNIT_ASSERT_DOUBLES_EQUAL( -96.0, candi2.sdma.iIntra.get_dBm(), 1E-5 );
}

void CandITest::testBooleanOperators()
{
	candi1.C = wns::Power::from_dBm(-32);
	candi1.I = wns::Power::from_dBm(-32);

	CPPUNIT_ASSERT( candi1 < candi2 );
	CPPUNIT_ASSERT( !(candi2 < candi1) );

	candi1.C = wns::Power::from_dBm(32);

	CPPUNIT_ASSERT( candi2 < candi1 );
	CPPUNIT_ASSERT( !(candi1 < candi2) );
}


