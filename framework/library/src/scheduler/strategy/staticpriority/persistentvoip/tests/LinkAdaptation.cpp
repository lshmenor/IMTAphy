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

#include <WNS/scheduler/strategy/staticpriority/persistentvoip/LinkAdaptation.hpp>
#include <WNS/scheduler/strategy/staticpriority/persistentvoip/ResourceGrid.hpp>
#include <WNS/scheduler/tests/LinkAdaptationProxyStub.hpp>
#include <WNS/pyconfig/Parser.hpp>

#include <WNS/CppUnit.hpp>
#include <WNS/logger/Logger.hpp>

namespace wns { namespace scheduler { namespace strategy { namespace staticpriority { namespace persistentvoip { namespace tests {

	class LinkAdaptationTest :
		public wns::TestFixture
	{
		CPPUNIT_TEST_SUITE(LinkAdaptationTest);
        CPPUNIT_TEST(testLAStub);
        CPPUNIT_TEST(testAtStart);
        CPPUNIT_TEST(testAll);
		CPPUNIT_TEST_SUITE_END();
	public:
		LinkAdaptationTest();
		~LinkAdaptationTest();
		void prepare();
		void cleanup();
        void testLAStub();
        void testAtStart();
        void testAll();

    private:
        wns::logger::Logger logger_;
        ILinkAdaptation* laStart_;
        ILinkAdaptation* laAll_;
        wns::scheduler::tests::LinkAdaptationProxyStub reg_;
        wns::simulator::Time slotDuration_;
	};

CPPUNIT_TEST_SUITE_REGISTRATION(LinkAdaptationTest);
}}}}}} 

using namespace wns::scheduler::strategy::staticpriority::persistentvoip::tests;

LinkAdaptationTest::LinkAdaptationTest() : 
	wns::TestFixture(),
    logger_("TEST", "TEST")
{
}

LinkAdaptationTest::~LinkAdaptationTest()
{
}

void LinkAdaptationTest::prepare()
{
	wns::pyconfig::Parser parser;
	parser.loadString("class LA(object):\n\treduceMCS = True\n\tpNull = \"4 dBm\"\n\talpha = 0.0\nla = LA()");

    slotDuration_ = 1E-3;

    laStart_ = new AtStart(parser.get("la"));
    laStart_->setLinkAdaptationProxy(&reg_);
    laStart_->setSlotDuration(slotDuration_);
    laStart_->setSchedulerSpot(wns::scheduler::SchedulerSpot::ULMaster()); 

    laAll_ = new All(parser.get("la"));
    laAll_->setLinkAdaptationProxy(&reg_);
    laAll_->setSlotDuration(slotDuration_);
    laAll_->setSchedulerSpot(wns::scheduler::SchedulerSpot::ULMaster()); 
}

