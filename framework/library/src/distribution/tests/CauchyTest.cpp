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

#include <WNS/distribution/tests/CauchyTest.hpp>
#include <WNS/pyconfig/Parser.hpp>

using namespace wns::distribution::tests;

CPPUNIT_TEST_SUITE_REGISTRATION( CauchyTest );


void
CauchyTest::setUp()
{
} // setUp


void
CauchyTest::tearDown()
{
} // tearDown


void
CauchyTest::testCDF()
{
    unsigned long int total = 1000000;
    unsigned long int hit = 0;

    pyconfig::Parser config;
    config.loadString(
        "median = 1.5\n"
        "sigma = 3.75\n");

    wns::distribution::Distribution* dis =
        wns::distribution::DistributionFactory::creator("Cauchy")
        ->create(config);

    for(long int i = 0; i < total; ++i)
    {
        double x = (*dis)();
        if(x < 10.0)
            hit++;
    }

    // P(x < X) = 0.5 + (1 / pi) * arctan((x - m) / sigma) => P(x < 10 | m = 1.5, sigma = 3.75) = 0.8678
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.87, double(hit) / double(total), 0.01);
    delete dis;
} // testCDF



