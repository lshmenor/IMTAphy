/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 16, D-52074 Aachen, Germany
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

#include <WNS/container/DynamicMatrix.hpp>
#include <WNS/TestFixture.hpp>
#include <WNS/simulator/ISimulator.hpp>

namespace wns { namespace container { namespace tests {

    class V
    {
        int member;
    public:
        V() : member(42)
        {
        }
        explicit
        V(int m) : member(m)
        {
        }
        int get() const
        {
            return member;
        }
    };

    class DynamicMatrixTest :
        public wns::TestFixture
    {

        CPPUNIT_TEST_SUITE( DynamicMatrixTest );
        CPPUNIT_TEST( construct );
        CPPUNIT_TEST( setget );
        CPPUNIT_TEST( outofrange );
        CPPUNIT_TEST_SUITE_END();
    public:
        void prepare();
        void cleanup();

        void construct();
        void setget();
        void outofrange();
    };

} // tests
} // container
} // wns

using namespace wns::container::tests;

CPPUNIT_TEST_SUITE_REGISTRATION( DynamicMatrixTest );

void
DynamicMatrixTest::prepare()
{
}

void
DynamicMatrixTest::cleanup()
{
}


void DynamicMatrixTest::construct()
{
    std::list<int> dimensions;
    dimensions.push_back(2);
    dimensions.push_back(3);
    dimensions.push_back(4);

    DynamicMatrix<int> i(dimensions);
    DynamicMatrix<double> d(dimensions);
    DynamicMatrix<V> v(dimensions);

    for (int x = 0; x < 2; ++x)
    {
        for (int y = 0; y < 3; ++y)
        {
            for (int z = 0; z < 4; ++z)
            {
                std::list<int> indices;
                indices.push_back(x);
                indices.push_back(y);
                indices.push_back(z);
                CPPUNIT_ASSERT_EQUAL( 0, i.getValue(indices) );
                CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, d.getValue(indices), 1e-8 );
                CPPUNIT_ASSERT_EQUAL( 42, v.getValue(indices).get() );
            }
        }
    }
}

void DynamicMatrixTest::setget()
{
    std::list<int> dimensions;
    dimensions.push_back(2);
    dimensions.push_back(3);
    dimensions.push_back(4);

    DynamicMatrix<V> v(dimensions);

    V something(13);

    std::list<int> indices;
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(3);

    v.setValue(indices, something);
    CPPUNIT_ASSERT_EQUAL( 13, v.getValue(indices).get() );
}

void  DynamicMatrixTest::outofrange()
{
    std::list<int> dimensions;
    dimensions.push_back(2);
    dimensions.push_back(3);
    dimensions.push_back(4);

    DynamicMatrix<V> v(dimensions);

    std::list<int> indices;
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(4); // index out of range

    CPPUNIT_ASSERT_THROW( v.getValue(indices), std::out_of_range );
}
