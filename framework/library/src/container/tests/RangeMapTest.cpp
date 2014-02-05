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

#include <WNS/container/RangeMap.hpp>
#include <WNS/Functor.hpp>
#include <WNS/Assure.hpp>
#include <WNS/TestFixture.hpp>
#include <string>

namespace wns { namespace container { namespace tests {

    class RangeMapTest : public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE( RangeMapTest );
        CPPUNIT_TEST( testIntInt );
        CPPUNIT_TEST( testFloatFloat );
        CPPUNIT_TEST( testFloatFunctor );
        CPPUNIT_TEST_SUITE_END();
        public:
        void setUp();
        void tearDown();
        void testIntInt();
        void testFloatFloat();
        void testFloatFunctor();
        private:
        struct TestFunctor
        {
            virtual std::string operator()(const std::string&) const = 0;
            virtual ~TestFunctor()
            {
            }
        };

        struct Group1 : public TestFunctor
        {
            virtual std::string operator()(const std::string& s) const
            {
            return "4 8 15 " + s;
            }
            virtual ~Group1()
            {
            }
        };

        struct Group2 : public TestFunctor
        {
            virtual std::string operator()(const std::string& s) const
            {
            return "16 23 42 " + s;
            }
            virtual ~Group2()
            {
            }
        };
    };

    CPPUNIT_TEST_SUITE_REGISTRATION( RangeMapTest );

    void RangeMapTest::setUp()
    {
    }

    void RangeMapTest::tearDown()
    {
    }

    void RangeMapTest::testIntInt()
    {
        typedef wns::container::RangeMap<int, int> MyMap;
        MyMap m;
        CPPUNIT_ASSERT_NO_THROW( m.insert(MyMap::IntervalType::From(0).To(2), 23) );
        CPPUNIT_ASSERT_NO_THROW( m.insert(MyMap::IntervalType::Between(3).And(5), 42) );
        WNS_ASSERT_ASSURE_EXCEPTION( m.insert(MyMap::IntervalType::Between(0).And(0), 0) );
        WNS_ASSERT_ASSURE_EXCEPTION( m.insert(MyMap::IntervalType::From(1).To(1), 0) );
        CPPUNIT_ASSERT( m.has(0) );
        CPPUNIT_ASSERT( !m.has(3) );
        CPPUNIT_ASSERT( m.get(0) == 23 );
        CPPUNIT_ASSERT( m.get(1) == 23 );
        CPPUNIT_ASSERT( m.get(2) == 23 );
        CPPUNIT_ASSERT( m.get(4) == 42 );
        CPPUNIT_ASSERT_THROW( m.get(3), MyMap::Exception );
    }

    void RangeMapTest::testFloatFloat()
    {
        typedef wns::container::RangeMap<float, float> MyMap;
        MyMap m;
        CPPUNIT_ASSERT_NO_THROW( m.insert(MyMap::IntervalType::From(0.0).To(0.1), 15) );
        CPPUNIT_ASSERT_NO_THROW( m.insert(MyMap::IntervalType::Between(0.1).And(0.2), 16) );
        WNS_ASSERT_ASSURE_EXCEPTION( m.insert(MyMap::IntervalType::Between(0.0).And(0.0), 0) );
        WNS_ASSERT_ASSURE_EXCEPTION( m.insert(MyMap::IntervalType::From(0.0).To(0.2), 0) );
        CPPUNIT_ASSERT( m.get(0.0) == 15 );
        CPPUNIT_ASSERT( m.get(0.05) == 15 );
        CPPUNIT_ASSERT( m.get(0.1) == 15 );
        CPPUNIT_ASSERT( m.get(0.10001) == 16 );
        CPPUNIT_ASSERT( m.get(0.19999) == 16 );
        CPPUNIT_ASSERT_THROW( m.get(0.2), MyMap::Exception );
    }

    void RangeMapTest::testFloatFunctor()
    {
        typedef wns::container::RangeMap<float, TestFunctor*> MyMap;
        MyMap m;
        Group1* g1 = new Group1();
        Group2* g2 = new Group2();
        CPPUNIT_ASSERT_NO_THROW( m.insert(MyMap::IntervalType::Between(0.0).And(16.0), g1) );
        CPPUNIT_ASSERT_NO_THROW( m.insert(MyMap::IntervalType::Between(16.0).And(43.0), g2) );
        CPPUNIT_ASSERT( (*m.get(6.0))("foo") == "4 8 15 foo" );
        CPPUNIT_ASSERT( (*m.get(23.0))("bar") == "16 23 42 bar" );
        CPPUNIT_ASSERT( (*m.get(42.0))("baz") == "16 23 42 baz" );
        delete g1;
        delete g2;
    }

} // tests
} // container
} // wns

/*
  Local Variables:
  mode: c++
  fill-column: 80
  c-basic-offset: 8
  c-tab-always-indent: t
  indent-tabs-mode: t
  tab-width: 8
  End:
*/

