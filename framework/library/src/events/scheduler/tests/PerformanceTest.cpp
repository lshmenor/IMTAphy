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

#include <WNS/events/scheduler/tests/PerformanceTest.hpp>
#include <WNS/events/NoOp.hpp>
#include <WNS/StopWatch.hpp>

#include <list>

using namespace wns::events::scheduler::tests;
using namespace wns::events::scheduler;

PerformanceTest::SchedulerObserver::SchedulerObserver() :
    Observer<INotification>(),
    counter(0),
    flag(true)
{
}

void
PerformanceTest::SchedulerObserver::doOnProcessOneEvent()
{
    if (flag)
    {
        ++counter;
    }
}

PerformanceTest::PerformanceTest() :
    scheduler(NULL),
    numberOfEvents(5E6)
{
}

void
PerformanceTest::prepare()
{
    this->scheduler = NULL;

    this->scheduler = this->newTestee();
} // setUp

void
PerformanceTest::cleanup()
{
    // process the last events
    this->scheduler->start();
    this->deleteTestee(this->scheduler);
} // tearDown

void
PerformanceTest::testNow()
{
    std::cout << "\ntestNow(): queuing " << this->numberOfEvents
              << " events upfront with scheduleNow()" << std::endl;
    wns::StopWatch sw;
    sw.start();

    for(int ii = 0; ii < this->numberOfEvents; ++ii)
    {
        this->scheduler->scheduleNow(NoOp());
    }
    this->scheduler->start();
    sw.stop();
    std::cout << "testNow() took " << sw.toString() << std::endl;
    std::cout << "Events/s: " << this->numberOfEvents/sw.getInSeconds()
              << std::endl;
} // testNow

void
PerformanceTest::testNowWithObserver()
{
    std::cout << "\ntestNowWithObserver(): queuing " << this->numberOfEvents
              << " events upfront with scheduleNow()" << std::endl;

    SchedulerObserver observer;
    observer.startObserving(this->scheduler);

    wns::StopWatch sw;
    sw.start();
    for(int ii = 0; ii < this->numberOfEvents; ++ii)
    {
        this->scheduler->scheduleNow(NoOp());
    }
    this->scheduler->start();
    sw.stop();
    std::cout << "testNow() took " << sw.toString() << std::endl;
    std::cout << "Events/s " << this->numberOfEvents/sw.getInSeconds() << std::endl;

} // testNowWithObserver

void
PerformanceTest::testRandom()
{
    std::cout << "\ntestRandom(): queuing " << this->numberOfEvents
              << " events upfront with scheduleDelay(rand())" << std::endl;
    wns::StopWatch sw;
    sw.start();
    for(int ii = 0; ii < this->numberOfEvents; ++ii)
    {
        this->scheduler->scheduleDelay(NoOp(), rand());
    }
    this->scheduler->start();
    sw.stop();
    std::cout << "testRandom() took " << sw.toString() << std::endl;
    std::cout << "Events/s " << this->numberOfEvents/sw.getInSeconds() << std::endl;
} // testRandom

void
PerformanceTest::testIncreasingTime()
{
    std::cout << "\ntestIncreasingTime(): queuing " << this->numberOfEvents
              << " events upfront, where each event is scheduled 1*10E-6 seconds after the previous." << std::endl;
    wns::StopWatch sw;
    sw.start();
    for(int ii = 0; ii < this->numberOfEvents; ++ii)
    {
        this->scheduler->scheduleDelay(NoOp(), 10E-6*ii);
    }
    this->scheduler->start();
    sw.stop();
    std::cout << "testIncreasingTime() took " << sw.toString() << std::endl;
    std::cout << "Events/s " << this->numberOfEvents/sw.getInSeconds() << std::endl;
} // testRandom

void
PerformanceTest::testQueuingDuringRun()
{
    std::cout << "\ntestQueuingDuringRun(): This should be close to most simulations (processing "
              << 100*100*1000 << " events)" << std::endl;
    wns::StopWatch sw;
    sw.start();
    for(int ii = 0; ii < 100; ++ii)
    {
        for(int jj = 0; jj < 100; ++jj)
        {
            this->scheduler->scheduleDelay(SelfQueuing(this->scheduler), 0.001*ii);
        }
    }
    this->scheduler->start();
    sw.stop();
    std::cout << "testQueuingDuringRun() took " << sw.toString() << std::endl;
    std::cout << "Events/s " << 1000*100*100/sw.getInSeconds() << std::endl;
}

void
PerformanceTest::testQueueAndDelete()
{
    std::cout << "\ntestQueueAndDelete(): processing " << 1000*5000
              << " events" << std::endl;
    std::list<IEventPtr> eventList;
    wns::StopWatch sw;
    sw.start();
    for(int ii = 0; ii < 1000; ++ii)
    {
        // and queue 5000 at the same point in time
        for(int jj = 0; jj < 5000; ++jj)
        {
            IEventPtr e = this->scheduler->scheduleDelay(NoOp(), ii*0.001);
            eventList.push_back(e);
        }
    }
    while (eventList.empty() == false)
    {
        this->scheduler->cancelEvent(eventList.front());
        eventList.pop_front();
    }
    this->scheduler->start();
    sw.stop();
    std::cout << "testQueueAndDelete() took " << sw.toString() << std::endl;
    std::cout << "Events/s " << 1000*5000/sw.getInSeconds() << std::endl;
}


void
PerformanceTest::testJistStyle()
{
    // This test is performed as described on http://jist.ece.cornell.edu
    std::cout << "\ntestJistStyle(): processing " << 5000000
              << " events" << std::endl;
    wns::StopWatch sw;
    sw.start();
    this->scheduler->scheduleDelay(JistStyle(this->scheduler), 1);
    this->scheduler->start();
    sw.stop();
    std::cout << "testJistStyle() took " << sw.toString() << std::endl;
    std::cout << "Events/s " << 5000000/sw.getInSeconds() << std::endl;
}
