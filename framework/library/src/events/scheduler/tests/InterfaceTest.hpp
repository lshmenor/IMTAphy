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

#ifndef WNS_EVENTS_SCHEDULER_TESTS_INTERFACE_HPP
#define WNS_EVENTS_SCHEDULER_TESTS_INTERFACE_HPP

#include <WNS/events/scheduler/Interface.hpp>
#include <WNS/events/scheduler/Callable.hpp>
#include <WNS/events/scheduler/INotification.hpp>
#include <WNS/Observer.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <functional>

namespace wns { namespace events { namespace scheduler { namespace tests {

    class InterfaceTest :
        public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE( InterfaceTest );
        CPPUNIT_TEST( testOneEvent );
        CPPUNIT_TEST( testStart );
        CPPUNIT_TEST( testStop );
        CPPUNIT_TEST( testReset );
        CPPUNIT_TEST( testCommand );
        CPPUNIT_TEST( testGetTime );
        CPPUNIT_TEST( testdeleteEvent );
        CPPUNIT_TEST( testSendNow );
        CPPUNIT_TEST( testSendDelay );
        CPPUNIT_TEST( testSendAt );
        CPPUNIT_TEST( testSize );
        CPPUNIT_TEST( testSimultaneousNowEvents );
        CPPUNIT_TEST( testSimultaneousDelayEvents );
        CPPUNIT_TEST( testEventStates );
        CPPUNIT_TEST( testOnProcessOneEvent );
        CPPUNIT_TEST( testOnCancelEvent );
        CPPUNIT_TEST( testOnScheduleNow );
        CPPUNIT_TEST( testOnSchedule );
        CPPUNIT_TEST( testOnScheduleDelay );
        CPPUNIT_TEST( testOnAddEvent );

        // This test fails in wns::events::scheduler::Heap and can't be
        // fixed easily.
        // CPPUNIT_TEST( testFIFO );

        CPPUNIT_TEST( testCancelLastEventAfterEndOfSimulation );
        CPPUNIT_TEST( testCancelAlreadyCanceledEvent );
        CPPUNIT_TEST( testCancelAlreadyCalledEvent );
        CPPUNIT_TEST( testCancelCurrentlyProcessedEvent );
        CPPUNIT_TEST_SUITE_END_ABSTRACT();

        class EventHandlerStub;

        class ObjectWithId
        {
        public:

            ObjectWithId(long int id,  EventHandlerStub* receiver) :
                id_(id),
                receiver_(receiver)
            {
            }

            virtual void
            operator()();

            virtual ~ObjectWithId()
            {
            }

            long int getID() const
            {
                return id_;
            }


            bool operator==(const ObjectWithId& rhs) const
            {
                return getID() == rhs.getID();
            }

        private:
            ObjectWithId() :
                receiver_(NULL)
            {}

            long int id_;
            EventHandlerStub* receiver_;
        };

        class EventHandlerStub
        {
        public:
            typedef std::vector<ObjectWithId> ContainerType;

            virtual
            ~EventHandlerStub()
            {
                flush();
            }

            void flush()
            {
                objects.clear();
            }

            virtual void onEvent(const ObjectWithId& objWithId)
            {
                objects.push_back(objWithId);
            }

            ContainerType objects;
        };


        class NoOp
        {
        public:
            virtual void
            operator()()
            {
            }
            virtual ~NoOp(){}
        };

        class SomeCommand
        {
        public:
            SomeCommand(bool* _called) :
                called(_called)
            {
                *called = false;
            }

            virtual
            ~SomeCommand()
            {}

            virtual void
            operator()()
            {
                *called = true;
            }

        private:
            bool* called;
        };


        class SelfDelete
        {
        public:
            SelfDelete(wns::events::scheduler::tests::InterfaceTest& it) :
                it_(it)
            {
            }

            virtual void
            operator()();

            virtual
            ~SelfDelete()
            {}

        private:
            // Not reference counted to avoid cyclic references.
            wns::events::scheduler::tests::InterfaceTest& it_;
        };

        class CheckIsRunning
        {
        public:
            CheckIsRunning(wns::events::scheduler::tests::InterfaceTest& it) :
                it_(it)
            {
            }

            virtual void
            operator()();

            virtual
            ~CheckIsRunning()
            {
            }

        private:
            // Not reference counted to avoid cyclic references.
            wns::events::scheduler::tests::InterfaceTest& it_;
        };

        class Stop
        {
        public:
            Stop(wns::events::scheduler::Interface* es) :
                scheduler(es)
            {
            }

            virtual void
            operator()()
            {
                scheduler->stop();
            }

            virtual
            ~Stop()
            {
            }

        private:
            wns::events::scheduler::Interface* scheduler;
        };


        class SchedulerObserver :
            public Observer<INotification>
        {
        public:
            SchedulerObserver();

            virtual void
            doOnProcessOneEvent();

            virtual void
            doOnAddEvent();

            virtual void
            doOnCancelEvent();

            virtual void
            doOnScheduleNow();

            virtual void
            doOnSchedule();

            virtual void
            doOnScheduleDelay();

            int onProcessOneEventCounter;
            int onAddEventCounter;
            int onCancelEventCounter;
            int onScheduleNowCounter;
            int onScheduleCounter;
            int onScheduleDelayCounter;
        };

    public:
        void setUp();
        void tearDown();

        void testOneEvent();
        void testStart();
        void testStop();
        void testReset();
        void testCommand();
        void testGetTime();
        void testdeleteEvent();
        void testSendNow();
        void testSendDelay();
        void testSendAt();
        void testSize();
        void testSimultaneousNowEvents();
        void testSimultaneousDelayEvents();
        void testEventStates();
        // test the observing feature
        void testOnProcessOneEvent();
        void testOnCancelEvent();
        void testOnScheduleNow();
        void testOnSchedule();
        void testOnScheduleDelay();
        void testOnAddEvent();
        // this one is disabled
        void testFIFO();
        // These should all throw CancelException
        void testCancelLastEventAfterEndOfSimulation();
        void testCancelAlreadyCalledEvent();
        void testCancelAlreadyCanceledEvent();
        void testCancelCurrentlyProcessedEvent();

    private:
        virtual Interface*
        newTestee() = 0;

        virtual void
        deleteTestee(Interface* scheduler) = 0;

        Interface* scheduler;
        EventHandlerStub* receiver;
        EventHandlerStub* receiver2;
        IEventPtr marker;
        bool exceptionThrown;

    };

} // tests
} // scheduler
} // events
} // wns

#endif // NOT defined WNS_EVENTS_SCHEDULER_TESTS_INTERFACE_HPP

