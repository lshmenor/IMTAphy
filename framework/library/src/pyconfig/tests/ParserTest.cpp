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

#include "ParserTest.hpp"

#include <iostream>
#include <vector>
#include <list>
#include <deque>
#include <set>

using namespace wns::pyconfig::tests;

CPPUNIT_TEST_SUITE_REGISTRATION( ParserTest );

void
ParserTest::setUp()
{
} // setUp


void
ParserTest::tearDown()
{
} // tearDown


void
ParserTest::testLoadString()
{
	Parser pyco;
	pyco.loadString(
		"foo = 42\n");

	long i;
	CPPUNIT_ASSERT(pyco.get(i, "foo"));
	CPPUNIT_ASSERT_EQUAL(42L, i);
} // testLoadString


void
ParserTest::testFromString()
{
	View v = Parser::fromString("test=42\n");
	CPPUNIT_ASSERT_EQUAL(42, v.get<int>("test"));
} // testFromString


void
ParserTest::testAppendPath()
{
	{
		Parser p;
		p.appendPath("/foo/bar");
		p.loadString("import sys");
		int pathLength = p.len("sys.path");
		std::string s = p.get<std::string>("sys.path", pathLength-1);
		CPPUNIT_ASSERT( s == "/foo/bar" );
	}

	{
		// fire up second parser after the first parser was deleted
		// the second parser will also have /foo/bar in its path
		Parser p;
		p.loadString("import sys");
		int pathLength = p.len("sys.path");
		std::string s = p.get<std::string>("sys.path", pathLength-1);
		CPPUNIT_ASSERT( s == "/foo/bar" );
	}
}


