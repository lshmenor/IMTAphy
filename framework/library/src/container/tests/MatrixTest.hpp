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

#include <WNS/container/Matrix.hpp>
#include <cppunit/extensions/HelperMacros.h>
#include <stdint.h>

#ifndef WNS_CONTAINER_TESTS_MATRIXTEST_HPP
#define WNS_CONTAINER_TESTS_MATRIXTEST_HPP

namespace wns { namespace container { namespace tests {

    class MatrixTest :
        public CppUnit::TestFixture
    {
        class DestructorCounter
        {
        public:
            ~DestructorCounter()
            {
                count++;
            }

            bool
            operator==(const DestructorCounter&) const
            {
                return true;
            }

            bool
            operator!=(const DestructorCounter&) const
            {
                return false;
            }

            static int count;
        };

        CPPUNIT_TEST_SUITE( MatrixTest );
        CPPUNIT_TEST( test1DDefaultConstructor );
        CPPUNIT_TEST( test1DSizeConstructor );
        CPPUNIT_TEST( test1DSize );
        CPPUNIT_TEST( test1DAccessOperators );
        CPPUNIT_TEST( test2DDefaultConstructor );
        CPPUNIT_TEST( test2DSizeConstructor );
        CPPUNIT_TEST( test2DSize );
        CPPUNIT_TEST( test2DAccessOperators );
        CPPUNIT_TEST( test3DDefaultConstructor );
        CPPUNIT_TEST( test3DSizeConstructor );
        CPPUNIT_TEST( test3DSize );
        CPPUNIT_TEST( test3DAccessOperators );
        CPPUNIT_TEST( testReturnReference );
        CPPUNIT_TEST( testEqual );
        CPPUNIT_TEST_SUITE_END();
    public:
        void setUp();
        void tearDown();
        void test1DDefaultConstructor();
        void test1DSizeConstructor();
        void test1DSize();
        void test1DAccessOperators();
        void test2DDefaultConstructor();
        void test2DSizeConstructor();
        void test2DSize();
        void test2DAccessOperators();
        void test3DDefaultConstructor();
        void test3DSizeConstructor();
        void test3DSize();
        void test3DAccessOperators();
        void testReturnReference();
        void testEqual();
    private:
        typedef Matrix<long int, 1> Matrix1D;
        typedef Matrix<long int, 2> Matrix2D;
        typedef Matrix<long int, 3> Matrix3D;
    };

}
}
}

#endif // NOT defined WNS_CONTAINER_TESTS_MATRIXTEST_HPP
