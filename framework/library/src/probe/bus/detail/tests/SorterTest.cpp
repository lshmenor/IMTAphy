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

#include <WNS/probe/bus/detail/Sorter.hpp>

#include <WNS/pyconfig/Parser.hpp>

#include <WNS/TestFixture.hpp>
#include <sstream>

namespace wns { namespace probe { namespace bus { namespace detail { namespace tests {

    /**
     * @brief Tests for the Sorter
     * @author Ralf Pabst <pab@comnets.rwth-aachen.de>
     */
    class SorterTest : public wns::TestFixture  {
        CPPUNIT_TEST_SUITE( SorterTest );
        CPPUNIT_TEST( constructor );
        CPPUNIT_TEST( ranges );
        CPPUNIT_TEST( lookup );
        CPPUNIT_TEST_SUITE_END();

    public:
        void prepare();
        void cleanup();

        void constructor();
        void ranges();
        void lookup();

    };
}}}}}

using namespace wns::probe::bus::detail::tests;

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( SorterTest, wns::testsuite::Default() );

void
SorterTest::prepare()
{
}

void
SorterTest::cleanup()
{
}

void
SorterTest::constructor()
{
    wns::pyconfig::View config =
        wns::pyconfig::Parser::fromString(
			"import openwns.probebus\n"
			"s = openwns.probebus.TabPar('x',1,7,3)\n"
			);

    Sorter a(config.get("s"));
    Sorter b("x",1,7,3);

	CPPUNIT_ASSERT( a.getIdName()=="x" );
	CPPUNIT_ASSERT( b.getIdName()=="x" );

	CPPUNIT_ASSERT_EQUAL(3, a.getResolution());
	CPPUNIT_ASSERT_EQUAL(3, b.getResolution());
	CPPUNIT_ASSERT_EQUAL(IDType(1), b.getMin(0));
	CPPUNIT_ASSERT_EQUAL(IDType(3), b.getMin(1));
	CPPUNIT_ASSERT_EQUAL(IDType(5), b.getMin(2));

	// Test Copy Constructor
	Sorter c(b);
	CPPUNIT_ASSERT( c.getIdName()=="x" );
	CPPUNIT_ASSERT_EQUAL(3, c.getResolution());
	CPPUNIT_ASSERT_EQUAL(IDType(1), c.getMin(0));
	CPPUNIT_ASSERT_EQUAL(IDType(3), c.getMin(1));
	CPPUNIT_ASSERT_EQUAL(IDType(5), c.getMin(2));
}

void
SorterTest::ranges()
{
    Sorter b("x",1,7,3);

	CPPUNIT_ASSERT( !b.checkIndex(0));
	CPPUNIT_ASSERT( b.checkIndex(1));
	CPPUNIT_ASSERT( b.checkIndex(2));
	CPPUNIT_ASSERT( b.checkIndex(3));
	CPPUNIT_ASSERT( b.checkIndex(4));
	CPPUNIT_ASSERT( b.checkIndex(5));
	CPPUNIT_ASSERT( b.checkIndex(6));
	CPPUNIT_ASSERT( b.checkIndex(7));
	CPPUNIT_ASSERT( !b.checkIndex(8));

	CPPUNIT_ASSERT( "[1-3[" == b.getInterval(0) );
	CPPUNIT_ASSERT( "[3-5[" == b.getInterval(1) );
	CPPUNIT_ASSERT( "[5-7]" == b.getInterval(2) );
}

void
SorterTest::lookup()
{
    Sorter b("x",1,7,3);

#if !defined(WNS_NDEBUG) && !defined(WNS_ASSERT)
	CPPUNIT_ASSERT_THROW( b.getIndex(0), wns::Assure::Exception );
	CPPUNIT_ASSERT_THROW( b.getIndex(8), wns::Assure::Exception );
#endif
	CPPUNIT_ASSERT_EQUAL( 0, b.getIndex(1) );
	CPPUNIT_ASSERT_EQUAL( 0, b.getIndex(2) );
	CPPUNIT_ASSERT_EQUAL( 1, b.getIndex(3) );
	CPPUNIT_ASSERT_EQUAL( 1, b.getIndex(4) );
	CPPUNIT_ASSERT_EQUAL( 2, b.getIndex(5) );
	CPPUNIT_ASSERT_EQUAL( 2, b.getIndex(6) );
	CPPUNIT_ASSERT_EQUAL( 2, b.getIndex(7) );
}
