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

#include <WNS/events/scheduler/tests/InterfaceTest.hpp>
#include <WNS/events/MemberFunction.hpp>

using namespace wns::events::scheduler::tests;
using namespace wns::events::scheduler;


void
InterfaceTest::ObjectWithId::operator()()
{
    if (receiver_ != NULL)
    {
        receiver_->onEvent(*this);
    }
}

void
InterfaceTest::SelfDelete::operator()()
{
    try
    {
        it_.marker->cancel();
    }
    catch (const wns::events::scheduler::IEvent::CancelException&)
    {
        it_.exceptionThrown = true;
    }
}

void
InterfaceTest::CheckIsRunning::operator()()
{
    CPPUNIT_ASSERT( it_.marker->isRunning() );
}

InterfaceTest::SchedulerObserver::SchedulerObserver() :
    onProcessOneEventCounter(0),
    onAddEventCounter(0),
    onCancelEventCounter(0),
    onScheduleNowCounter(0),
    onScheduleCounter(0),
    onScheduleDelayCounter(0)
{
}

void
InterfaceTest::SchedulerObserver::doOnProcessOneEvent()
{
    ++onProcessOneEventCounter;
}

void
InterfaceTest::SchedulerObserver::doOnAddEvent()
{
    ++onAddEventCounter;
}

void
InterfaceTest::SchedulerObserver::doOnCancelEvent()
{
    ++onCancelEventCounter;
}

void
InterfaceTest::SchedulerObserver::doOnScheduleNow()
{
    ++onScheduleNowCounter;
}

void
InterfaceTest::SchedulerObserver::doOnSchedule()
{
    ++onScheduleCounter;
}

void
InterfaceTest::SchedulerObserver::doOnScheduleDelay()
{
    ++onScheduleDelayCounter;
}



void
InterfaceTest::setUp()
{
    exceptionThrown = false;
    scheduler = NULL;
    receiver = NULL;
    receiver2 = NULL;

    scheduler = this->newTestee();

    receiver = new EventHandlerStub();
    receiver2 = new EventHandlerStub();
} // setUp

void
InterfaceTest::tearDown()
{
    // process the last events
    // scheduler->start();
    if (receiver != NULL)
    {
        delete receiver;
        receiver = NULL;
    }
    if (receiver2 != NULL)
    {
        delete receiver2;
        receiver2 = NULL;
    }
    this->deleteTestee(scheduler);
} // tearDown

void
InterfaceTest::testOneEvent()
{
    ObjectWithId e = ObjectWithId(4711, receiver);
    ObjectWithId e2 = ObjectWithId(4712, receiver);

    scheduler->scheduleNow(e);
    scheduler->scheduleNow(e2);
    scheduler->processOneEvent();

    CPPUNIT_ASSERT(receiver->objects.size() == 1);
    CPPUNIT_ASSERT(receiver->objects.front() == e);

    // process the last event
    scheduler->processOneEvent();

    receiver->flush();
} // testOneEvent

void
InterfaceTest::testStart()
{
    scheduler->scheduleNow(ObjectWithId(4711, receiver));
    scheduler->scheduleDelay(ObjectWithId(4712, receiver), 42.0);

    scheduler->start();
    CPPUNIT_ASSERT_EQUAL(wns::simulator::Time(42.0), scheduler->getTime());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), receiver->objects.size());
    CPPUNIT_ASSERT_EQUAL(static_cast<long int>(4711), receiver->objects[0].getID());
    CPPUNIT_ASSERT_EQUAL(static_cast<long int>(4712), receiver->objects[1].getID());

    receiver->flush();
} // testStart

void
InterfaceTest::testStop()
{
    scheduler->scheduleNow(ObjectWithId(4711, receiver));
    scheduler->scheduleDelay(ObjectWithId(4712, receiver), 23.0);
    scheduler->scheduleDelay(ObjectWithId(4713, receiver), 42.0);
    scheduler->scheduleDelay(Stop(scheduler), 42.0);
    scheduler->scheduleDelay(ObjectWithId(4714, receiver), 42.0);
    scheduler->scheduleDelay(ObjectWithId(4715, receiver), 52.0);

    scheduler->start();

    CPPUNIT_ASSERT_EQUAL(wns::simulator::Time(42.0), scheduler->getTime());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), receiver->objects.size());

    receiver->flush();
} // testStop

