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

#include <WNS/tests/DerefLessTest.hpp>

#include <string>

using namespace wns;

CPPUNIT_TEST_SUITE_REGISTRATION( DerefLessTest );


void
DerefLessTest::setUp()
{
} // setUp


void
DerefLessTest::tearDown()
{
} // tearDown


void
DerefLessTest::testIt()
{
	int i = 2;
	int j = 3;
	int* ip = &i;
	int* jp = &j;

	CPPUNIT_ASSERT( DerefLess<int*>()(ip, jp) );
	CPPUNIT_ASSERT( ! DerefLess<int*>()(jp, ip) );
	CPPUNIT_ASSERT( ! DerefLess<int*>()(ip, ip) );
} // testIt



