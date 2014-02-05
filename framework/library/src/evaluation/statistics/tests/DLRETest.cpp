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

#include <WNS/evaluation/statistics/dlre.hpp>
#include <WNS/evaluation/statistics/dlreg.hpp>
#include <WNS/evaluation/statistics/dlref.hpp>
#include <WNS/evaluation/statistics/dlrep.hpp>

#include <WNS/pyconfig/Parser.hpp>
#include <WNS/testing/TestTool.hpp>
#include <WNS/TestFixture.hpp>
#include <WNS/simulator/ISimulator.hpp>
#include <WNS/events/scheduler/Interface.hpp>
#include <WNS/events/NoOp.hpp>
#include <WNS/container/UntypedRegistry.hpp>
#include <WNS/distribution/Uniform.hpp>

#include <sstream>
#include <cfloat>

namespace wns { namespace evaluation { namespace statistics { namespace tests {

                /**
                 * @brief Tests for the DLREs
                 *
                 */
                class DLRETest : public wns::TestFixture
                {
                    CPPUNIT_TEST_SUITE( DLRETest );
                    CPPUNIT_TEST( dlreg );
                    CPPUNIT_TEST( dlref );
                    CPPUNIT_TEST( dlrep );
                    CPPUNIT_TEST_SUITE_END();

                public:
                    void prepare();
                    void cleanup();

                    void dlreg();
                    void dlref();
                    void dlrep();

                private:
                    void generateSamples(StatEvalInterface* ifc) const;
                };

}
}
}
}

using namespace wns::evaluation::statistics::tests;

CPPUNIT_TEST_SUITE_REGISTRATION( DLRETest );

void
DLRETest::prepare()
{
}

void
DLRETest::cleanup()
{
}

void DLRETest::generateSamples(StatEvalInterface* ifc) const
{
    wns::distribution::Uniform u(0.0, 1.0);

    ifc->reset();

    for(int i = 0; i < 100000; ++i)
    {
        ifc->put(u());
    }
}

void
DLRETest::dlreg()
{
    pyconfig::View pyco = pyconfig::Parser::fromString(
        "import openwns.evaluation.statistics\n"
        "stat = openwns.evaluation.statistics.DLREEval(mode = \"g\", xMin = 0.0, xMax = 1.0, intervalWidth = 0.1, minLevel = 1e-5)\n"
        );

    std::string name = pyco.get<std::string>("stat.nameInFactory");

    CPPUNIT_ASSERT( name == "openwns.evaluation.statistics.DLREG" );
    StatEvalInterface* ifc = NULL;
    CPPUNIT_ASSERT_NO_THROW( ifc = Factory::creator(name)->create(pyco.get("stat")); );

    wns::evaluation::statistics::DLREG* dlreg =
        dynamic_cast<wns::evaluation::statistics::DLREG*>(ifc);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, dlreg->curGLev(), 1e-7);

    generateSamples(ifc);

    //dlreg->print();

    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5, dlreg->mean(), 1e-1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, dlreg->min(), 1e-1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, dlreg->max(), 1e-1);


    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, dlreg->curGLev(), 1e-7);

    delete ifc;
}

void
DLRETest::dlref()
{
    pyconfig::View pyco = pyconfig::Parser::fromString(
        "import openwns.evaluation.statistics\n"
        "stat = openwns.evaluation.statistics.DLREEval(mode = \"f\", xMin = 0.0, xMax = 1.0, intervalWidth = 0.1, minLevel = 1e-5)\n"
        );

    std::string name = pyco.get<std::string>("stat.nameInFactory");

    CPPUNIT_ASSERT( name == "openwns.evaluation.statistics.DLREF" );
    StatEvalInterface* ifc = NULL;
    CPPUNIT_ASSERT_NO_THROW( ifc = Factory::creator(name)->create(pyco.get("stat")); );

    wns::evaluation::statistics::DLREF* dlref =
        dynamic_cast<wns::evaluation::statistics::DLREF*>(ifc);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, dlref->curFLev(), 1e-7);

    generateSamples(ifc);

    //dlref->print();

    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5, dlref->mean(), 1e-1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, dlref->min(), 1e-1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, dlref->max(), 1e-1);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, dlref->curFLev(), 1e-7);

    delete ifc;
}

void
DLRETest::dlrep()
{
    pyconfig::View pyco = pyconfig::Parser::fromString(
        "import openwns.evaluation.statistics\n"
        "stat = openwns.evaluation.statistics.DLREEval(mode = \"p\", xMin = 0.0, xMax = 1.0, intervalWidth = 0.1, minLevel = 1e-5)\n"
        );

    std::string name = pyco.get<std::string>("stat.nameInFactory");

    CPPUNIT_ASSERT( name == "openwns.evaluation.statistics.DLREP" );
    StatEvalInterface* ifc = NULL;
    CPPUNIT_ASSERT_NO_THROW( ifc = Factory::creator(name)->create(pyco.get("stat")); );

    wns::evaluation::statistics::DLREP* dlrep =
        dynamic_cast<wns::evaluation::statistics::DLREP*>(ifc);


    generateSamples(ifc);

    //dlrep->print();

    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5, dlrep->mean(), 1e-1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, dlrep->min(), 1e-1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, dlrep->max(), 1e-1);

    delete ifc;
}
