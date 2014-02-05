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

#include <WNS/distribution/tests/UniformTest.hpp>

#include <WNS/pyconfig/Parser.hpp>
#include <WNS/Average.hpp>
#include <WNS/distribution/tests/VarEstimator.hpp>

#include <string>

using namespace wns::distribution::test;

CPPUNIT_TEST_SUITE_REGISTRATION( UniformTest );


void
UniformTest::setUp()
{
} // setUp


void
UniformTest::tearDown()
{
} // tearDown


void
UniformTest::testIt()
{
    wns::pyconfig::View config = wns::pyconfig::Parser::fromString(
            "import openwns.distribution\n"
            "dist = openwns.distribution.Uniform(40.0, 44.0)\n"
            );
    Uniform dis(config.get("dist"));

    Average<double> average;
    for(int i = 0; i < 1000; ++i)
        average.put(dis());

    CPPUNIT_ASSERT_DOUBLES_EQUAL(42.0, average.get(), 1);
} // testIt

void
UniformTest::testStandardUniform()
{
    wns::pyconfig::View config = wns::pyconfig::Parser::fromString(
            "import openwns.distribution\n"
            "dist = openwns.distribution.StandardUniform()\n"
            );

    StandardUniform dis(config.get("dist"));

    Average<double> average;
    for(int i = 0; i < 100000; ++i)
        average.put(dis());

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5, average.get(), 0.01);
} // testNormUniform

void
UniformTest::testVar()
{
    Uniform dis = Uniform(40.0, 44.0);

    VarEstimator var;
    for(int i = 0; i < 1000000; ++i)
        var.put(dis());

    CPPUNIT_ASSERT_DOUBLES_EQUAL(4.0 / 3.0, var.get(), 0.01);
} // testIt

void
UniformTest::testStandardUniformVar()
{
    StandardUniform dis = StandardUniform();

    VarEstimator var;
    for(int i = 0; i < 100000; ++i)
        var.put(dis());

    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0 / 12.0, var.get(), 0.01);
} // testNormUniform


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
