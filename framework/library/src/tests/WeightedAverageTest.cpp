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

#include <WNS/tests/WeightedAverageTest.hpp>

using namespace std;
using namespace wns;

CPPUNIT_TEST_SUITE_REGISTRATION( WeightedAverageTest );

void WeightedAverageTest::setUp()
{
	average.reset();
}

void WeightedAverageTest::tearDown()
{
}

void WeightedAverageTest::testPutAndGet()
{
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0, average.get(), 0.00001 );
	average.put(23,0);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0, average.get(), 0.00001 );
	average.put(4,1);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 4, average.get(), 0.00001 );
	average.put(7,2);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 6, average.get(), 0.00001);
}

void WeightedAverageTest::testReset()
{
	average.reset();
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0, average.get(), 0.00001 );
	average.put(5.5, 2);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 5.5, average.get(), 0.00001 );
	average.reset();
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0, average.get(), 0.00001 );
	average.put(7.5, 2);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 7.5, average.get(), 0.00001 );
}


