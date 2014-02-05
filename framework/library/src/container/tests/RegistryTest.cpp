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
#include <WNS/container/Registry.hpp>
#include <WNS/Singleton.hpp>
#include <WNS/TestFixture.hpp>

namespace wns { namespace container { namespace tests {

    /**
     * @brief Test for Registry<...>
     * @author Marc Schinnenburg <marc@schinnenburg.net>
     */
    class RegistryTest :
        public wns::TestFixture
    {
        CPPUNIT_TEST_SUITE( RegistryTest );
        CPPUNIT_TEST( empty );
        CPPUNIT_TEST( clear );
        CPPUNIT_TEST( knows );
        CPPUNIT_TEST( find );
        CPPUNIT_TEST( size );
        CPPUNIT_TEST( findUnknown );
        CPPUNIT_TEST( update );
        CPPUNIT_TEST( updateUnknown );
        CPPUNIT_TEST( findInSingleton );
        CPPUNIT_TEST( erase );
        CPPUNIT_TEST( eraseUnknown );
        CPPUNIT_TEST( const_iterator );
        CPPUNIT_TEST( doubleInsert );
        CPPUNIT_TEST( deleteOnErase );
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
                ++dtorCounter;
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
            operator <(const A& a) const
            {
                return foo < a.foo;
            }

            int foo;
            static int dtorCounter;
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

        typedef wns::container::Registry<std::string, A*> ARegistry;
        typedef wns::SingletonHolder<ARegistry> GlobalARegistry;
        typedef wns::container::Registry<std::string, A*, registry::DeleteOnErase> ADeleteRegistry;
        typedef wns::container::Registry<std::string, B*> BRegistry;
        typedef wns::SingletonHolder<BRegistry> GlobalBRegistry;


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
            ARegistry r;
            A* foo = new A();
            A* bar = new A();

            r.insert("foo", foo);
            r.insert("bar", bar);

            CPPUNIT_ASSERT( r.knows("foo") );
            CPPUNIT_ASSERT( r.knows("bar") );
            CPPUNIT_ASSERT( !r.knows("baz") );
            delete foo;
            delete bar;
        }

        void
        empty()
        {
            ARegistry r;

            CPPUNIT_ASSERT( r.empty() );

            A* foo = new A();
            r.insert("foo", foo);

            CPPUNIT_ASSERT( !r.empty() );

            delete foo;
        }

        void
        clear()
        {
            Registry<std::string, int> r;
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
            Registry<std::string, int> r;
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
            ARegistry r;
            A* foo = new A();
            A* bar = new A();

            r.insert("foo", foo);
            r.insert("bar", bar);

            CPPUNIT_ASSERT( r.find("foo") == foo );
            CPPUNIT_ASSERT( r.find("bar") == bar );

            CPPUNIT_ASSERT_THROW( r.find("baz"), ARegistry::UnknownKeyValue );
            delete foo;
            delete bar;
        }

        void
        findUnknown()
        {
            typedef Registry<std::string, int > IntReg;
            IntReg r;

            // Not in and nothing else registered
            CPPUNIT_ASSERT_THROW( r.find("foo"), IntReg::UnknownKeyValue );

            r.insert("bar", 42);
            // Not in only "bar" registered
            CPPUNIT_ASSERT_THROW( r.find("foo"), IntReg::UnknownKeyValue );

            r.insert("foo", 42);
            r.erase("foo");
            // Not in, but "foo" was registered once
            CPPUNIT_ASSERT_THROW( r.find("foo"), IntReg::UnknownKeyValue );
        }

        void
        update()
        {
            ARegistry r;
            A* foo = new A();
            A* bar = new A();

            r.insert("foo", foo);
            CPPUNIT_ASSERT( r.find("foo") == foo );

            r.update("foo", bar);
            CPPUNIT_ASSERT( r.find("foo") == bar );

            CPPUNIT_ASSERT_THROW( r.update("baz", bar), ARegistry::UnknownKeyValue );

            delete foo;
            delete bar;
        }

        void
        updateUnknown()
        {
            typedef Registry<std::string, A > AReg;
            AReg r;
            A foo;

            // Not in and nothing else registered
            CPPUNIT_ASSERT_THROW( r.update("foo", foo), AReg::UnknownKeyValue );

            r.insert("bar", foo);
            // Not in only "bar" registered
            CPPUNIT_ASSERT_THROW( r.update("foo", foo), AReg::UnknownKeyValue );

            r.insert("foo", foo);
            r.erase("foo");
            // Not in, but "foo" was registered once
            CPPUNIT_ASSERT_THROW( r.update("foo", foo), AReg::UnknownKeyValue );
        }

