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

#include <WNS/tests/AverageTest.hpp>

using namespace wns::tests;

CPPUNIT_TEST_SUITE_REGISTRATION( AverageTest );

void AverageTest::setUp()
{
	average.reset();
}

void AverageTest::tearDown()
{
}

void AverageTest::testPutAndGet()
{
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0, average.get(), 0.0001 );
	average.put(4);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 4, average.get(), 0.0001 );
	average.put(6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 5, average.get(), 0.0001 );
	average.put(6);
	average.put(6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 5.5, average.get(), 0.0001 );
}

void AverageTest::testReset()
{
	average.reset();
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0, average.get(), 0.0001 );
	average.put(5.5);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 5.5, average.get(), 0.0001 );
	average.reset();
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0, average.get(), 0.0001 );
	average.put(7.5);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 7.5, average.get(), 0.0001 );
}

/*
  Local Variables:
  mode: c++
  fill-column: 80
  c-basic-offset: 8
  c-tab-always-indent: t
  indent-tabs-mode: t
  tab-width: 8
  End:
*/
