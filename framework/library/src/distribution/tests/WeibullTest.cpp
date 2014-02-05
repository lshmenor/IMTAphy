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

#include <WNS/distribution/tests/WeibullTest.hpp>

#include <WNS/pyconfig/Parser.hpp>
#include <WNS/Average.hpp>
#include <WNS/distribution/tests/VarEstimator.hpp>

using namespace wns::distribution::test;

CPPUNIT_TEST_SUITE_REGISTRATION( WeibullTest );


void
WeibullTest::setUp()
{
} // setUp


void
WeibullTest::tearDown()
{
} // tearDown


void
WeibullTest::testIt()
{
  wns::pyconfig::View config =
    wns::pyconfig::Parser::fromString("import openwns.distribution\n"
				      "dist = openwns.distribution.Weibull(2.0, 1.0)\n");


  Weibull dis(config.get("dist"));

  Average<double> average;
  for(long int i = 0; i < 1000000; ++i)
    {
      average.put((dis)());
    }
  double calcMean = dis.Weibull::getMean();

  WNS_ASSERT_MAX_REL_ERROR(calcMean, average.get(), 0.01);

} // testIt

void
WeibullTest::testVar()
{
  Weibull dis = Weibull(2.0, 1.0);

  VarEstimator var;
  for(long int i = 0; i < 1000000; ++i)
    {
      var.put(dis());
    }

  double calcVar = 2.0 * 2.0 * dis.Weibull::gamma(1.0 + (2.0 / 1.0)) - pow(dis.Weibull::getMean(), 2);

  WNS_ASSERT_MAX_REL_ERROR(calcVar, var.get(), 0.01);

} // testVar

