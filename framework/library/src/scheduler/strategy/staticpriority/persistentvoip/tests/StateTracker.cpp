/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2009
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

#include <WNS/scheduler/strategy/staticpriority/persistentvoip/StateTracker.hpp>

#include <WNS/CppUnit.hpp>
#include <WNS/logger/Logger.hpp>

namespace wns { namespace scheduler { namespace strategy { namespace staticpriority { namespace persistentvoip { namespace tests {

	class StateTrackerTest :
		public wns::TestFixture
	{
		CPPUNIT_TEST_SUITE(StateTrackerTest);
        CPPUNIT_TEST(testStates);
		CPPUNIT_TEST_SUITE_END();
	public:
		StateTrackerTest();
		~StateTrackerTest();
		void prepare();
		void cleanup();
        void testStates();

    private:
        wns::logger::Logger logger_;
        StateTracker* st_;
	};

CPPUNIT_TEST_SUITE_REGISTRATION(StateTrackerTest);
}}}}}} 

using namespace wns::scheduler::strategy::staticpriority::persistentvoip::tests;

StateTrackerTest::StateTrackerTest() : 
	wns::TestFixture(),
    logger_("TEST", "TEST")
{
}

StateTrackerTest::~StateTrackerTest()
{
}

void StateTrackerTest::prepare()
{
    st_ = new StateTracker(3, logger_);
}