void LinkAdaptationTest::testLAStub()
{
    std::set<unsigned int> scs;
    scs.insert(0);

    wns::Ratio eSINR;
    eSINR = reg_.getEffectiveUplinkSINR(UserID(), scs, 0, wns::Power::from_dBm(4.0));
    /* RB 0 has TxPower - 4.0 dB = 0 dB */
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, eSINR.get_factor(), 0.001);

    scs.clear();
    scs.insert(0);
    scs.insert(1);
    eSINR = reg_.getEffectiveUplinkSINR(UserID(), scs, 0, wns::Power::from_dBm(4.0));
    /* RB 0 and 1 both have TxPower - 4.0 dB = 0 dB (1.0 linear) */
    CPPUNIT_ASSERT_DOUBLES_EQUAL(eSINR.get_factor(), 1.0, 0.001);

    eSINR = reg_.getEffectiveUplinkSINR(UserID(), scs, 0, wns::Power::from_dBm(5.0));
    /* RB 0 and 1 both have TxPower - 4.0 dB = 1 dB (1.259 linear) 
    if TxPower is increased to 5 dBm */
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.258925, eSINR.get_factor(), 0.001);

    scs.clear();
    scs.insert(1);
    scs.insert(2);
    eSINR = reg_.getEffectiveUplinkSINR(UserID(), scs, 0, wns::Power::from_dBm(4.0));
    /* 
    RB 1 has 4.0 dBm - 4.0 dB = 0 dB
    RB 2 has 4.0 dBm - (-1.0 dB) = 5 dB
    effSINR must be in between
    Its actual value is 3.18301 dB (2.081 linear)
     */
    CPPUNIT_ASSERT(eSINR >= wns::Ratio::from_dB(0.0));
    CPPUNIT_ASSERT(eSINR <= wns::Ratio::from_dB(5.0));
    CPPUNIT_ASSERT_DOUBLES_EQUAL(2.08113, eSINR.get_factor(), 0.001);

    scs.clear();
    scs.insert(3);
    scs.insert(4);
    eSINR = reg_.getEffectiveUplinkSINR(UserID(), scs, 0, wns::Power::from_dBm(4.0));
    /* 
    RB 3 has 4.0 dBm - (-1.0 dB) = 5 dB
    RB 4 has 4.0 dBm - (-7.0 dB) = 11 dB
    effSINR must be in between
    Its actual value is 8.9629 dB (7.87577 linear)
     */
    CPPUNIT_ASSERT(eSINR >= wns::Ratio::from_dB(5.0));
    CPPUNIT_ASSERT(eSINR <= wns::Ratio::from_dB(11.0));
    CPPUNIT_ASSERT_DOUBLES_EQUAL(7.87577, eSINR.get_factor(), 0.001);

    scs.clear();
    scs.insert(0);
    scs.insert(1);
    scs.insert(2);
    scs.insert(3);
    scs.insert(4);
    scs.insert(5);
    eSINR = reg_.getEffectiveUplinkSINR(UserID(), scs, 0, wns::Power::from_dBm(4.0));
    /* 
    RBs 0 and 1 have 4.0 dBm - 4.0 dB = 0 dB
    RBs 2 and 3 have 4.0 dBm - (-1.0 dB) = 5 dB
    RBs 4 and 5 have 4.0 dBm - (-7.0 dB) = 11 dB
    effSINR must be between 0 dB and 11 dB
    Its actual value is 7.4693 dB (5.58384 linear)
     */
    CPPUNIT_ASSERT(eSINR >= wns::Ratio::from_dB(0.0));
    CPPUNIT_ASSERT(eSINR <= wns::Ratio::from_dB(11.0));
    CPPUNIT_ASSERT_DOUBLES_EQUAL(5.58384, eSINR.get_factor(), 0.001);


    /* 
    Now test the PhyModes (Modulation and Coding Schemes (MCSs)):
    SINR < 3 dB          =>  BPSK 3/4
    3 dB >= SINR < 10 dB => 16QAM 3/4
    SINR >= 10 dB        => 64QAM 3/4
    */
    
    scs.clear();
    scs.insert(0);
    wns::Ratio eSINR1 = reg_.getEffectiveUplinkSINR(UserID(), scs, 0, wns::Power::from_dBm(4.0));
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, eSINR1.get_factor(), 0.001); // 0 dB
    scs.clear();
    scs.insert(2);
    wns::Ratio eSINR2 = reg_.getEffectiveUplinkSINR(UserID(), scs, 0, wns::Power::from_dBm(4.0));
    CPPUNIT_ASSERT_DOUBLES_EQUAL(3.16227, eSINR2.get_factor(), 0.001); // 5 dB
    scs.clear();
    scs.insert(4);
    wns::Ratio eSINR3 = reg_.getEffectiveUplinkSINR(UserID(), scs, 0, wns::Power::from_dBm(4.0));
    CPPUNIT_ASSERT_DOUBLES_EQUAL(12.5893, eSINR3.get_factor(), 0.001); // 11 dB

    wns::service::phy::phymode::PhyModeInterfacePtr pm1;
    /* BPSK 3/4 */
    pm1 = reg_.getBestPhyMode(eSINR1); 

    wns::service::phy::phymode::PhyModeInterfacePtr pm2;
    /* 16QAM 3/4 */
    pm2 = reg_.getBestPhyMode(eSINR2);

    wns::service::phy::phymode::PhyModeInterfacePtr pm3;
    /* 64QAM 3/4 */
    pm3 = reg_.getBestPhyMode(eSINR3);

    /* 
    12 subcarriers, 14 symbols per slot (TTI), 3/4 coding 
    => cap = 12 * 14 / 4 * 3 * ModulationOrder
    */
    CPPUNIT_ASSERT_EQUAL((unsigned int)126, pm1->getBitCapacityFractional(slotDuration_));
    CPPUNIT_ASSERT_EQUAL((unsigned int)504, pm2->getBitCapacityFractional(slotDuration_));
    CPPUNIT_ASSERT_EQUAL((unsigned int)756, pm3->getBitCapacityFractional(slotDuration_));
}

