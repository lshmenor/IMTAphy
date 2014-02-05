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
#include <WNS/events/scheduler/Interface.hpp>
#include <WNS/events/PeriodicRealTimeout.hpp>

#include <WNS/TestFixture.hpp>


namespace wns { namespace events { namespace tests {

    class PeriodicRealTimeoutTest :
        public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE( PeriodicRealTimeoutTest );
        CPPUNIT_TEST( create );
        CPPUNIT_TEST( configureTimeout );
        CPPUNIT_TEST( testPeriod );
        CPPUNIT_TEST( testDelay );
        CPPUNIT_TEST( deleteWhileRunning );
        CPPUNIT_TEST_SUITE_END();
    public:
        void setUp();
        void tearDown();

        void create();
        void configureTimeout();
        void testPeriod();
        void testDelay();
        void deleteWhileRunning();
    private:

        int counter;

        class DerivedPeriodicRealTimeout :
            public PeriodicRealTimeout
        {
        public:
            explicit
            DerivedPeriodicRealTimeout(PeriodicRealTimeoutTest* _pToT) :
                PeriodicRealTimeout(),
                pToT(_pToT)
            {
            }

            virtual
            ~DerivedPeriodicRealTimeout()
            {
                pToT = NULL;
            }

            void periodically()
            {
                ++pToT->counter;
            };

            PeriodicRealTimeoutTest* pToT;
        };

    };

    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( PeriodicRealTimeoutTest, wns::testsuite::Disabled() );

    void
    PeriodicRealTimeoutTest::setUp()
    {
        wns::simulator::getEventScheduler()->reset();
        counter = 0;
    } // setUp

    void
    PeriodicRealTimeoutTest::tearDown()
    {
    } // tearDown

    void
    PeriodicRealTimeoutTest::create()
    {
        DerivedPeriodicRealTimeout* pTo = NULL;
        pTo = new DerivedPeriodicRealTimeout(this);
        CPPUNIT_ASSERT(pTo != NULL);
        delete pTo;
    } // create

    void
    PeriodicRealTimeoutTest::configureTimeout()
    {
        DerivedPeriodicRealTimeout* pTo = NULL;
        pTo = new DerivedPeriodicRealTimeout(this);

        CPPUNIT_ASSERT(!pTo->hasPeriodicRealTimeoutSet());

        pTo->startPeriodicTimeout(0.2);
        CPPUNIT_ASSERT(pTo->hasPeriodicRealTimeoutSet());

        pTo->cancelPeriodicRealTimeout();
        CPPUNIT_ASSERT(!pTo->hasPeriodicRealTimeoutSet());

        delete pTo;
    }

    void
    PeriodicRealTimeoutTest::testPeriod()
    {
        wns::events::scheduler::Interface* scheduler = wns::simulator::getEventScheduler();
        CPPUNIT_ASSERT(!scheduler->processOneEvent()); // assure clean scheduler

        DerivedPeriodicRealTimeout* pTo = new DerivedPeriodicRealTimeout(this);
        double duration = 0.2;
        pTo->startPeriodicTimeout(duration, 0.0);
        CPPUNIT_ASSERT_EQUAL(0, this->counter);

        // sleep for 0.1 seconds
        usleep(100000);
        scheduler->processOneEvent();
        CPPUNIT_ASSERT_EQUAL(1, this->counter);

        // sleep for 0.2 seconds
        usleep(200000);
        scheduler->processOneEvent();
        scheduler->processOneEvent();
        CPPUNIT_ASSERT_EQUAL(2, this->counter);

        // sleep for 0.2 seconds
        usleep(200000);
        scheduler->processOneEvent();
        scheduler->processOneEvent();
        CPPUNIT_ASSERT_EQUAL(3, this->counter);

        pTo->cancelPeriodicRealTimeout();
        // sleep for 0.2 seconds
        usleep(200000);
        scheduler->processOneEvent();
        scheduler->processOneEvent();
        CPPUNIT_ASSERT_EQUAL(3, this->counter);

        delete pTo;
    }

    void
    PeriodicRealTimeoutTest::testDelay()
    {
        wns::events::scheduler::Interface *scheduler = wns::simulator::getEventScheduler();
        CPPUNIT_ASSERT(!scheduler->processOneEvent()); // assure clean scheduler

        DerivedPeriodicRealTimeout* pTo = new DerivedPeriodicRealTimeout(this);
        double duration = 0.2;
        double delay = 0.2;
        pTo->startPeriodicTimeout(duration, delay);

        // sleep for 0.1 seconds
        usleep(100000);
        scheduler->processOneEvent();
        CPPUNIT_ASSERT_EQUAL(0, this->counter);

        // sleep for 0.2 seconds
        usleep(200000);
        scheduler->processOneEvent();
        scheduler->processOneEvent();
        CPPUNIT_ASSERT_EQUAL(1, this->counter);

        // sleep for 0.2 seconds
        usleep(200000);
        scheduler->processOneEvent();
        scheduler->processOneEvent();
        CPPUNIT_ASSERT_EQUAL(2, this->counter);

        pTo->cancelPeriodicRealTimeout();
        // sleep for 0.2 seconds
        usleep(200000);
        scheduler->processOneEvent();
        scheduler->processOneEvent();
        CPPUNIT_ASSERT_EQUAL(2, this->counter);

        delete pTo;
    }

    void
    PeriodicRealTimeoutTest::deleteWhileRunning()
    {
        wns::events::scheduler::Interface *scheduler = wns::simulator::getEventScheduler();
        CPPUNIT_ASSERT(!scheduler->processOneEvent()); // assure clean scheduler

        DerivedPeriodicRealTimeout* pTo = new DerivedPeriodicRealTimeout(this);
        double duration = 0.2;
        double delay = 0.0;
        pTo->startPeriodicTimeout(duration, delay);

        // sleep for 0.1 seconds
        usleep(100000);
        scheduler->processOneEvent();
        scheduler->processOneEvent();
        CPPUNIT_ASSERT_EQUAL(1, this->counter); // one event

        // sleep for 0.2 seconds
        usleep(200000);
        scheduler->processOneEvent();
        scheduler->processOneEvent();
        CPPUNIT_ASSERT_EQUAL(2, this->counter); // one event

        delete pTo;

        // sleep for 0.2 seconds
        usleep(200000);
        scheduler->processOneEvent();
        scheduler->processOneEvent();
        CPPUNIT_ASSERT_EQUAL(2, this->counter); // no additional events
                            // after delete
    }

} // tests
} // events
} // wns


