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

#ifndef WNS_EVENTS_SCHEDULER_TESTS_PERFORMANCETEST_HPP
#define WNS_EVENTS_SCHEDULER_TESTS_PERFORMANCETEST_HPP

#include <WNS/events/scheduler/Interface.hpp>

#include <WNS/TestFixture.hpp>
#include <WNS/Observer.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <functional>

namespace wns { namespace events { namespace scheduler { namespace tests {

    class PerformanceTest :
        public wns::TestFixture
    {
        CPPUNIT_TEST_SUITE( PerformanceTest );
        CPPUNIT_TEST( testJistStyle );
        CPPUNIT_TEST( testNow );
        CPPUNIT_TEST( testNowWithObserver );
        CPPUNIT_TEST( testRandom );
        CPPUNIT_TEST( testIncreasingTime );
        CPPUNIT_TEST( testQueuingDuringRun );
        CPPUNIT_TEST( testQueueAndDelete );
        CPPUNIT_TEST_SUITE_END_ABSTRACT();

        class SelfQueuing
        {
        public:
            SelfQueuing(wns::events::scheduler::Interface* _scheduler) :
                count(1000),
                scheduler(_scheduler)
            {
            }

            virtual
            void operator()()
            {
                --count;
                if (count > 0)
                {
                    // reque myself with 2s delay
                    this->scheduler->scheduleDelay(*this, 2);
                }
            }

            virtual
            ~SelfQueuing()
            {
            }
        private:
            int count;
            wns::events::scheduler::Interface* scheduler;
        };


        class JistStyle
        {
        public:
            JistStyle(wns::events::scheduler::Interface* _scheduler) :
                count(5E6),
                scheduler(_scheduler)
            {
            }

            void operator()()
            {
                if (--count > 0)
                {
                    // reque myself with 1s delay
                    this->scheduler->scheduleDelay(*this, 1);
                }
            }

            ~JistStyle()
            {
            }
        private:
            int count;
            wns::events::scheduler::Interface* scheduler;
        };

        class SchedulerObserver :
            public Observer<INotification>,
            public wns::events::scheduler::IgnoreAllNotifications
        {
        public:
            SchedulerObserver();
        private:

            virtual void
            doOnProcessOneEvent();

            int counter;
            bool flag;
        };

    public:
        PerformanceTest();

        void prepare();
        void cleanup();

        void testNow();
        void testNowWithObserver();
        void testRandom();
        void testIncreasingTime();
        void testQueuingDuringRun();
        void testQueueAndDelete();
        void testJistStyle();

    private:
        virtual Interface*
        newTestee() = 0;

        virtual void
        deleteTestee(Interface* scheduler) = 0;

        Interface* scheduler;
        int numberOfEvents;

    };

} // tests
} // scheduler
} // events
} // wns

#endif // NOT defined WNS_EVENTS_SCHEDULER_TESTS_PERFORMANCETEST_HPP
