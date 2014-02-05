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

#include <WNS/pyconfig/helper/Functions.hpp>
#include <WNS/container/UntypedRegistry.hpp>
#include <WNS/Singleton.hpp>
#include <WNS/TestFixture.hpp>

namespace wns { namespace container { namespace tests {

    /**
     * @brief Test for UntypedRegistry<...>
     * @author Marc Schinnenburg <marc@schinnenburg.net>
     */
    class UntypedRegistryTest :
        public wns::TestFixture
    {
        CPPUNIT_TEST_SUITE( UntypedRegistryTest );
        CPPUNIT_TEST( empty );
        CPPUNIT_TEST( clear );
        CPPUNIT_TEST( size );
        CPPUNIT_TEST( knows );
        CPPUNIT_TEST( find );
        CPPUNIT_TEST( findUnknown );
        CPPUNIT_TEST( update );
        CPPUNIT_TEST( updateUnknown );
        CPPUNIT_TEST( findInSingleton );
        CPPUNIT_TEST( erase );
        CPPUNIT_TEST( eraseUnknown );
        CPPUNIT_TEST( findWrongType );
        CPPUNIT_TEST( doubleInsert );
        CPPUNIT_TEST( keys );
        CPPUNIT_TEST( sortingPolicy );
        CPPUNIT_TEST_SUITE_END();

        class A
        {
        public:
            A() :
                foo()
            {
            }

            virtual
            ~A()
            {
            }

            void
            test()
            {
            }

            void
            test_const() const
            {
            }

            bool
            operator < (const A& a) const
            {
                return foo < a.foo;
            }

            int foo;
        };

        class B
        {
        };

        class AlwaysLess
        {
        public:
            bool
            operator()(int, int) const
            {
                return true;
            }
        };

        typedef wns::container::UntypedRegistry<std::string> StringKeyRegistry;
        typedef wns::SingletonHolder<StringKeyRegistry> GlobalStringKeyRegistry;


    public:
        void
        prepare()
        {
        }

        void
        cleanup()
        {
        }

        void
        knows()
        {
            StringKeyRegistry r;

            r.insert("foo", 2);
            r.insert("bar", 3);

            CPPUNIT_ASSERT( r.knows("foo") );
            CPPUNIT_ASSERT( r.knows("bar") );
            CPPUNIT_ASSERT( !r.knows("baz") );
        }

        void
        empty()
        {
            StringKeyRegistry r;

            CPPUNIT_ASSERT( r.empty() );

            A foo;
            r.insert("foo", foo);

            CPPUNIT_ASSERT( !r.empty() );
        }

        void
        clear()
        {
            StringKeyRegistry r;
            // clear already empty registry
            r.clear();
            CPPUNIT_ASSERT( r.empty() );

            r.insert("foo", 42);
            r.insert("bar", 23);
            CPPUNIT_ASSERT( !r.empty() );

            r.clear();
            CPPUNIT_ASSERT( r.empty() );
        }

        void
        size()
        {
            StringKeyRegistry r;
            CPPUNIT_ASSERT_EQUAL( size_t(0), r.size() );

            r.insert("foo", 23);
            CPPUNIT_ASSERT_EQUAL( size_t(1), r.size() );

            r.insert("bar", 42);
            CPPUNIT_ASSERT_EQUAL( size_t(2), r.size() );

            r.insert("baz", 42);
            CPPUNIT_ASSERT_EQUAL( size_t(3), r.size() );

            r.update("foo", 42);
            CPPUNIT_ASSERT_EQUAL( size_t(3), r.size() );

            r.erase("baz");
            CPPUNIT_ASSERT_EQUAL( size_t(2), r.size() );

            r.clear();
            CPPUNIT_ASSERT_EQUAL( size_t(0), r.size() );
        }

        void
        find()
        {
            StringKeyRegistry r;
            A* foo = new A();
            B* bar = new B();

            r.insert("foo", foo);
            r.insert("bar", bar);
            r.insert("ernie", 23);
            r.insert("bert", 42.0);
            r.insert("tiffi", "samson");
            r.insert("oscar", std::string("grobi"));

            CPPUNIT_ASSERT( r.find<A*>("foo") == foo );
            CPPUNIT_ASSERT( r.find<B*>("bar") == bar );
            CPPUNIT_ASSERT( r.find<int>("ernie") == 23 );
            CPPUNIT_ASSERT( r.find<double>("bert") == 42.0 );
            CPPUNIT_ASSERT( r.find<const char*>("tiffi") == std::string("samson"));
            CPPUNIT_ASSERT( r.find<std::string>("oscar") == std::string("grobi"));

            delete foo;
            delete bar;
        }

        void
        findUnknown()
        {
            StringKeyRegistry r;
            A foo;

            // Not in and nothing else registered
            CPPUNIT_ASSERT_THROW( r.find<A>("foo"), StringKeyRegistry::UnknownKeyValue );

            r.insert("bar", foo);
            // Not in only "bar" registered
            CPPUNIT_ASSERT_THROW( r.find<A>("foo"), StringKeyRegistry::UnknownKeyValue );

            r.insert("foo", foo);
            r.erase("foo");
            // Not in, but "foo" was registered once
            CPPUNIT_ASSERT_THROW( r.find<A>("foo"), StringKeyRegistry::UnknownKeyValue );
        }

        void
        update()
        {
            StringKeyRegistry r;
            A* foo = new A();
            A* foo2 = new A();
            B* bar = new B();

            r.insert("foo", foo);
            CPPUNIT_ASSERT( r.find<A*>("foo") == foo );

            // should work with same type
            r.update("foo", foo2);
            CPPUNIT_ASSERT( r.find<A*>("foo") == foo2 );

            // should also work with different type
            r.update("foo", bar);
            CPPUNIT_ASSERT( r.find<B*>("foo") == bar );

            delete foo;
            delete bar;
            delete foo2;
        }

        void
        updateUnknown()
        {
            StringKeyRegistry r;
            A foo;

            // Not in and nothing else registered
            CPPUNIT_ASSERT_THROW( r.update("foo", foo), StringKeyRegistry::UnknownKeyValue );

            r.insert("bar", foo);
            // Not in only "bar" registered
            CPPUNIT_ASSERT_THROW( r.update("foo", foo), StringKeyRegistry::UnknownKeyValue );

            r.insert("foo", foo);
            r.erase("foo");
            // Not in, but "foo" was registered once
            CPPUNIT_ASSERT_THROW( r.update("foo", foo), StringKeyRegistry::UnknownKeyValue );
        }

        void
        findInSingleton()
        {
            // We can test the singletons here safely, because these
            // "singletons" will never exists in a real environment. The classes
            // to be stored in the UntypedRegistry are privte within this test ...
            A* foo = new A();
            B* bar = new B();
            GlobalStringKeyRegistry::Instance().insert("foo", foo);
            GlobalStringKeyRegistry::Instance().insert("bar", bar);

            CPPUNIT_ASSERT( GlobalStringKeyRegistry::Instance().find<A*>("foo") == foo );
            CPPUNIT_ASSERT( GlobalStringKeyRegistry::Instance().find<B*>("bar") == bar );

            delete foo;
            delete bar;
        }

        void
        erase()
        {
            StringKeyRegistry r;
            A foo;
            B bar;

            r.insert("foo", foo);
            r.insert("bar", bar);

            CPPUNIT_ASSERT( r.knows("foo") );
            CPPUNIT_ASSERT( r.knows("bar") );

            r.erase("foo");

            CPPUNIT_ASSERT( !r.knows("foo") );
            CPPUNIT_ASSERT( r.knows("bar") );

        }

        void
        eraseUnknown()
        {
            StringKeyRegistry r;
            A foo;

            // Not in and nothing else registered
            CPPUNIT_ASSERT_THROW( r.erase("foo"), StringKeyRegistry::UnknownKeyValue );

            r.insert("bar", foo);
            // Not in only "bar" registered
            CPPUNIT_ASSERT_THROW( r.erase("foo"), StringKeyRegistry::UnknownKeyValue );

            r.insert("foo", foo);
            r.erase("foo");
            // Not in, but "foo" was registered once
            CPPUNIT_ASSERT_THROW( r.erase("foo"), StringKeyRegistry::UnknownKeyValue );
        }

        void
        findWrongType()
        {
            StringKeyRegistry r;
            A foo;

            r.insert("foo", foo);
            CPPUNIT_ASSERT_THROW( r.find<B>("foo"), StringKeyRegistry::BadCast );
        }

        void
        doubleInsert()
        {
            StringKeyRegistry r;
            r.insert("foo", 8);
            CPPUNIT_ASSERT_THROW( r.insert("foo", 9), StringKeyRegistry::DuplicateKeyValue );
        }

        void
        keys()
        {
            StringKeyRegistry r;

            r.insert("foo", 23);
            r.insert("bar", 42);

            StringKeyRegistry::KeyList keys = r.keys();

            CPPUNIT_ASSERT_EQUAL( size_t(2),  keys.size());
            CPPUNIT_ASSERT( keys.front() == "bar" );
            CPPUNIT_ASSERT( keys.back() == "foo" );
        }

        void
        sortingPolicy()
        {
            UntypedRegistry < int, AlwaysLess> r;
            r.insert(1, 2);
            // This should normally throw. But with the AlwaysLess-SortingPolicy it
            // should be possible to add the same key twice. If it works, this
            // means the sorting policy is correctly applied.
            CPPUNIT_ASSERT_NO_THROW(r.insert(1, 2));
        }
    };

    CPPUNIT_TEST_SUITE_REGISTRATION( UntypedRegistryTest );
} // tests
} // container
} // wns
