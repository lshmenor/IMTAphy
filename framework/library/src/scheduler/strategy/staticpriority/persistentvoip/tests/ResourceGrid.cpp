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

#include <WNS/scheduler/strategy/staticpriority/persistentvoip/ResourceGrid.hpp>
#include <WNS/scheduler/tests/RegistryProxyStub.hpp>

#include <WNS/CppUnit.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/pyconfig/Parser.hpp>
#include <WNS/pyconfig/View.hpp>

namespace wns { namespace scheduler { namespace strategy { namespace staticpriority { namespace persistentvoip { namespace tests {

	class ResourceGridTest :
		public wns::TestFixture
	{
		CPPUNIT_TEST_SUITE(ResourceGridTest);
        CPPUNIT_TEST(testDimension);
        CPPUNIT_TEST(testSched);
        CPPUNIT_TEST(testAllPotentialTBs);
		CPPUNIT_TEST_SUITE_END();
	public:
		ResourceGridTest();
		~ResourceGridTest();
		void prepare();
		void cleanup();
        void testDimension();
        void testSched();
        void testAllPotentialTBs();

    private:
        wns::logger::Logger logger_;
        ResourceGrid* rg_;
        wns::scheduler::tests::RegistryProxyStub reg_;
	};

CPPUNIT_TEST_SUITE_REGISTRATION(ResourceGridTest);
}}}}}} 

using namespace wns::scheduler::strategy::staticpriority::persistentvoip::tests;

ResourceGridTest::ResourceGridTest() : 
	wns::TestFixture(),
    logger_("TEST", "TEST")
{
}

ResourceGridTest::~ResourceGridTest()
{
}

void ResourceGridTest::prepare()
{
	wns::pyconfig::Parser rgConfig;
	std::stringstream ss;
	ss << "from openwns.Scheduler import PersistentVoIP\n"
	   << "rg = PersistentVoIP.ResourceGrid(\"First\", \"AtStart\")\n";
	rgConfig.loadString(ss.str());

    rg_ = new ResourceGrid(rgConfig.getView("rg"), logger_, 5, 10, 
        &reg_, 1E-3, wns::scheduler::SchedulerSpot::ULMaster()); 
}

void ResourceGridTest::testDimension()
{
    unsigned int nf = rg_->getNumberOfFrames();
    CPPUNIT_ASSERT_EQUAL(nf, (unsigned int)5);
    CPPUNIT_ASSERT_THROW(rg_->getFrame(nf), wns::Assure::Exception);
    CPPUNIT_ASSERT_EQUAL(rg_->getFrame(3)->getFrameIndex(), (unsigned int)3);
}

