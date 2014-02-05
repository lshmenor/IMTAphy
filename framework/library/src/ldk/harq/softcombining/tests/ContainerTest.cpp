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

#include <WNS/TestFixture.hpp>
#include <WNS/ldk/harq/softcombining/Container.hpp>
#include <WNS/ldk/Compound.hpp>

namespace wns { namespace ldk { namespace harq { namespace softcombining { namespace tests {

    /**
	 * @brief test for wns::ldk::harq::softcombining::Container
	 * @author Daniel Bueltmann <me@daniel-bueltmann.de>
	 */
    class ContainerTest :
        public wns::TestFixture
    {
        CPPUNIT_TEST_SUITE( ContainerTest );
        CPPUNIT_TEST( testGetNumRVs );
        CPPUNIT_TEST( testGetEmptyCompoundList );
        CPPUNIT_TEST( testAppendAndGetCompoundList );
        CPPUNIT_TEST( testGetCompoundListForInvalidTB );
        CPPUNIT_TEST( testGetCompoundListForInvalidRV );
        CPPUNIT_TEST( testAppendCompoundListForInvalidRV );
        CPPUNIT_TEST( testClear );
        CPPUNIT_TEST_SUITE_END();

    public:

        void
        prepare();

        void
        cleanup();

        void
        testGetNumRVs();

        void
        testGetEmptyCompoundList();

        void
        testAppendAndGetCompoundList();

        void
        testGetCompoundListForInvalidTB();

        void
        testGetCompoundListForInvalidRV();

        void
        testAppendCompoundListForInvalidRV();

        void
        testClear();
    };

    typedef Container<wns::ldk::CompoundPtr> TestContainer;
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ContainerTest, wns::testsuite::Default() );

} // tests
} // softcombining
} // harq
} // ldk
} // wns

using namespace wns::ldk::harq::softcombining::tests;

void
ContainerTest::prepare()
{
}

void
ContainerTest::cleanup()
{
}

void
ContainerTest::testGetNumRVs()
{
    TestContainer c = TestContainer(5);

    CPPUNIT_ASSERT_EQUAL(c.getNumRVs(), 5);
}

void
ContainerTest::testGetEmptyCompoundList()
{
    TestContainer c = TestContainer(9);

    CPPUNIT_ASSERT_EQUAL(c.getNumRVs(), 9);

    std::list<int> r;
    r = c.getAvailablePosInTB();

    CPPUNIT_ASSERT_EQUAL(r.size(), (size_t) 0);
}


void
ContainerTest::testAppendAndGetCompoundList()
{
    TestContainer c = TestContainer(3);

    CPPUNIT_ASSERT_EQUAL(c.getNumRVs(), 3);

    c.appendEntryForRV(72, 1, wns::ldk::CompoundPtr());

    c.appendEntryForRV(72, 2, wns::ldk::CompoundPtr());

    c.appendEntryForRV(72, 2, wns::ldk::CompoundPtr());

    std::list<int> r;
    r = c.getAvailablePosInTB();

    CPPUNIT_ASSERT_EQUAL((size_t) 1, r.size());

    CPPUNIT_ASSERT_EQUAL(r.front(), 72);

    CPPUNIT_ASSERT_EQUAL(c.getEntriesForRV(72, 0).size(), (size_t) 0);

    CPPUNIT_ASSERT_EQUAL(c.getEntriesForRV(72, 1).size(), (size_t) 1);

    CPPUNIT_ASSERT_EQUAL(c.getEntriesForRV(72, 2).size(), (size_t) 2);
}

void
ContainerTest::testGetCompoundListForInvalidTB()
{
    TestContainer c = TestContainer(3);
    
    CPPUNIT_ASSERT_THROW(c.getEntriesForRV(1, 3), TestContainer::InvalidPositionInTB);
}

void
ContainerTest::testGetCompoundListForInvalidRV()
{
    TestContainer c = TestContainer(3);

    c.appendEntryForRV(17, 2, wns::ldk::CompoundPtr());
    
    CPPUNIT_ASSERT_THROW(c.getEntriesForRV(17, 3), TestContainer::InvalidRV);
}


void
ContainerTest::testAppendCompoundListForInvalidRV()
{
    TestContainer c = TestContainer(5);

    CPPUNIT_ASSERT_THROW(c.appendEntryForRV(18, 6, wns::ldk::CompoundPtr()), TestContainer::InvalidRV);
}

void
ContainerTest::testClear()
{
    TestContainer c = TestContainer(3);

    CPPUNIT_ASSERT_EQUAL(c.getNumRVs(), 3);

    c.appendEntryForRV(9, 1, wns::ldk::CompoundPtr());

    c.appendEntryForRV(9, 2, wns::ldk::CompoundPtr());

    c.appendEntryForRV(9, 2, wns::ldk::CompoundPtr());

    CPPUNIT_ASSERT_EQUAL(c.getEntriesForRV(9, 0).size(), (size_t) 0);

    CPPUNIT_ASSERT_EQUAL(c.getEntriesForRV(9, 1).size(), (size_t) 1);

    CPPUNIT_ASSERT_EQUAL(c.getEntriesForRV(9, 2).size(), (size_t) 2);

    c.clear();

    CPPUNIT_ASSERT_EQUAL(c.getNumRVs(), 3);

    CPPUNIT_ASSERT_THROW(c.getEntriesForRV(9, 0), TestContainer::InvalidPositionInTB);

    CPPUNIT_ASSERT_THROW(c.getEntriesForRV(9, 1), TestContainer::InvalidPositionInTB);

    CPPUNIT_ASSERT_THROW(c.getEntriesForRV(9, 2), TestContainer::InvalidPositionInTB);
}

