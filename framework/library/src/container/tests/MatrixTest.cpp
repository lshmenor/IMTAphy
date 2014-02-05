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

#include <WNS/container/tests/MatrixTest.hpp>

using namespace wns::container::tests;

int MatrixTest::DestructorCounter::count = 0;

CPPUNIT_TEST_SUITE_REGISTRATION( MatrixTest );

void MatrixTest::setUp()
{
}

void MatrixTest::tearDown()
{
}

void MatrixTest::test1DDefaultConstructor()
{
    Matrix1D m1 = Matrix1D();
    CPPUNIT_ASSERT( m1.isEmpty() );
}

void MatrixTest::test1DSizeConstructor()
{
    const Matrix1D::SizeType sizes[1] = {2};
    Matrix1D m1 = Matrix1D(sizes);
    CPPUNIT_ASSERT( !(m1.isEmpty()) );
    CPPUNIT_ASSERT( m1[0] == Matrix1D::ValueType() );
    CPPUNIT_ASSERT( m1[1] == Matrix1D::ValueType() );
    m1 = Matrix1D(sizes, 3);
    CPPUNIT_ASSERT( !(m1.isEmpty()) );
    CPPUNIT_ASSERT( m1[0] == 3);
    CPPUNIT_ASSERT( m1[1] == 3);
}

void MatrixTest::test1DSize()
{
    Matrix1D m1 = Matrix1D();
    CPPUNIT_ASSERT( m1.dimSize(0) == 0 );
    CPPUNIT_ASSERT( m1.size() == 0 );
    const Matrix1D::SizeType sizes[1] = {2};
    m1 = Matrix1D(sizes);
    CPPUNIT_ASSERT( m1.dimSize(0) == sizes[0] );
    CPPUNIT_ASSERT( m1.size() == sizes[0] );
}

void MatrixTest::test1DAccessOperators()
{
    const Matrix1D::SizeType sizes[1] = {2};
    Matrix1D m1 = Matrix1D(sizes, 3);
    m1[1] = 4;
    CPPUNIT_ASSERT( m1[0] == 3 );
    CPPUNIT_ASSERT( m1[1] == 4 );
}

void MatrixTest::test2DDefaultConstructor()
{
    Matrix2D m2 = Matrix2D();
    CPPUNIT_ASSERT( m2.isEmpty() );
}

void MatrixTest::test2DSizeConstructor()
{
    const Matrix2D::SizeType sizes[2] = {2, 2};
    Matrix2D m2 = Matrix2D(sizes);
    CPPUNIT_ASSERT( !(m2.isEmpty()) );
    CPPUNIT_ASSERT( !(m2[0].isEmpty()) );
    CPPUNIT_ASSERT( m2[0][0] == Matrix2D::ValueType() );
    CPPUNIT_ASSERT( m2[0][1] == Matrix2D::ValueType() );
    CPPUNIT_ASSERT( m2[1][0] == Matrix2D::ValueType() );
    CPPUNIT_ASSERT( m2[1][1] == Matrix2D::ValueType() );
    m2 = Matrix2D(sizes, 3);
    CPPUNIT_ASSERT( !(m2.isEmpty()) );
    CPPUNIT_ASSERT( !(m2[0].isEmpty()) );
    CPPUNIT_ASSERT( m2[0][0] == 3);
    CPPUNIT_ASSERT( m2[0][1] == 3);
    CPPUNIT_ASSERT( m2[1][0] == 3);
    CPPUNIT_ASSERT( m2[1][1] == 3);
}

void MatrixTest::test2DSize()
{
    Matrix2D m2 = Matrix2D();
    CPPUNIT_ASSERT( m2.dimSize(0) == 0 );
    CPPUNIT_ASSERT( m2.dimSize(1) == 0 );
    CPPUNIT_ASSERT( m2.size() == 0 );
    const Matrix2D::SizeType sizes[2] = {2, 3};
    m2 = Matrix2D(sizes);
    CPPUNIT_ASSERT( m2.dimSize(0) == sizes[0] );
    CPPUNIT_ASSERT( m2.dimSize(1) == sizes[1] );
    CPPUNIT_ASSERT( m2.size() == (sizes[0] * sizes[1]) );
}

void MatrixTest::test2DAccessOperators()
{
    const Matrix2D::SizeType sizes[2] = {2, 3};
    Matrix2D m2 = Matrix2D(sizes, 3);
    m2[0][1] = 4;
    m2[1][2] = 5;
    CPPUNIT_ASSERT( m2[0][0] == 3 );
    CPPUNIT_ASSERT( m2[0][1] == 4 );
    CPPUNIT_ASSERT( m2[0][2] == 3 );
    CPPUNIT_ASSERT( m2[1][0] == 3 );
    CPPUNIT_ASSERT( m2[1][1] == 3 );
    CPPUNIT_ASSERT( m2[1][2] == 5 );
}

void MatrixTest::test3DDefaultConstructor()
{
    Matrix3D m3 = Matrix3D();
    CPPUNIT_ASSERT( m3.isEmpty() );
}

