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

#include "MarkovBaseTest.hpp"

#include <string>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <string>

using namespace wns::markovchain;

CPPUNIT_TEST_SUITE_REGISTRATION( MarkovBaseTest );

void
MarkovBaseTest::setUp()
{
}

void
MarkovBaseTest::tearDown()
{
}

void
MarkovBaseTest::testDefaultConstructor()
{
	int numberOfStates = 3;
	int numberOfChains = 5;

	MarkovBase<int> markovModel = MarkovBase<int>(numberOfChains);
	CPPUNIT_ASSERT( markovModel.numberOfStates == 0 );
	CPPUNIT_ASSERT( markovModel.numberOfChains == 5 );
	markovModel.setNumberOfStates(numberOfStates);
	CPPUNIT_ASSERT( markovModel.numberOfStates == 3 );
	CPPUNIT_ASSERT( markovModel.numberOfChains == 5 );
}

void
MarkovBaseTest::testCompleteConstructor()
{
	int numberOfStates = 4;
	int numberOfChains = 4;

	std::vector<int> states = std::vector<int>(numberOfStates);
	for(int i = 0; i < numberOfStates; i++) states[i] = i;
	CPPUNIT_ASSERT(states.size() == 4);
	std::vector<int> startStates = std::vector<int>(numberOfChains, 4);
	TransitionMatrixType 
        matrix(boost::numeric::ublas::zero_matrix<double>(numberOfStates, numberOfStates));

	CPPUNIT_ASSERT( matrix(0, 0) == 0.0 );

	for(int x=0; x < numberOfStates; x++)
		for(int y=0; y < numberOfStates; y++)
			matrix(x, y) = x + y;

	CPPUNIT_ASSERT( matrix(0, 0) == 0 );
    CPPUNIT_ASSERT( matrix(2, 2) == 4 );
    CPPUNIT_ASSERT( matrix(3, 3) == 6 );

	MarkovBase<int> markovModel = MarkovBase<int>(numberOfStates, numberOfChains, states, matrix, startStates);

	CPPUNIT_ASSERT( markovModel.numberOfStates == numberOfStates );
	CPPUNIT_ASSERT( markovModel.vectorOfStates == states );
	//CPPUNIT_ASSERT( markovModel.transitionsMatrix == matrix );
	CPPUNIT_ASSERT( markovModel.vectorOfStates[1] == 1 );
	CPPUNIT_ASSERT( markovModel.startStates[1] == 4 );
	CPPUNIT_ASSERT( markovModel.actualStates[1] == 4 );
}

void
MarkovBaseTest::testReadNumberStates()
{
	int numberOfChains = 1;

	std::istringstream in(
		"# markov_onoff1.gdf\n"
		"# \n"
		"# Number of States\n"
		"N = 2\n"
		"# \n"
		"# Arguments\n"
		"# StateNr. PDF Arg0 Arg1 Rate(optional)[c/s] \n"
		"0 constant 5.0e+12 0.0 \n"
		"1 constant 10e-6 0.0 \n"
		"# \n"
		"0.0 10e3 \n"
		"10e3 0.0 \n"
		"# \n"
		,std::istringstream::in);

	MarkovBase<int> markovModel = MarkovBase<int>(numberOfChains);

	int nos = markovModel.readNumberOfStates(in);
	CPPUNIT_ASSERT_EQUAL( nos, 2 );
	CPPUNIT_ASSERT_EQUAL( markovModel.numberOfStates, 2 );
}

void
MarkovBaseTest::testReadTransitionsFromFile()
{
	int numberOfChains = 1;
	std::string line;

	std::stringstream file;
	file <<	"# markov_onoff1.gdf"<< std::endl;
	file << "# "<< std::endl;
	file << "# "<< std::endl;
	file << "# Number of States"<< std::endl;
	file << "N = 2"<< std::endl;
	file << "# "<< std::endl;
	file << "# Arguments"<< std::endl;
	file << "# StateNr. PDF Arg0 Arg1 Rate(optional)[c/s] "<< std::endl;
	file << "0 constant 5.0e+12 0.0 "<< std::endl;
	file << "1 constant 10e-6 0.0 "<< std::endl;
	file << "# "<< std::endl;
	file << "0.0 10e3 "<< std::endl;
	file << "10e3 0.0 "<< std::endl;
	file << "# "<< std::endl;

	MarkovBase<int> markovModel = MarkovBase<int>(numberOfChains);

	markovModel.readNumberOfStates(file);
	// skip the lines with the traffic specification (MarkovContinuousTimeTraffic.hpp)
	int stateCounter = 0;
	while (file.good() && (stateCounter < markovModel.numberOfStates)) {
		std::getline(file, line);
		int comment_pos = line.find_first_not_of(" \t");
		if (line[comment_pos] != '#'){
			stateCounter++;
		}
	}
    markovModel.readTransitionsFromFile(file);
	CPPUNIT_ASSERT_EQUAL( markovModel.transitionsMatrix(0, 0), 0.0 );
	CPPUNIT_ASSERT_EQUAL( markovModel.transitionsMatrix(0, 1), 10e3 );
	CPPUNIT_ASSERT_EQUAL( markovModel.transitionsMatrix(1, 0), 10e3 );
	CPPUNIT_ASSERT_EQUAL( markovModel.transitionsMatrix(1, 1), 0.0 );
}

