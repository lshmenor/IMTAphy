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

#include <WNS/tests/ClassTest.hpp>

using namespace std;
using namespace wns;

CPPUNIT_TEST_SUITE_REGISTRATION( ClassTest );

void ClassTest::setUp()
{
	d=0;
	i=0;
}

void ClassTest::tearDown()
{
}

void ClassTest::test() {
	CPPUNIT_ASSERT( 0==d );
	Class<double> foo( 3.2 );
	d = foo;
	CPPUNIT_ASSERT( 3.2==d );
	Class<double> foo2(d);
	CPPUNIT_ASSERT( foo2==d );
	i=5;
	d=5;
	CPPUNIT_ASSERT( d==i );
	double foo3 = d + i;
	CPPUNIT_ASSERT( foo3==10 );
	d += i;
	CPPUNIT_ASSERT( 10==d );
}


