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

#include <WNS/distribution/tests/CDFTableTest.hpp>

#include <WNS/pyconfig/Parser.hpp>
#include <WNS/Average.hpp>
#include <WNS/distribution/tests/VarEstimator.hpp>
#include <WNS/TestFixture.hpp>

using namespace wns::distribution::test;

CPPUNIT_TEST_SUITE_REGISTRATION( CDFTableTest );


void
CDFTableTest::setUp()
{
}


void
CDFTableTest::tearDown()
{
}


void
CDFTableTest::testIt()
{
    pyconfig::Parser config;
    config.loadString(
        "cdfTable = ((40, 0.5),\n"
                "(1500, 1.0))\n"
    );

    CDFTable* dis =
        dynamic_cast<CDFTable*>(
        wns::distribution::DistributionFactory::creator("CDFTable")
        ->create(config));

    Average<double> average;

    for(int i = 0; i < 100000; ++i)
    {
        average.put((*dis)());
    }

    double calculatedAverage = dis->getMean();
    /**
     * @todo (rs) Sometimes "randomness" even exceeds any given bound.
     */
    WNS_ASSERT_MAX_REL_ERROR(calculatedAverage, average.get(), 0.01);

    delete dis;
}

void
CDFTableTest::testVar()
{
    pyconfig::Parser config;
    config.loadString(
        "cdfTable = ((40, 0.5),\n"
                "(1500, 1.0))\n"
    );

    CDFTable* dis =
        dynamic_cast<CDFTable*>(
        wns::distribution::DistributionFactory::creator("CDFTable")
        ->create(config));

    VarEstimator var;
    for(int i = 0; i < 100000; ++i)
    {
        var.put((*dis)());
    }

    double calcMean = (1500.0 - 40.0) / 2.0;
    double calcVar = 0.5 * (calcMean - 40.0) * (calcMean - 40.0) 
                     + 0.5 * (calcMean - 1500.0) * (calcMean - 1500.0);

    WNS_ASSERT_MAX_REL_ERROR(calcVar, var.get(), 0.01);

    delete dis;
}


void
CDFTableTest::testPyConfig()
{
    pyconfig::Parser config;
    config.loadString(
        "import openwns.distribution\n"
        "dis = openwns.distribution.ExampleCDFTable()\n"
    );

    wns::pyconfig::View disConfig(config, "dis");

    CDFTable* dis =
        dynamic_cast<CDFTable*>(
        wns::distribution::DistributionFactory::creator(disConfig.get<std::string>("__plugin__"))
        ->create(disConfig));

    Average<double> average;
    for(int i = 0; i < 100000; ++i)
    {
        average.put((*dis)());
    }

    double calculatedAverage = dis->getMean();

    WNS_ASSERT_MAX_REL_ERROR(calculatedAverage, average.get(), 0.01);

    delete dis;
}

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