void StateTrackerTest::testStates()
{
    wns::scheduler::ConnectionSet csCompare;

    wns::scheduler::ConnectionSet cs;
    cs.insert(1);
    cs.insert(2);
    cs.insert(3);

    StateTracker::ClassifiedConnections cc;
    cc = st_->updateState(cs, 0);

    /* CIDs never seen before => unpersistent */
    CPPUNIT_ASSERT(cc.newPersistentCIDs.empty());
    CPPUNIT_ASSERT(cc.reactivatedPersistentCIDs.empty());
    CPPUNIT_ASSERT(cc.persistentCIDs.empty());
    CPPUNIT_ASSERT(cc.silencedCIDs.empty());    
    CPPUNIT_ASSERT_EQUAL(cc.unpersistentCIDs.size(), (size_t)3);
    CPPUNIT_ASSERT(cc.unpersistentCIDs == cs);

    /* Two CIDs (1, 2) seen second time, one new */
    cs.erase(3);
    cs.insert(4);
    csCompare.insert(1);
    csCompare.insert(2);
    cc = st_->updateState(cs, 0);
    CPPUNIT_ASSERT_EQUAL(cc.newPersistentCIDs.size(), (size_t)2);
    CPPUNIT_ASSERT(cc.newPersistentCIDs == csCompare);
    CPPUNIT_ASSERT(cc.reactivatedPersistentCIDs.empty());
    CPPUNIT_ASSERT(cc.persistentCIDs.empty());
    CPPUNIT_ASSERT(cc.silencedCIDs.empty());    
    CPPUNIT_ASSERT_EQUAL(cc.unpersistentCIDs.size(), (size_t)1);
    CPPUNIT_ASSERT_EQUAL(*(cc.unpersistentCIDs.begin()), (ConnectionID)4);


    /* No changes => 1,2 persistent; 4 new persistent */
    cc = st_->updateState(cs, 0);
    CPPUNIT_ASSERT_EQUAL(cc.newPersistentCIDs.size(), (size_t)1);
    CPPUNIT_ASSERT_EQUAL(*(cc.newPersistentCIDs.begin()), (ConnectionID)4);
    CPPUNIT_ASSERT(cc.reactivatedPersistentCIDs.empty());
    CPPUNIT_ASSERT_EQUAL(cc.persistentCIDs.size(), (size_t)2);
    CPPUNIT_ASSERT(cc.persistentCIDs == csCompare);
    CPPUNIT_ASSERT(cc.silencedCIDs.empty());    
    CPPUNIT_ASSERT(cc.unpersistentCIDs.empty());

    /* No changes => 1,2,4 persistent */
    cc = st_->updateState(cs, 0);
    CPPUNIT_ASSERT(cc.newPersistentCIDs.empty());
    CPPUNIT_ASSERT(cc.reactivatedPersistentCIDs.empty());
    CPPUNIT_ASSERT_EQUAL(cc.persistentCIDs.size(), (size_t)3);
    CPPUNIT_ASSERT(cc.persistentCIDs == cs);
    CPPUNIT_ASSERT(cc.silencedCIDs.empty());    
    CPPUNIT_ASSERT(cc.unpersistentCIDs.empty());

    /* Remove 4 */
    cs.erase(4);
    csCompare.clear();
    csCompare.insert(1);
    csCompare.insert(2);

    cc = st_->updateState(cs, 0);
    CPPUNIT_ASSERT(cc.newPersistentCIDs.empty());
    CPPUNIT_ASSERT(cc.reactivatedPersistentCIDs.empty());
    CPPUNIT_ASSERT_EQUAL(cc.persistentCIDs.size(), (size_t)2);
    CPPUNIT_ASSERT(cc.persistentCIDs == csCompare);
    CPPUNIT_ASSERT_EQUAL(cc.silencedCIDs.size(), (size_t)1);    
    CPPUNIT_ASSERT_EQUAL(*(cc.silencedCIDs.begin()), (ConnectionID)4);    
    CPPUNIT_ASSERT(cc.unpersistentCIDs.empty());
    
    /* Reenable 4 => First time considered comfort noise */
    cs.insert(4);
    cc = st_->updateState(cs, 0);
    CPPUNIT_ASSERT(cc.newPersistentCIDs.empty());
    CPPUNIT_ASSERT(cc.reactivatedPersistentCIDs.empty());
    CPPUNIT_ASSERT_EQUAL(cc.persistentCIDs.size(), (size_t)2);
    CPPUNIT_ASSERT(cc.persistentCIDs == csCompare);
    CPPUNIT_ASSERT(cc.silencedCIDs.empty());    
    CPPUNIT_ASSERT_EQUAL(cc.unpersistentCIDs.size(), (size_t)1);
    CPPUNIT_ASSERT_EQUAL(*(cc.unpersistentCIDs.begin()), (ConnectionID)4);    

    /* Remove 4 => Not considered at all */
    cs.erase(4);
    cc = st_->updateState(cs, 0);
    CPPUNIT_ASSERT(cc.newPersistentCIDs.empty());
    CPPUNIT_ASSERT(cc.reactivatedPersistentCIDs.empty());
    CPPUNIT_ASSERT_EQUAL(cc.persistentCIDs.size(), (size_t)2);
    CPPUNIT_ASSERT(cc.persistentCIDs == csCompare);
    CPPUNIT_ASSERT(cc.silencedCIDs.empty());    
    CPPUNIT_ASSERT(cc.unpersistentCIDs.empty());

    /* Try to Reenable 4 in frame 1 
    => Is ignored because it belongs to frame 0 */
    cs.insert(4);
    cc = st_->updateState(cs, 1);
    CPPUNIT_ASSERT(cc.newPersistentCIDs.empty());
    CPPUNIT_ASSERT(cc.reactivatedPersistentCIDs.empty());
    CPPUNIT_ASSERT(cc.persistentCIDs.empty());
    CPPUNIT_ASSERT(cc.silencedCIDs.empty());    
    CPPUNIT_ASSERT(cc.unpersistentCIDs.empty());

    /* No connections in frame 0 => 1 and 2 silenced*/
    cs.clear();
    csCompare.clear();
    csCompare.insert(1);
    csCompare.insert(2);

    cc = st_->updateState(cs, 0);
    CPPUNIT_ASSERT(cc.newPersistentCIDs.empty());
    CPPUNIT_ASSERT(cc.reactivatedPersistentCIDs.empty());
    CPPUNIT_ASSERT(cc.persistentCIDs.empty());
    CPPUNIT_ASSERT_EQUAL(cc.silencedCIDs.size(), (size_t)2);    
    CPPUNIT_ASSERT(cc.silencedCIDs == csCompare);    
    CPPUNIT_ASSERT(cc.unpersistentCIDs.empty());

    /* No connections => nothing to classify */
    cc = st_->updateState(cs, 0);
    CPPUNIT_ASSERT(cc.newPersistentCIDs.empty());
    CPPUNIT_ASSERT(cc.reactivatedPersistentCIDs.empty());
    CPPUNIT_ASSERT(cc.persistentCIDs.empty());
    CPPUNIT_ASSERT(cc.silencedCIDs.empty());    
    CPPUNIT_ASSERT(cc.unpersistentCIDs.empty());
}

void StateTrackerTest::cleanup()
{
    delete st_;
}