void MatrixTest::test3DSizeConstructor()
{
    const Matrix3D::SizeType sizes[3] = {2, 2, 2};
    Matrix3D m3 =  Matrix3D(sizes);
    CPPUNIT_ASSERT( !(m3.isEmpty()) );
    CPPUNIT_ASSERT( !(m3[0].isEmpty()) );
    CPPUNIT_ASSERT( !(m3[0][0].isEmpty()) );
    CPPUNIT_ASSERT( m3[0][0][0] == Matrix3D::ValueType() );
    CPPUNIT_ASSERT( m3[0][0][1] == Matrix3D::ValueType() );
    CPPUNIT_ASSERT( m3[0][1][0] == Matrix3D::ValueType() );
    CPPUNIT_ASSERT( m3[0][1][1] == Matrix3D::ValueType() );
    CPPUNIT_ASSERT( m3[1][0][0] == Matrix3D::ValueType() );
    CPPUNIT_ASSERT( m3[1][0][1] == Matrix3D::ValueType() );
    CPPUNIT_ASSERT( m3[1][1][0] == Matrix3D::ValueType() );
    CPPUNIT_ASSERT( m3[1][1][1] == Matrix3D::ValueType() );
    m3 = Matrix3D(sizes, 3);
    CPPUNIT_ASSERT( !(m3.isEmpty()) );
    CPPUNIT_ASSERT( !(m3[0].isEmpty()) );
    CPPUNIT_ASSERT( !(m3[0][0].isEmpty()) );
    CPPUNIT_ASSERT( m3[0][0][0] == 3);
    CPPUNIT_ASSERT( m3[0][0][1] == 3);
    CPPUNIT_ASSERT( m3[0][1][0] == 3);
    CPPUNIT_ASSERT( m3[0][1][1] == 3);
    CPPUNIT_ASSERT( m3[1][0][0] == 3);
    CPPUNIT_ASSERT( m3[1][0][1] == 3);
    CPPUNIT_ASSERT( m3[1][1][0] == 3);
    CPPUNIT_ASSERT( m3[1][1][1] == 3);
}

void MatrixTest::test3DSize()
{
    Matrix3D m3 = Matrix3D();
    CPPUNIT_ASSERT( m3.dimSize(0) == 0 );
    CPPUNIT_ASSERT( m3.dimSize(1) == 0 );
    CPPUNIT_ASSERT( m3.dimSize(2) == 0 );
    CPPUNIT_ASSERT( m3.size() == 0 );
    const Matrix3D::SizeType sizes[3] = {2, 3, 4};
    m3 = Matrix3D(sizes);
    CPPUNIT_ASSERT( m3.dimSize(0) == sizes[0] );
    CPPUNIT_ASSERT( m3.dimSize(1) == sizes[1] );
    CPPUNIT_ASSERT( m3.dimSize(2) == sizes[2] );
    CPPUNIT_ASSERT( m3.size() == (sizes[0] * sizes[1] * sizes[2]) );
}

void MatrixTest::test3DAccessOperators()
{
    const Matrix3D::SizeType sizes[3] = {2, 3, 3};
    Matrix3D m3 = Matrix3D(sizes, 3);
    m3[0][1][2] = 4;
    m3[1][2][2] = 5;
    CPPUNIT_ASSERT( m3[0][0][0] == 3 );
    CPPUNIT_ASSERT( m3[0][0][1] == 3 );
    CPPUNIT_ASSERT( m3[0][0][2] == 3 );
    CPPUNIT_ASSERT( m3[0][1][0] == 3 );
    CPPUNIT_ASSERT( m3[0][1][1] == 3 );
    CPPUNIT_ASSERT( m3[0][1][2] == 4 );
    CPPUNIT_ASSERT( m3[0][2][0] == 3 );
    CPPUNIT_ASSERT( m3[0][2][1] == 3 );
    CPPUNIT_ASSERT( m3[0][2][2] == 3 );
    CPPUNIT_ASSERT( m3[1][0][0] == 3 );
    CPPUNIT_ASSERT( m3[1][0][1] == 3 );
    CPPUNIT_ASSERT( m3[1][0][2] == 3 );
    CPPUNIT_ASSERT( m3[1][1][0] == 3 );
    CPPUNIT_ASSERT( m3[1][1][1] == 3 );
    CPPUNIT_ASSERT( m3[1][1][2] == 3 );
    CPPUNIT_ASSERT( m3[1][2][0] == 3 );
    CPPUNIT_ASSERT( m3[1][2][1] == 3 );
    CPPUNIT_ASSERT( m3[1][2][2] == 5 );
}

void MatrixTest::testReturnReference()
{
    const Matrix3D::SizeType sizes[3] = {2, 3, 3};
    Matrix<DestructorCounter, 3> m3(sizes);
    m3[0][1][2] = DestructorCounter();
    m3[1][2][2] = DestructorCounter();

    DestructorCounter::count = 0;

    m3[0][1][2];

    CPPUNIT_ASSERT_EQUAL(0, DestructorCounter::count);
}

void MatrixTest::testEqual()
{

    const Matrix1D::SizeType sizes[1] = {2};
    Matrix1D m1 = Matrix1D(sizes, 3);
    const Matrix1D::SizeType sizes2[1] = {2};
    Matrix1D m2 = Matrix1D(sizes2, 3);
    const Matrix1D::SizeType sizes3[1] = {2};
    Matrix1D m3 = Matrix1D(sizes3, 7);

    CPPUNIT_ASSERT_EQUAL(m1==m2, true);
    CPPUNIT_ASSERT_EQUAL(m1==m3, false);

    const Matrix2D::SizeType sizes4[2] = {2, 3};
    Matrix2D m4 = Matrix2D(sizes4, 3);
    const Matrix2D::SizeType sizes5[2] = {2, 3};
    Matrix2D m5 = Matrix2D(sizes5, 3);
    CPPUNIT_ASSERT_EQUAL(m4==m5, true);

    m4[0][1] = 4;
    m4[1][2] = 5;
    CPPUNIT_ASSERT_EQUAL(m4==m5, false);

    const Matrix3D::SizeType sizes6[3] = {2, 2, 2};
    Matrix3D m6 =  Matrix3D(sizes6, 3);
    Matrix3D m7 =  Matrix3D(sizes6, 3);
    CPPUNIT_ASSERT_EQUAL(m6==m7, true);

    m7[0][0][0] = 4;
    CPPUNIT_ASSERT_EQUAL(m6==m7, false);
}
