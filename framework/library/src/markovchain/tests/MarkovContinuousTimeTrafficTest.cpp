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

#include "MarkovContinuousTimeTrafficTest.hpp"
#include <WNS/markovchain/MarkovBase.hpp>
#include <WNS/markovchain/MarkovContinuousTime.hpp>
#include <WNS/markovchain/MarkovContinuousTimeTraffic.hpp>
#include <WNS/CppUnit.hpp>
#include <iostream>
#include <fstream>

using namespace wns::markovchain;

CPPUNIT_TEST_SUITE_REGISTRATION( MarkovContinuousTimeTrafficTest );

void
MarkovContinuousTimeTrafficTest::setUp()
{
}

void
MarkovContinuousTimeTrafficTest::tearDown()
{
}

void
MarkovContinuousTimeTrafficTest::testReadFile()
{
	int numberOfChains = 1;
	MarkovContinuousTimeTraffic markovmodel(numberOfChains); 

	wns::pyconfig::Parser config;
	config.loadString(
		"import openwns\n"
		"import os\n"
		"filename = os.path.join(openwns.getPyConfigPath(), 'openwns', 'markov', 'markov_onoff1.gdf')\n"
		);

	std::string filename = config.get<std::string>("filename");
	std::ifstream in(filename.c_str());
	CPPUNIT_ASSERT_MESSAGE("error opening file " + filename, in.good());

	int nos=markovmodel.readNumberOfStates(in);
	CPPUNIT_ASSERT_EQUAL(nos, 2);

	markovmodel.readStatesFromFile(in);
	markovmodel.readTransitionsFromFile(in);
	in.close();

	CPPUNIT_ASSERT(markovmodel.getStates()[0].interArrivalTimeDistribution != NULL);
	CPPUNIT_ASSERT(markovmodel.getStates()[0].packetSizeDistribution != NULL);
	CPPUNIT_ASSERT(markovmodel.getStates()[1].interArrivalTimeDistribution != NULL);
	CPPUNIT_ASSERT(markovmodel.getStates()[1].packetSizeDistribution != NULL);
}

std::string
MarkovContinuousTimeTrafficTest::fileExample01()
{
	return std::string(
  		"# modified_markov_onoff1.gdf\n"
		"# \n"
		"# Number of States\n"
		"N = 2\n"
		"# \n"
		"# Arguments\n"
		"# StateNr. PDF Arg0 Arg1 Rate(optional)[c/s] \n"
		"0 constant 5.0e+12 0.0 \n"
		"1 constant 10e-6 0.0 \n"
		"# \n"
		"0.0 40e3 \n"
		"10e3 0.0 \n"
		"# \n"
		);
}

void
MarkovContinuousTimeTrafficTest::testRunEvents()
{
	int numberOfChains = 1;

	simTimeType startTime = 0.0;
	simTimeType stopTime = 1.0;

	std::istringstream in( fileExample01(), std::istringstream::in);

	CPPUNIT_ASSERT(in.good());

	MarkovContinuousTimeTraffic markovmodel(numberOfChains);
	int nos=markovmodel.readNumberOfStates(in);
	CPPUNIT_ASSERT_EQUAL(nos, 2);
	markovmodel.readStatesFromFile(in);
	markovmodel.readTransitionsFromFile(in);
    // in.close(); // in case in is a file
	CPPUNIT_ASSERT(markovmodel.getStates()[0].interArrivalTimeDistribution != NULL);
	CPPUNIT_ASSERT(markovmodel.getStates()[0].packetSizeDistribution != NULL);
	CPPUNIT_ASSERT(markovmodel.getStates()[1].interArrivalTimeDistribution != NULL);
	CPPUNIT_ASSERT(markovmodel.getStates()[1].packetSizeDistribution != NULL);

	// matrix arithmetic (GSL: SVD):
	markovmodel.calculateStateProbabilities();
	// check the state probabilities (specific for the input transition matrix):
	WNS_ASSERT_MAX_REL_ERROR(0.2, markovmodel.getStateProbability(0), 1E-6);
	WNS_ASSERT_MAX_REL_ERROR(0.8, markovmodel.getStateProbability(1), 1E-6);

	markovmodel.setStartTime(startTime);
	markovmodel.setStopTime(stopTime);
	std::vector<int> startStates=std::vector<int>(numberOfChains, 0);
	markovmodel.setStartStates(startStates);

	// assure clean scheduler
	wns::events::scheduler::Interface* scheduler = wns::simulator::getEventScheduler();
	scheduler->reset(); // start at time 0 again
	markovmodel.startEvents();

	// now (numberOfChains) events are scheduled
	int it=0;
	simTimeType now;
	int lastStateIndex=markovmodel.getActualStateIndex(0 /* chain */);
	do {
		it++;
		CPPUNIT_ASSERT(scheduler->processOneEvent());
		int stateIndex=markovmodel.getActualStateIndex(0 /* chain */);
		// a state change must have happened:
		CPPUNIT_ASSERT(stateIndex != lastStateIndex);
		lastStateIndex = stateIndex;
		const TrafficSpec *traffic = markovmodel.getStateContent(stateIndex);
		CPPUNIT_ASSERT(traffic!= NULL);
		/*
		  for (int chain = 0; chain < numberOfChains ;chain++){
				int stateNum = markovmodel.getActualState(chain);
			    std::cout	   << "Traffic in chain  " << chain << "  is   "
					  << std::endl;
					  }
		*/

			now = scheduler->getTime();
	} while ((now < 1.0) && (it < 100));
}

