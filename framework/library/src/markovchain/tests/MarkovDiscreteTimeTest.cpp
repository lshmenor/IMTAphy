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


#include <WNS/markovchain/tests/MarkovDiscreteTimeTest.hpp>

using namespace wns::markovchain;


CPPUNIT_TEST_SUITE_REGISTRATION( MarkovDiscreteTimeTest );

MarkovDiscreteTimeTest::MarkovDiscreteTimeTest() :
	wns::TestFixture(),
	useCout(false)
{
}

MarkovDiscreteTimeTest::~MarkovDiscreteTimeTest()
{
}

void
MarkovDiscreteTimeTest::prepare()
{
	wns::events::scheduler::Interface* scheduler = wns::simulator::getEventScheduler();
	scheduler->reset();
	assure(!scheduler->processOneEvent(), "Scheduler must be clean!");
}

void
MarkovDiscreteTimeTest::cleanup()
{
}

void
MarkovDiscreteTimeTest::testCheckSum()
{
	int numberOfStates = 3;
	int numberOfChains = 1;
    TransitionMatrixType matrix(numberOfStates, numberOfStates);
	for(int x = 0; x < numberOfStates; x++)
		for(int y = 0; y < numberOfStates; y++)
			matrix(x, y)=0.3333;

	MarkovDiscreteTime<int> markovmodel(numberOfChains);
	markovmodel.setNumberOfStates(numberOfStates);
	markovmodel.setTransitionMatrix(matrix);
	CPPUNIT_ASSERT( markovmodel.checkSum() == true );
}

void
MarkovDiscreteTimeTest::testNextState()
{
	int numberOfStates = 4;
	int numberOfChains = 2;

	simTimeType startTime = 0.0;
	simTimeType stopTime = 1.0;
	simTimeType slotTime = 0.001;
	std::vector<int> states=std::vector<int>(numberOfStates);
	for(int i = 0; i < numberOfStates; i++) states[i] = i;
	CPPUNIT_ASSERT( states.size() == 4 );

	std::vector<int> startStates=std::vector<int>(numberOfChains, 0);

    TransitionMatrixType matrix(numberOfStates, numberOfStates);

	matrix(0, 0)=0.005;
	matrix(0, 1)=0.205;
	matrix(0, 2)=0.390;
	matrix(0, 3)=0.400;

	matrix(1, 0)=0.005;
	matrix(1, 1)=0.605;
	matrix(1, 2)=0.390;
	matrix(1, 3)=0.000;

	matrix(2, 0)=0.105;
	matrix(2, 1)=0.405;
	matrix(2, 2)=0.390;
	matrix(2, 3)=0.100;

	matrix(3, 0)=0.105;
	matrix(3, 1)=0.405;
	matrix(3, 2)=0.005;
	matrix(3, 3)=0.485;

	MarkovDiscreteTime<int> markovmodel = MarkovDiscreteTime<int>(numberOfStates, numberOfChains, states, matrix, startStates, startTime, slotTime, stopTime);

	// assure clean scheduler
	wns::events::scheduler::Interface* scheduler = wns::simulator::getEventScheduler();
	scheduler->reset(); // start at time 0 again 
	markovmodel.startEvents();

	// now (numberOfChains) events are scheduled

	int it=0;
	simTimeType now;
	do {
		it++;
		CPPUNIT_ASSERT(scheduler->processOneEvent());
			now = scheduler->getTime();
	} while ((now < 1.0) && (it < 100));
} // testNextState()

// ./wns-core -y "WNS.masterLogger.enabled = True"  -T wns::markovchain::MarkovDiscreteTimeTest::testCalculateStateProbabilities
void
MarkovDiscreteTimeTest::testCalculateStateProbabilities()
{
	int numberOfChains = 1;
	int maxNumberOfStates=10;
	int maxTrials=10;

	wns::distribution::StandardUniform* randomDist = 
        new wns::distribution::StandardUniform();
	for (int trial=1; trial<maxTrials; ++trial) {
	    for (int numberOfStates=2; numberOfStates<maxNumberOfStates; ++numberOfStates) {
			MarkovDiscreteTime<int> markovmodel(numberOfChains);
			markovmodel.setNumberOfStates(numberOfStates);
			TransitionMatrixType matrix(numberOfStates, numberOfStates);
			// fill in the matrix
			for(int x=0; x < numberOfStates; x++) {
				double rowSum = 0.0;
				for(int y=0; y < numberOfStates; y++) {
					double random = (*randomDist)();
					// random is in [0.0, 1.0]
					if (x == y) {
						matrix(x, y) = 10.0 * random; // diagonal >> off-diagonal
					} else {
						matrix(x, y) = random;
					}
					rowSum += matrix(x, y);
				} // for y
				// normalize to rowSum=1.0
				for(int y=0; y < numberOfStates; y++) {
					matrix(x, y) /= rowSum;
				}
			} // for x=row
			markovmodel.setTransitionMatrix(matrix);
			CPPUNIT_ASSERT( markovmodel.checkSum() == true );
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
				CPPUNIT_ASSERT_DOUBLES_EQUAL(stateProbabilities[x], result, 1e-6);
			}
            CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, sumP, 1e-6);
	    } // foreach numberOfStates
	} // foreach trial
} // testCalculateStateProbabilities

