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

#include <WNS/distribution/tests/PoissonTest.hpp>

#include <WNS/pyconfig/Parser.hpp>
#include <WNS/Average.hpp>
#include <WNS/distribution/tests/VarEstimator.hpp>

using namespace wns::distribution::test;

CPPUNIT_TEST_SUITE_REGISTRATION( PoissonTest );


void
PoissonTest::setUp()
{
} // setUp


void
PoissonTest::tearDown()
{
} // tearDown


void
PoissonTest::testIt()
{
    wns::pyconfig::View config = wns::pyconfig::Parser::fromString(
            "import openwns.distribution\n"
            "dist = openwns.distribution.Poisson(10.0)\n"
            );
    Poisson dis(config.get("dist"));

    Average<double> average;
    for (long int ii = 0; ii < 100000; ++ii)
    {
        average.put(dis());
    }

    WNS_ASSERT_MAX_REL_ERROR(dis.getMean(), average.get(), 0.01);
} // testIt

void
PoissonTest::testVar()
{
    Poisson dis = Poisson(10.0);

    VarEstimator var;
    for (long int ii = 0; ii < 100000; ++ii)
    {
        var.put(dis());
    }

    WNS_ASSERT_MAX_REL_ERROR(10.0, var.get(), 0.01);
} // testIt

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
