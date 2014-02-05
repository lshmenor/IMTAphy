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

#include <WNS/tests/EnumTest.hpp>

using namespace wns;
using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( EnumTest );

void EnumTest::setUp()
{}

void EnumTest::tearDown()
{}

void EnumTest::testConvertToInt()
{
	CPPUNIT_ASSERT_EQUAL( 0, static_cast<int>(ApplicationTest::WWW()) );
	CPPUNIT_ASSERT_EQUAL( 1, (int)ApplicationTest::FTP() );
	CPPUNIT_ASSERT_EQUAL( 0, (int)TLTest::TCP() );
	CPPUNIT_ASSERT_EQUAL( 1, static_cast<int>(TLTest::UDP()) );
}

void EnumTest::testConvertToString()
{
	CPPUNIT_ASSERT( ApplicationTest::toString(0) == "WWW" );
	CPPUNIT_ASSERT( ApplicationTest::toString(1) == "FTP" );
	CPPUNIT_ASSERT( TLTest::toString(0) == "TCP" );
	CPPUNIT_ASSERT( TLTest::toString(1) == "UDP" );
	CPPUNIT_ASSERT( ApplicationTest::toString(ApplicationTest::WWW()) == "WWW" );
	CPPUNIT_ASSERT( ApplicationTest::toString(ApplicationTest::FTP()) == "FTP" );
	CPPUNIT_ASSERT( TLTest::toString(TLTest::TCP()) == "TCP" );
	CPPUNIT_ASSERT( TLTest::toString(TLTest::UDP()) == "UDP" );
}

void EnumTest::testConvertFromString()
{
	CPPUNIT_ASSERT_EQUAL( 0, ApplicationTest::fromString("WWW") );
	CPPUNIT_ASSERT_EQUAL( 1, ApplicationTest::fromString("FTP") );
	CPPUNIT_ASSERT_EQUAL( 0, TLTest::fromString("TCP") );
	CPPUNIT_ASSERT_EQUAL( 1, TLTest::fromString("UDP") );
}

