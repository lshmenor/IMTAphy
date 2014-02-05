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

#include <WNS/events/tests/PeriodicTimeoutTest.hpp>
#include <WNS/simulator/ISimulator.hpp>
#include <WNS/events/NoOp.hpp>

using namespace wns::events::tests;

CPPUNIT_TEST_SUITE_REGISTRATION( PeriodicTimeoutTest );

void
PeriodicTimeoutTest::prepare()
{
}

void
PeriodicTimeoutTest::cleanup()
{
}

void
PeriodicTimeoutTest::create()
{
    derivedPeriodicTimeout* pTo = NULL;
    pTo = new derivedPeriodicTimeout();
    CPPUNIT_ASSERT(pTo != NULL);
    delete pTo;
}

void
PeriodicTimeoutTest::configureTimeout()
{
    derivedPeriodicTimeout* pTo = NULL;
    pTo = new derivedPeriodicTimeout();

    CPPUNIT_ASSERT(pTo != NULL);
    CPPUNIT_ASSERT(!pTo->hasPeriodicTimeoutSet());

    pTo->startPeriodicTimeout(0.01);
    CPPUNIT_ASSERT(pTo->hasPeriodicTimeoutSet());

    pTo->cancelPeriodicTimeout();
    CPPUNIT_ASSERT(!pTo->hasPeriodicTimeoutSet());

    delete pTo;
}

void
PeriodicTimeoutTest::testPeriod()
{
    wns::events::scheduler::Interface *scheduler = wns::simulator::getEventScheduler();
    CPPUNIT_ASSERT(!scheduler->processOneEvent()); // assure clean scheduler

    derivedPeriodicTimeout* pTo = new derivedPeriodicTimeout();
    double duration = 0.01;
    pTo->startPeriodicTimeout(duration);

    double now;
    int loops = 15;
    for (int i = 0; i < loops; ++i)
    {
        CPPUNIT_ASSERT(scheduler->processOneEvent()); // assure event is scheduled
        now = scheduler->getTime();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(now, (duration * i), 1E-6);
        CPPUNIT_ASSERT(pTo->test_counter == i + 1); // assure that function periodically() is periodically executed
    }

    delete pTo;
}

void
PeriodicTimeoutTest::testDelay()
{
    wns::events::scheduler::Interface *scheduler = wns::simulator::getEventScheduler();
    CPPUNIT_ASSERT(!scheduler->processOneEvent()); // assure clean scheduler

    derivedPeriodicTimeout* pTo = new derivedPeriodicTimeout();
    double duration = 0.01;
    double delay = 0.01;
    pTo->startPeriodicTimeout(duration, delay);

    double now;
    int loops = 15;
    for (int i = 0; i < loops; ++i)
    {
        CPPUNIT_ASSERT(scheduler->processOneEvent()); // assure event is scheduled
        now = scheduler->getTime();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(now, (duration * i) + delay, 1E-6);
        CPPUNIT_ASSERT(pTo->test_counter == i + 1); // assure that function periodically() is periodically executed
    }
    delete pTo;
}


void
PeriodicTimeoutTest::testCopyConstructor()
{
    wns::events::scheduler::Interface *scheduler = wns::simulator::getEventScheduler();
    CPPUNIT_ASSERT(!scheduler->processOneEvent()); // assure clean scheduler

    derivedPeriodicTimeout* pTo = new derivedPeriodicTimeout();
    double duration = 0.01;
    double delay = 0.02;
    double now = 0.0;
    pTo->startPeriodicTimeout(duration, delay);

    CPPUNIT_ASSERT(scheduler->processOneEvent()); // assure event is scheduled
    now = scheduler->getTime();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(now, 0.02, 1E-6);
    CPPUNIT_ASSERT(pTo->test_counter == 1); // assure that function periodically() is periodically executed

    CPPUNIT_ASSERT(scheduler->processOneEvent()); // assure event is scheduled
    now = scheduler->getTime();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(now, 0.03, 1E-6);
    CPPUNIT_ASSERT(pTo->test_counter == 2); // assure that function periodically() is periodically executed


    scheduler->scheduleDelay(NoOp(), 0.005);
    CPPUNIT_ASSERT(scheduler->processOneEvent()); // assure event is scheduled
    now = scheduler->getTime();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(now, 0.035, 1E-6);
    CPPUNIT_ASSERT(pTo->test_counter == 2); // assure that function
                        // periodically() is NOT periodically executed

    // now copy!
    derivedPeriodicTimeout* pTo2 = new derivedPeriodicTimeout(*pTo);
    pTo2->test_counter = 0;

    CPPUNIT_ASSERT(scheduler->processOneEvent());
    CPPUNIT_ASSERT(scheduler->processOneEvent()); // assure both events are scheduled
    now = scheduler->getTime();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(now, 0.04, 1E-6);
    CPPUNIT_ASSERT(pTo->test_counter == 3); // assure that function periodically() is periodically executed
    CPPUNIT_ASSERT(pTo2->test_counter == 1); // assure that function periodically() is periodically executed

    // and delete again
    delete pTo2;

    CPPUNIT_ASSERT(scheduler->processOneEvent()); // assure old event is
                              // still scheduled and new
    CPPUNIT_ASSERT(scheduler->processOneEvent()); // event no longer

    now = scheduler->getTime();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(now, 0.06, 1E-6);
    CPPUNIT_ASSERT(pTo->test_counter == 5); // assure that function periodically() is periodically executed

    delete pTo;
}


