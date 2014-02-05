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


#include <WNS/simulator/ISimulator.hpp>
#include <WNS/events/MultipleTimeout.hpp>

#include <WNS/CppUnit.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace wns { namespace events { namespace tests {

    class MultipleTimeoutTest :
        public wns::TestFixture
    {
        CPPUNIT_TEST_SUITE( MultipleTimeoutTest );
        CPPUNIT_TEST( constructor );
        CPPUNIT_TEST( setTimeout );
        CPPUNIT_TEST( cancelTimeout );
        CPPUNIT_TEST( setNewTimeout );
        CPPUNIT_TEST( setTimeoutTwice );
        CPPUNIT_TEST( setTimeouts );
        CPPUNIT_TEST( cancelTimeoutNotSet );
        CPPUNIT_TEST( cancelTimeouts );
        CPPUNIT_TEST( cancelAllTimeouts );
        CPPUNIT_TEST( cancelAllTimeoutsNoTimeoutSet );
        CPPUNIT_TEST( testTimeouts );
        CPPUNIT_TEST( testNewTimeout );
        CPPUNIT_TEST( destructor );
        CPPUNIT_TEST_SUITE_END();

    public:
        void prepare();
        void cleanup();

        void constructor();

        void setTimeout();
        void cancelTimeout();
        void setNewTimeout();
        void setTimeoutTwice();
        void cancelTimeoutNotSet();

        void setTimeouts();
        void cancelTimeouts();
        void cancelAllTimeouts();
        void cancelAllTimeoutsNoTimeoutSet();

        void testTimeouts();
        void testNewTimeout();

        void destructor();

    private:
        class DerivedMultipleTimeout :
            public MultipleTimeout<int>
        {
        public:
            DerivedMultipleTimeout() :
                MultipleTimeout<int>(),
                timeoutNumber(0)
            {
            } // DerivedMultipleTimeout

            void onTimeout(const int& number)
            {
                timeoutNumber = number;
            } // onTimeout

            int timeoutNumber;
        };

        DerivedMultipleTimeout* timeouts;
        wns::events::scheduler::Interface* scheduler;
    };

    CPPUNIT_TEST_SUITE_REGISTRATION( MultipleTimeoutTest );

    void
    MultipleTimeoutTest::prepare()
    {
        scheduler = wns::simulator::getEventScheduler();
        scheduler->reset();
        timeouts = new DerivedMultipleTimeout();

        assure(!scheduler->processOneEvent(), "Scheduler must be clean!");
    } // prepare

    void
    MultipleTimeoutTest::cleanup()
    {
        delete timeouts;
    } // cleanup

    void
    MultipleTimeoutTest::constructor()
    {
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), timeouts->numberOfTimeoutsSet());
    } // constructor

    void
    MultipleTimeoutTest::setTimeout()
    {
        timeouts->setTimeout(1, 3.0);
        CPPUNIT_ASSERT(timeouts->hasTimeoutSet(1));
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), timeouts->numberOfTimeoutsSet());
    } // setTimeout

    void
    MultipleTimeoutTest::cancelTimeout()
    {
        timeouts->setTimeout(1, 3.0);
        CPPUNIT_ASSERT(timeouts->hasTimeoutSet(1));
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), timeouts->numberOfTimeoutsSet());

        timeouts->cancelTimeout(1);
        CPPUNIT_ASSERT(!timeouts->hasTimeoutSet(1));
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), timeouts->numberOfTimeoutsSet());

        // assure clean scheduler
        CPPUNIT_ASSERT(!scheduler->processOneEvent());

    } // cancelTimeout

    void
    MultipleTimeoutTest::setNewTimeout()
    {
        timeouts->setNewTimeout(1, 3.0);
        CPPUNIT_ASSERT(timeouts->hasTimeoutSet(1));
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), timeouts->numberOfTimeoutsSet());

        timeouts->setNewTimeout(1, 1.0);
        CPPUNIT_ASSERT(timeouts->hasTimeoutSet(1));
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), timeouts->numberOfTimeoutsSet());
    } // setNewTimeout

    void
    MultipleTimeoutTest::setTimeoutTwice()
    {
        timeouts->setTimeout(1, 3.0);
        CPPUNIT_ASSERT(timeouts->hasTimeoutSet(1));
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), timeouts->numberOfTimeoutsSet());

        WNS_ASSERT_ASSURE_EXCEPTION(timeouts->setTimeout(1, 3.0));
    } // setTimeoutTwice

    void
    MultipleTimeoutTest::cancelTimeoutNotSet()
    {
        WNS_ASSERT_ASSURE_EXCEPTION(timeouts->cancelTimeout(1));
    } // cancelTimeoutNotSet

    void
    MultipleTimeoutTest::setTimeouts()
    {
        timeouts->setTimeout(1, 3.0);
        CPPUNIT_ASSERT(timeouts->hasTimeoutSet(1));
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), timeouts->numberOfTimeoutsSet());

        timeouts->setTimeout(2, 1.0);
        CPPUNIT_ASSERT(timeouts->hasTimeoutSet(2));
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), timeouts->numberOfTimeoutsSet());
    } // setTimeouts

    void
    MultipleTimeoutTest::cancelTimeouts()
    {
        timeouts->setTimeout(1, 3.0);
        CPPUNIT_ASSERT(timeouts->hasTimeoutSet(1));
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), timeouts->numberOfTimeoutsSet());

        timeouts->setTimeout(2, 1.0);
        CPPUNIT_ASSERT(timeouts->hasTimeoutSet(2));
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), timeouts->numberOfTimeoutsSet());

        timeouts->cancelTimeout(2);
        CPPUNIT_ASSERT(!timeouts->hasTimeoutSet(2));
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), timeouts->numberOfTimeoutsSet());

        timeouts->cancelTimeout(1);
        CPPUNIT_ASSERT(!timeouts->hasTimeoutSet(1));
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), timeouts->numberOfTimeoutsSet());

        // assure clean scheduler
        CPPUNIT_ASSERT(!scheduler->processOneEvent());

    } // cancelTimeouts

    void
    MultipleTimeoutTest::cancelAllTimeouts()
    {
        timeouts->setTimeout(1, 3.0);
        CPPUNIT_ASSERT(timeouts->hasTimeoutSet(1));
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), timeouts->numberOfTimeoutsSet());

        timeouts->setTimeout(2, 1.0);
        CPPUNIT_ASSERT(timeouts->hasTimeoutSet(2));
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), timeouts->numberOfTimeoutsSet());

        timeouts->cancelAllTimeouts();
        CPPUNIT_ASSERT(!timeouts->hasTimeoutSet(1));
        CPPUNIT_ASSERT(!timeouts->hasTimeoutSet(2));
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), timeouts->numberOfTimeoutsSet());

        // assure clean scheduler
        CPPUNIT_ASSERT(!scheduler->processOneEvent());

    } // cancelAllTimeouts

    void
    MultipleTimeoutTest::cancelAllTimeoutsNoTimeoutSet()
    {
        timeouts->cancelAllTimeouts();

        // assure clean scheduler
        CPPUNIT_ASSERT(!scheduler->processOneEvent());

    } // cancelAllTimeoutsNoTimeoutSet

    void
    MultipleTimeoutTest::testTimeouts()
    {
        timeouts->setTimeout(1, 3.0);
        timeouts->setTimeout(2, 1.0);
        timeouts->setTimeout(3, 2.0);

        simTimeType startTime = wns::simulator::getEventScheduler()->getTime();

        CPPUNIT_ASSERT(scheduler->processOneEvent());

        CPPUNIT_ASSERT_EQUAL((startTime + 1.0), wns::simulator::getEventScheduler()->getTime());
        CPPUNIT_ASSERT_EQUAL(2, timeouts->timeoutNumber);
        CPPUNIT_ASSERT(!timeouts->hasTimeoutSet(2));
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), timeouts->numberOfTimeoutsSet());

        CPPUNIT_ASSERT(scheduler->processOneEvent());

        CPPUNIT_ASSERT_EQUAL((startTime + 2.0), wns::simulator::getEventScheduler()->getTime());
        CPPUNIT_ASSERT_EQUAL(3, timeouts->timeoutNumber);
        CPPUNIT_ASSERT(!timeouts->hasTimeoutSet(3));
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), timeouts->numberOfTimeoutsSet());

        CPPUNIT_ASSERT(scheduler->processOneEvent());

        CPPUNIT_ASSERT_EQUAL((startTime + 3.0), wns::simulator::getEventScheduler()->getTime());
        CPPUNIT_ASSERT_EQUAL(1, timeouts->timeoutNumber);
        CPPUNIT_ASSERT(!timeouts->hasTimeoutSet(1));
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), timeouts->numberOfTimeoutsSet());

        // assure clean scheduler
        CPPUNIT_ASSERT(!scheduler->processOneEvent());

    } // testTimeouts

    void
    MultipleTimeoutTest::testNewTimeout()
    {
        timeouts->setNewTimeout(1, 3.0);
        timeouts->setNewTimeout(1, 1.0);

        simTimeType startTime = wns::simulator::getEventScheduler()->getTime();

        CPPUNIT_ASSERT(scheduler->processOneEvent());

        CPPUNIT_ASSERT_EQUAL((startTime + 1.0), wns::simulator::getEventScheduler()->getTime());
        CPPUNIT_ASSERT_EQUAL(1, timeouts->timeoutNumber);
        CPPUNIT_ASSERT(!timeouts->hasTimeoutSet(1));
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), timeouts->numberOfTimeoutsSet());

        // assure clean scheduler
        CPPUNIT_ASSERT(!scheduler->processOneEvent());

    } // testNewTimeout

    void
    MultipleTimeoutTest::destructor()
    {
        DerivedMultipleTimeout* timeoutTest = new DerivedMultipleTimeout();

        timeoutTest->setTimeout(1, 3.0);
        CPPUNIT_ASSERT(timeoutTest->hasTimeoutSet(1));
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), timeoutTest->numberOfTimeoutsSet());

        delete timeoutTest;

        // assure clean scheduler
        CPPUNIT_ASSERT(!scheduler->processOneEvent());

    } // destructor

} // tests
} // events
} // wns


