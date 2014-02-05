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


#include <cppunit/extensions/HelperMacros.h>
#include <WNS/NearestNeighbour.hpp>

#ifndef classNearestNeighbourTest
#define classNearestNeighbourTest

namespace wns {

    class NearestNeighbourTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE( NearestNeighbourTest );
		CPPUNIT_TEST( test1D );
		CPPUNIT_TEST( test2D );
		CPPUNIT_TEST( test3D );
		CPPUNIT_TEST_SUITE_END();
    public:
		void setUp();
		void tearDown();
		void test1D();
		void test2D();
		void test3D();

    private:

		typedef NearestNeighbour<double, 1> NearestNeighbour1D;
		typedef NearestNeighbour<double, 2> NearestNeighbour2D;
		typedef NearestNeighbour<double, 3> NearestNeighbour3D;

		NearestNeighbour1D* n1;
		NearestNeighbour2D* n2;
		NearestNeighbour3D* n3;
    };
}

#endif // classNearestNeighbourTest
