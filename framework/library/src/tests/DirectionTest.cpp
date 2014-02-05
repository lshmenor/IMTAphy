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

#include <WNS/tests/DirectionTest.hpp>
#include <WNS/Position.hpp>

// needed on cygwin
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


using namespace wns;
using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( DirectionTest );

void DirectionTest::setUp()
{}

void DirectionTest::tearDown()
{}

void DirectionTest::testConstructors()
{
	Direction d;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0, d.getElevation(), 1E-6 );
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0, d.getAzimuth(), 1E-6 );
	Direction d2(0.5, 0.6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5, d2.getElevation(), 1E-6 );
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.6, d2.getAzimuth(), 1E-6 );
	Direction d3(d2);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5, d3.getElevation(), 1E-6 );
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.6, d3.getAzimuth(), 1E-6 );

	Direction d4 = d3;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5, d4.getElevation(), 1E-6 );
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.6, d4.getAzimuth(), 1E-6 );
}

void DirectionTest::testDirectionCalcAzimuthUntwisted()
{
	Direction d;
	PositionOffset po(3, 13, 2);
	Direction res = d.calcAngles(po);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( po.getAzimuth(), res.getAzimuth(), 1E-6 );
}

void DirectionTest::testDirectionCalcElevationUntwisted()
{
	Direction d;
	PositionOffset po(3, 13, 2);
	Direction res = d.calcAngles(po);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( po.getElevation(), res.getElevation(), 1E-6 );
}

void DirectionTest::testDirectionCalcAzimuthTwisted()
{
	Direction twisted( 0.25*M_PI, 0.25*M_PI );
	PositionOffset po(1, 1, 1);
	Direction res = twisted.calcAngles(po);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( po.getAzimuth() - twisted.getAzimuth(), res.getAzimuth(), 1E-6 );
}

void DirectionTest::testDirectionCalcElevationTwisted()
{
	Direction twisted( 0.25*M_PI, 0.25*M_PI );
	PositionOffset po(1, 1, 1);
	Direction res = twisted.calcAngles(po);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( po.getElevation() - twisted.getElevation(), res.getElevation(), 1E-6 );
}

void DirectionTest::testNoNegativeResult()
{
	Direction   reverse( M_PI/2.0,    -M_PI/3.0 );
	Direction noreverse( M_PI/2.0, 5.0*M_PI/3.0 );

	PositionOffset alsoReverse;
	alsoReverse.setPolar(1, -M_PI/2.0, M_PI/2.0);

	Direction diff1 =   reverse.calcAngles(alsoReverse);
	Direction diff2 = noreverse.calcAngles(alsoReverse);

	CPPUNIT_ASSERT_DOUBLES_EQUAL( 11.0*M_PI/6.0, diff1.getAzimuth(), 1E-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 11.0*M_PI/6.0, diff2.getAzimuth(), 1E-6);
}


