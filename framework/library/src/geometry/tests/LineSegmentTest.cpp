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
#include <WNS/geometry/LineSegment.hpp>
#include <WNS/CppUnit.hpp>

#include <cppunit/extensions/HelperMacros.h>

#include <cmath>
#include <stdexcept>


namespace wns { namespace geometry { namespace tests {

    class LineSegmentTest :
        public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE( LineSegmentTest );
        CPPUNIT_TEST( testEqual );
        CPPUNIT_TEST( testUnequal );
        CPPUNIT_TEST( testGet );
        CPPUNIT_TEST( testContains );
        CPPUNIT_TEST( testLeftOf );
        CPPUNIT_TEST( testRightOf );
        CPPUNIT_TEST( testTouches );
        CPPUNIT_TEST( testStraddles );
        CPPUNIT_TEST( testIntersects );
        CPPUNIT_TEST( testCountBorderIntersections );
        CPPUNIT_TEST( testCrossProduct );
        CPPUNIT_TEST_SUITE_END();
        public:

        void setUp();
        void tearDown();
        void testEqual();
        void testUnequal();
        void testGet();
        void testContains();
        void testLeftOf();
        void testRightOf();
        void testTouches();
        void testStraddles();
        void testIntersects();
        void testCountBorderIntersections();
        void testCrossProduct();


        private:
        Point p1;
        Point p2;
        Point p3;
        Point p4;
        Point p5;
        Point p6;
        Point p7;
        Point p8;

        Point a;
        Point b;

        LineSegment s;
        LineSegment s1;
        LineSegment s2;
        LineSegment s3;
        LineSegment s4;
        LineSegment s5;
        LineSegment s6;
    };

    CPPUNIT_TEST_SUITE_REGISTRATION( LineSegmentTest );

    void LineSegmentTest::setUp()
    {
        p1 = Point(1, 1, 0);
        p2 = Point(3, 3, 0);
        p3 = Point(1, 2, 0);
        p4 = Point(4, 2, 0);
        p5 = Point(2, 4, 1);
        p6 = Point(1, 0, 0);
        p7 = Point(1, 4, 0);
        p8 = Point(3, 4, 0);


        s1 = LineSegment(p1, p2);
        s2 = LineSegment(p3, p4);
        s3 = LineSegment(p1, p3);
        s4 = LineSegment(p4, p5);
        s5 = LineSegment(p6, p7);
        s6 = LineSegment(p4, p8);
    }

    void LineSegmentTest::tearDown()
    {
    }

    void LineSegmentTest::testEqual()
    {
        s = s2;
        CPPUNIT_ASSERT( (s == s2) );
    }

    void LineSegmentTest::testUnequal()
    {
        s = LineSegment();
        CPPUNIT_ASSERT( !(s==s2) );
    }


    void LineSegmentTest::testGet()
    {
        CPPUNIT_ASSERT( (s1.getA() == p1) );
        CPPUNIT_ASSERT( (s1.getB() == p2) );
        CPPUNIT_ASSERT( (s4.getB() == Point(2,4,0)) );
    }

    void LineSegmentTest::testContains()
    {
        CPPUNIT_ASSERT( s1.contains(p1) );
        CPPUNIT_ASSERT( s1.contains(p2) );
        CPPUNIT_ASSERT( s1.contains(Point(2,2,0)) );
        CPPUNIT_ASSERT( !s1.contains(p3) ); 
        CPPUNIT_ASSERT( !s1.contains(Point(1,3,0)) );
    }

    void LineSegmentTest::testLeftOf()
    {
        CPPUNIT_ASSERT( s1.leftOf(Point(1,3,0)) );
        CPPUNIT_ASSERT( !s3.rightOf(Point(1,3,0)) );
        CPPUNIT_ASSERT( !s1.leftOf(p1) );
    }

    void LineSegmentTest::testRightOf()
    {
        CPPUNIT_ASSERT( s1.rightOf(Point(3,1,0)) );
        CPPUNIT_ASSERT( !s1.leftOf(Point(3,1,0)) );
        CPPUNIT_ASSERT( !s1.rightOf(p1) );
    }

    void LineSegmentTest::testTouches()
    {
        CPPUNIT_ASSERT(  s4.touches(s1) );
        CPPUNIT_ASSERT(  s5.touches(s1) );
        CPPUNIT_ASSERT( !s1.touches(s5) ); 
        CPPUNIT_ASSERT( !s5.touches(s3) );
        CPPUNIT_ASSERT( !s3.touches(s5) );
    }

    void LineSegmentTest::testStraddles()
    {
        CPPUNIT_ASSERT(  s1.straddles(s2) ); //crossed lines
        CPPUNIT_ASSERT(  s4.straddles(s1) ); //end Point on other line
        CPPUNIT_ASSERT(  s3.straddles(s5) ); //lines on eachother
        CPPUNIT_ASSERT( !s1.straddles(s4) ); //other end Point on line
        CPPUNIT_ASSERT( !s1.straddles(s3) ); //same edge point
        CPPUNIT_ASSERT( !s3.straddles(s4) ); //no intersection
        CPPUNIT_ASSERT( !s1.straddles(s6) ); //intersecting AABB
    }


    void LineSegmentTest::testIntersects()
    {
        CPPUNIT_ASSERT(  s1.intersects(s2) ); //crossed lines
        CPPUNIT_ASSERT(  s1.intersects(s3) ); //same edge point
        CPPUNIT_ASSERT(  s1.intersects(s4) ); //end Point on other line
        CPPUNIT_ASSERT(  s3.intersects(s5) ); //lines on eachother
        CPPUNIT_ASSERT( !s3.intersects(s4) ); //no intersection
        CPPUNIT_ASSERT( !s1.intersects(s6) ); //intersecting AABB
    }

    void LineSegmentTest::testCountBorderIntersections()
    {
        CPPUNIT_ASSERT( s1.countBorderIntersections(s2) == 1 );
        CPPUNIT_ASSERT( s1.countBorderIntersections(s3) == 1 ); 
        CPPUNIT_ASSERT( s3.countBorderIntersections(s4) == 0 );
    }

    void LineSegmentTest::testCrossProduct()
    {
        CPPUNIT_ASSERT( s1.crossProduct(p1) == 0 );
        CPPUNIT_ASSERT( s1.crossProduct( Point(2,2,0) ) == 0 );
        CPPUNIT_ASSERT( s1.crossProduct( Point(4,4,0) ) == 0 );
        CPPUNIT_ASSERT( s1.crossProduct( Point(1,3,0) ) == 4 );
        CPPUNIT_ASSERT( s1.crossProduct( Point(4,5,0) ) == 2 );
    }



}//tests
}//geometry
}//wns



