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

#include <WNS/TestFixture.hpp>

#include <WNS/probe/bus/ProbeBusRegistry.hpp>
#include <WNS/pyconfig/Parser.hpp>

namespace wns { namespace probe { namespace bus { namespace tests {

    class ProbeBusRegistryTest :
        public wns::TestFixture
    {
        CPPUNIT_TEST_SUITE( ProbeBusRegistryTest );
        CPPUNIT_TEST( measurementSource );
        CPPUNIT_TEST( measurementSink );
        CPPUNIT_TEST_EXCEPTION( measurementSinkThrows, ProbeBusRegistry::ProbeBusRegistryContainer::UnknownKeyValue );
        CPPUNIT_TEST_SUITE_END();
    public:
        void prepare();
        void cleanup();
        void measurementSource();
        void measurementSink();
        void measurementSinkThrows();
    };

    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ProbeBusRegistryTest, wns::testsuite::Default() );

}
}
}
}

using namespace wns::probe::bus::tests;

void
ProbeBusRegistryTest::prepare()
{
}

void
ProbeBusRegistryTest::cleanup()
{
}

void
ProbeBusRegistryTest::measurementSource()
{
    std::string config =
        "import openwns.probebus\n"
        "pbr = openwns.probebus.ProbeBusRegistry()\n";

    ProbeBusRegistry* pbr = new ProbeBusRegistry(wns::pyconfig::Parser::fromString(config).get("pbr"));

    ProbeBus* pb = pbr->getMeasurementSource("wns.probe.bus.tests.TestMeasurementSource");

    CPPUNIT_ASSERT(pb != NULL);

    ProbeBus* samePb = pbr->getMeasurementSource("wns.probe.bus.tests.TestMeasurementSource");

    CPPUNIT_ASSERT(pb == samePb);

    delete pbr;
}

void
ProbeBusRegistryTest::measurementSink()
{
    std::string config =
        "import openwns.probebus\n"
        "pbr = openwns.probebus.ProbeBusRegistry()\n"
        "pb = openwns.probebus.PassThroughProbeBus()\n"
        "pbr.getMeasurementSource(\"wns.probe.bus.tests.TestMeasurementsink\").addObserver(pb)\n";

    ProbeBusRegistry* pbr = new ProbeBusRegistry(wns::pyconfig::Parser::fromString(config).get("pbr"));

    ProbeBus* pb = pbr->getMeasurementSource("wns.probe.bus.tests.TestMeasurementsink");

    pbr->startup();

    CPPUNIT_ASSERT(pb->hasObservers());
    
    delete pbr;
}

void
ProbeBusRegistryTest::measurementSinkThrows()
{
    std::string config =
        "import openwns.probebus\n"
        "pbr = openwns.probebus.ProbeBusRegistry()\n"
        "pb = openwns.probebus.PassThroughProbeBus()\n"
        "pbr.getMeasurementSource(\"wns.probe.bus.tests.TestMeasurementSinkThrows\").addObserver(pb)\n";

    ProbeBusRegistry* pbr = new ProbeBusRegistry(wns::pyconfig::Parser::fromString(config).get("pbr"));

    pbr->getMeasurementSource("wns.probe.bus.tests.TestMeasurementsink");

    pbr->startup();
    
    delete pbr;
}

