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

#include <WNS/distribution/tests/LogNormTest.hpp>

#include <WNS/pyconfig/Parser.hpp>
#include <WNS/Average.hpp>
#include <WNS/distribution/tests/VarEstimator.hpp>

using namespace wns::distribution::test;

CPPUNIT_TEST_SUITE_REGISTRATION( LogNormTest );


void
LogNormTest::setUp()
{
} // setUp


void
LogNormTest::tearDown()
{
} // tearDown


void
LogNormTest::testIt()
{
  wns::pyconfig::View config =
    wns::pyconfig::Parser::fromString("import openwns.distribution\n"
				      "dist = openwns.distribution.LogNorm(42.0, 1.0)\n");


  LogNorm dis(config.get("dist"));

  Average<double> average;
  for(long int i = 0; i < 1000000; ++i)
    {
      average.put((dis)());
    }

  WNS_ASSERT_MAX_REL_ERROR(42.0, average.get(), 0.01);

} // testIt

void
LogNormTest::testVar()
{
  LogNorm dis = LogNorm(1.0, 2.0);

  VarEstimator var;
  for(long int i = 0; i < 5000000; ++i)
    {
      var.put(dis());
    }

  double calcVar = 2.0 * 2.0;

  WNS_ASSERT_MAX_REL_ERROR(calcVar, var.get(), 0.01);

} // testVar
