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

#include <WNS/probe/bus/TableProbeBus.hpp>
#include <WNS/probe/bus/tests/ProbeBusStub.hpp>

#include <WNS/pyconfig/Parser.hpp>

#include <WNS/TestFixture.hpp>
#include <sstream>

namespace wns { namespace probe { namespace bus { namespace tests {

    /**
     * @brief Tests for the TableProbeBus
     * @author Ralf Pabst <pab@comnets.rwth-aachen.de>
     */
    class TableProbeBusTest : public wns::TestFixture  {
        CPPUNIT_TEST_SUITE( TableProbeBusTest );
        CPPUNIT_TEST( test );
        CPPUNIT_TEST( onedimensional );
        CPPUNIT_TEST_SUITE_END();

    public:
        void prepare();
        void cleanup();

        void test();
        void onedimensional();

    private:
        ProbeBus* theTableProbeBus;
    };
}}}}

using namespace wns::probe::bus::tests;

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( TableProbeBusTest, wns::testsuite::Default() );

void
TableProbeBusTest::prepare()
{
}

void
TableProbeBusTest::cleanup()
{
}

void
TableProbeBusTest::test()
{
    wns::pyconfig::View config =
        wns::pyconfig::Parser::fromString(
			"import openwns.probebus\n"
			"xPar = openwns.probebus.TabPar('x',1,4,3)\n"
			"yPar = openwns.probebus.TabPar('y',3,6,3)\n"
			"zPar = openwns.probebus.TabPar('z',16,18,2)\n"
			"a = openwns.probebus.TableProbeBus([zPar,yPar,xPar],'testTable', ['mean','trials'], ['HumanReadable','PythonReadable','MatlabReadable'])\n"
			);

    theTableProbeBus = new TableProbeBus(config.get("a"));

    ProbeBusStub listener;

    listener.startObserving(theTableProbeBus);

	wns::simulator::Time t(42.0);

    {
        // both ids in range --> forwards the measurement
        wns::probe::bus::Context tmp;
        tmp.insertInt("x", 3);
        tmp.insertInt("y", 5);
        tmp.insertInt("z", 17);
        this->theTableProbeBus->forwardMeasurement(t, 1.0, tmp);
        CPPUNIT_ASSERT(listener.receivedCounter == 1);
        CPPUNIT_ASSERT(listener.receivedValues[0] == 1.0);
    }

    {
        // both ids in range --> forwards the measurement
        wns::probe::bus::Context tmp;
        tmp.insertInt("x", 3);
        tmp.insertInt("y", 5);
        tmp.insertInt("z", 17);
        this->theTableProbeBus->forwardMeasurement(t, 2.0, tmp);
        CPPUNIT_ASSERT(listener.receivedCounter == 2);
        CPPUNIT_ASSERT(listener.receivedValues[1] == 2.0);
    }

    {
        // overflow in second id --> do not forward the measurement
        wns::probe::bus::Context tmp;
        tmp.insertInt("x", 3);
        tmp.insertInt("y", 7);
        tmp.insertInt("z", 17);
        this->theTableProbeBus->forwardMeasurement(t, 1.0, tmp);
        CPPUNIT_ASSERT(listener.receivedCounter == 2);
        CPPUNIT_ASSERT(listener.receivedValues[1] == 2.0);
    }

	theTableProbeBus->forwardOutput();

    delete theTableProbeBus;
}

void
TableProbeBusTest::onedimensional()
{
    wns::pyconfig::View config =
        wns::pyconfig::Parser::fromString(
			"import openwns.probebus\n"
			"xPar = openwns.probebus.TabPar('x',1,4,3)\n"
			"a = openwns.probebus.TableProbeBus([xPar],'oldTable', ['mean','trials'], ['HumanReadable','PythonReadable','MatlabReadable'])\n"
			);

    theTableProbeBus = new TableProbeBus(config.get("a"));

    ProbeBusStub listener;

    listener.startObserving(theTableProbeBus);

	wns::simulator::Time t(42.0);

    {
        // both ids in range --> forwards the measurement
        wns::probe::bus::Context tmp;
        tmp.insertInt("x", 1);
        this->theTableProbeBus->forwardMeasurement(t, 1.0, tmp);
        CPPUNIT_ASSERT(listener.receivedCounter == 1);
        CPPUNIT_ASSERT(listener.receivedValues[0] == 1.0);
    }

    {
        // both ids in range --> forwards the measurement
        wns::probe::bus::Context tmp;
        tmp.insertInt("x", 2);
        this->theTableProbeBus->forwardMeasurement(t, 2.0, tmp);
        CPPUNIT_ASSERT(listener.receivedCounter == 2);
        CPPUNIT_ASSERT(listener.receivedValues[1] == 2.0);
    }

    {
        // overflow in second id --> do not forward the measurement
        wns::probe::bus::Context tmp;
        tmp.insertInt("x", 5);
        this->theTableProbeBus->forwardMeasurement(t, 1.0, tmp);
        CPPUNIT_ASSERT(listener.receivedCounter == 2);
        CPPUNIT_ASSERT(listener.receivedValues[1] == 2.0);
    }

	theTableProbeBus->forwardOutput();

    delete theTableProbeBus;
}
