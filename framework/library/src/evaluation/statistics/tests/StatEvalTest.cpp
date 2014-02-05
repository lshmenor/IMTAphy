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

#include <WNS/evaluation/statistics/stateval.hpp>
#include <WNS/pyconfig/Parser.hpp>
#include <WNS/testing/TestTool.hpp>
#include <WNS/TestFixture.hpp>
#include <WNS/simulator/ISimulator.hpp>
#include <WNS/events/scheduler/Interface.hpp>
#include <WNS/events/NoOp.hpp>
#include <WNS/container/UntypedRegistry.hpp>

#include <sstream>
#include <cfloat>

namespace wns { namespace evaluation { namespace statistics { namespace tests {

                /**
                 * @brief Tests for the StatEvals
                 * @author Ralf Pabst <pab@comnets.rwth-aachen.de>
                 *
                 * This test mainly tests the instatiation of the stateval objects from
                 * the StaticFactory and ensures that they behave similar to the ones
                 * created from the "old" constructors
                 */
                class StatEvalTest : public wns::TestFixture
                {
                    CPPUNIT_TEST_SUITE( StatEvalTest );
                    CPPUNIT_TEST( Moments );
                    CPPUNIT_TEST( PDF );
                    CPPUNIT_TEST_SUITE_END();

                public:
                    void prepare();
                    void cleanup();

                    void Moments();
                    void PDF();

                private:
                    /**
                     * @brief perform some very simple tests on the given
                     * stateval::Interface
                     */
                    void
                    tester(StatEvalInterface* ifc) const;

                    /**
                     * @brief Match the output of the given stateval::Interface 'ifc'
                     * against the regexp given in the string 'expectation'.
                     */
                    void testOutput(StatEvalInterface* ifc, const std::string& expectation) const;

                    /**
                     * @brief check whether the output of the given stateval::StatEvalInterfaces
                     * is identical (-->true) or not (-->false)
                     */
                    bool outputEqual(StatEvalInterface* first, StatEvalInterface* second) const;
                };

}
}
}
}

using namespace wns::evaluation::statistics::tests;

CPPUNIT_TEST_SUITE_REGISTRATION( StatEvalTest );

void
StatEvalTest::prepare()
{
}

void
StatEvalTest::cleanup()
{
}

void
StatEvalTest::Moments()
{
    pyconfig::View pyco = pyconfig::Parser::fromString(
        "import openwns.evaluation.statistics\n"
        "stat = openwns.evaluation.statistics.MomentsEval()\n"
        );

    std::string name = pyco.get<std::string>("stat.nameInFactory");

    CPPUNIT_ASSERT( name == "openwns.evaluation.statistics.Moments" );
    StatEvalInterface* ifc = NULL;
    CPPUNIT_ASSERT_NO_THROW( ifc = Factory::creator(name)->create(pyco.get("stat")); );

    this->tester(ifc);

    delete ifc;
}

void
StatEvalTest::PDF()
{
    pyconfig::View pyco = pyconfig::Parser::fromString(
        "import openwns.evaluation.statistics\n"
        "stat = openwns.evaluation.statistics.PDFEval()\n"
        );

    std::string name = pyco.get<std::string>("stat.nameInFactory");

    CPPUNIT_ASSERT( name == "openwns.evaluation.statistics.PDF" );
    StatEvalInterface* ifc = NULL;
    CPPUNIT_ASSERT_NO_THROW( ifc = Factory::creator(name)->create(pyco.get("stat")); );

    this->tester(ifc);

    delete ifc;
}

void
StatEvalTest::tester(StatEvalInterface* ifc) const
{
    wns::evaluation::statistics::StatEval* se =
        dynamic_cast<wns::evaluation::statistics::StatEval*>(ifc);

    CPPUNIT_ASSERT( se != NULL );

    CPPUNIT_ASSERT( ifc->getName() == std::string("no name available") );
    CPPUNIT_ASSERT( se->getDesc() == std::string("no description available") );
    CPPUNIT_ASSERT( se->getFormat() == wns::evaluation::statistics::StatEval::fixed );

    ifc->put(1.0);
    ifc->put(2.0);
    ifc->put(3.0);

    CPPUNIT_ASSERT_DOUBLES_EQUAL( 2.0, se->mean(), 1e-7);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, se->min(), 1e-7);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 3.0, se->max(), 1e-7);
    CPPUNIT_ASSERT_EQUAL( static_cast<unsigned long int>(3), se->trials() );

    ifc->reset();

    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, se->mean(), 1e-7);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(  DBL_MAX, se->min(), 1e-7);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( -DBL_MAX, se->max(), 1e-7);
    CPPUNIT_ASSERT_EQUAL( static_cast<unsigned long int>(0), se->trials() );
}

void
StatEvalTest::testOutput(StatEvalInterface* ifc, const std::string& expectation) const
{
    std::stringstream output;
    CPPUNIT_ASSERT_NO_THROW( ifc->print(output) );
    std::string result = output.str();

    bool comparison = wns::testing::compareString(result, expectation);
    if (comparison == false)
    {
        wns::Exception e;
        e << "\nStatEval Output did not match expectation!\n\n"
          << "Output was:\n\n"
          << result << "-----End-----\n\n\n";

        throw e;
    }
}

bool
StatEvalTest::outputEqual(StatEvalInterface* first, StatEvalInterface* second) const
{
    std::stringstream output;
    CPPUNIT_ASSERT_NO_THROW( first->print(output) );
    std::string firstResult = output.str();
    output.str("");
    CPPUNIT_ASSERT_NO_THROW( second->print(output) );
    std::string secondResult = output.str();
    if (firstResult == secondResult)
    {
        return true;
    }
    else
    {
        std::stringstream e;
        e << "\n\nMismatch between strings!\n\nFirst:\n\n"
          << firstResult << "--End--\n\nSecond:\n\n"
          << secondResult << "--End--\n\n";
        throw wns::Exception(e.str());
        return false;
    }
}