        void
        findInSingleton()
        {
            // We can test the singletons here safely, because these
            // "singletons" will never exists in a real environment. The classes
            // to be stored in the Registry are privte within this test ...
            A* foo = new A();
            A* bar = new A();
            GlobalARegistry::Instance().insert("foo", foo);
            GlobalARegistry::Instance().insert("bar", bar);

            CPPUNIT_ASSERT( GlobalARegistry::Instance().find("foo") == foo );
            CPPUNIT_ASSERT( GlobalARegistry::Instance().find("bar") == bar );

            delete foo;
            delete bar;
        }

        void
        erase()
        {
            ARegistry r;
            A* foo = new A();
            A* bar = new A();

            r.insert("foo", foo);
            r.insert("bar", bar);

            CPPUNIT_ASSERT( r.knows("foo") );
            CPPUNIT_ASSERT( r.knows("bar") );
            CPPUNIT_ASSERT( !r.knows("baz") );

            r.erase("foo");

            CPPUNIT_ASSERT( !r.knows("foo") );
            CPPUNIT_ASSERT( r.knows("bar") );
            CPPUNIT_ASSERT( !r.knows("baz") );

            CPPUNIT_ASSERT_THROW( r.erase("foo"), ARegistry::UnknownKeyValue );

            delete foo;
            delete bar;
        }

        void
        eraseUnknown()
        {
            typedef Registry<std::string, A > AReg;
            AReg r;
            A foo;

            // Not in and nothing else registered
            CPPUNIT_ASSERT_THROW( r.erase("foo"), AReg::UnknownKeyValue );

            r.insert("bar", foo);
            // Not in only "bar" registered
            CPPUNIT_ASSERT_THROW( r.erase("foo"), AReg::UnknownKeyValue );

            r.insert("foo", foo);
            r.erase("foo");
            // Not in, but "foo" was registered once
            CPPUNIT_ASSERT_THROW( r.erase("foo"), AReg::UnknownKeyValue );
        }

        void
        const_iterator()
        {
            ARegistry r;
            A* foo = new A();
            A* bar = new A();

            r.insert("foo", foo);
            r.insert("bar", bar);

            wns::container::Registry<std::string, A*>::const_iterator testConstIterator;
            testConstIterator = r.begin();

            CPPUNIT_ASSERT( testConstIterator->first == "bar" );
            CPPUNIT_ASSERT( testConstIterator->second == bar );

            testConstIterator++;

            CPPUNIT_ASSERT( testConstIterator->first == "foo" );
            CPPUNIT_ASSERT( testConstIterator->second == foo );

            testConstIterator++;

            CPPUNIT_ASSERT( testConstIterator == r.end() );

            // just check if const functions may be called
            wns::container::Registry<std::string, A> lala;
            lala.insert("foo", A());
            lala.begin()->second.test_const();

            delete foo;
            delete bar;
        }

        void
        doubleInsert()
        {
            ARegistry r;
            A* foo = new A();

            r.insert("foo", foo);
            CPPUNIT_ASSERT_THROW( r.insert("foo", foo), ARegistry::DuplicateKeyValue );

            delete foo;
        }

        void
        deleteOnErase()
        {
            A::dtorCounter = 0;

            {
                ADeleteRegistry r;

                CPPUNIT_ASSERT_EQUAL(0, A::dtorCounter);
                r.insert("foo", new A());
                r.erase("foo");
                CPPUNIT_ASSERT_EQUAL(1, A::dtorCounter);

                r.insert("foo", new A());
                r.insert("bar", new A());
                r.update("bar", new A());
                // update overwrites (erases) the old bar.
                CPPUNIT_ASSERT_EQUAL(2, A::dtorCounter);

                // remove all entries
                r.clear();
                CPPUNIT_ASSERT_EQUAL(4, A::dtorCounter);
                r.insert("bar", new A());
                r.insert("baz", new A());
            }
            // remove element on destruction of registry
            CPPUNIT_ASSERT_EQUAL(6, A::dtorCounter);
        }

        void
        keys()
        {
            ARegistry r;
            A* foo = new A();
            A* bar = new A();

            r.insert("foo", foo);
            r.insert("bar", bar);

            ARegistry::KeyList keys = r.keys();

            CPPUNIT_ASSERT_EQUAL( size_t(2),  keys.size());
            CPPUNIT_ASSERT( keys.front() == "bar" );
            CPPUNIT_ASSERT( keys.back() == "foo" );
            delete foo;
            delete bar;
        }

        void
        sortingPolicy()
        {
            Registry < int, int, registry::NoneOnErase, AlwaysLess > r;
            r.insert(1, 2);
            // This should normally throw. But with the AlwaysLess-SortingPolicy it
            // should be possible to add the same key twice. If it works, this
            // means the sorting policy is correctly applied.
            CPPUNIT_ASSERT_NO_THROW(r.insert(1, 2));
        }
    };

    CPPUNIT_TEST_SUITE_REGISTRATION( RegistryTest );

    int RegistryTest::A::dtorCounter = 0;
} // tests
} // container
} // wns



