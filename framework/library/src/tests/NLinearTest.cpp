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

#include <cmath>
#include <iostream>
#include "NLinearTest.hpp"

using namespace wns;
using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( NLinearTest );

// This asserts A and B are equal +- 1E-10%
#define ASSERT_DOUBLES_EQUAL(A, B) { \
  double a = (A), b = (B); \
  CPPUNIT_ASSERT_DOUBLES_EQUAL(a, b, a/1.0E12); \
  }

void NLinearTest::setUp()
{
    const Linear::InitType::SizeType sizes1[1] = {2};
    const Bilinear::InitType::SizeType sizes2[2] = {2, 2};
    const Trilinear::InitType::SizeType sizes3[3] = {2, 2, 2};
    m1 = Linear::InitType(sizes1);
    m2 = Bilinear::InitType(sizes2);
    m3 = Trilinear::InitType(sizes3);

    m1[0] = 0;
    m1[1] = 1;

    m2[0][0] = 0;
    m2[0][1] = 1;
    m2[1][0] = 2;
    m2[1][1] = 3;

    m3[0][0][0] = 0;
    m3[0][0][1] = 1;
    m3[0][1][0] = 2;
    m3[0][1][1] = 3;
    m3[1][0][0] = 4;
    m3[1][0][1] = 5;
    m3[1][1][0] = 6;
    m3[1][1][1] = 7;

    linear = new Linear(m1);
    bilinear = new Bilinear(m2);
    trilinear = new Trilinear(m3);
}

void NLinearTest::tearDown()
{
    delete linear;
    delete bilinear;
    delete trilinear;
}

void NLinearTest::testLinear()
{
    ASSERT_DOUBLES_EQUAL( (*linear).discretes()[0] , m1[0] );
    ASSERT_DOUBLES_EQUAL( (*linear).discretes()[1] , m1[1] );
    ASSERT_DOUBLES_EQUAL( (*linear)[0.0] , m1[0] );
    ASSERT_DOUBLES_EQUAL( (*linear)[1.0] , m1[1] );
    ASSERT_DOUBLES_EQUAL( (*linear)[0.3] , linearInterpolation(0.3, m1) );
    m1[1] = 1.5;
    ASSERT_DOUBLES_EQUAL( (*linear).discretes()[1] , m1[1] );
    ASSERT_DOUBLES_EQUAL( (*linear)[1.0] , m1[1] );
    ASSERT_DOUBLES_EQUAL( (*linear)[0.5] , linearInterpolation(0.5, m1) );
}

