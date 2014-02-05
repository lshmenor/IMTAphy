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

#include <WNS/pyconfig/Parser.hpp>
#include <WNS/Average.hpp>
#include <WNS/distribution/tests/VarEstimator.hpp>
#include <WNS/distribution/Binomial.hpp>
#include <WNS/simulator/Simulator.hpp>
#include <WNS/TestFixture.hpp>


namespace wns { namespace distribution { namespace test {

    class BinomialTest :
        public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE( BinomialTest );
        CPPUNIT_TEST( testMean );
        CPPUNIT_TEST( testVar );
        CPPUNIT_TEST_SUITE_END();
    public:
        void setUp();
        void tearDown();

        void testMean();
        void testVar();
    };

    CPPUNIT_TEST_SUITE_REGISTRATION( BinomialTest );


    void
    BinomialTest::setUp()
    {
    }


    void
    BinomialTest::tearDown()
    {
    }


    void
    BinomialTest::testMean()
    {
        wns::pyconfig::View config = wns::pyconfig::Parser::fromString(
            "import openwns.distribution\n"
            "dist = openwns.distribution.Binomial(20, 0.4)\n"
            );

        Binomial dis = Binomial(config.get("dist"));

        Average<double> average;
        for(int i = 0; i < 10000; ++i)
        {
            average.put(dis());
        }

        WNS_ASSERT_MAX_REL_ERROR(average.get(), dis.getMean(), 0.01);
    }

    void
    BinomialTest::testVar()
    {
        Binomial dis = Binomial(20, 0.4, wns::simulator::getRNG());

        VarEstimator var;
        for(int i = 0; i < 10000; ++i)
        {
            var.put(dis());
        }

        WNS_ASSERT_MAX_REL_ERROR(20.0 * 0.4 * 0.6, var.get(), 0.01);
    }

} // tests
} // distribution
} // wns

/*
  Local Variables:
  mode: c++
  fill-column: 80
  c-basic-offset: 8
  c-comment-only-line-offset: 0
  c-tab-always-indent: t
  indent-tabs-mode: t
  tab-width: 8
  End:
*/
