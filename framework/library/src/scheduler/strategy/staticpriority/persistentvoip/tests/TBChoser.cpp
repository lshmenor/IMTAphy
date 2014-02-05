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

#include <WNS/scheduler/strategy/staticpriority/persistentvoip/TBChoser.hpp>
#include <WNS/pyconfig/Parser.hpp>

#include <WNS/CppUnit.hpp>

namespace wns { namespace scheduler { namespace strategy { namespace staticpriority { namespace persistentvoip { namespace tests {

	class TBChoserTest :
		public wns::TestFixture
	{
		CPPUNIT_TEST_SUITE(TBChoserTest);
        CPPUNIT_TEST(testFirst);
        CPPUNIT_TEST(testBest);
        CPPUNIT_TEST(testWorst);
        CPPUNIT_TEST(testSmallest);
        CPPUNIT_TEST(testPrevious);
        CPPUNIT_TEST(testRandom);
        CPPUNIT_TEST(testEmpty);
		CPPUNIT_TEST_SUITE_END();
	public:
		TBChoserTest();
		~TBChoserTest();
		void prepare();
		void cleanup();
        void testFirst();
        void testBest();
        void testWorst();
        void testRandom();
        void testSmallest();
        void testPrevious();
        void testEmpty();

    private:
        ITBChoser* tbc_;
        Frame::SearchResultSet srs1;
        wns::pyconfig::Parser parser_;
	};

CPPUNIT_TEST_SUITE_REGISTRATION(TBChoserTest);
}}}}}} 

using namespace wns::scheduler::strategy::staticpriority::persistentvoip::tests;

TBChoserTest::TBChoserTest() : 
	wns::TestFixture()
{
}

TBChoserTest::~TBChoserTest()
{
}

void TBChoserTest::prepare()
{
    Frame::SearchResult sr;

    /* Index     0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 */
    /* Occupied  T T 0 0 1 1 1 T 0 1 1 1 1 1 1 0 0 T T 0 */
    sr.success = true;
    sr.cid = 1;

    sr.tbStart = 0;
    sr.tbLength = 2;
    sr.start = 0;
    sr.length = 4;
    srs1.insert(sr);
    sr.tbStart = 7;
    sr.tbLength = 1;
    sr.start = 7;
    sr.length = 2;
    srs1.insert(sr);
    sr.tbStart = 17;
    sr.tbLength = 2;
    sr.start = 15;
    sr.length = 5;
    srs1.insert(sr);

	parser_.loadString("class TBC(object):\n"
                            "\t__plugin__ = \"Smallest\"\n"
                            "\treturnRandom = False\n"
                        "tbc = TBC()\n"
                        "tbc.fallbackChoser = TBC()\n");
}

void TBChoserTest::testFirst()
{
    tbc_ = new First(parser_.get("tbc"));
    Frame::SearchResult sr;
    sr = tbc_->choseTB(srs1);
    CPPUNIT_ASSERT_EQUAL(sr.start, (unsigned int)0);
    CPPUNIT_ASSERT_EQUAL(sr.length, (unsigned int)4);
    delete tbc_;
}

void TBChoserTest::testBest()
{
    tbc_ = new BestFit(parser_.get("tbc"));
    Frame::SearchResult sr;
    sr = tbc_->choseTB(srs1);
    CPPUNIT_ASSERT_EQUAL(sr.start, (unsigned int)7);
    CPPUNIT_ASSERT_EQUAL(sr.length, (unsigned int)2);
    delete tbc_;
}

void TBChoserTest::testWorst()
{
    tbc_ = new WorstFit(parser_.get("tbc"));
    Frame::SearchResult sr;
    sr = tbc_->choseTB(srs1);
    CPPUNIT_ASSERT_EQUAL(sr.start, (unsigned int)15);
    CPPUNIT_ASSERT_EQUAL(sr.length, (unsigned int)5);
    delete tbc_;
}

void TBChoserTest::testRandom()
{
    tbc_ = new Random(parser_.get("tbc"));
    Frame::SearchResult sr;

    unsigned int trials = 1000000;
    double sum = 0;
    for(int i = 0; i < trials; i++)
    {
        sr = tbc_->choseTB(srs1);
        sum += sr.start;
    }

    double mean = sum / double(trials);
    double calcMean;
    calcMean = (0.0 + 7.0 + 15.0) / 3.0;

    /* This is a statistic test so there is always a chance it could fail */
    /* In this case reduce tolerance or increase trials */
    CPPUNIT_ASSERT_DOUBLES_EQUAL(mean, calcMean, 0.01);
    delete tbc_;
}