void
InterfaceTest::testReset()
{
    scheduler->scheduleNow(ObjectWithId(4711, receiver));
    scheduler->scheduleDelay(ObjectWithId(4712, receiver), 42.0);
    scheduler->start();
    receiver->flush();

    scheduler->reset();
    scheduler->start();

    CPPUNIT_ASSERT_EQUAL(wns::simulator::Time(0.0), scheduler->getTime());
    CPPUNIT_ASSERT(receiver->objects.empty());
} // testReset

void
InterfaceTest::testCommand()
{
    ObjectWithId e = ObjectWithId(4711, receiver);
    scheduler->scheduleNow(e);

    bool called;
    ICommandPtr commandHandle = scheduler->queueCommand(SomeCommand(&called));
    CPPUNIT_ASSERT(!commandHandle->isNotSubmitted());
    CPPUNIT_ASSERT(commandHandle->isQueued());
    CPPUNIT_ASSERT(!commandHandle->isRunning());
    CPPUNIT_ASSERT(!commandHandle->isFinished());
    CPPUNIT_ASSERT(!commandHandle->isCanceled());

    CPPUNIT_ASSERT(!called);
    scheduler->start();
    CPPUNIT_ASSERT(!commandHandle->isNotSubmitted());
    CPPUNIT_ASSERT(!commandHandle->isQueued());
    CPPUNIT_ASSERT(!commandHandle->isRunning());
    CPPUNIT_ASSERT(commandHandle->isFinished());
    CPPUNIT_ASSERT(!commandHandle->isCanceled());
    CPPUNIT_ASSERT(called);

    scheduler->queueCommand(SomeCommand(&called));

    CPPUNIT_ASSERT(!called);
    scheduler->processOneEvent();
    CPPUNIT_ASSERT(called);
    receiver->flush();
} // testCommand

void
InterfaceTest::testGetTime()
{
    CPPUNIT_ASSERT( scheduler->getTime() == 0.0 );

    scheduler->schedule(ObjectWithId(4711, receiver), 23.0);
    scheduler->processOneEvent();
    CPPUNIT_ASSERT_EQUAL(wns::simulator::Time(23.0), scheduler->getTime());
    receiver->flush();

    scheduler->schedule(ObjectWithId(4712, receiver), 42.0);
    scheduler->processOneEvent();
    CPPUNIT_ASSERT_EQUAL(wns::simulator::Time(42.0), scheduler->getTime());
    receiver->flush();
}

void
InterfaceTest::testdeleteEvent()
{
    ObjectWithId ev = ObjectWithId(4711, receiver);
    scheduler->schedule(ObjectWithId(4712, receiver), 11.0);
    scheduler->schedule(ObjectWithId(4713, receiver), 23.0);
    IEventPtr event = scheduler->schedule(ev, 23.0);
    scheduler->schedule(ObjectWithId(4714, receiver), 23.0);
    scheduler->schedule(ObjectWithId(4715, receiver), 42.0);

    scheduler->cancelEvent(event);

    scheduler->start();

    CPPUNIT_ASSERT_EQUAL( wns::simulator::Time(42.0), scheduler->getTime());
    CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(4), receiver->objects.size() );
    CPPUNIT_ASSERT_EQUAL( static_cast<long int>(4712), receiver->objects[0].getID() );
    CPPUNIT_ASSERT_EQUAL( static_cast<long int>(4713), receiver->objects[1].getID() );
    CPPUNIT_ASSERT_EQUAL( static_cast<long int>(4714), receiver->objects[2].getID() );
    CPPUNIT_ASSERT_EQUAL( static_cast<long int>(4715), receiver->objects[3].getID() );
    receiver->flush();
}

void
InterfaceTest::testSendNow()
{
    scheduler->schedule(ObjectWithId(4712, receiver), 11.0);
    scheduler->schedule(ObjectWithId(4713, receiver), 23.0);

    scheduler->processOneEvent();
    CPPUNIT_ASSERT_EQUAL( wns::simulator::Time(11.0), scheduler->getTime() );
    CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(1), receiver->objects.size() );
    CPPUNIT_ASSERT_EQUAL( static_cast<long int>(4712), receiver->objects.at(0).getID() );

    scheduler->scheduleNow(ObjectWithId(4714, receiver));
    scheduler->processOneEvent();
    CPPUNIT_ASSERT_EQUAL( wns::simulator::Time(11.0), scheduler->getTime() );
    CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(2), receiver->objects.size() );
    CPPUNIT_ASSERT_EQUAL( static_cast<long int>(4714), receiver->objects.at(1).getID() );

    scheduler->scheduleNow(ObjectWithId(4715, receiver));
    scheduler->processOneEvent();
    CPPUNIT_ASSERT_EQUAL( wns::simulator::Time(11.0), scheduler->getTime() );
    CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(3), receiver->objects.size() );
    CPPUNIT_ASSERT_EQUAL( static_cast<long int>(4715), receiver->objects.at(2).getID() );

    scheduler->processOneEvent();
    CPPUNIT_ASSERT_EQUAL( wns::simulator::Time(23.0), scheduler->getTime() );
    CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(4), receiver->objects.size() );
    CPPUNIT_ASSERT_EQUAL( static_cast<long int>(4713), receiver->objects.at(3).getID() );

    receiver->flush();
}

