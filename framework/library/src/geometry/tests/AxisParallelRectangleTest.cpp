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

#include <WNS/geometry/Point.hpp>
#include <WNS/geometry/AxisParallelRectangle.hpp>
#include <WNS/CppUnit.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cmath>
#include <stdexcept>

namespace wns{ namespace geometry { namespace tests {

    class AxisParallelRectangleTest : 
    public CppUnit::TestFixture
    {
    CPPUNIT_TEST_SUITE( AxisParallelRectangleTest );
    CPPUNIT_TEST( testEqual );
    CPPUNIT_TEST( testUnequal );
    CPPUNIT_TEST( testGet );
    CPPUNIT_TEST( testContainsPoint );
    CPPUNIT_TEST( testContainsRectangle );
    CPPUNIT_TEST( testIntersectsLineSegment );
    CPPUNIT_TEST( testIntersectsRectangle );
    CPPUNIT_TEST( testCountBorderIntersections );
    CPPUNIT_TEST( testBordersIntersect );
    CPPUNIT_TEST_SUITE_END();

    public:
    void setUp();
    void tearDown();
    void testEqual();
    void testUnequal();
    void testGet();
    void testContainsPoint();
    void testContainsRectangle();
    void testIntersectsLineSegment();
    void testIntersectsRectangle();
    void testCountBorderIntersections();
    void testBordersIntersect();

    private:
    Point a1;
    Point b1;
    Point a2;
    Point b2;
    Point a3;
    Point b3;
    Point a4;
    Point b4;

    LineSegment L1;
    LineSegment L2;
    LineSegment L3;
    LineSegment L4;
    LineSegment L5;
    LineSegment L6;


    AxisParallelRectangle APR;
    AxisParallelRectangle APR1;
    AxisParallelRectangle APR2;
    AxisParallelRectangle APR3;
    AxisParallelRectangle APR4;
    };

    void AxisParallelRectangleTest::setUp()
    {
    a1 = Point(1,1,0);
    b1 = Point(3,4,0);
    a2 = Point(4,4,0);
    b2 = Point(2,2,0);
    a3 = Point(4,2,0);
    b3 = Point(6,1,0);
    a4 = Point(3,2,0);
    b4 = Point(4,3,0);

    L1 = LineSegment(b2,Point(2,3,0));
    L2 = LineSegment(a4,Point(3,3,0));
    L3 = LineSegment(b2,a3);
    L4 = LineSegment(a3,b4);
    L5 = LineSegment(Point(0,3,0),a3);
    L6 = LineSegment(a2,Point(4,1,0));

    APR1 = AxisParallelRectangle(a1,b1);
    APR2 = AxisParallelRectangle(a2,b2);
    APR3 = AxisParallelRectangle(a3,b3);
    APR4 = AxisParallelRectangle(a4,b4);
    }

    void AxisParallelRectangleTest::tearDown()
    {
    }

    void AxisParallelRectangleTest::testEqual()
    {
    APR = APR1;
    CPPUNIT_ASSERT( APR == APR1 );
    }


    void AxisParallelRectangleTest::testUnequal()
    {
    APR = AxisParallelRectangle(Point(0,1,1),Point(1,0,0));
    CPPUNIT_ASSERT(  APR != APR1 );
    }

    void AxisParallelRectangleTest::testGet()
    {
    CPPUNIT_ASSERT( APR1.getA() == a1 );
    CPPUNIT_ASSERT( APR1.getB() == b1 );

    CPPUNIT_ASSERT( APR2.getA() == b2 );
    CPPUNIT_ASSERT( APR2.getB() == a2 );

    CPPUNIT_ASSERT( APR3.getA() == Point(4,1,0) );
    CPPUNIT_ASSERT( APR3.getB() == Point(6,2,0) );
    }

    void AxisParallelRectangleTest::testContainsPoint()
    {
    CPPUNIT_ASSERT(  APR1.contains(b2) );
    CPPUNIT_ASSERT(  APR1.contains(a1) );
    CPPUNIT_ASSERT(  APR1.contains(a4) );
    CPPUNIT_ASSERT( !APR1.contains(a3) );
    }

    void AxisParallelRectangleTest::testContainsRectangle()
    {
    CPPUNIT_ASSERT(  APR1.contains(APR1) );
    CPPUNIT_ASSERT(  APR2.contains(APR4) );
    CPPUNIT_ASSERT( !APR1.contains(APR2) );
    CPPUNIT_ASSERT( !APR1.contains(APR3) );
    CPPUNIT_ASSERT( !APR1.contains(APR4) );
    }

    void AxisParallelRectangleTest::testIntersectsLineSegment()
    {
    CPPUNIT_ASSERT(  APR1.intersects(L5) );
    CPPUNIT_ASSERT(  APR4.intersects(L6) ); 
    CPPUNIT_ASSERT( !APR1.intersects(L1) );
    CPPUNIT_ASSERT( !APR1.intersects(L2) );
    CPPUNIT_ASSERT( !APR1.intersects(L3) );
    CPPUNIT_ASSERT( !APR1.intersects(L4) );
    }

    void AxisParallelRectangleTest::testIntersectsRectangle()
    {
    CPPUNIT_ASSERT(  APR1.intersects(APR2) );
    CPPUNIT_ASSERT(  APR1.intersects(APR4) );
    CPPUNIT_ASSERT( !APR1.intersects(APR1) );
    CPPUNIT_ASSERT( !APR1.intersects(APR3) );
    }

    void AxisParallelRectangleTest::testCountBorderIntersections()
    {
    CPPUNIT_ASSERT( APR1.countBorderIntersections(L1) == 0 );
    CPPUNIT_ASSERT( APR1.countBorderIntersections(L2) == 0 );
    CPPUNIT_ASSERT( APR1.countBorderIntersections(L3) == 1 );
    CPPUNIT_ASSERT( APR1.countBorderIntersections(L4) == 0 );
    CPPUNIT_ASSERT( APR1.countBorderIntersections(L5) == 2 );
    CPPUNIT_ASSERT( APR4.countBorderIntersections(L6) == 2 ); 
    }

    void AxisParallelRectangleTest::testBordersIntersect()
    {
    CPPUNIT_ASSERT(  APR1.bordersIntersect(L5) );
    CPPUNIT_ASSERT(  APR1.bordersIntersect(L6) ); 
    CPPUNIT_ASSERT( !APR1.bordersIntersect(L1) );
    CPPUNIT_ASSERT( !APR1.bordersIntersect(L2) );
    CPPUNIT_ASSERT( !APR1.bordersIntersect(L3) );
    CPPUNIT_ASSERT( !APR1.bordersIntersect(L4) );

    }

}//tests
}//geometry
}//wns






