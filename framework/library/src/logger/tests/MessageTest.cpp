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

#include "MessageTest.hpp"

using namespace wns::logger;
using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( MessageTest );

void MessageTest::setUp()
{}

void MessageTest::tearDown()
{}

void MessageTest::constructor()
{
	Message m; // Default Constructor
	CPPUNIT_ASSERT( m.getString() == "" );
	Message m2("foobar"); // simple constructor
	CPPUNIT_ASSERT( m2.getString() == "foobar" );
	Message m3(m2); // copy constructor
	CPPUNIT_ASSERT( m3.getString() == "foobar" );
	Message m4 = m2; // copy constructor, part 2
	CPPUNIT_ASSERT( m4.getString() == "foobar" );
	CPPUNIT_ASSERT( m4.getLevel() == 0 ); // 0 is the default level.


	Message m5("test",3); // test setting string and level
	CPPUNIT_ASSERT( m5.getString() == "test" );
	CPPUNIT_ASSERT( m5.getLevel() == 3 );


}

void MessageTest::getters()
{
	Message m("foobar",2);
	CPPUNIT_ASSERT( m.getString() == "foobar" );
	CPPUNIT_ASSERT( m.getLevel()  == 2 );
}

void MessageTest::stream()
{
	Message m("foobar");
	m << " foobaz";
	CPPUNIT_ASSERT( m.getString() == "foobar foobaz" );
 	stringstream ss;
 	ss << m;
 	CPPUNIT_ASSERT( ss.str() == "foobar foobaz" );
}