void TBChoserTest::testSmallest()
{
    tbc_ = new Smallest(parser_.get("tbc"));
    Frame::SearchResult sr;

    sr = tbc_->choseTB(srs1);
    CPPUNIT_ASSERT_EQUAL(sr.start, (unsigned int)7);
    CPPUNIT_ASSERT_EQUAL(sr.length, (unsigned int)2);
    delete tbc_;
}

void TBChoserTest::testPrevious()
{
    tbc_ = new Previous(parser_.get("tbc"));
    Frame::SearchResult sr;

    /* Fallback strategy Smallest will pick RB 7 */
    sr = tbc_->choseTB(srs1);
    unsigned int start = sr.tbStart;
    unsigned int length = sr.tbLength;
    CPPUNIT_ASSERT_EQUAL(start, (unsigned int)7);
    CPPUNIT_ASSERT_EQUAL(length, (unsigned int)1);

    /* RB 7 will be picked again */
    sr = tbc_->choseTB(srs1);
    CPPUNIT_ASSERT_EQUAL(sr.tbStart, start);
    CPPUNIT_ASSERT_EQUAL(sr.tbLength, length);

    srs1.erase(sr);
    /* Fallback strategy Smallest will pick RB 0 & 1 */
    sr = tbc_->choseTB(srs1);
    start = sr.tbStart;
    length = sr.tbLength;
    CPPUNIT_ASSERT_EQUAL(start, (unsigned int)0);
    CPPUNIT_ASSERT_EQUAL(length, (unsigned int)2);

    /* RB 0 & 1 will be picked again */
    sr = tbc_->choseTB(srs1);
    CPPUNIT_ASSERT_EQUAL(sr.tbStart, start);
    CPPUNIT_ASSERT_EQUAL(sr.tbLength, length);

    /* Block RB 0 but make RB 1,2,3 availbale now */
    sr = *srs1.begin();
    srs1.erase(sr);
    sr.tbStart = 1;
    sr.tbLength = 3;
    srs1.insert(sr);

    /* RB 1,2,3 will be picked now because RB 1 was used before */
    sr = tbc_->choseTB(srs1);
    CPPUNIT_ASSERT_EQUAL(sr.tbStart, (unsigned int)1);
    CPPUNIT_ASSERT_EQUAL(sr.tbLength, (unsigned int)3);

    delete tbc_;
}

void TBChoserTest::testEmpty()
{
    Frame::SearchResult sr;
    Frame::SearchResultSet srs;
    sr.success = true;
    sr.tbLength = 5;
    sr.length = 10;
    sr.start = 0;
    srs.insert(sr);

    tbc_ = new First(parser_.get("tbc"));
    sr = tbc_->choseTB(srs);
    CPPUNIT_ASSERT_EQUAL(sr.start, (unsigned int)0);
    CPPUNIT_ASSERT_EQUAL(sr.length, (unsigned int)10);
    delete tbc_;
    tbc_ = new BestFit(parser_.get("tbc"));
    sr = tbc_->choseTB(srs);
    CPPUNIT_ASSERT_EQUAL(sr.start, (unsigned int)0);
    CPPUNIT_ASSERT_EQUAL(sr.length, (unsigned int)10);
    delete tbc_;
    tbc_ = new WorstFit(parser_.get("tbc"));
    sr = tbc_->choseTB(srs);
    CPPUNIT_ASSERT_EQUAL(sr.start, (unsigned int)0);
    CPPUNIT_ASSERT_EQUAL(sr.length, (unsigned int)10);
    delete tbc_;
    tbc_ = new Random(parser_.get("tbc"));
    sr = tbc_->choseTB(srs);
    CPPUNIT_ASSERT_EQUAL(sr.start, (unsigned int)0);
    CPPUNIT_ASSERT_EQUAL(sr.length, (unsigned int)10);
    delete tbc_;
}


void TBChoserTest::cleanup()
{
}


