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
#include <WNS/distribution/Uniform.hpp>

#include <cppunit/extensions/HelperMacros.h>

#include <algorithm>
#include <map>
#include <vector>

#include <cmath>
#include <stdexcept>

// needed on cygwin
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace wns { namespace geometry { namespace tests {

	class PointTest :
	public CppUnit::TestFixture
	{
		CPPUNIT_TEST_SUITE( PointTest );
		CPPUNIT_TEST( testEqual );
		CPPUNIT_TEST( testUnequal );
		CPPUNIT_TEST( testSubstract );
		CPPUNIT_TEST( testAddVector );
		CPPUNIT_TEST( testGet );
		CPPUNIT_TEST( testSet );
		CPPUNIT_TEST( testSetAtOnce );
		CPPUNIT_TEST( testSetPolar );
		CPPUNIT_TEST_EXCEPTION( createNegativePointX, std::out_of_range );
		CPPUNIT_TEST_EXCEPTION( createNegativePointY, std::out_of_range );
		CPPUNIT_TEST_EXCEPTION( createNegativePointZ, std::out_of_range );
		CPPUNIT_TEST_EXCEPTION( setNegativeX, std::out_of_range );
		CPPUNIT_TEST_EXCEPTION( setNegativeY, std::out_of_range );
		CPPUNIT_TEST_EXCEPTION( setNegativeZ, std::out_of_range );
        CPPUNIT_TEST( testPointAsKey );
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp();
		void tearDown();
		void testEqual();
		void testUnequal();
		void testSubstract();
		void testAddVector();
		void testGet();
		void testSet();
		void testSetAtOnce();
		void testSetPolar();
		void createNegativePointX();
		void createNegativePointY();
		void createNegativePointZ();
		void setNegativeX();
		void setNegativeY();
		void setNegativeZ();
        void testPointAsKey();

	private:
		Point p1;
		Point p2;
		Point p3;
		Point p4;
	};

	CPPUNIT_TEST_SUITE_REGISTRATION( PointTest );

	void PointTest::setUp()
	{
		p1  = Point(1, 1, 1);
		p2  = Point(0, 0, 0);
		p3  = Point(4, 5, 6);
		p4  = Point(2, 1, 3);
	}

	void PointTest::tearDown()
	{
	}

	void PointTest::testEqual()
	{
		p2 = p3;
		CPPUNIT_ASSERT( (p2 == p3) );
	}

	void PointTest::testUnequal()
	{
		CPPUNIT_ASSERT( !(p1 == p3) );
	}

	void PointTest::testSubstract()
	{
 		CPPUNIT_ASSERT( (p3 - p4) == Vector(2, 4, 3) );
	}

	void PointTest::testAddVector()
	{
		CPPUNIT_ASSERT( (p4 + Vector(2, 4, 3) ) == p3 );
	}

	void PointTest::testGet()
	{
		CPPUNIT_ASSERT( p4.getX() == 2 );
		CPPUNIT_ASSERT( p4.getY() == 1 );
		CPPUNIT_ASSERT( p4.getZ() == 3 );
	}
	void PointTest::testSet()
	{
		p1.setX(1);
		p1.setY(2);
		p1.setZ(3);
		CPPUNIT_ASSERT( p1.getX() == 1 );
		CPPUNIT_ASSERT( p1.getY() == 2 );
		CPPUNIT_ASSERT( p1.getZ() == 3 );
	}

	void PointTest::testSetAtOnce()
	{
		p1.set(3, 2, 1);
		CPPUNIT_ASSERT( p1.getX() == 3 );
		CPPUNIT_ASSERT( p1.getY() == 2 );
		CPPUNIT_ASSERT( p1.getZ() == 1 );
	}

	void PointTest::createNegativePointX()
	{
		p1=Point(-1,1,1);
	}

	void PointTest::createNegativePointY()
	{
		p1=Point(1,-1,1);
	}

	void PointTest::createNegativePointZ()
	{
		p1=Point(1,1,-1);
	}

	void PointTest::setNegativeX()
	{
		p1.setX(-1);
	}

	void PointTest::setNegativeY()
	{
		p1.setY(-1);
	}

	void PointTest::setNegativeZ()
	{
		p1.setZ(-1);
	}

	void PointTest::testSetPolar()
	{
		Point polar;
		polar.setPolar(1, 0, M_PI/2);
		CPPUNIT_ASSERT_DOUBLES_EQUAL( 1, polar.getX(), 1E-5 );
		CPPUNIT_ASSERT_DOUBLES_EQUAL( 0, polar.getY(), 1E-5 );
		CPPUNIT_ASSERT_DOUBLES_EQUAL( 0, polar.getZ(), 1E-5 );

		polar.setPolar(1, M_PI/2, M_PI/2);
		CPPUNIT_ASSERT_DOUBLES_EQUAL( 0, polar.getX(), 1E-5 );
		CPPUNIT_ASSERT_DOUBLES_EQUAL( 1, polar.getY(), 1E-5 );
		CPPUNIT_ASSERT_DOUBLES_EQUAL( 0, polar.getZ(), 1E-5 );

		polar.setPolar(1, M_PI, M_PI/2);
		CPPUNIT_ASSERT_DOUBLES_EQUAL( -1, polar.getX(), 1E-5 );
		CPPUNIT_ASSERT_DOUBLES_EQUAL( 0, polar.getY(), 1E-5 );
		CPPUNIT_ASSERT_DOUBLES_EQUAL( 0, polar.getZ(), 1E-5 );

		polar.setPolar(1, -M_PI/2, M_PI/2);
		CPPUNIT_ASSERT_DOUBLES_EQUAL( 0, polar.getX(), 1E-5 );
		CPPUNIT_ASSERT_DOUBLES_EQUAL( -1, polar.getY(), 1E-5 );
		CPPUNIT_ASSERT_DOUBLES_EQUAL( 0, polar.getZ(), 1E-5 );

		polar.setPolar(1, 0, 0);
		CPPUNIT_ASSERT_DOUBLES_EQUAL( 0, polar.getX(), 1E-5 );
		CPPUNIT_ASSERT_DOUBLES_EQUAL( 0, polar.getY(), 1E-5 );
		CPPUNIT_ASSERT_DOUBLES_EQUAL( 1, polar.getZ(), 1E-5 );

		polar.setPolar(1, 0, M_PI);
		CPPUNIT_ASSERT_DOUBLES_EQUAL( 0, polar.getX(), 1E-5 );
		CPPUNIT_ASSERT_DOUBLES_EQUAL( 0, polar.getY(), 1E-5 );
		CPPUNIT_ASSERT_DOUBLES_EQUAL( -1, polar.getZ(), 1E-5 );

		polar.setPolar(1, M_PI/4, M_PI/2);
		CPPUNIT_ASSERT_DOUBLES_EQUAL( sqrt(2.0)/2, polar.getX(), 1E-5 );
		CPPUNIT_ASSERT_DOUBLES_EQUAL( sqrt(2.0)/2, polar.getY(), 1E-5 );
		CPPUNIT_ASSERT_DOUBLES_EQUAL( 0, polar.getZ(), 1E-5 );

		polar.setPolar(1, M_PI/4, M_PI/4);
		CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5, polar.getX(), 1E-5 );
		CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5, polar.getY(), 1E-5 );
		CPPUNIT_ASSERT_DOUBLES_EQUAL( sqrt(2.0)/2, polar.getZ(), 1E-5 );
	}

    void PointTest::testPointAsKey()
    {
        int size = 100000;
        std::map<wns::geometry::Point, int> pointMap;
        std::vector<wns::geometry::Point> pointVector;
        std::vector<int> indices;

        wns::distribution::Uniform dis = wns::distribution::Uniform(0.0, 1000.0, wns::simulator::getRNG());

        indices.resize(size);
        pointVector.resize(size);

        // create set containing random points and remember them
        for (int i = 0; i < size; i++)
        {
             wns::geometry::Point p(dis(), dis(), dis());
             pointVector[i] = p;
             indices[i] = i;
             pointMap[p] = i;
        }

        // check that these random points can be found afterwards
        for (int i = size - 1; i >= 0; i--)
        {
            CPPUNIT_ASSERT(pointMap[pointVector[i]] == i);
        }

        // even when searched for in a different order
        std::random_shuffle(indices.begin(), indices.end());
        for (int i = 0; i < size; i++)
        {
            CPPUNIT_ASSERT(pointMap[pointVector[indices[i]]] == indices[i]);
        }
    }

} // tests
} // geometry
} // wns