void ResourceGridTest::testSched()
{
    Frame::SearchResult sr;

    Frame* f0 = rg_->getFrame(0);

    /* Start is out of range */
    sr.tbStart = 10;
    sr.tbLength = 4;
    CPPUNIT_ASSERT_THROW(f0->reserve(0, sr, false), wns::Assure::Exception);
    /* End is  out of range */
    sr.tbStart = 7;
    sr.tbLength = 4;
    CPPUNIT_ASSERT_THROW(f0->reserve(0, sr, true), wns::Assure::Exception);

    Frame::SearchResultSet srs;
    
    srs = f0->findTransmissionBlocks();
    /*       RB: 0 1 2 3 4 5 6 7 8 9 */
    /* Occupied: 0 0 0 0 0 0 0 0 0 0 */    

    /* Everything is free so there is one successfull result */
    CPPUNIT_ASSERT_EQUAL(srs.size(), (size_t)1);
    CPPUNIT_ASSERT_EQUAL(srs.begin()->success, true);

    /* Starting at zero */
    CPPUNIT_ASSERT_EQUAL(srs.begin()->start, (unsigned int)0);

    /* 10 RBs long */
    CPPUNIT_ASSERT_EQUAL(srs.begin()->length, (unsigned int)10);

    /* Reserve RBs 5 and 6 for CID 0*/
    sr.tbStart = 5;
    sr.tbLength = 2;
    f0->reserve(0, sr, true);
    /*       RB: 0 1 2 3 4 5 6 7 8 9 */
    /* Occupied: 0 0 0 0 0 1 1 0 0 0 */    

    srs = f0->findTransmissionBlocks();
    /* Now there should be two potential TBs */
    CPPUNIT_ASSERT_EQUAL(srs.size(), (size_t)2);

    Frame::SearchResultSet::iterator it = srs.begin();

    /* First potential TB: 0 to 4; length 5 */
    CPPUNIT_ASSERT_EQUAL(it->start, (unsigned int)0);
    CPPUNIT_ASSERT_EQUAL(it->length, (unsigned int)5);

    it++;

    /* Second potential TB: 7 to 9; length 3 */
    CPPUNIT_ASSERT_EQUAL(it->start, (unsigned int)7);
    CPPUNIT_ASSERT_EQUAL(it->length, (unsigned int)3);

    /* Clear unpersistent reservations for frame 0 */
    rg_->onNewFrame(0);

    /* Now there are two holes */
    srs = f0->findTransmissionBlocks();
    CPPUNIT_ASSERT_EQUAL(srs.size(), (size_t)2);

    /* Now we reserve 0 to 2 for CID 1 unpersistently */
    sr.tbStart = 0;
    sr.tbLength = 3;
    f0->reserve(1, sr, false);
    /*       RB: 0 1 2 3 4 5 6 7 8 9 */
    /* Occupied: 1 1 1 0 0 1 1 0 0 0 */    

    /* Do not allow to reserve occupied blocks (try to reserve 2 to 3)*/
    sr.tbStart = 2;
    sr.tbLength = 2;
    CPPUNIT_ASSERT_THROW(f0->reserve(2, sr, false), wns::Assure::Exception);
    
    /* Clear unpersistent reservations for frame 0 */
    rg_->onNewFrame(0);
    /*       RB: 0 1 2 3 4 5 6 7 8 9 */
    /* Occupied: 0 0 0 0 0 1 1 0 0 0 */    

    /* Try to reserve for CID 0 already having reservation */

    /* Unpersistently */
    sr.tbStart = 0;
    sr.tbLength = 1;
    CPPUNIT_ASSERT_THROW(f0->reserve(0, sr, false), wns::Assure::Exception);
    /* Persistently */
    sr.tbStart = 0;
    sr.tbLength = 1;
    CPPUNIT_ASSERT_THROW(f0->reserve(0, sr, true), wns::Assure::Exception);
    /* Other frame */
    Frame* f1 = rg_->getFrame(1);
    sr.tbStart = 0;
    sr.tbLength = 1;
    CPPUNIT_ASSERT_NO_THROW(f1->reserve(0, sr, true));

    /* Now lets try to find something in a fully occupied frame */
    Frame* f3 = rg_->getFrame(3);
    sr.tbStart = 0;
    sr.tbLength = 10;
    f3->reserve(1, sr, false);
    /*       RB: 0 1 2 3 4 5 6 7 8 9 */
    /* Occupied: 1 1 1 1 1 1 1 1 1 1 */
    srs = f3->findTransmissionBlocks();
    CPPUNIT_ASSERT(srs.empty());
}

void ResourceGridTest::testAllPotentialTBs()
{
    wns::service::phy::phymode::PhyModeInterfacePtr pm;
    Frame* f0 = rg_->getFrame(0);

    Frame::SearchResultSet srs;
    Frame::SearchResult sr;

    sr.tbStart = 5;
    sr.tbLength = 1;
    f0->reserve(0, sr, true);
    sr.tbStart = 7;
    sr.tbLength = 2;
    f0->reserve(1, sr, true);
    sr.tbStart = 0;
    sr.tbLength = 3;
    f0->reserve(2, sr, true);

    /*       RB: 0 1 2 3 4 5 6 7 8 9 */
    /* Occupied: 1 1 1 0 0 1 0 1 1 0 */

    srs = f0->findTransmissionBlocks();
    CPPUNIT_ASSERT_EQUAL(srs.size(), (size_t)3); 

    Frame::SearchResultSet::iterator it;

    it = srs.begin();
    CPPUNIT_ASSERT_EQUAL(it->start, (unsigned int)3);
    CPPUNIT_ASSERT_EQUAL(it->length, (unsigned int)2);

    it++;
    CPPUNIT_ASSERT_EQUAL(it->start, (unsigned int)6);
    CPPUNIT_ASSERT_EQUAL(it->length, (unsigned int)1);

    it++;
    CPPUNIT_ASSERT_EQUAL(it->start, (unsigned int)9);
    CPPUNIT_ASSERT_EQUAL(it->length, (unsigned int)1);
}
void ResourceGridTest::cleanup()
{
    delete rg_;
}


