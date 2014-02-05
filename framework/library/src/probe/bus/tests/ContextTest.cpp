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

#include <WNS/TestFixture.hpp>

#include <WNS/probe/bus/Context.hpp>

namespace wns { namespace probe { namespace bus { namespace tests {

    class ContextTest :
        public wns::TestFixture
    {
        CPPUNIT_TEST_SUITE( ContextTest );
        CPPUNIT_TEST( idreg );
        CPPUNIT_TEST_SUITE_END();
    public:
        void prepare();
        void cleanup();
        void idreg();
    };

    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ContextTest, wns::testsuite::Default() );

}
}
}
}

using namespace wns::probe::bus::tests;

void
ContextTest::prepare()
{
}

void
ContextTest::cleanup()
{
}

void
ContextTest::idreg()
{
    wns::probe::bus::Context c;
    wns::probe::bus::IContext& context = c;

    context.insertInt("foo", 42);
    context.insertInt("bar", 100);
    context.insertInt("baz", -20);

    context.insertString("first", "Hello");
    context.insertString("second", "World");

    CPPUNIT_ASSERT_THROW( context.insertInt("bar", 101), wns::probe::bus::context::DuplicateKey );
    CPPUNIT_ASSERT_THROW( context.insertString("first", "lala"), wns::probe::bus::context::DuplicateKey );

    CPPUNIT_ASSERT(context.knows("foo"));
    CPPUNIT_ASSERT(context.knows("bar"));

    int lala = context.getInt("foo");
    CPPUNIT_ASSERT_EQUAL(42, lala);
    lala = context.getInt("bar");
    CPPUNIT_ASSERT_EQUAL(100, lala);
    lala = context.getInt("baz");
    CPPUNIT_ASSERT_EQUAL(-20, lala);
    lala = context.getInt("foo");
    CPPUNIT_ASSERT_EQUAL(42, lala);
    lala = context.getInt("bar");
    CPPUNIT_ASSERT_EQUAL(100, lala);
    lala = context.getInt("baz");
    CPPUNIT_ASSERT_EQUAL(-20, lala);
    lala = context.getInt("foo");
    CPPUNIT_ASSERT_EQUAL(42, lala);
    lala = context.getInt("bar");
    CPPUNIT_ASSERT_EQUAL(100, lala);
    lala = context.getInt("baz");
    CPPUNIT_ASSERT_EQUAL(-20, lala);

    std::string s = context.getString("first");
    CPPUNIT_ASSERT( s == "Hello" );
    s = context.getString("second");
    CPPUNIT_ASSERT( s == "World" );
    s = context.getString("first");
    CPPUNIT_ASSERT( s == "Hello" );
    s = context.getString("second");
    CPPUNIT_ASSERT( s == "World" );
    s = context.getString("first");
    CPPUNIT_ASSERT( s == "Hello" );
    s = context.getString("second");
    CPPUNIT_ASSERT( s == "World" );

    // expect an exception upon unknown key
    CPPUNIT_ASSERT_THROW( context.getInt("hasenpfote"), wns::probe::bus::context::NotFound);
    CPPUNIT_ASSERT_THROW( context.getString("hasenpfote"), wns::probe::bus::context::NotFound);

    // expect an exception when requesting wrong type
    CPPUNIT_ASSERT_THROW( context.getInt("second"), wns::probe::bus::context::TypeError);
    CPPUNIT_ASSERT_THROW( context.getString("foo"), wns::probe::bus::context::TypeError);
}
