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

#include <WNS/events/MemberFunction.hpp>
#include <WNS/TestFixture.hpp>

namespace wns { namespace events { namespace tests {

    class MemberFunctionTest :
        public wns::TestFixture
    {
        CPPUNIT_TEST_SUITE( MemberFunctionTest );
        CPPUNIT_TEST( sendNow );
        CPPUNIT_TEST_SUITE_END();

        class TestObject
        {
        public:
            TestObject() :
                foo(0)
            {
            }

            void increaseFoo()
            {
                ++foo;
            }

            int foo;
        };

    public:
        void prepare();
        void cleanup();

        void sendNow();

    };

    CPPUNIT_TEST_SUITE_REGISTRATION( MemberFunctionTest );

    void
    MemberFunctionTest::prepare()
    {
    }

    void
    MemberFunctionTest::cleanup()
    {
    }

    void
    MemberFunctionTest::sendNow()
    {
        // begin example "wns.events.MemberFunction.sendNow.example"
        TestObject t;

        CPPUNIT_ASSERT_EQUAL(0, t.foo);

        wns::simulator::getEventScheduler()->
            scheduleNow(wns::events::MemberFunction<TestObject>(&t, &TestObject::increaseFoo));

        wns::simulator::getEventScheduler()->processOneEvent();
        CPPUNIT_ASSERT_EQUAL(1, t.foo);
        // end example

        wns::simulator::getEventScheduler()->
            scheduleNow(wns::events::MemberFunction<TestObject>(&t, &TestObject::increaseFoo));

        wns::simulator::getEventScheduler()->
            scheduleNow(wns::events::MemberFunction<TestObject>(&t, &TestObject::increaseFoo));

        wns::simulator::getEventScheduler()->processOneEvent();
        CPPUNIT_ASSERT_EQUAL(2, t.foo);

        wns::simulator::getEventScheduler()->processOneEvent();
        CPPUNIT_ASSERT_EQUAL(3, t.foo);

        CPPUNIT_ASSERT(!wns::simulator::getEventScheduler()->processOneEvent());
    }

} // tests
} // events
} // wns
