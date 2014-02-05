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

#include <WNS/probe/bus/ContextProvider.hpp>
#include <WNS/osi/PDU.hpp>

#include <boost/bind.hpp>

namespace wns { namespace probe { namespace bus { namespace tests {

    // begin example "contextprovider.callback.userclass.example"
    class SomeClass
    {
        int someInternalMember_;
    public:
        SomeClass(int aValue) : someInternalMember_(aValue) {}

        int
        getter()
            {
                return someInternalMember_;
            }
    };
    // end example

    class ContextProviderTest :
        public wns::TestFixture
    {
        CPPUNIT_TEST_SUITE( ContextProviderTest );
        CPPUNIT_TEST( constant );
        CPPUNIT_TEST( variable );
        CPPUNIT_TEST( container );
        CPPUNIT_TEST( callback );
        CPPUNIT_TEST_SUITE_END();
    public:
        void prepare();
        void cleanup();

        void constant();
        void variable();
        void container();
        void callback();
    };

    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ContextProviderTest, wns::testsuite::Default() );

}
}
}
}

using namespace wns::probe::bus;
using namespace wns::probe::bus::tests;

void
ContextProviderTest::prepare()
{
}

void
ContextProviderTest::cleanup()
{
}

void
ContextProviderTest::constant()
{
	contextprovider::Constant provider("foo",42);

	Context c;
	IContext& context = c;

	CPPUNIT_ASSERT( !context.knows("foo") );
	provider.visit(context);
	CPPUNIT_ASSERT( context.knows("foo") );
	CPPUNIT_ASSERT_EQUAL( 42, context.getInt("foo") );
}

void
ContextProviderTest::variable()
{
	contextprovider::Variable provider("foo",42);
	CPPUNIT_ASSERT_EQUAL( 42, provider.get() );

	Context c;
	IContext& context = c;

	CPPUNIT_ASSERT( !context.knows("foo") );
	provider.visit(context);
	CPPUNIT_ASSERT( context.knows("foo") );
	CPPUNIT_ASSERT_EQUAL( 42, context.getInt("foo") );

	// Provide another value from now on
	provider.set(43);
	CPPUNIT_ASSERT_EQUAL( 43, provider.get() );

	Context c2;
	IContext& context2 = c2;

	CPPUNIT_ASSERT( !context2.knows("foo") );
	provider.visit(context2);
	CPPUNIT_ASSERT( context2.knows("foo") );
	CPPUNIT_ASSERT_EQUAL( 43, context2.getInt("foo") );
}

void
ContextProviderTest::container()
{
	contextprovider::Variable* provider = new contextprovider::Variable("foo", 42);
	contextprovider::Container container(provider);

	Context c;
	IContext& context = c;

	CPPUNIT_ASSERT( !context.knows("foo") );
	container.visit(context);
	CPPUNIT_ASSERT( context.knows("foo") );
	CPPUNIT_ASSERT_EQUAL( 42, context.getInt("foo") );

	// Provide another value from now on
	provider->set(43);

	// Copy the Container, the 'inner' provider is not affected
	contextprovider::Container otherContainer(container);

	Context c2;
	IContext& context2 = c2;

	CPPUNIT_ASSERT( !context2.knows("foo") );
	container.visit(context2);
	CPPUNIT_ASSERT( context2.knows("foo") );
	CPPUNIT_ASSERT_EQUAL( 43, context2.getInt("foo") );

	delete provider;
}

void
ContextProviderTest::callback()
{
    // begin example "contextprovider.callback.usage.example"

    // Suppose you have an instance of some class.
    SomeClass myInstance(100);

    // We now create a Callback context provider that calls the
    // getter() function of SomeClass on the instance myInstance
    // everytime a context value is needed.

    // The key of the context (all context information are key value pairs)
    std::string key = std::string("foo");

    // Use boost::bind to create a function object for the function
    // SomeClass::getter on the instance myInstance
    // boost::function0<int> declares a function object with no arguments
    // and a return value of type int.
    boost::function0<int> callback = boost::bind(&SomeClass::getter, &myInstance);

    // Create the callback context provider
    contextprovider::Callback provider(key, callback);

    Context c;
    IContext& context = c;

    CPPUNIT_ASSERT( !context.knows("foo") );
    provider.visit(context);
    CPPUNIT_ASSERT( context.knows("foo") );
    CPPUNIT_ASSERT_EQUAL( 100, context.getInt("foo") );
    // end example
}
