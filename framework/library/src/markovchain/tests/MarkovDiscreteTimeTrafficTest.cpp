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


#include "MarkovDiscreteTimeTrafficTest.hpp"
#include <WNS/markovchain/MarkovBase.hpp>
#include <WNS/markovchain/MarkovDiscreteTime.hpp>
#include <WNS/markovchain/MarkovDiscreteTimeTraffic.hpp>
#include <iostream>
#include <fstream>
#include <stdlib.h>


using namespace wns::markovchain;

CPPUNIT_TEST_SUITE_REGISTRATION( MarkovDiscreteTimeTrafficTest );

void
MarkovDiscreteTimeTrafficTest::setUp()
{
}

void
MarkovDiscreteTimeTrafficTest::tearDown()
{
}

std::string
MarkovDiscreteTimeTrafficTest::fileExample01()
{
	return std::string(
		"# markov_d_onoff1.gdf\n"
		"#\n"
		"#\n"
		"# Number of States\n"
		"N = 2\n"
		"#\n"
		"# Arguments\n"
		"# StateNr. PDF Arg0 Arg1 \n"
		"0 constant 1e+12 0.0 \n"
		"1 constant 1e-3 0.0 \n"
		"#\n"
		"0.900 0.100 \n"
		"0.100 0.900 \n"
		"#\n"
		);
}

void
MarkovDiscreteTimeTrafficTest::testRead()
{
	int numberOfChains = 1;

	std::istringstream in( fileExample01(), std::istringstream::in);
	CPPUNIT_ASSERT(in.good());

	MarkovDiscreteTimeTraffic markovmodel(numberOfChains);

	int nos=markovmodel.readNumberOfStates(in);
	CPPUNIT_ASSERT_EQUAL(nos, 2);
	markovmodel.readStatesFromFile(in);
	markovmodel.readTransitionsFromFile(in);

	CPPUNIT_ASSERT(markovmodel.getStates()[0].interArrivalTimeDistribution != NULL);
	CPPUNIT_ASSERT(markovmodel.getStates()[0].packetSizeDistribution != NULL);
	CPPUNIT_ASSERT(markovmodel.getStates()[1].interArrivalTimeDistribution != NULL);
	CPPUNIT_ASSERT(markovmodel.getStates()[1].packetSizeDistribution != NULL);

	// matrix arithmetic (GSL: SVD):
	//markovmodel.calculateStateProbabilities();
	// check the state probabilities (specific for the input transition matrix):
	//WNS_ASSERT_MAX_REL_ERROR(0.5, markovmodel.getStateProbability(0), 1E-6);
	//WNS_ASSERT_MAX_REL_ERROR(0.5, markovmodel.getStateProbability(1), 1E-6);
}


void
MarkovDiscreteTimeTrafficTest::testMarkovDiscreteTime()
{
	int numberOfChains = 1;

	std::istringstream in( fileExample01(), std::istringstream::in);
	CPPUNIT_ASSERT(in.good());

	simTimeType startTime = 0.0;
	simTimeType stopTime = 1.0;
	simTimeType slotTime = 0.003;

	MarkovDiscreteTimeTraffic markovmodel(numberOfChains);

	int nos=markovmodel.readNumberOfStates(in);
	CPPUNIT_ASSERT_EQUAL(nos, 2);
	markovmodel.readStatesFromFile(in);
	markovmodel.readTransitionsFromFile(in);

	markovmodel.setStartTime(startTime);
	markovmodel.setStopTime(stopTime);
	markovmodel.setSlotTime(slotTime);

	std::vector<int> startStates=std::vector<int>(numberOfChains, 0);
	markovmodel.setStartStates(startStates);

	// assure clean scheduler
	wns::events::scheduler::Interface* scheduler = wns::simulator::getEventScheduler();
	scheduler->reset();
	markovmodel.startEvents();

	int it=0;
	simTimeType now;
	do {
		it++;
		CPPUNIT_ASSERT(scheduler->processOneEvent());
		now = scheduler->getTime();
	} while ((now < 1.0) && (it < 160));
}