void NLinearTest::testBilinear()
{
    ASSERT_DOUBLES_EQUAL( (*bilinear).discretes()[0][0] , m2[0][0] );
    ASSERT_DOUBLES_EQUAL( (*bilinear).discretes()[0][1] , m2[0][1] );
    ASSERT_DOUBLES_EQUAL( (*bilinear).discretes()[1][0] , m2[1][0] );
    ASSERT_DOUBLES_EQUAL( (*bilinear).discretes()[1][1] , m2[1][1] );
    ASSERT_DOUBLES_EQUAL( (*bilinear)[0.0][0.0] , m2[0][0] );
    ASSERT_DOUBLES_EQUAL( (*bilinear)[0.0][1.0] , m2[0][1] );
    ASSERT_DOUBLES_EQUAL( (*bilinear)[1.0][0.0] , m2[1][0] );
    ASSERT_DOUBLES_EQUAL( (*bilinear)[1.0][1.0] , m2[1][1] );

    ASSERT_DOUBLES_EQUAL( bilinearInterpolation(0.0, 0.5, m2) , 0.5);
    ASSERT_DOUBLES_EQUAL( bilinearInterpolation(1.0, 0.5, m2) , 2.5);
    ASSERT_DOUBLES_EQUAL( bilinearInterpolation(0.5, 0.0, m2) , 1.0);
    ASSERT_DOUBLES_EQUAL( bilinearInterpolation(0.5, 1.0, m2) , 2.0);
    ASSERT_DOUBLES_EQUAL( bilinearInterpolation(0.5, 0.5, m2) , 1.5);
 
    ASSERT_DOUBLES_EQUAL( (*bilinear)[0][0.5] , linearInterpolation(0.5, m2[0]) );
    ASSERT_DOUBLES_EQUAL( (*bilinear)[0][0.5] , bilinearInterpolation(0.0, 0.5, m2) );
    ASSERT_DOUBLES_EQUAL( (*bilinear)[0.5][0.0] , bilinearInterpolation(0.5, 0.0, m2) );
    ASSERT_DOUBLES_EQUAL( (*bilinear)[0.2][0.4] , bilinearInterpolation(0.2, 0.4, m2) );
    m2[1][1] = 7.5;
    ASSERT_DOUBLES_EQUAL( (*bilinear).discretes()[1][1] , m2[1][1] );
    ASSERT_DOUBLES_EQUAL( (*bilinear)[1.0][1.0] , m2[1][1] );
    ASSERT_DOUBLES_EQUAL( (*bilinear)[1.0][0.8] , bilinearInterpolation(1.0, 0.8, m2) );
    ASSERT_DOUBLES_EQUAL( (*bilinear)[0.8][0.6], bilinearInterpolation(0.8, 0.6, m2) );
}

