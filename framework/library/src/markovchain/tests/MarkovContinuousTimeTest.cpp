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


#include "MarkovContinuousTimeTest.hpp"
#include <WNS/CppUnit.hpp>
#include <WNS/container/Matrix.hpp>

#include <iostream>
#include <fstream>

using namespace wns::markovchain;

CPPUNIT_TEST_SUITE_REGISTRATION( MarkovContinuousTimeTest );

MarkovContinuousTimeTest::MarkovContinuousTimeTest() :
	wns::TestFixture(),
	useCout(false)
{
}

MarkovContinuousTimeTest::~MarkovContinuousTimeTest()
{
}

void
MarkovContinuousTimeTest::prepare()
{
	wns::events::scheduler::Interface* scheduler = wns::simulator::getEventScheduler();
	scheduler->reset();
	assure(!scheduler->processOneEvent(), "Scheduler must be clean!");
}

void
MarkovContinuousTimeTest::cleanup()
{
}

void
MarkovContinuousTimeTest::testPrepare()
{
	int numberOfStates = 3;
	int numberOfChains = 1;
	simTimeType startTime = 0.0;
	simTimeType stopTime = 1.0;

	std::vector<int> states = std::vector<int>(numberOfStates);
	for(int i = 0; i < numberOfStates; i++) states[i] = i;

	std::vector<int> startStates = std::vector<int>(numberOfChains, 0);

	const MatrixDistributions::SizeType sizes1[2] = {numberOfStates, numberOfStates};
	MatrixDistributions matrixD = MatrixDistributions(sizes1);

	TransitionMatrixType matrix(numberOfStates, numberOfStates);

	// fill in the matrix
	for(int x = 0; x < numberOfStates; x++){
		for(int y = 0; y < numberOfStates; y++){
			if (x == y){
				matrix(x, y) = 0.0; // diagonal = 0
			}
			else{
				matrix(x, y) = 1000 + x + y;
			}
		}
	}

	MarkovContinuousTime<int> markovContinuous(numberOfStates, numberOfChains, states, matrix, startStates, startTime, stopTime);

 	CPPUNIT_ASSERT( markovContinuous.transitionsMatrix(1, 1) == 0.0 ); // diagonal = 0
	CPPUNIT_ASSERT( markovContinuous.transitionsMatrix(0, 1) == 1001 ); // elements are correct

	markovContinuous.prepareMatrixOfDistributions();

	CPPUNIT_ASSERT( markovContinuous.matrixDistribution[0][0] == NULL ); // diagonal = 0
	CPPUNIT_ASSERT( markovContinuous.matrixDistribution[1][1] == NULL ); // diagonal = 0
	CPPUNIT_ASSERT( markovContinuous.matrixDistribution[2][2] == NULL ); // diagonal = 0
	CPPUNIT_ASSERT( markovContinuous.matrixDistribution[0][1] != NULL ); // elements are correct

}//testPrepare


void
MarkovContinuousTimeTest::testNextState()
{

	int numberOfChains = 3;
	int numberOfStates=4;

	simTimeType startTime = 0.0;
	simTimeType stopTime = 1.0;
	std::vector<int> states=std::vector<int>(numberOfStates);
	for(int i = 0; i < numberOfStates; i++) states[i]=i;
	CPPUNIT_ASSERT( states.size() == 4 );

	std::vector<int> startStates=std::vector<int>(numberOfChains, 0);

    TransitionMatrixType matrix(numberOfStates, numberOfStates);

	// fill in the matrix
	for(int x=0; x < numberOfStates; x++){
		for(int y=0; y < numberOfStates; y++){
			if (x == y){
				matrix(x, y) = 0.0; // diagonal = 0
			}
			else{
					matrix(x, y) = 1000 + x + y;
			}
		}
	}

	MarkovContinuousTime<int> markovContinuous(numberOfStates, numberOfChains, states, matrix, startStates, startTime, stopTime);

	// assure clean scheduler
	wns::events::scheduler::Interface* scheduler = wns::simulator::getEventScheduler();
	scheduler->reset(); // start at time 0 again
	markovContinuous.startEvents();
	// now (numberOfChains) events are scheduled
	CPPUNIT_ASSERT( markovContinuous.nextTransitionTime[1] != 0 );
	CPPUNIT_ASSERT( markovContinuous.nextTransitionTime[0] != 0 );
	CPPUNIT_ASSERT( markovContinuous.nextTransitionTime[2] != 0 );

	int it=0;
	simTimeType now;
	do {
		it++;
		CPPUNIT_ASSERT(scheduler->processOneEvent());
		now = scheduler->getTime();
	} while ((now < 1.0) && (it < 100));

}

