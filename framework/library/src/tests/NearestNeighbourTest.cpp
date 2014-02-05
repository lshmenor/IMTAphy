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

#include <WNS/tests/NearestNeighbourTest.hpp>
#include <WNS/container/Matrix.hpp>

using namespace wns;

CPPUNIT_TEST_SUITE_REGISTRATION( NearestNeighbourTest );

typedef container::Matrix<double, 1, double> Matrix1D;
typedef container::Matrix<double, 2, double> Matrix2D;
typedef container::Matrix<double, 3, double> Matrix3D;

void NearestNeighbourTest::setUp()
{
}

void NearestNeighbourTest::tearDown()
{
}

void NearestNeighbourTest::test1D()
{
    const Matrix1D::SizeType sizes[1] = {2};
    Matrix1D m = Matrix1D(sizes);
    m[0] = 0;
    m[1] = 1;
    n1 = new NearestNeighbour1D(m);
    CPPUNIT_ASSERT( (*n1).discretes()[0] == m[0] );
    CPPUNIT_ASSERT( (*n1).discretes()[1] == m[1] );
    CPPUNIT_ASSERT( (*n1)[0.0] == m[0] );
    CPPUNIT_ASSERT( (*n1)[1.0] == m[1] );
    CPPUNIT_ASSERT( (*n1)[0.4] == m[0] );
    CPPUNIT_ASSERT( (*n1)[0.5] == m[1] );
    m[1] = 2;
    CPPUNIT_ASSERT( (*n1).discretes()[1] == m[1] );
    CPPUNIT_ASSERT( (*n1)[0.9] == m[1] );
    delete n1;
}

void NearestNeighbourTest::test2D()
{
    const Matrix2D::SizeType sizes[2] = {2, 2};
    Matrix2D m = Matrix2D(sizes);
    m[0][0] = 0;
    m[0][1] = 1;
    m[1][0] = 2;
    m[1][1] = 3;
    n2 = new NearestNeighbour2D(m);
    CPPUNIT_ASSERT( (*n2).discretes()[0][0] == m[0][0] );
    CPPUNIT_ASSERT( (*n2).discretes()[0][1] == m[0][1] );
    CPPUNIT_ASSERT( (*n2).discretes()[1][0] == m[1][0] );
    CPPUNIT_ASSERT( (*n2).discretes()[1][1] == m[1][1] );
    CPPUNIT_ASSERT( (*n2)[0.0][0.0] == m[0][0] );
    CPPUNIT_ASSERT( (*n2)[0.0][1.0] == m[0][1] );
    CPPUNIT_ASSERT( (*n2)[1.0][0.0] == m[1][0] );
    CPPUNIT_ASSERT( (*n2)[1.0][1.0] == m[1][1] );
    CPPUNIT_ASSERT( (*n2)[0.4][0.5] == m[0][1] );
    CPPUNIT_ASSERT( (*n2)[0.5][0.4] == m[1][0] );
    m[1][1] = 4;
    CPPUNIT_ASSERT( (*n2).discretes()[1][1] == m[1][1] );
    CPPUNIT_ASSERT( (*n2)[0.9][0.8] == m[1][1] );
    delete n2;
}

void NearestNeighbourTest::test3D()
{
    const Matrix3D::SizeType sizes[3] = {2, 2, 2};
    Matrix3D m = Matrix3D(sizes);
    m[0][0][0] = 0;
    m[0][0][1] = 1;
    m[0][1][0] = 2;
    m[0][1][1] = 3;
    m[1][0][0] = 4;
    m[1][0][1] = 5;
    m[1][1][0] = 6;
    m[1][1][1] = 7;
    n3 = new NearestNeighbour3D(m);
    CPPUNIT_ASSERT( (*n3).discretes()[0][0][0] == m[0][0][0] );
    CPPUNIT_ASSERT( (*n3).discretes()[0][0][1] == m[0][0][1] );
    CPPUNIT_ASSERT( (*n3).discretes()[0][1][0] == m[0][1][0] );
    CPPUNIT_ASSERT( (*n3).discretes()[0][1][1] == m[0][1][1] );
    CPPUNIT_ASSERT( (*n3).discretes()[1][0][0] == m[1][0][0] );
    CPPUNIT_ASSERT( (*n3).discretes()[1][0][1] == m[1][0][1] );
    CPPUNIT_ASSERT( (*n3).discretes()[1][1][0] == m[1][1][0] );
    CPPUNIT_ASSERT( (*n3).discretes()[1][1][1] == m[1][1][1] );
    CPPUNIT_ASSERT( (*n3)[0.0][0.0][0.0] == m[0][0][0] );
    CPPUNIT_ASSERT( (*n3)[0.0][0.0][1.0] == m[0][0][1] );
    CPPUNIT_ASSERT( (*n3)[0.0][1.0][0.0] == m[0][1][0] );
    CPPUNIT_ASSERT( (*n3)[0.0][1.0][1.0] == m[0][1][1] );
    CPPUNIT_ASSERT( (*n3)[1.0][0.0][0.0] == m[1][0][0] );
    CPPUNIT_ASSERT( (*n3)[1.0][0.0][1.0] == m[1][0][1] );
    CPPUNIT_ASSERT( (*n3)[1.0][1.0][0.0] == m[1][1][0] );
    CPPUNIT_ASSERT( (*n3)[1.0][1.0][1.0] == m[1][1][1] );
    CPPUNIT_ASSERT( (*n3)[0.5][0.4][0.3] == m[1][0][0] );
    CPPUNIT_ASSERT( (*n3)[0.4][0.5][0.6] == m[0][1][1] );
    m[1][1][1] = 8;
    CPPUNIT_ASSERT( (*n3).discretes()[1][1][1] == m[1][1][1] );
    CPPUNIT_ASSERT( (*n3)[0.9][0.8][0.7] == m[1][1][1] );
    delete n3;
}
