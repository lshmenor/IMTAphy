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
#include <WNS/geometry/AABoundingBox.hpp>
#include <WNS/CppUnit.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cmath>
#include <stdexcept>

namespace wns{ namespace geometry { namespace tests {

    class AABoundingBoxTest : 
        public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE( AABoundingBoxTest );
        CPPUNIT_TEST( testEqual );
        CPPUNIT_TEST( testUnequal );
        CPPUNIT_TEST( testGet );
        CPPUNIT_TEST( testMinMax );
        CPPUNIT_TEST( testIntersects );	
        CPPUNIT_TEST( testContainsPoint );
        CPPUNIT_TEST( testContainsAABB );
        CPPUNIT_TEST_SUITE_END();

        public:
        void setUp();
        void tearDown();
        void testEqual();
        void testUnequal();
        void testGet();
        void testMinMax();
        void testIntersects();
        void testContainsPoint();
        void testContainsAABB();


        private:
        Point a1;
        Point b1;
        Point a2;
        Point b2;
        Point a3;
        Point b3;
        Point a4;
        Point b4;
        AABoundingBox AABB;
        AABoundingBox AABB1;
        AABoundingBox AABB2;
        AABoundingBox AABB3;
        AABoundingBox AABB4;

    };
    CPPUNIT_TEST_SUITE_REGISTRATION( AABoundingBoxTest );


    void AABoundingBoxTest::setUp()
    {
        a1 = Point(1,1,0);
        b1 = Point(3,4,0);
        a2 = Point(4,4,0);
        b2 = Point(2,2,0);
        a3 = Point(4,2,0);
        b3 = Point(6,1,0);
        a4 = Point(3,2,0);
        b4 = Point(4,3,0);


        AABB1 = AABoundingBox(a1,b1);
        AABB2 = AABoundingBox(a2,b2);
        AABB3 = AABoundingBox(a3,b3);
        AABB4 = AABoundingBox(a4,b4);
    }

    void AABoundingBoxTest::tearDown()
    {
    }

    void AABoundingBoxTest::testEqual()
    {
        AABB = AABB1;
        CPPUNIT_ASSERT( AABB == AABB1 );
    }


    void AABoundingBoxTest::testUnequal()
    {
        AABB = AABoundingBox();
        CPPUNIT_ASSERT(  AABB != AABB1 );
    }


    void AABoundingBoxTest::testGet()
    {
        CPPUNIT_ASSERT( AABB1.getA() == a1 );
        CPPUNIT_ASSERT( AABB1.getB() == b1 );

        CPPUNIT_ASSERT( AABB2.getA() == b2 );
        CPPUNIT_ASSERT( AABB2.getB() == a2 );

        CPPUNIT_ASSERT( AABB3.getA() == Point(4,1,0) );
        CPPUNIT_ASSERT( AABB3.getB() == Point(6,2,0) );
    }

    void AABoundingBoxTest::testMinMax()
    {
        CPPUNIT_ASSERT( AABB3.getMinX() == 4 );
        CPPUNIT_ASSERT( AABB3.getMinY() == 1 );
        CPPUNIT_ASSERT( AABB3.getMaxX() == 6 );
        CPPUNIT_ASSERT( AABB3.getMaxY() == 2 );
    }

    void AABoundingBoxTest::testIntersects()
    {
        CPPUNIT_ASSERT(  AABB1.intersects(AABB2) );
        CPPUNIT_ASSERT(  AABB2.intersects(AABB1) );
        CPPUNIT_ASSERT( !AABB1.intersects(AABB3) );
        CPPUNIT_ASSERT(  AABB2.intersects(AABB3) );
        CPPUNIT_ASSERT(  AABB2.intersects(AABB4) );
    }

    void AABoundingBoxTest::testContainsPoint()
    {
        CPPUNIT_ASSERT(  AABB1.contains(a1) );
        CPPUNIT_ASSERT(  AABB1.contains(b2) );
        CPPUNIT_ASSERT(  AABB1.contains(a4) );
        CPPUNIT_ASSERT( !AABB1.contains(b3) );
    }

    void AABoundingBoxTest::testContainsAABB()
    {
        CPPUNIT_ASSERT(  AABB1.contains(AABB1) );
        CPPUNIT_ASSERT(  AABB2.contains(AABB4) );
        CPPUNIT_ASSERT( !AABB1.contains(AABB2) );
        CPPUNIT_ASSERT( !AABB1.contains(AABB3) );
        CPPUNIT_ASSERT( !AABB1.contains(AABB4) );
        CPPUNIT_ASSERT( !AABB4.contains(AABB2) );
    }

}//tests
}//geometry
}//wns



