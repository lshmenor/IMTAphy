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

#ifndef WNS_DISTRIBUTION_TEST_OPERATION_HPP
#define WNS_DISTRIBUTION_TEST_OPERATION_HPP

#include <WNS/distribution/Operation.hpp>

#include <WNS/TestFixture.hpp>

namespace wns { namespace distribution { namespace test {

    class OperationTest :
        public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE( OperationTest );
        CPPUNIT_TEST( testFixedAdd );
        CPPUNIT_TEST( testAdd );
        CPPUNIT_TEST( testFixedMul );
        CPPUNIT_TEST( testMul );
        CPPUNIT_TEST( testFixedSub );
        CPPUNIT_TEST( testFixedDiv );
        CPPUNIT_TEST( testConstant );
        CPPUNIT_TEST( testAbove );
        CPPUNIT_TEST( testBelow );
        CPPUNIT_TEST_SUITE_END();
    public:
        void setUp();
        void tearDown();

        void testFixedAdd();
        void testAdd();
        void testFixedMul();
        void testMul();
        void testFixedSub();
        void testFixedDiv();
        void testConstant();
        void testAbove();
        void testBelow();
    private:
    };
}
}
}

#endif // NOT defined WNS_DISTRIBUTION_TEST_OPERATION_HPP

/*
  Local Variables:
  mode: c++
  fill-column: 80
  c-basic-offset: 8
  c-comment-only-line-offset: 0
  c-tab-always-indent: t
  indent-tabs-mode: t
  tab-width: 8
  End:
*/
