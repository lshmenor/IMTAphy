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

#include <WNS/tests/PositionableTest.hpp>

using namespace wns::tests;

CPPUNIT_TEST_SUITE_REGISTRATION( PositionableTest );

PositionableTest::PositionableTest()
{
}

PositionableTest::~PositionableTest()
{
}

void
PositionableTest::setUp()
{
}

void
PositionableTest::tearDown()
{
}

void
PositionableTest::getPosition()
{
	PositionableObject positionable;
	CPPUNIT_ASSERT( positionable.getPosition() == Position(0,0,0) );
	Position p(2,4,5);
	positionable.move(p);
	CPPUNIT_ASSERT( positionable.getPosition() == p );
}

void
PositionableTest::setPosition()
{
	PositionableObject positionable;
	PositionObserverTester observer1;
	PositionObserverTester observer2;
	PositionObserverTester observer3;

	observer1.startObserving(&positionable);
	observer2.startObserving(&positionable);

	positionable.move(Position(2,3,5));
	CPPUNIT_ASSERT( observer1.hasBeenInformedBefore == true );
	CPPUNIT_ASSERT( observer2.hasBeenInformedBefore == true );
	CPPUNIT_ASSERT( observer3.hasBeenInformedBefore == false );
	CPPUNIT_ASSERT( observer1.hasBeenInformedAfter == true );
	CPPUNIT_ASSERT( observer2.hasBeenInformedAfter == true );
	CPPUNIT_ASSERT( observer3.hasBeenInformedAfter == false );

	// reset has been informed
	observer1.hasBeenInformedBefore = false;
	observer1.hasBeenInformedAfter = false;
	observer2.hasBeenInformedBefore = false;
	observer2.hasBeenInformedAfter = false;

	observer3.startObserving(&positionable);

	positionable.move(Position(2,3,6));
	CPPUNIT_ASSERT( observer1.hasBeenInformedBefore == true );
	CPPUNIT_ASSERT( observer2.hasBeenInformedBefore == true );
	CPPUNIT_ASSERT( observer3.hasBeenInformedBefore == true );
	CPPUNIT_ASSERT( observer1.hasBeenInformedAfter == true );
	CPPUNIT_ASSERT( observer2.hasBeenInformedAfter == true );
	CPPUNIT_ASSERT( observer3.hasBeenInformedAfter == true );
}

void
PositionableTest::getDistance()
{
	PositionableObject positionable;
	PositionableObject positionable2;
	positionable2.move(Position(3,4,0));
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 5, positionable.getDistance(positionable2), 0.00001);
}


void
PositionableTest::getAngle()
{
    PositionableObject positionable;
    PositionableObject positionable2;
    positionable2.move(Position(1,1,0));
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.78539816339744828, positionable.getAngle(positionable2), 0.00001);
}

