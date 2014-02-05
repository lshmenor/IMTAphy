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

#include <WNS/simulator/ISimulator.hpp>

#include <WNS/events/CanTimeout.hpp>
#include <WNS/TestFixture.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace wns { namespace events { namespace tests {

    class CanTimeoutTest :
        public wns::TestFixture
    {
        CPPUNIT_TEST_SUITE( CanTimeoutTest );
        CPPUNIT_TEST( constructor );
        CPPUNIT_TEST( setTimeout );
        CPPUNIT_TEST( cancelTimeout );
        CPPUNIT_TEST( setNewTimeout );
        CPPUNIT_TEST( setTimeoutTwice );
        CPPUNIT_TEST( cancelTimeoutNotSet );
        CPPUNIT_TEST( testTimeout );
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

        void testTimeout();
        void testNewTimeout();

        void destructor();

    private:
        class DerivedCanTimeout :
            public CanTimeout
        {
        public:
            DerivedCanTimeout() :
                CanTimeout(),
                count(0)
            {
            } // DerivedCanTimeout

            void onTimeout()
            {
                ++count;
            } // onTimeout

            int count;
        };

        DerivedCanTimeout* timeout;
        wns::events::scheduler::Interface* scheduler;
    };

    CPPUNIT_TEST_SUITE_REGISTRATION( CanTimeoutTest );

    void
    CanTimeoutTest::prepare()
    {
        scheduler = wns::simulator::getEventScheduler();
        scheduler->reset();
        timeout = new DerivedCanTimeout();

        assure(!scheduler->processOneEvent(), "Scheduler must be clean!");
    } // prepare

    void
    CanTimeoutTest::cleanup()
    {
        delete timeout;
    } // cleanup

    void
    CanTimeoutTest::constructor()
    {
        CPPUNIT_ASSERT(!timeout->hasTimeoutSet());
        CPPUNIT_ASSERT_EQUAL(0, timeout->count);
    } // constructor

    void
    CanTimeoutTest::setTimeout()
    {
        timeout->setTimeout(3.0);
        CPPUNIT_ASSERT(timeout->hasTimeoutSet());
        CPPUNIT_ASSERT_EQUAL(0, timeout->count);
    } // setTimeout

    void
    CanTimeoutTest::cancelTimeout()
    {
        timeout->setTimeout(3.0);
        CPPUNIT_ASSERT(timeout->hasTimeoutSet());

        timeout->cancelTimeout();
        CPPUNIT_ASSERT(!timeout->hasTimeoutSet());

        CPPUNIT_ASSERT_EQUAL(0, timeout->count);

        // assure clean scheduler
        CPPUNIT_ASSERT(!scheduler->processOneEvent());

    } // cancelTimeout

    void
    CanTimeoutTest::setNewTimeout()
    {
        timeout->setNewTimeout(3.0);
        CPPUNIT_ASSERT(timeout->hasTimeoutSet());

        timeout->setNewTimeout(1.0);
        CPPUNIT_ASSERT(timeout->hasTimeoutSet());

        CPPUNIT_ASSERT_EQUAL(0, timeout->count);
    } // setNewTimeout

    void
    CanTimeoutTest::setTimeoutTwice()
    {
        timeout->setTimeout(3.0);
        CPPUNIT_ASSERT(timeout->hasTimeoutSet());

        WNS_ASSERT_ASSURE_EXCEPTION(timeout->setTimeout(3.0));
    } // setTimeoutTwice

    void
    CanTimeoutTest::cancelTimeoutNotSet()
    {
        WNS_ASSERT_ASSURE_EXCEPTION(timeout->cancelTimeout());
    } // cancelTimeoutNotSet

    void
    CanTimeoutTest::testTimeout()
    {
        timeout->setTimeout(3.0);

        simTimeType startTime = wns::simulator::getEventScheduler()->getTime();

        CPPUNIT_ASSERT_EQUAL(0, timeout->count);

        CPPUNIT_ASSERT(scheduler->processOneEvent());

        CPPUNIT_ASSERT_EQUAL((startTime + 3.0), wns::simulator::getEventScheduler()->getTime());
        CPPUNIT_ASSERT_EQUAL(1, timeout->count);
        CPPUNIT_ASSERT(!timeout->hasTimeoutSet());

        // assure clean scheduler
        CPPUNIT_ASSERT(!scheduler->processOneEvent());

    } // testTimeout

    void
    CanTimeoutTest::testNewTimeout()
    {
        timeout->setNewTimeout(3.0);
        CPPUNIT_ASSERT(timeout->hasTimeoutSet());

        timeout->setNewTimeout(1.0);
        CPPUNIT_ASSERT(timeout->hasTimeoutSet());

        CPPUNIT_ASSERT_EQUAL(0, timeout->count);

        simTimeType startTime = wns::simulator::getEventScheduler()->getTime();

        CPPUNIT_ASSERT(scheduler->processOneEvent());

        CPPUNIT_ASSERT_EQUAL((startTime + 1.0), wns::simulator::getEventScheduler()->getTime());
        CPPUNIT_ASSERT_EQUAL(1, timeout->count);
        CPPUNIT_ASSERT(!timeout->hasTimeoutSet());

        // assure clean scheduler
        CPPUNIT_ASSERT(!scheduler->processOneEvent());

    } // testNewTimeout

    void
    CanTimeoutTest::destructor()
    {
        DerivedCanTimeout* timeoutTest = new DerivedCanTimeout();

        timeoutTest->setTimeout(3.0);
        CPPUNIT_ASSERT(timeoutTest->hasTimeoutSet());

        delete timeoutTest;

        // assure clean scheduler
        CPPUNIT_ASSERT(!scheduler->processOneEvent());

    } // destructor

} // tests
} // events
} // wns

/*
  Local Variables:
  mode: c++
  fill-column: 80
  c-basic-offset: 8
  c-comment-only-line-offset: 0
  c-tab-always-indent: t
  indent-tabs-mode: t
  tab-width: 8
  End:
*/
