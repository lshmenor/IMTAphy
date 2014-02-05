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

#include <WNS/container/Pool.hpp>

#include <WNS/simulator/ISimulator.hpp>

#include <WNS/TestFixture.hpp>

#include <ostream>

namespace wns { namespace container { namespace tests {

    class A
    {
    public:
        virtual
        ~A()
        {
        }

        void
        test()
        {
        };

        void
        test_const() const
        {
        };

        bool
        operator < (const A& a) const
        {
            return foo < a.foo;
        }

        const A
        operator ++()
        {
            ++foo;
            return (*this);
        }

        bool
        operator <=(const A& a) const
        {
            return foo <= a.foo;
        }

        bool
        operator >=(const A& a) const
        {
            return foo >= a.foo;
        }

        friend
        std::ostream&
        operator <<(std::ostream& str, const A& a)
        {
            str << a.foo;
            return str;
        }

        int foo;
    };

    class B
    {
    };

    class PoolTest :
        public CppUnit::TestFixture
    {

        CPPUNIT_TEST_SUITE( PoolTest );
        CPPUNIT_TEST( constructor );
        CPPUNIT_TEST( IdInRange );
        CPPUNIT_TEST( searchFreeId );
        CPPUNIT_TEST( freeId );
        CPPUNIT_TEST( checkTimeout );
        CPPUNIT_TEST( useId );
        CPPUNIT_TEST_SUITE_END();
    public:
        void setUp();
        void tearDown();

        void constructor();
        void IdInRange();
        void searchFreeId();
        void freeId();
        void checkTimeout();
        void useId();

        typedef Pool<A> testPool;
    };

    typedef wns::container::Pool<A*> APool;
    typedef wns::container::Pool<int> IntPool;

} // tests
} // container
} // wns

using namespace wns::container::tests;

CPPUNIT_TEST_SUITE_REGISTRATION( PoolTest );

void
PoolTest::setUp()
{
    wns::simulator::getEventScheduler()->reset();
}

void
PoolTest::tearDown()
{
}

void
PoolTest::constructor()
{
    A low, high;
    low.foo = 0;
    high.foo = 10;

    testPool pool(12.0, low, high);

    CPPUNIT_ASSERT(pool.getUnbindDelay() == 12.0);
}

void
PoolTest::IdInRange()
{
    A low, high;
    low.foo = 1;
    high.foo = 10;

    testPool pool(12.0, low, high);

    A id; id.foo = 0;
    WNS_ASSERT_ASSURE_EXCEPTION(pool.isAvailable(id));
    id.foo = 11;
    WNS_ASSERT_ASSURE_EXCEPTION(pool.isAvailable(id));
}

void
PoolTest::searchFreeId()
{
    A low, high;
    low.foo = 100;
    high.foo = 200;

    testPool pool(12.0, low, high);

    for(int i = 100; i <= 200; ++i)
    {
        A id;
        id.foo = i;

        pool.bind(id);
    }

    // Try to bind twice
    for(int i = 100; i <= 200; ++i)
    {
        A id;
        id.foo = i;

        WNS_ASSERT_ASSURE_EXCEPTION(pool.bind(id));
    }

    WNS_ASSERT_ASSURE_EXCEPTION(pool.suggestPort());
}

void PoolTest::freeId()
{

    A low, high;
    low.foo = 100;
    high.foo = 200;

    testPool pool(12.0, low, high);

    wns::events::scheduler::Interface *scheduler = wns::simulator::getEventScheduler();
    CPPUNIT_ASSERT(!scheduler->processOneEvent()); // assure clean scheduler

    // port numbers out of range
    A id; id.foo = 201;
    WNS_ASSERT_ASSURE_EXCEPTION(pool.unbind(id));
    id.foo = 99;
    WNS_ASSERT_ASSURE_EXCEPTION(pool.unbind(id));

    // port to unbind not in use
    WNS_ASSERT_ASSURE_EXCEPTION(pool.unbind(pool.suggestPort()));

    id.foo = 152;
    pool.bind(id);

    int idsInUse = pool.getCapacity();

    // Unbind should not directly return the port to the port pool.
    // A configurable timeout should be used to delay the unbinding of the port.

    pool.unbind(id);

    // No time has passed by now.
    CPPUNIT_ASSERT(idsInUse == pool.getCapacity());
    WNS_ASSERT_ASSURE_EXCEPTION(pool.bind(id));

    // Let time pass, i.e. process the delayed unbind event.
    scheduler->processOneEvent();

    CPPUNIT_ASSERT(idsInUse + 1 == pool.getCapacity());
    CPPUNIT_ASSERT(scheduler->getTime() == 12.0);
}

void PoolTest::checkTimeout()
{
    A low, high;
    low.foo = 100;
    high.foo = 200;

    testPool pool(12.0, low, high);

    wns::events::scheduler::Interface *scheduler = wns::simulator::getEventScheduler();
    CPPUNIT_ASSERT(!scheduler->processOneEvent()); // assure clean scheduler

    A id; id.foo = 150;
    pool.bind(id);
    pool.unbind(id);

    // Let time pass, i.e. process the delayed unbind event.
    scheduler->processOneEvent();
    CPPUNIT_ASSERT(scheduler->getTime() == 12.0);

    testPool otherPool(1217.0, low, high);

    CPPUNIT_ASSERT(!scheduler->processOneEvent()); // assure clean scheduler

    otherPool.bind(id);
    otherPool.unbind(id);

    // Let time pass, i.e. process the delayed unbind event.
    scheduler->processOneEvent();
    CPPUNIT_ASSERT(scheduler->getTime() == 1229.0);

}

void PoolTest::useId()
{
    A low, high;
    low.foo = 100;
    high.foo = 200;

    testPool pool(12.0, low, high);

    int idsInUse = pool.getCapacity();

    // port numbers out of range
    A id; id.foo = 201;
    WNS_ASSERT_ASSURE_EXCEPTION(pool.bind(id));
    id.foo = 99;
    WNS_ASSERT_ASSURE_EXCEPTION(pool.bind(id));

    // port is not available
    id.foo = 123;
    pool.bind(id);
    WNS_ASSERT_ASSURE_EXCEPTION(pool.bind(id));

    CPPUNIT_ASSERT(idsInUse == pool.getCapacity() + 1);
}
