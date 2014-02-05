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

#include <vector>

namespace wns { namespace events { namespace tests {

    class DelayedMemberFunctionTest :
        public wns::TestFixture
    {
        CPPUNIT_TEST_SUITE( DelayedMemberFunctionTest );
        CPPUNIT_TEST( sendDelayed );
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

        void sendDelayed();

    };

    CPPUNIT_TEST_SUITE_REGISTRATION( DelayedMemberFunctionTest );

    void
    DelayedMemberFunctionTest::prepare()
    {
    }

    void
    DelayedMemberFunctionTest::cleanup()
    {
    }

    void
    DelayedMemberFunctionTest::sendDelayed()
    {
        // begin example "wns.events.DelayedMemberFunction.sendDelayed.example"
        std::vector<TestObject*> tos;
        tos.push_back(new TestObject);
        tos.push_back(new TestObject);
        tos.push_back(new TestObject);

        // counter should be zero for all objects
        CPPUNIT_ASSERT_EQUAL(0, tos[0]->foo);
        CPPUNIT_ASSERT_EQUAL(0, tos[1]->foo);
        CPPUNIT_ASSERT_EQUAL(0, tos[2]->foo);

        // queue Events for all objects, calling increaseFoo after 1 second
        std::for_each(
            tos.begin(),
            tos.end(),
            DelayedMemberFunction<TestObject>(&TestObject::increaseFoo, 1.0));

        // only one of the objects should have the counter
        // increased. Since events may be executed in unperdictble order
        // we're testing the sum rather than one specifc element!
        wns::simulator::getEventScheduler()->processOneEvent();
        CPPUNIT_ASSERT_EQUAL(1, tos[0]->foo + tos[1]->foo + tos[2]->foo);

        // two of the objects should have the counter
        // increased. None of them should be called twice!
        wns::simulator::getEventScheduler()->processOneEvent();
        CPPUNIT_ASSERT_EQUAL(2, tos[0]->foo + tos[1]->foo + tos[2]->foo);
        CPPUNIT_ASSERT(tos[0]->foo < 2 && tos[1]->foo < 2 && tos[2]->foo < 2);

        // two of the objects should have the counter
        // increased. None of them should be called twice!
        wns::simulator::getEventScheduler()->processOneEvent();
        CPPUNIT_ASSERT_EQUAL(3, tos[0]->foo + tos[1]->foo + tos[2]->foo);
        CPPUNIT_ASSERT(tos[0]->foo < 2 && tos[1]->foo < 2 && tos[2]->foo < 2);

        // end example

        while(tos.empty() == false)
        {
            delete *tos.begin();
            tos.erase(tos.begin());
        }

        // There should be no more events ...
        CPPUNIT_ASSERT(!wns::simulator::getEventScheduler()->processOneEvent());
    }

} // tests
} // events
} // wns
