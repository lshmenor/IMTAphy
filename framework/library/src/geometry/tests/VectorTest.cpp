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
#include <WNS/geometry/Vector.hpp>
#include <WNS/CppUnit.hpp>

#include <cppunit/extensions/HelperMacros.h>

#include <cmath>
#include <stdexcept>

// needed on cygwin
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace wns { namespace geometry { namespace tests {

    class VectorTest :
        public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE( VectorTest );
        CPPUNIT_TEST( testEqual );
        CPPUNIT_TEST( testUnequal );
        CPPUNIT_TEST( testAddPoint );
        CPPUNIT_TEST( testSetDoubles );
        CPPUNIT_TEST( testMultiplyWithInt );
        CPPUNIT_TEST( testMultiplyWithDouble );
        CPPUNIT_TEST( testPhi );
        CPPUNIT_TEST( testTheta );
        CPPUNIT_TEST( testProducts );
        CPPUNIT_TEST( testR );
        CPPUNIT_TEST( testGetPhi );
        CPPUNIT_TEST( testGetTheta );
        CPPUNIT_TEST( testGet );
        CPPUNIT_TEST( testSet );
        CPPUNIT_TEST( testSetAtOnce);
        CPPUNIT_TEST( testSetPolar );
        CPPUNIT_TEST( testSetR );
        CPPUNIT_TEST( testSetAzimuth );
        CPPUNIT_TEST( testSetElevation );
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp();
        void tearDown();
        void testEqual();
        void testUnequal();
        void testAddPoint();
        void testSetDoubles();
        void testMultiplyWithInt();
        void testMultiplyWithDouble();
        void testPhi();
        void testTheta();
        void testProducts();
        void testR();
        void testGetPhi();
        void testGetTheta();
        void testGet();
        void testSet();
        void testSetAtOnce();
        void testSetPolar();
        void testSetR();
        void testSetAzimuth();
        void testSetElevation();

    private:
        Point p1;
        Point p2;
        Point p3;
        Vector po1;
        Vector po2;
        Vector po3;
        Vector po4;
        Vector po5;
        Vector po6;
        Vector po7;
        Vector po8;
        Vector po9;
        Vector po10;

    };

    CPPUNIT_TEST_SUITE_REGISTRATION( VectorTest );

    void VectorTest::setUp()
    {
        po1 = Vector( 1, 1, 1 );
        po2 = Vector( 1, 1, -1 );
        po3 = Vector( 1, 1, 0 );
        po4 = Vector( -1, 1, 0 );
        po5 = Vector( -1, -1, 0 );
        po6 = Vector( 1, -1, 0 );
        po7 = Vector( 0, -1, 0 );
        po8 = Vector( 0, 1, 0 );
        po9 = Vector( 2, 4, 5 );
        po10 = Vector( 7, 10, 3 );
    }

    void VectorTest::tearDown()
    {
    }

    void VectorTest::testEqual()
    {
        CPPUNIT_ASSERT( (po9 == po9) );
    }

    void VectorTest::testUnequal()
    {
        CPPUNIT_ASSERT( (po10 != po9) );
    }

    void VectorTest::testAddPoint()
    {
        CPPUNIT_ASSERT( (po9 + Point(1, 2, 4)) == Point(3, 6, 9) );
    }

    void VectorTest::testSetDoubles()
    {
        po9.set(1,2,3);
        CPPUNIT_ASSERT_DOUBLES_EQUAL( po9.getDeltaX(), 1.0, 1E-5 );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( po9.getDeltaY(), 2.0, 1E-5 );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( po9.getDeltaZ(), 3.0, 1E-5 );
    }

    void VectorTest::testMultiplyWithInt()
    {
        po9 = po9 * 2;
        CPPUNIT_ASSERT_DOUBLES_EQUAL( po9.getDeltaX(), 4.0, 1E-5 );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( po9.getDeltaY(), 8.0, 1E-5 );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( po9.getDeltaZ(), 10.0, 1E-5 );
    }

    void VectorTest::testMultiplyWithDouble()
    {
        po9 = po9 * 0.5;
        CPPUNIT_ASSERT_DOUBLES_EQUAL( po9.getDeltaX(), 1.0, 1E-5 );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( po9.getDeltaY(), 2.0, 1E-5 );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( po9.getDeltaZ(), 2.5, 1E-5 );
    }

    void VectorTest::testGetPhi()
    {
        CPPUNIT_ASSERT( Vector().getPhi() == 0.0 );
    }

    void VectorTest::testGetTheta()
    {
        CPPUNIT_ASSERT( Vector().getTheta() == 0.0 );
    }


    void VectorTest::testProducts()
    {
        Vector v1(1.0, 2.0, 3.0);
        Vector v2(4.0, 3.0, -1.0);
        Vector v3 = v1.cross(v2);
        CPPUNIT_ASSERT_DOUBLES_EQUAL( v1.dot(v2), 7.0, 1E-5 );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( v3.getDeltaX(), -11.0, 1E-5 );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( v3.getDeltaY(), 13.0, 1E-5 );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( v3.getDeltaZ(), -5.0, 1E-5 );
    }

    void VectorTest::testR()
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL( sqrt(3.0), po1.getR(), 1E-5 );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( sqrt(3.0), po1.abs(), 1E-5 );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( sqrt(2.0), po3.abs(), 1E-5 );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( sqrt(2.0), po3.getR(), 1E-5 );
    }

    void VectorTest::testPhi()
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL( M_PI/4, po1.getPhi(), 1E-5 );
        CPPUNIT_ASSERT( po1.getPhi()  == po2.getPhi() );
        CPPUNIT_ASSERT( po2.getPhi()  == po3.getPhi() );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.75*M_PI, po4.getPhi(), 1E-5 );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( -0.75*M_PI, po5.getPhi(), 1E-5 );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( -M_PI/4, po6.getPhi(), 1E-5 );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( -1.0*M_PI/2, po7.getPhi(), 1E-5 );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( M_PI/2, po8.getPhi(), 1E-5 );
    }

    void VectorTest::testTheta()
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.95531618, po1.getTheta(), 1E-5 );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( M_PI-0.95531618, po2.getTheta(), 1E-5 );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( M_PI/2, po3.getTheta(), 1E-5 );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( M_PI/2, po4.getTheta(), 1E-5 );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( M_PI/2, po5.getTheta(), 1E-5 );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( M_PI/2, po6.getTheta(), 1E-5 );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( M_PI/2, po7.getTheta(), 1E-5 );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( M_PI/2, po8.getTheta(), 1E-5 );
    }

    void VectorTest::testGet()
    {
        CPPUNIT_ASSERT( po1.getDeltaX() == 1 );
        CPPUNIT_ASSERT( po1.getDeltaY() == 1 );
        CPPUNIT_ASSERT( po1.getDeltaZ() == 1 );
    }

    void VectorTest::testSet()
    {
        po1.setDeltaX(1);
        po1.setDeltaY(2);
        po1.setDeltaZ(3);
        CPPUNIT_ASSERT( po1.getDeltaX() == 1 );
        CPPUNIT_ASSERT( po1.getDeltaY() == 2 );
        CPPUNIT_ASSERT( po1.getDeltaZ() == 3 );
    }

    void VectorTest::testSetAtOnce()
    {
        po1.set(3, 2, 1);
        CPPUNIT_ASSERT( po1.getDeltaX() == 3 );
        CPPUNIT_ASSERT( po1.getDeltaY() == 2 );
        CPPUNIT_ASSERT( po1.getDeltaZ() == 1 );
    }

    void VectorTest::testSetPolar()
    {
        po1.setPolar(1, M_PI, M_PI/4);
        CPPUNIT_ASSERT_DOUBLES_EQUAL( po1.getR() , 1, 1E-5  );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( po1.getAzimuth() , M_PI, 1E-5  );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( po1.getElevation() , M_PI/4, 1E-5  );
    }

    void VectorTest::testSetR()
    {
        po1.setPolar(1, M_PI, M_PI/4);
        po1.setR(2.0);
        CPPUNIT_ASSERT_DOUBLES_EQUAL( po1.getR() , 2.0, 1E-5  );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( po1.getAzimuth() , M_PI, 1E-5  );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( po1.getElevation() , M_PI/4 , 1E-5 );
    }

    void VectorTest::testSetAzimuth()
    {
        po1.setPolar(1, M_PI, M_PI/4);
        po1.setR(2.0);
        po1.setAzimuth(0.75*M_PI);
        CPPUNIT_ASSERT_DOUBLES_EQUAL( po1.getR() , 2.0, 1E-5  );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( po1.getAzimuth() , 0.75*M_PI, 1E-5  );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( po1.getElevation() , M_PI/4, 1E-5  );
    }

    void VectorTest::testSetElevation()
    {
        po1.setPolar(1, M_PI, M_PI/4);
        po1.setR(2.0);
        po1.setAzimuth(0.75*M_PI);
        po1.setElevation(0.33*M_PI);
        CPPUNIT_ASSERT_DOUBLES_EQUAL( po1.getR() , 2.0, 1E-5  );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( po1.getAzimuth() , 0.75*M_PI, 1E-5  );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( po1.getElevation() , 0.33*M_PI, 1E-5  );
    }

} // tests
} // geometry
} // wns