void
InterfaceTest::testSendDelay()
{
    scheduler->scheduleDelay(ObjectWithId(4711, receiver), 11.0);
    scheduler->scheduleDelay(ObjectWithId(4712, receiver), 23.0);
    scheduler->scheduleDelay(ObjectWithId(4714, receiver), 42.0);

    scheduler->processOneEvent();
    CPPUNIT_ASSERT( scheduler->getTime() == 11.0 );

    scheduler->scheduleDelay(ObjectWithId(4713, receiver), 0.0);
    scheduler->scheduleDelay(ObjectWithId(4715, receiver), 42.0);
    scheduler->scheduleDelay(ObjectWithId(4716, receiver), 11.0);

    scheduler->processOneEvent();
    scheduler->processOneEvent();
    scheduler->processOneEvent();
    scheduler->processOneEvent();
    scheduler->processOneEvent();


    CPPUNIT_ASSERT_DOUBLES_EQUAL( 53.0, scheduler->getTime(), 0.0 );

    CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(6), receiver->objects.size() );
    CPPUNIT_ASSERT_EQUAL( static_cast<long int>(4711), receiver->objects.at(0).getID() );
    CPPUNIT_ASSERT_EQUAL( static_cast<long int>(4713), receiver->objects.at(1).getID() );
    CPPUNIT_ASSERT_EQUAL( static_cast<long int>(4716), receiver->objects.at(2).getID() );
    CPPUNIT_ASSERT_EQUAL( static_cast<long int>(4712), receiver->objects.at(3).getID() );
    CPPUNIT_ASSERT_EQUAL( static_cast<long int>(4714), receiver->objects.at(4).getID() );
    CPPUNIT_ASSERT_EQUAL( static_cast<long int>(4715), receiver->objects.at(5).getID() );

    receiver->flush();
}

void
InterfaceTest::testSendAt()
{
    scheduler->schedule(ObjectWithId(4711, receiver), 11.0);
    scheduler->schedule(ObjectWithId(4712, receiver), 23.0);
    scheduler->schedule(ObjectWithId(4713, receiver), 23.0);
    scheduler->schedule(ObjectWithId(4714, receiver), 42.0);

    scheduler->processOneEvent();
    scheduler->schedule(ObjectWithId(4715, receiver), 53.0);
    scheduler->schedule(ObjectWithId(4716, receiver), 22.0);
    scheduler->processOneEvent();
    scheduler->processOneEvent();
    scheduler->processOneEvent();
    scheduler->processOneEvent();
    scheduler->processOneEvent();


    CPPUNIT_ASSERT( 53.0 == scheduler->getTime() );

    CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(6), receiver->objects.size() );
    CPPUNIT_ASSERT_EQUAL( static_cast<long int>(4711), receiver->objects.at(0).getID() );
    CPPUNIT_ASSERT_EQUAL( static_cast<long int>(4716), receiver->objects.at(1).getID() );
    CPPUNIT_ASSERT_EQUAL( static_cast<long int>(4712), receiver->objects.at(2).getID() );
    CPPUNIT_ASSERT_EQUAL( static_cast<long int>(4713), receiver->objects.at(3).getID() );
    CPPUNIT_ASSERT_EQUAL( static_cast<long int>(4714), receiver->objects.at(4).getID() );
    CPPUNIT_ASSERT_EQUAL( static_cast<long int>(4715), receiver->objects.at(5).getID() );

    receiver->flush();
}