void LinkAdaptationTest::testAtStart()
{
    CPPUNIT_ASSERT(laStart_ != NULL);

    Frame::SearchResultSet srs;
    Frame::SearchResultSet srsResult;
    Frame::SearchResultSet::iterator it;
    Frame::SearchResult sr;

    /*
    RB:           0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7
    SINR [dB]:    0  0  5  5 11 11  0  0  5  5 11 11  0  0  5  5 11 11
    Occupied:     1  1  0  0  1  1  1  0  0  1  1  1  0  0  0  0  0  0 
    */

    sr.start = 2;
    sr.length = 2;
    srs.insert(sr);
    sr.start = 7;
    sr.length = 2;
    srs.insert(sr);
    sr.start = 12;
    sr.length = 6;
    srs.insert(sr);

    /* PDU size 126 Bit fits everywhere and only needs 1 RB (lowest MCS) */
    srsResult = laStart_->setTBSizes(srs, ConnectionID(), 126);
    CPPUNIT_ASSERT_EQUAL((size_t)3, srsResult.size());
    it = srsResult.begin();
    CPPUNIT_ASSERT_EQUAL((unsigned int)2, it->tbStart);
    CPPUNIT_ASSERT_EQUAL((unsigned int)1, it->tbLength);
    it++;
    CPPUNIT_ASSERT_EQUAL((unsigned int)7, it->tbStart);
    CPPUNIT_ASSERT_EQUAL((unsigned int)1, it->tbLength);
    it++;
    CPPUNIT_ASSERT_EQUAL((unsigned int)12, it->tbStart);
    CPPUNIT_ASSERT_EQUAL((unsigned int)1, it->tbLength);

    /* PDU size 3024 Bit only fits last hole */
    srsResult = laStart_->setTBSizes(srs, ConnectionID(), 3024);
    CPPUNIT_ASSERT_EQUAL((size_t)1, srsResult.size());
    it = srsResult.begin();
    CPPUNIT_ASSERT_EQUAL((unsigned int)12, it->tbStart);
    CPPUNIT_ASSERT_EQUAL((unsigned int)6, it->tbLength);
}

void LinkAdaptationTest::testAll()
{
    CPPUNIT_ASSERT(laAll_ != NULL);

    Frame::SearchResultSet srs;
    Frame::SearchResultSet srsResult;
    Frame::SearchResultSet::iterator it;
    Frame::SearchResult sr;

    /*
    RB:           0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7
    SINR [dB]:    0  0  5  5 11 11  0  0  5  5 11 11  0  0  5  5 11 11
    Occupied:     1  1  0  0  1  1  1  0  0  1  1  1  0  0  0  0  0  0 
    */

    sr.start = 2;
    sr.length = 2;
    srs.insert(sr);
    sr.start = 7;
    sr.length = 2;
    srs.insert(sr);
    sr.start = 12;
    sr.length = 6;
    srs.insert(sr);

    /* PDU size 126 Bit fits in every single free RB (lowest MCS) */
    srsResult = laAll_->setTBSizes(srs, ConnectionID(), 126);
    CPPUNIT_ASSERT_EQUAL((size_t)10, srsResult.size());

    /* First hole */
    it = srsResult.begin();
    CPPUNIT_ASSERT_EQUAL((unsigned int)2, it->tbStart);
    CPPUNIT_ASSERT_EQUAL((unsigned int)1, it->tbLength);
    it++;
    CPPUNIT_ASSERT_EQUAL((unsigned int)3, it->tbStart);
    CPPUNIT_ASSERT_EQUAL((unsigned int)1, it->tbLength);

    /* Second hole */
    it++;
    CPPUNIT_ASSERT_EQUAL((unsigned int)7, it->tbStart);
    CPPUNIT_ASSERT_EQUAL((unsigned int)1, it->tbLength);
    it++;
    CPPUNIT_ASSERT_EQUAL((unsigned int)8, it->tbStart);
    CPPUNIT_ASSERT_EQUAL((unsigned int)1, it->tbLength);

    /* Last hole */
    it++;
    CPPUNIT_ASSERT_EQUAL((unsigned int)12, it->tbStart);
    CPPUNIT_ASSERT_EQUAL((unsigned int)1, it->tbLength);
    it++;
    CPPUNIT_ASSERT_EQUAL((unsigned int)13, it->tbStart);
    CPPUNIT_ASSERT_EQUAL((unsigned int)1, it->tbLength);
    it++;
    CPPUNIT_ASSERT_EQUAL((unsigned int)14, it->tbStart);
    CPPUNIT_ASSERT_EQUAL((unsigned int)1, it->tbLength);
    it++;
    CPPUNIT_ASSERT_EQUAL((unsigned int)15, it->tbStart);
    CPPUNIT_ASSERT_EQUAL((unsigned int)1, it->tbLength);
    it++;
    CPPUNIT_ASSERT_EQUAL((unsigned int)16, it->tbStart);
    CPPUNIT_ASSERT_EQUAL((unsigned int)1, it->tbLength);
    it++;
    CPPUNIT_ASSERT_EQUAL((unsigned int)17, it->tbStart);
    CPPUNIT_ASSERT_EQUAL((unsigned int)1, it->tbLength);
}

void LinkAdaptationTest::cleanup()
{
    delete laStart_;
    delete laAll_;
}


