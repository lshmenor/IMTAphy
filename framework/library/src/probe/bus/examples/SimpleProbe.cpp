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

#include <WNS/CppUnit.hpp>
#include <WNS/simulator/ISimulator.hpp>

#include <WNS/probe/bus/ProbeBusRegistry.hpp>
#include <WNS/probe/bus/ProbeBus.hpp>
#include <WNS/probe/bus/ContextCollector.hpp>
#include <WNS/probe/bus/tests/ProbeBusStub.hpp>

#include <WNS/pyconfig/Parser.hpp>
#include <WNS/testing/TestTool.hpp>

namespace wns { namespace probe { namespace bus { namespace examples {

	class SimpleProbe :
		public wns::TestFixture
	{
		CPPUNIT_TEST_SUITE( SimpleProbe );
		CPPUNIT_TEST( underTheHood );
		CPPUNIT_TEST( theWNSWay );
		CPPUNIT_TEST_SUITE_END();
	public:
		void prepare();
		void cleanup();

		void underTheHood();
		void theWNSWay();
	private:
		// Create Empty global context
		ContextProviderCollection globalContext_;
	};

	// Registered in the Spike() testsuite, so it is not run
	// automatically with the other unittests
	CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( SimpleProbe, wns::testsuite::Spike() );

}
}
}
}

using namespace wns::probe::bus::examples;

void
SimpleProbe::prepare()
{
}

void
SimpleProbe::cleanup()
{
}

void
SimpleProbe::underTheHood()
{
	// begin example "wns::probe::bus::example::SimpleProbe::underTheHood.example"
	// Source and sink are only coupled via a Name
	std::string probeBusName = "ThisNameLinksSourceAndSink";

	// Prepare Sink
	ProbeBusRegistry* pbr = wns::simulator::getProbeBusRegistry();
	// Ask registry for probeBus, implicitly creating a default probeBus (as link) under
	// the given name if none is known already
	ProbeBus* link = pbr->getMeasurementSource(probeBusName);
	// Create the actual sink object
	wns::probe::bus::tests::ProbeBusStub sink;
	// observe the root of the tree for the given Name
	sink.startObserving(link);

	// Prepare Source, it will approach the global ProbeBusRegistry and ask for probeBusName
	ContextCollector source(globalContext_, probeBusName);
	// end example

	// If everything has worked out as planned, the source will now put values
	// into the probebus under name "ThisNameLinksSourceAndSink", the link_
	// will then propagate the value to all its children, and consequently into
	// our sink_

	// The sink starts up in "vanilla" state
	CPPUNIT_ASSERT_EQUAL( 0, sink.receivedCounter );

	// After having connected source and sink, it is child's play. Just trigger
	// the forwarding of values and they will be delivered to the source
	source.put(42.0);
	source.put(43.0);

	// Two values received by sink
	CPPUNIT_ASSERT_EQUAL( 2, sink.receivedCounter );
	CPPUNIT_ASSERT_EQUAL( 42.0, sink.receivedValues.at(0) );
	CPPUNIT_ASSERT_EQUAL( 43.0, sink.receivedValues.at(1) );

	// Tear down
	sink.stopObserving(link);
}

void
SimpleProbe::theWNSWay()
{
	// begin example "wns::probe::bus::example::SimpleProbe::theWNSWay.example"
	// Config specifies the (joint) name for source and sink and configures
	// the sink, in this case a simple statistical evaluation Object (MomentsEval)
	wns::pyconfig::View config = wns::pyconfig::Parser::fromString(
		"import wns.ProbeBus\n"
		"import wns.probe.StatEval\n"
		"import wns.WNS\n"
		"sim = wns.WNS.WNS()\n"
		"probeBusName = 'ThisNameLinksSourceAndSink'\n"
		"pbr = sim.environment.probeBusRegistry\n"
		"bus = wns.ProbeBus.StatEvalProbeBus('outFile_Mom.dat', wns.probe.StatEval.MomentsEval())\n"
		"pbr.getMeasurementSource(probeBusName).addObserver(bus)\n"
		);

	// Prepare Source, it will approach the global ProbeBusRegistry and ask for probeBusName
	ContextCollector source(globalContext_, config.get<std::string>("probeBusName"));

	// mimic some measurements
	source.put(10.0);
	source.put(20.0);
	source.put(30.0);
	// end example



	// trigger the output just for testing purposes, normally WNS does this for you at appropriate
	// points in time
	wns::simulator::getProbeBusRegistry()->forwardOutput();

	// Check output for consistency
	std::stringstream fileName;
	fileName << wns::simulator::getConfiguration().get<std::string>("outputDir")
		 << "/outFile_Mom.dat";

	std::vector<std::string> expectations;
	expectations.push_back("#.*Trials: 3.*\n");
	expectations.push_back("#.*Mean: 20.000*\n");

	CPPUNIT_ASSERT( wns::testing::matchInFile(fileName.str(), expectations) );
}