void
InterfaceTest::testSize()
{
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), scheduler->size());

    scheduler->schedule(NoOp(), 5.0);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), scheduler->size());

    scheduler->schedule(NoOp(), 11.0);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), scheduler->size());

    scheduler->schedule(NoOp(), 11.0);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), scheduler->size());

    scheduler->schedule(NoOp(), 11.0);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), scheduler->size());

    scheduler->schedule(NoOp(), 13.0);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), scheduler->size());

    scheduler->processOneEvent();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), scheduler->size());

    scheduler->processOneEvent();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), scheduler->size());

    scheduler->processOneEvent();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), scheduler->size());

    scheduler->processOneEvent();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), scheduler->size());

    scheduler->processOneEvent();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), scheduler->size());
}

void
InterfaceTest::testSimultaneousNowEvents()
{
    ObjectWithId e = ObjectWithId(4711, receiver);
    ObjectWithId e2 = ObjectWithId(4712, receiver);

    scheduler->scheduleNow(e);
    scheduler->scheduleNow(e2);
    scheduler->start();

    CPPUNIT_ASSERT(receiver->objects[0] == e);
    CPPUNIT_ASSERT(receiver->objects[1] == e2);

    receiver->flush();


    ObjectWithId e3= ObjectWithId(4712, receiver);
    ObjectWithId e4 = ObjectWithId(4711, receiver);

    scheduler->scheduleNow(e3);
    scheduler->scheduleNow(e4);
    scheduler->start();

    CPPUNIT_ASSERT(receiver->objects[0] == e3);
    CPPUNIT_ASSERT(receiver->objects[1] == e4);

    receiver->flush();
} // testSimultaneousNowEvents

void
InterfaceTest::testSimultaneousDelayEvents()
{
    ObjectWithId e = ObjectWithId(4711, receiver);
    ObjectWithId e2 = ObjectWithId(4712, receiver);

    scheduler->scheduleDelay(e, 42.0);
    scheduler->scheduleDelay(e2, 42.0);
    scheduler->start();

    CPPUNIT_ASSERT(receiver->objects[0] == e);
    CPPUNIT_ASSERT(receiver->objects[1] == e2);

    receiver->flush();


    ObjectWithId e3 = ObjectWithId(4712, receiver);
    ObjectWithId e4 = ObjectWithId(4711, receiver);

    scheduler->scheduleDelay(e3, 42.0);
    scheduler->scheduleDelay(e4, 42.0);
    scheduler->start();

    CPPUNIT_ASSERT(receiver->objects[0] == e3);
    CPPUNIT_ASSERT(receiver->objects[1] == e4);

    receiver->flush();
} // testSimultaneousDelayEvents

void
InterfaceTest::testEventStates()
{
    // isRunning is always checked by SelfDelete
    marker = scheduler->schedule(CheckIsRunning(*this), 5);
    IEventPtr cancelEvent = scheduler->schedule(NoOp(), 5);
    IEventPtr cancelEvent2 = scheduler->schedule(NoOp(), 5);

    CPPUNIT_ASSERT( marker->isQueued() );
    CPPUNIT_ASSERT( !marker->isNotSubmitted() );
    CPPUNIT_ASSERT( !marker->isRunning() );
    CPPUNIT_ASSERT( !marker->isCanceled() );
    CPPUNIT_ASSERT( !marker->isFinished() );

    CPPUNIT_ASSERT( cancelEvent->isQueued() );
    CPPUNIT_ASSERT( !cancelEvent->isNotSubmitted() );
    CPPUNIT_ASSERT( !cancelEvent->isRunning() );
    CPPUNIT_ASSERT( !cancelEvent->isCanceled() );
    CPPUNIT_ASSERT( !cancelEvent->isFinished() );

    CPPUNIT_ASSERT( cancelEvent2->isQueued() );
    CPPUNIT_ASSERT( !cancelEvent2->isNotSubmitted() );
    CPPUNIT_ASSERT( !cancelEvent2->isRunning() );
    CPPUNIT_ASSERT( !cancelEvent2->isCanceled() );
    CPPUNIT_ASSERT( !cancelEvent2->isFinished() );

    cancelEvent->cancel();
    CPPUNIT_ASSERT( !cancelEvent->isQueued() );
    CPPUNIT_ASSERT( !cancelEvent->isNotSubmitted() );
    CPPUNIT_ASSERT( !cancelEvent->isRunning() );
    CPPUNIT_ASSERT( cancelEvent->isCanceled() );
    CPPUNIT_ASSERT( !cancelEvent->isFinished() );

    scheduler->cancelEvent(cancelEvent2);
    CPPUNIT_ASSERT( !cancelEvent2->isQueued() );
    CPPUNIT_ASSERT( !cancelEvent2->isNotSubmitted() );
    CPPUNIT_ASSERT( !cancelEvent2->isRunning() );
    CPPUNIT_ASSERT( cancelEvent2->isCanceled() );
    CPPUNIT_ASSERT( !cancelEvent2->isFinished() );

    scheduler->start();
    CPPUNIT_ASSERT( !marker->isQueued() );
    CPPUNIT_ASSERT( !marker->isNotSubmitted() );
    CPPUNIT_ASSERT( !marker->isRunning() );
    CPPUNIT_ASSERT( !marker->isCanceled() );
    CPPUNIT_ASSERT( marker->isFinished() );

    CPPUNIT_ASSERT( !cancelEvent->isQueued() );
    CPPUNIT_ASSERT( !cancelEvent->isNotSubmitted() );
    CPPUNIT_ASSERT( !cancelEvent->isRunning() );
    CPPUNIT_ASSERT( cancelEvent->isCanceled() );
    CPPUNIT_ASSERT( !cancelEvent->isFinished() );

    CPPUNIT_ASSERT( !cancelEvent2->isQueued() );
    CPPUNIT_ASSERT( !cancelEvent2->isNotSubmitted() );
    CPPUNIT_ASSERT( !cancelEvent2->isRunning() );
    CPPUNIT_ASSERT( cancelEvent2->isCanceled() );
    CPPUNIT_ASSERT( !cancelEvent2->isFinished() );
}

