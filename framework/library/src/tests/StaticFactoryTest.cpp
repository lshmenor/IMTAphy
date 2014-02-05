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

#include <WNS/LongCreator.hpp>
#include <WNS/Exception.hpp>
#include <WNS/StaticFactory.hpp>

#include <iostream>
#include <cppunit/extensions/HelperMacros.h>
#include <stdexcept>

namespace wns { namespace tests {

    class StaticFactoryTest :
        public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE( StaticFactoryTest );
        CPPUNIT_TEST( testSimpleCreation );
        CPPUNIT_TEST( testCreation );
        CPPUNIT_TEST( testNotThere );
        CPPUNIT_TEST( testNonDefaultConstructor );
        CPPUNIT_TEST_SUITE_END();
    public:
        void setUp();
        void tearDown();

        void testSimpleCreation();
        void testCreation();
        void testNotThere();
        void testNonDefaultConstructor();
    private:
    };

    //
    // TestInterface is a pure virtual class, defining the interface
    // of Test implementations.

    // begin example "wns.StaticFactory.InterfaceDefinition.example"
    struct TestInterface
    {
        // this is a base class...
        virtual ~TestInterface() {}
        // some state...
        long foo;
        // some behaviour...
        virtual void set(long) = 0;
    };
    // end example

    // begin example "wns.StaticFactory.Thingies.example"
    typedef Creator<TestInterface> TestCreator;
    typedef StaticFactory<TestCreator> TestFactory;
    // end example

    //
    // Two implementations
    //
    // begin example "wns.StaticFactory.InterfaceImplementation.example"
    class Plugin1 :
        public TestInterface
    {
    public:
        Plugin1()
        { foo = 666; }
        virtual void set(long i)
        { foo = i; }
    };
    STATIC_FACTORY_REGISTER(Plugin1, TestInterface, "plugin 1");
    // end example

    class Plugin2 :
        public TestInterface
    {
    public:
        virtual void set(long i)
        {
            foo = 2 * i;
        }
    };
    STATIC_FACTORY_REGISTER(Plugin2, TestInterface, "plugin 2");

    class OtherInterface
    {
    public:
        virtual ~OtherInterface() {}
        virtual long get() = 0;
    };
    typedef Creator<OtherInterface> OtherCreator;
    typedef StaticFactory<OtherCreator> OtherFactory;

    class Something :
        public OtherInterface
    {
    public:
        virtual ~Something() {}
        virtual long get()
        {
            return 4711;
        }
    };
    STATIC_FACTORY_REGISTER(Something, OtherInterface, "plugin 3");


    // begin example "wns.StaticFactory.FunkInterface.example"
    struct FunkInterface
    {
        FunkInterface(long) {}
        virtual ~FunkInterface() {}

        virtual long getIt() = 0;
    };
    typedef LongCreator<FunkInterface> FunkCreator;
    typedef StaticFactory<FunkCreator> FunkFactory;
    // end example

    // begin example "wns.StaticFactory.Hancock.example"
    class Hancock :
        public FunkInterface
    {
    public:
        Hancock(long _l) : FunkInterface(_l)
        { l = _l; }
        long getIt()
        { return l; }
    private:
        long l;
    };
    STATIC_FACTORY_REGISTER_WITH_CREATOR(Hancock, FunkInterface, "Hancock", LongCreator);
    // end example
}}

using namespace std;
using namespace wns::tests;

CPPUNIT_TEST_SUITE_REGISTRATION( StaticFactoryTest );

void StaticFactoryTest::setUp()
{
}

void StaticFactoryTest::tearDown()
{
}

void
StaticFactoryTest::testSimpleCreation()
{
    // begin example "wns.StaticFactory.testSimpleCreation.example"
    // assert, that the plugin with name "plugin 1" has been registered
    CPPUNIT_ASSERT( TestFactory::knows("plugin 1") );

    // to create instances of a plugin, we first need to query
    // the factory for a creator.
    TestCreator* c = TestFactory::creator("plugin 1");

    // once having such a creator, we can create any number of
    // plugins using the creator's create method.
    TestInterface* p1 = c->create();
    // assure that the constructor of the plugin has been called and
    // set the attribute foo.
    CPPUNIT_ASSERT( p1->foo == 666 );

    // we can call anything the interface provides...
    p1->set(23);
    CPPUNIT_ASSERT( p1->foo == 23 );
    delete p1;
    // end example
} // testSimpleCreation


void
StaticFactoryTest::testCreation()
{
    TestCreator* c = TestFactory::creator("plugin 1");
    TestInterface* p1 = c->create();
    p1->set(23);

    TestInterface* p2 = TestFactory::creator("plugin 2")->create();
    p2->set(21);

    OtherInterface* o = OtherFactory::creator("plugin 3")->create();

    CPPUNIT_ASSERT( p1->foo == 23 );
    CPPUNIT_ASSERT( p2->foo == 42 );
    CPPUNIT_ASSERT( o->get() == 4711 );

    delete p1;
    delete p2;
    delete o;
} // testCreation


void
StaticFactoryTest::testNotThere()
{
    CPPUNIT_ASSERT( TestFactory::knows("plugin 1") );
    CPPUNIT_ASSERT( TestFactory::knows("plugin 2") );
    CPPUNIT_ASSERT( !TestFactory::knows("plugin 3") );

    CPPUNIT_ASSERT( !OtherFactory::knows("plugin 1") );
    CPPUNIT_ASSERT( !OtherFactory::knows("plugin 2") );
    CPPUNIT_ASSERT( OtherFactory::knows("plugin 3") );

    TestCreator* c;
    bool thrown_up = false;

    try {
        c = TestFactory::creator("plugin 3");
    }
    catch(Exception e) {
        thrown_up = true;
    }

    CPPUNIT_ASSERT(thrown_up);
} // testNotThere

void
StaticFactoryTest::testNonDefaultConstructor()
{
    FunkCreator* c = FunkFactory::creator("Hancock");
    FunkInterface* l = c->create(42);
    CPPUNIT_ASSERT_EQUAL( 42L, l->getIt() );
    delete l;
} // testNonDefaultConstructor
