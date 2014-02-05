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

#include <WNS/distribution/tests/ErlangTest.hpp>

#include <WNS/pyconfig/Parser.hpp>
#include <WNS/Average.hpp>
#include <WNS/distribution/tests/VarEstimator.hpp>

using namespace wns::distribution::test;

CPPUNIT_TEST_SUITE_REGISTRATION( ErlangTest );


void
ErlangTest::setUp()
{
} // setUp


void
ErlangTest::tearDown()
{
} // tearDown


void
ErlangTest::testIt()
{
    wns::pyconfig::View config = wns::pyconfig::Parser::fromString(
            "import openwns.distribution\n"
            "dist = openwns.distribution.Erlang(42.0, 4)\n"
            );

    Erlang dis(config.get("dist"));

    Average<double> average;
    for(int i = 0; i < 100000; ++i)
        average.put(dis());

    WNS_ASSERT_MAX_REL_ERROR(dis.getMean(), average.get(), 0.01);
} // testIt

void
ErlangTest::testVar()
{
    Erlang dis = Erlang(42.0, 4);

    VarEstimator var;
    for(int i = 0; i < 1000000; ++i)
       var.put(dis());

    WNS_ASSERT_MAX_REL_ERROR(4.0 / (42.0 * 42.0), var.get(), 0.01); // trying even harder ;)
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