void
InterfaceTest::testFIFO()
{
    /*
     * Matthias Malkowski pointed out a pretty nasty habit of the HeapScheduler that
     * the STLScheduler doesn't show:
     *
     *  - Assume the following instances of type Event are created in the following
     *    chronological order:
     *    E1, E2, E3, E4
     *
     *  - Assume these events are scheduled for the same point in time (e.g. 1.0s)
     *    in the following chronological order:
     *    E2, E4, E1, E3
     *
     * Then, the STLScheduler will execute the events in the following order:
     *    E2, E4, E1, E3
     *
     * while the HeapScheduler will execute the events in the following order:
     *    E1, E2, E3, E4
     *
     * The execution order of the STLScheduler is the one, that is expected by most
     * people I think. Therefor the test assures this order.
     */

    ObjectWithId e1 = ObjectWithId(4711, receiver);
    ObjectWithId e2 = ObjectWithId(4712, receiver);
    ObjectWithId e3 = ObjectWithId(4713, receiver);
    ObjectWithId e4 = ObjectWithId(4714, receiver);

    scheduler->scheduleDelay(e2, 42);
    scheduler->scheduleDelay(e4, 42);
    scheduler->scheduleDelay(e1, 42);
    scheduler->scheduleDelay(e3, 42);

    scheduler->start();

    CPPUNIT_ASSERT(receiver->objects[0] == e2);
    CPPUNIT_ASSERT(receiver->objects[1] == e4);
    CPPUNIT_ASSERT(receiver->objects[2] == e1);
    CPPUNIT_ASSERT(receiver->objects[3] == e3);

    receiver->flush();
}

void
InterfaceTest::testCancelLastEventAfterEndOfSimulation()
{
    IEventPtr event1 = scheduler->scheduleDelay(ObjectWithId(4711, receiver), 11);

    typedef wns::events::MemberFunction<wns::events::scheduler::Interface>
        EventSchedulerFunction;

    scheduler->schedule(
        EventSchedulerFunction(
            scheduler,
            &wns::events::scheduler::Interface::stop),
        23);

    IEventPtr event2 = scheduler->scheduleDelay(ObjectWithId(4711, receiver), 42);

    scheduler->start();

    CPPUNIT_ASSERT( wns::simulator::Time(23) == scheduler->getTime() );
    CPPUNIT_ASSERT_NO_THROW( scheduler->cancelEvent(event2) );
}

void
InterfaceTest::testCancelAlreadyCanceledEvent()
{

    IEventPtr event1 = scheduler->scheduleDelay(ObjectWithId(4711, receiver), 11);
    IEventPtr event2 = scheduler->scheduleDelay(ObjectWithId(4711, receiver), 42);

    scheduler->cancelEvent(event2);

    CPPUNIT_ASSERT_THROW( scheduler->cancelEvent(event2), wns::events::scheduler::IEvent::CancelException );
}

