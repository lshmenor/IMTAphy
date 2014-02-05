/******************************************************************************
 * WNS (Wireless Network Simulator)                                           *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2004-2006                                                    *
 * Chair of Communication Networks (ComNets)                                  *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnets.rwth-aachen.de                                          *
 * www: http://wns.comnets.rwth-aachen.de                                     *
 ******************************************************************************/

#include <WNS/distribution/tests/RiceTest.hpp>

#include <WNS/pyconfig/Parser.hpp>
#include <WNS/Average.hpp>
#include <WNS/distribution/tests/VarEstimator.hpp>

using namespace wns::distribution::test;

CPPUNIT_TEST_SUITE_REGISTRATION( RiceTest );


void
RiceTest::setUp()
{
} // setUp


void
RiceTest::tearDown()
{
} // tearDown


void
RiceTest::testIt()
{
    pyconfig::Parser config;
    config.loadString(
        "losFactor = 0\n"
        "variance = 10.0\n"
    );

    Rice* dis =
        dynamic_cast<Rice*>(wns::distribution::DistributionFactory::creator("Rice")
        ->create(config));

    Average<double> average;
    for(long int ii = 0; ii < 100000; ++ii) {
        average.put((*dis)());
    }
 
    WNS_ASSERT_MAX_REL_ERROR(10.0 * sqrt(M_PI / 2.0), average.get(), 0.01);

    delete dis;
} // testIt

void
RiceTest::testVar()
{
    pyconfig::Parser config;
    config.loadString(
        "losFactor = 0\n"
        "variance = 10.0\n"
    );

    Rice* dis =
        dynamic_cast<Rice*>(wns::distribution::DistributionFactory::creator("Rice")
        ->create(config));

    VarEstimator var;
    for(long int ii = 0; ii < 100000; ++ii) {
        var.put((*dis)());
    }
 
    WNS_ASSERT_MAX_REL_ERROR(10.0 * 10.0 * (2.0 - M_PI / 2.0), var.get(), 0.01);

    delete dis;
} // testVar

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