std::string
MarkovContinuousTimeTest::fileExample01()
{
	return std::string(
  		"# modified_markov_onoff1.gdf\n"
		"# \n"
		"# Number of States\n"
		"N = 2\n"
		"# \n"
		"# Arguments\n"
		"# StateNr. PDF Arg0 Arg1 Rate(optional)[c/s] \n"
		"#0 constant 5.0e+12 0.0 \n"
		"#1 constant 10e-6 0.0 \n"
		"# \n"
		"0.0 40e3 \n"
		"10e3 0.0 \n"
		"# \n"
		);
}

void
MarkovContinuousTimeTest::testRead()
{
	int numberOfChains = 1;
	MarkovContinuousTime<int> markovmodel(numberOfChains); 
	std::istringstream in( fileExample01(),	std::istringstream::in);
	CPPUNIT_ASSERT(in.good());
	int nos=markovmodel.readNumberOfStates(in);
	CPPUNIT_ASSERT_EQUAL(nos, 2);
	//markovmodel.readStatesFromFile(in); // not possible here
	markovmodel.readTransitionsFromFile(in);
	//in.close();

	// matrix arithmetic (GSL: SVD):
	markovmodel.calculateStateProbabilities();
	// check the state probabilities (specific for the input transition matrix):
	WNS_ASSERT_MAX_REL_ERROR(markovmodel.getStateProbability(0), 0.2, 1E-6);
	WNS_ASSERT_MAX_REL_ERROR(markovmodel.getStateProbability(1), 0.8, 1E-6);

}

// ./wns-core -y "WNS.masterLogger.enabled = True"  -T wns::markovchain::MarkovContinuousTimeTest::testCalculateStateProbabilities
void
MarkovContinuousTimeTest::testCalculateStateProbabilities()
{
	int numberOfChains = 1;
	int maxNumberOfStates=10;
	int maxTrials=10;

	wns::distribution::StandardUniform* randomDist = new wns::distribution::StandardUniform();
	for (int trial=1; trial<maxTrials; ++trial) {
	    for (int numberOfStates=2; numberOfStates<maxNumberOfStates; ++numberOfStates) {
		MarkovContinuousTime<int> markovmodel(numberOfChains);
		markovmodel.setNumberOfStates(numberOfStates);

        TransitionMatrixType matrix(numberOfStates, numberOfStates);

		// fill in the matrix
		for(int x=0; x < numberOfStates; x++) {
		    double rowSum = 0.0;
		    for(int y=0; y < numberOfStates; y++){
			if (x == y) {
				matrix(x, y) = 0.0; // diagonal = 0
			} else {
				double random = (*randomDist)();
				// random is in [0.0, 1.0]
				matrix(x, y) = random;
				rowSum += random;
			}
		    }
		    matrix(x, x) = - rowSum; // diagonal
		    // ^ done in 
		    // but here it is necessary because of w*Q calculation
		}
		markovmodel.setTransitionMatrix(matrix);
		if (useCout)
		    std::cout << "matrix =\n" << matrix << std::endl;

		markovmodel.calculateStateProbabilities();
		// check the state probabilities (specific for the input transition matrix):
		const std::vector<double> stateProbabilities = markovmodel.getStateProbabilities();

        boost::numeric::ublas::vector<double> p(numberOfStates);

        for(int i = 0; i < numberOfStates; i++)
            p(i) = stateProbabilities[i];
        
        boost::numeric::ublas::vector<double> test(numberOfStates);

        test = boost::numeric::ublas::prod(p, matrix);
    
        double sumP = 0;

		for(int x=0; x < numberOfStates; x++) {
			double result = test(x);
            sumP += p(x);
			if (useCout)
			    std::cout << "result["<<x<<"] = " << result << std::endl;
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, result, 1e-6);
		}
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, sumP, 1e-6);
	    } // foreach numberOfStates
	} // foreach trial
} // testCalculateStateProbabilities()