void
InterfaceTest::testCancelAlreadyCalledEvent()
{

    IEventPtr event1 = scheduler->scheduleDelay(ObjectWithId(4711, receiver), 11);
    IEventPtr event2 = scheduler->scheduleDelay(ObjectWithId(4711, receiver), 42);

    scheduler->start();

    CPPUNIT_ASSERT_THROW( scheduler->cancelEvent(event2), wns::events::scheduler::IEvent::CancelException );
}

void
InterfaceTest::testCancelCurrentlyProcessedEvent()
{
    scheduler->schedule(NoOp(), 5);
    scheduler->schedule(NoOp(), 11);

    SelfDelete selfDelete (*this);
    this->marker = scheduler->schedule(selfDelete, 11);
    scheduler->schedule(NoOp(), 11);
    scheduler->schedule(NoOp(), 13);
    scheduler->start();
    CPPUNIT_ASSERT( exceptionThrown == true );
}

void
InterfaceTest::testOnProcessOneEvent()
{
    SchedulerObserver observer;
    observer.startObserving(scheduler);
    IEventPtr event = scheduler->schedule(NoOp(), 5);
    CPPUNIT_ASSERT_EQUAL( 0, observer.onProcessOneEventCounter );
    scheduler->processOneEvent();
    CPPUNIT_ASSERT_EQUAL( 1, observer.onProcessOneEventCounter );
}

void
InterfaceTest::testOnCancelEvent()
{
    SchedulerObserver observer;
    observer.startObserving(scheduler);
    IEventPtr event = scheduler->schedule(NoOp(), 5);
    IEventPtr event2 = scheduler->schedule(NoOp(), 10);
    CPPUNIT_ASSERT_EQUAL( 0, observer.onCancelEventCounter );
    event->cancel();
    CPPUNIT_ASSERT_EQUAL( 1, observer.onCancelEventCounter );
    scheduler->cancelEvent(event2);
    CPPUNIT_ASSERT_EQUAL( 2, observer.onCancelEventCounter );
}

void
InterfaceTest::testOnScheduleNow()
{
    SchedulerObserver observer;
    observer.startObserving(scheduler);
    CPPUNIT_ASSERT_EQUAL( 0, observer.onScheduleNowCounter );
    IEventPtr event = scheduler->scheduleNow(NoOp());
    CPPUNIT_ASSERT_EQUAL( 1, observer.onScheduleNowCounter );
    IEventPtr event2 = scheduler->scheduleNow(NoOp());
    CPPUNIT_ASSERT_EQUAL( 2, observer.onScheduleNowCounter );
}

void
InterfaceTest::testOnSchedule()
{
    SchedulerObserver observer;
    observer.startObserving(scheduler);
    CPPUNIT_ASSERT_EQUAL( 0, observer.onScheduleCounter );
    IEventPtr event = scheduler->schedule(NoOp(), 3);
    CPPUNIT_ASSERT_EQUAL( 1, observer.onScheduleCounter );
    IEventPtr event2 = scheduler->schedule(NoOp(), 3);
    CPPUNIT_ASSERT_EQUAL( 2, observer.onScheduleCounter );
}

void
InterfaceTest::testOnScheduleDelay()
{
    SchedulerObserver observer;
    observer.startObserving(scheduler);
    CPPUNIT_ASSERT_EQUAL( 0, observer.onScheduleDelayCounter );
    IEventPtr event = scheduler->scheduleDelay(NoOp(), 3);
    CPPUNIT_ASSERT_EQUAL( 1, observer.onScheduleDelayCounter );
    IEventPtr event2 = scheduler->scheduleDelay(NoOp(), 3);
    CPPUNIT_ASSERT_EQUAL( 2, observer.onScheduleDelayCounter );
}

void
InterfaceTest::testOnAddEvent()
{
    SchedulerObserver observer;
    observer.startObserving(scheduler);
    CPPUNIT_ASSERT_EQUAL( 0, observer.onAddEventCounter );
    IEventPtr event = scheduler->scheduleDelay(NoOp(), 3);
    CPPUNIT_ASSERT_EQUAL( 1, observer.onAddEventCounter );
    IEventPtr event2 = scheduler->schedule(NoOp(), 3);
    CPPUNIT_ASSERT_EQUAL( 2, observer.onAddEventCounter );
    IEventPtr event3 = scheduler->scheduleNow(NoOp());
    CPPUNIT_ASSERT_EQUAL( 3, observer.onAddEventCounter );
}