void NLinearTest::testTrilinear()
{
    ASSERT_DOUBLES_EQUAL( (*trilinear).discretes()[0][0][0] , m3[0][0][0] );
    ASSERT_DOUBLES_EQUAL( (*trilinear).discretes()[0][0][1] , m3[0][0][1] );
    ASSERT_DOUBLES_EQUAL( (*trilinear).discretes()[0][1][0] , m3[0][1][0] );
    ASSERT_DOUBLES_EQUAL( (*trilinear).discretes()[0][1][1] , m3[0][1][1] );
    ASSERT_DOUBLES_EQUAL( (*trilinear).discretes()[1][0][0] , m3[1][0][0] );
    ASSERT_DOUBLES_EQUAL( (*trilinear).discretes()[1][0][1] , m3[1][0][1] );
    ASSERT_DOUBLES_EQUAL( (*trilinear).discretes()[1][1][0] , m3[1][1][0] );
    ASSERT_DOUBLES_EQUAL( (*trilinear).discretes()[1][1][1] , m3[1][1][1] );

    ASSERT_DOUBLES_EQUAL( trilinearInterpolation(0.0, 0.0, 0.5, m3) , 0.5 );
    ASSERT_DOUBLES_EQUAL( trilinearInterpolation(0.0, 1.0, 0.5, m3) , 2.5 );
    ASSERT_DOUBLES_EQUAL( trilinearInterpolation(0.0, 0.5, 0.0, m3) , 1.0 );
    ASSERT_DOUBLES_EQUAL( trilinearInterpolation(0.0, 0.5, 1.0, m3) , 2.0 );
    ASSERT_DOUBLES_EQUAL( trilinearInterpolation(1.0, 0.0, 0.5, m3) , 4.5 );
    ASSERT_DOUBLES_EQUAL( trilinearInterpolation(1.0, 1.0, 0.5, m3) , 6.5 );
    ASSERT_DOUBLES_EQUAL( trilinearInterpolation(1.0, 0.5, 0.0, m3) , 5.0 );
    ASSERT_DOUBLES_EQUAL( trilinearInterpolation(1.0, 0.5, 1.0, m3) , 6.0 );
    ASSERT_DOUBLES_EQUAL( trilinearInterpolation(0.5, 0.0, 0.0, m3) , 2.0 );
    ASSERT_DOUBLES_EQUAL( trilinearInterpolation(0.5, 0.0, 1.0, m3) , 3.0 );
    ASSERT_DOUBLES_EQUAL( trilinearInterpolation(0.5, 1.0, 0.0, m3) , 4.0 );
    ASSERT_DOUBLES_EQUAL( trilinearInterpolation(0.5, 1.0, 1.0, m3) , 5.0 );
    ASSERT_DOUBLES_EQUAL( trilinearInterpolation(0.0, 0.5, 0.5, m3) , 1.5 );
    ASSERT_DOUBLES_EQUAL( trilinearInterpolation(1.0, 0.5, 0.5, m3) , 5.5 );
    ASSERT_DOUBLES_EQUAL( trilinearInterpolation(0.5, 0.5, 0.5, m3) , 3.5 );

    ASSERT_DOUBLES_EQUAL( (*trilinear)[0.0][0.0][0.0] , m3[0][0][0] );
    ASSERT_DOUBLES_EQUAL( (*trilinear)[0.0][0.0][1.0] , m3[0][0][1] );
    ASSERT_DOUBLES_EQUAL( (*trilinear)[0.0][1.0][0.0] , m3[0][1][0] );
    ASSERT_DOUBLES_EQUAL( (*trilinear)[0.0][1.0][1.0] , m3[0][1][1] );
    ASSERT_DOUBLES_EQUAL( (*trilinear)[1.0][0.0][0.0] , m3[1][0][0] );
    ASSERT_DOUBLES_EQUAL( (*trilinear)[1.0][0.0][1.0] , m3[1][0][1] );
    ASSERT_DOUBLES_EQUAL( (*trilinear)[1.0][1.0][0.0] , m3[1][1][0] );
    ASSERT_DOUBLES_EQUAL( (*trilinear)[1.0][1.0][1.0] , m3[1][1][1] );

    ASSERT_DOUBLES_EQUAL( (*trilinear)[0][0][0.5] , linearInterpolation(0.5, m3[0][0]) );
    ASSERT_DOUBLES_EQUAL( (*trilinear)[0][0][0.5] , bilinearInterpolation(0.0, 0.5, m3[0]) );
    ASSERT_DOUBLES_EQUAL( (*trilinear)[0][0][0.5] , trilinearInterpolation(0.0, 0.0, 0.5, m3) );

    ASSERT_DOUBLES_EQUAL( (*trilinear)[0.5][0.7][0.0], trilinearInterpolation(0.5, 0.7, 0.0, m3) );
    ASSERT_DOUBLES_EQUAL( (*trilinear)[0.2][0.4][0.6], trilinearInterpolation(0.2, 0.4, 0.6, m3) );

    m3[1][1][1] = 7.5;
    ASSERT_DOUBLES_EQUAL( (*trilinear).discretes()[1][1][1] , m3[1][1][1] );
    ASSERT_DOUBLES_EQUAL( (*trilinear)[1.0][1.0][1.0] , m3[1][1][1] );
    ASSERT_DOUBLES_EQUAL( (*trilinear)[1.0][0.8][0.6] , trilinearInterpolation(1.0, 0.8, 0.6, m3) );
}

// the following functions are modelled from the corresponding
// entries in en.wikipedia.org

NLinearTest::Linear::ValueType NLinearTest::linearInterpolation(const NLinearTest::Linear::CoordType& x,
								const Linear::InitType& m)
{
    const Linear::DiscreteContainer::IndexType x0 = (Linear::DiscreteContainer::IndexType)(floor(x));
    const Linear::DiscreteContainer::IndexType x1 = (Linear::DiscreteContainer::IndexType)(ceil(x));

    const Linear::InitType::ValueType& y0 = m[x0];
    const Linear::InitType::ValueType& y1 = m[x1];
    return y0 + ((y1 - y0)/(x1 - x0)) * (x - x0);
}

