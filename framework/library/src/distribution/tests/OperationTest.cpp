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

#include <WNS/distribution/tests/OperationTest.hpp>

#include <WNS/pyconfig/Parser.hpp>
#include <WNS/Average.hpp>

using namespace wns::distribution::test;

CPPUNIT_TEST_SUITE_REGISTRATION( OperationTest );


void
OperationTest::setUp()
{
} // setUp


void
OperationTest::tearDown()
{
} // tearDown


void
OperationTest::testFixedAdd()
{
    pyconfig::Parser config;
    config.loadString(
        "from openwns.distribution import *\n"
        "foo = Fixed(20) + Fixed(22)\n"
        );

    pyconfig::View disConfig(config, "foo");
    std::string name = disConfig.get<std::string>("__plugin__");

    wns::distribution::Distribution* dis =
        wns::distribution::DistributionFactory::creator(name)
        ->create(disConfig);

    double mean = dynamic_cast<IHasMean*>(dis)->getMean();

    CPPUNIT_ASSERT_DOUBLES_EQUAL(42.0, (*dis)(), 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(42.0, mean, 0.0001);
} // testFixedAdd


void
OperationTest::testAdd()
{
    pyconfig::Parser config;
    config.loadString(
        "from openwns.distribution import *\n"
        "foo = NegExp(20) + Uniform(44)\n"
        );

    pyconfig::View disConfig(config, "foo");
    std::string name = disConfig.get<std::string>("__plugin__");

    wns::distribution::Distribution* dis =
        wns::distribution::DistributionFactory::creator(name)
        ->create(disConfig);

    Average<double> average;
    for(int i = 0; i < 100000; ++i)
        average.put((*dis)());

    double mean = dynamic_cast<IHasMean*>(dis)->getMean();

    WNS_ASSERT_MAX_REL_ERROR(42.0, average.get(), 0.01);
    WNS_ASSERT_MAX_REL_ERROR(42.0, mean, 0.01);
    delete dis;
} // testAdd


void
OperationTest::testFixedMul()
{
    pyconfig::Parser config;
    config.loadString(
        "from openwns.distribution import *\n"
        "foo = Fixed(20) * Fixed(22)\n"
        );

    pyconfig::View disConfig(config, "foo");
    std::string name = disConfig.get<std::string>("__plugin__");

    wns::distribution::Distribution* dis =
        wns::distribution::DistributionFactory::creator(name)
        ->create(disConfig);

    double mean = dynamic_cast<IHasMean*>(dis)->getMean();

    WNS_ASSERT_MAX_REL_ERROR(20.0 * 22.0, (*dis)(), 0.01);
    WNS_ASSERT_MAX_REL_ERROR(20.0 * 22.0, mean, 0.01);
} // testFixedMul


void
OperationTest::testMul()
{
    pyconfig::Parser config;
    config.loadString(
        "from openwns.distribution import *\n"
        "foo = NegExp(20) * Uniform(44)\n"
        );

    pyconfig::View disConfig(config, "foo");
    std::string name = disConfig.get<std::string>("__plugin__");

    wns::distribution::Distribution* dis =
        wns::distribution::DistributionFactory::creator(name)
        ->create(disConfig);

    Average<double> average;
    for(int i = 0; i < 100000; ++i)
        average.put((*dis)());

    double mean = dynamic_cast<IHasMean*>(dis)->getMean();

    WNS_ASSERT_MAX_REL_ERROR(20.0 * 22.0, average.get(), 0.01);
    WNS_ASSERT_MAX_REL_ERROR(20.0 * 22.0, mean, 0.01);
    delete dis;
} // testMul


void
OperationTest::testFixedSub()
{
    pyconfig::Parser config;
    config.loadString(
        "from openwns.distribution import *\n"
        "foo = Fixed(84) - Fixed(42)\n"
        );

    pyconfig::View disConfig(config, "foo");
    std::string name = disConfig.get<std::string>("__plugin__");

    wns::distribution::Distribution* dis =
        wns::distribution::DistributionFactory::creator(name)
        ->create(disConfig);

    double mean = dynamic_cast<IHasMean*>(dis)->getMean();

    WNS_ASSERT_MAX_REL_ERROR(42.0, (*dis)(), 0.01);
    WNS_ASSERT_MAX_REL_ERROR(42.0, mean, 0.01);
} // testFixedSub


void
OperationTest::testFixedDiv()
{
    pyconfig::Parser config;
    config.loadString(
        "from openwns.distribution import *\n"
        "foo = Fixed(84) / Fixed(2)\n"
        );

    pyconfig::View disConfig(config, "foo");
    std::string name = disConfig.get<std::string>("__plugin__");

    wns::distribution::Distribution* dis =
        wns::distribution::DistributionFactory::creator(name)
        ->create(disConfig);

    double mean = dynamic_cast<IHasMean*>(dis)->getMean();

    WNS_ASSERT_MAX_REL_ERROR(42.0, (*dis)(), 0.01);
    WNS_ASSERT_MAX_REL_ERROR(42.0, mean, 0.01);
} // testFixedDiv


void
OperationTest::testConstant()
{
    pyconfig::Parser config;
    config.loadString(
        "from openwns.distribution import *\n"
        "add = Fixed(40) + 2\n"
        "mul = Fixed(21) * 2\n"
        "sub = Fixed(44) - 2\n"
        "div = Fixed(84) / 2\n"
        );
    std::string foo[4] = {"add", "mul", "sub", "div"};

    for(int i = 0; i < 4; ++i)
    {
        std::string viewName = foo[i];

        pyconfig::View disConfig(config, viewName);
        std::string name = disConfig.get<std::string>("__plugin__");

        wns::distribution::Distribution* dis =
            wns::distribution::DistributionFactory::creator(name)
            ->create(disConfig);

        double mean = dynamic_cast<IHasMean*>(dis)->getMean();

        WNS_ASSERT_MAX_REL_ERROR(42.0, (*dis)(), 0.01);
        WNS_ASSERT_MAX_REL_ERROR(42.0, mean, 0.01);
    }
} // testConstant

void
OperationTest::testAbove()
{
    pyconfig::Parser all;
    all.loadString(
        "from openwns.distribution import Uniform\n"
        "it = Uniform().above(0.5)\n"
        );
    pyconfig::View config(all, "it");

    wns::distribution::Distribution* dis =
        wns::distribution::DistributionFactory::creator(
            config.get<std::string>("__plugin__"))
        ->create(config);

    for(int i = 0; i < 10000; ++i)
        CPPUNIT_ASSERT(0.5 < (*dis)());

    delete dis;
} // testAbove


void
OperationTest::testBelow()
{
    pyconfig::Parser all;
    all.loadString(
        "from openwns.distribution import Uniform\n"
        "it = Uniform().below(0.5)\n"
        );
    pyconfig::View config(all, "it");

    wns::distribution::Distribution* dis =
        wns::distribution::DistributionFactory::creator(
            config.get<std::string>("__plugin__"))
        ->create(config);

    for(int i = 0; i < 10000; ++i)
        CPPUNIT_ASSERT(0.5 > (*dis)());

    delete dis;
} // testBelow


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
