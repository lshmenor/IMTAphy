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

#include <WNS/probe/bus/PythonProbeBus.hpp>
#include <WNS/probe/bus/tests/ProbeBusStub.hpp>
#include <WNS/pyconfig/Parser.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <sstream>

namespace wns { namespace probe { namespace bus { namespace tests {
    /**
     * @brief Tests for the PythonProbeBusTest
     * @author Daniel Bültmann <me@daniel-bueltmann.de>
     */
    class PythonProbeBusTest : public CppUnit::TestFixture  {
        CPPUNIT_TEST_SUITE( PythonProbeBusTest );
        CPPUNIT_TEST( testIsAccepting );
        CPPUNIT_TEST( testOnMeasurement );
        CPPUNIT_TEST( testBrokenFunction );
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp();
        void tearDown();

        void testIsAccepting();
        void testOnMeasurement();
        void testBrokenFunction();
    };
}}}}

using namespace wns::probe::bus::tests;

CPPUNIT_TEST_SUITE_REGISTRATION( PythonProbeBusTest );

void
PythonProbeBusTest::setUp()
{
}

void
PythonProbeBusTest::tearDown()
{
}

void
PythonProbeBusTest::testIsAccepting()
{
    std::stringstream config;
    config << "class SomeClass:\n"
           << "   def __init__(self):\n"
           << "      self.reportErrors = True\n"
           << "   def accepts(self, timestamp, registry):\n"
           << "      return registry[\"testkey\"] == 5\n"
           << "   def onMeasurement(self, timestamp, value, registry):\n"
           << "      pass\n"
           << "   def output(self):\n"
           << "      pass\n"
           << "a = SomeClass()\n";

    wns::pyconfig::Parser pp;
    pp.loadString(config.str());

    ProbeBus* theProbeBus = new PythonProbeBus(pp.get<wns::pyconfig::View>("a"));
    ProbeBusStub listener;

    listener.startObserving(theProbeBus);

    PyContext reg;
    reg.insertInt("testkey", 5);
    reg.insertInt("lala", 2);

    // Test filtering according to Registry
    PyContext reg2;
    reg2.insertInt("lala", 8);
    reg2.insertInt("testkey", 7);

    theProbeBus->forwardMeasurement(1.0, 100.234, reg2);
    CPPUNIT_ASSERT(listener.receivedCounter == 0);

    // Test filtering according to Registry
    PyContext reg3;
    reg3.insertInt("testkey", 5);
    reg3.insertInt("lala", 4);

    theProbeBus->forwardMeasurement(2.01, 100.234, reg3);
    CPPUNIT_ASSERT(listener.receivedCounter == 1);
    CPPUNIT_ASSERT(listener.receivedTimestamps[0] == 2.01);
    CPPUNIT_ASSERT(listener.receivedValues[0] == 100.234);

    delete theProbeBus;
}

void
PythonProbeBusTest::testOnMeasurement()
{
    std::stringstream config;
    config << "class SomeClass:\n"
           << "   def __init__(self):\n"
           << "      self.reportErrors = True\n"
           << "   def accepts(self, timestamp, registry):\n"
           << "      return registry[\"testkey\"] == 5\n"
           << "   def onMeasurement(self, timestamp, value, registry):\n"
           << "      self.value = value\n"
           << "      self.key = registry[\"testkey\"]\n"
           << "   def output(self):\n"
           << "      pass\n"
           << "a = SomeClass()\n";

    wns::pyconfig::Parser pp;
    pp.loadString(config.str());

    ProbeBus* theProbeBus = new PythonProbeBus(pp.get<wns::pyconfig::View>("a"));
    ProbeBusStub listener;

    listener.startObserving(theProbeBus);

    PyContext reg;
    reg.insertInt("testkey", 5);
    reg.insertInt("lala", 2);

    // Test filtering according to Registry
    PyContext reg2;
    reg2.insertInt("lala", 8);
    reg2.insertInt("testkey", 7);

    theProbeBus->forwardMeasurement(5.0, 100.234, reg2);
    CPPUNIT_ASSERT(listener.receivedCounter == 0);

    // Test filtering according to Registry
    PyContext reg3;
    reg3.insertInt("testkey", 5);
    reg3.insertInt("lala", 4);

    theProbeBus->forwardMeasurement(3.32, 100.234, reg3);
    CPPUNIT_ASSERT(listener.receivedCounter == 1);
    CPPUNIT_ASSERT(listener.receivedValues[0] == 100.234);
    CPPUNIT_ASSERT(listener.receivedTimestamps[0] == 3.32);
    CPPUNIT_ASSERT(pp.get<int>("a.key") == 5);
    CPPUNIT_ASSERT(pp.get<double>("a.value") == 100.234);
    delete theProbeBus;
}

void
PythonProbeBusTest::testBrokenFunction()
{
    std::stringstream config;
    config << "class SomeClassWithBrokenCallable:\n"
           << "   def __init__(self):\n"
           << "     pass\n"
           << "   def __call__(self, reg):\n"
           << "      tmp = IAmAnUndefinedValue\n"
           << "class ConfigurationClass:\n"
           << "   def __init__(self, function):\n"
           << "      self.accepts = function\n"
           << "      self.reportErrors = False\n"
           << "   def onMeasurement(self, timestamp, value, reg):\n"
           << "      pass\n"
           << "a = ConfigurationClass(SomeClassWithBrokenCallable())\n";

    wns::pyconfig::Parser pp;
    pp.loadString(config.str());

    ProbeBus* theProbeBus = new PythonProbeBus(pp.get<wns::pyconfig::View>("a"));
    ProbeBusStub listener;

    listener.startObserving(theProbeBus);

    PyContext reg;
    reg.insertInt("festknie", 9);
    reg.insertInt("papa", 8);

    CPPUNIT_ASSERT_THROW(theProbeBus->forwardMeasurement(9982.2, 1404.2, reg),
                         Exception );

    delete theProbeBus;
}