NLinearTest::Bilinear::ValueType NLinearTest::bilinearInterpolation(const NLinearTest::Bilinear::CoordType& x,
								    const NLinearTest::Bilinear::CoordType& y,
								    const Bilinear::InitType& m)
{
    const Bilinear::DiscreteContainer::IndexType x1 = (Bilinear::DiscreteContainer::IndexType)(floor(x));
    Bilinear::DiscreteContainer::IndexType x2 = (Bilinear::DiscreteContainer::IndexType)(ceil(x));
    const Bilinear::DiscreteContainer::IndexType y1 = (Bilinear::DiscreteContainer::IndexType)(floor(y));
    Bilinear::DiscreteContainer::IndexType y2 = (Bilinear::DiscreteContainer::IndexType)(ceil(y));
    // Q11 = (x1, y1), Q12 = (x1, y2) ...
    const Linear::InitType::ValueType& fQ11 = m[x1][y1];
    const Linear::InitType::ValueType& fQ12 = m[x1][y2];
    const Linear::InitType::ValueType& fQ21 = m[x2][y1];
    const Linear::InitType::ValueType& fQ22 = m[x2][y2];
    // R1 = (x, y1), R2 = (x, y2)
    const Bilinear::ValueType fR1 = (x1 == x2) ? fQ11 :
                                      ((x - x2)/(double)(long int)(x1 - x2)) * fQ11
				    + ((x - x1)/(double)(long int)(x2 - x1)) * fQ21;
    const Bilinear::ValueType fR2 = (x1 == x2) ? fQ12 :
                                      ((x - x2)/(double)(long int)(x1 - x2)) * fQ12
				    + ((x - x1)/(double)(long int)(x2 - x1)) * fQ22;
    return (y1 == y2) ? fR1 : ((y - y2)/(double)(long int)(y1 - y2)) * fR1
                            + ((y - y1)/(double)(long int)(y2 - y1)) * fR2;
}

NLinearTest::Trilinear::ValueType NLinearTest::trilinearInterpolation(const NLinearTest::Trilinear::CoordType& x,
								      const NLinearTest::Trilinear::CoordType& y,
								      const NLinearTest::Trilinear::CoordType& z,
								      const Trilinear::InitType& m)
{
    // x1 = LowerGauss(x), x2 = UpperGauss(x)
    const Trilinear::DiscreteContainer::IndexType x1 = (Bilinear::DiscreteContainer::IndexType)(floor(x));
    const Trilinear::DiscreteContainer::IndexType x2 = (Bilinear::DiscreteContainer::IndexType)(ceil(x));
    const Trilinear::DiscreteContainer::IndexType y1 = (Bilinear::DiscreteContainer::IndexType)(floor(y));
    const Trilinear::DiscreteContainer::IndexType y2 = (Bilinear::DiscreteContainer::IndexType)(ceil(y));
    const Trilinear::DiscreteContainer::IndexType z1 = (Bilinear::DiscreteContainer::IndexType)(floor(z));
    const Trilinear::DiscreteContainer::IndexType z2 = (Bilinear::DiscreteContainer::IndexType)(ceil(z));

    const Trilinear::CoordType xf = x - x1;
    const Trilinear::CoordType yf = y - y1;
    const Trilinear::CoordType zf = z - z1;

    const Trilinear::ValueType i1 = m[x1][y1][z1] * (1.0 - zf) + m[x1][y1][z2] * zf;
    const Trilinear::ValueType i2 = m[x1][y2][z1] * (1.0 - zf) + m[x1][y2][z2] * zf;
    const Trilinear::ValueType j1 = m[x2][y1][z1] * (1.0 - zf) + m[x2][y1][z2] * zf;
    const Trilinear::ValueType j2 = m[x2][y2][z1] * (1.0 - zf) + m[x2][y2][z2] * zf;

    const Trilinear::ValueType w1 = i1 * (1.0 - yf) + i2 * yf;
    const Trilinear::ValueType w2 = j1 * (1.0 - yf) + j2 * yf;

    return w1 * (1 - xf) + w2 * xf;
}
