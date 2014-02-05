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
// cd ~/src/beast/WNS--main--3.0/tests/unit/unitTests
// ./wns-core -T "constanze::tests::MeasurementTest"

#include <CONSTANZE/Measurement.hpp>
#include <CONSTANZE/HMM.hpp>

#include <WNS/pyconfig/Parser.hpp>
#include <WNS/pyconfig/helper/Functions.hpp>
#include <WNS/node/tests/Stub.hpp>
#include <WNS/CppUnit.hpp>
#include <cppunit/extensions/HelperMacros.h>

#include <WNS/distribution/Uniform.hpp>

#include <iostream>
#include <iomanip>

// unix system performance measurements:
#include <sys/time.h>
#include <sys/resource.h>

namespace constanze { namespace tests {

	class MeasurementTest
		: public wns::TestFixture
	{
 		CPPUNIT_TEST_SUITE( MeasurementTest );
		CPPUNIT_TEST( testConstructorDestructor );
		CPPUNIT_TEST( testMeasurement );
		CPPUNIT_TEST_SUITE_END();
	public:
		MeasurementTest();
		~MeasurementTest();
                void prepare();
                void cleanup();
		void testConstructorDestructor();
		void testMeasurement();
		bool useCout;
	private:
		wns::pyconfig::View makePyconfigString();
                //int estimatorNumberOfStates;
		//std::vector<double> *meanRateVector;
		//std::vector<double> *boundaryVector;
		//std::string MMPPestimationResultFileName;
		std::vector<int> *observationVector;
	        Measurement* measurement;
	};

} // tests
} // constanze

using namespace constanze;
using namespace constanze::tests;

/********************************* MeasurementTest *****************************/

CPPUNIT_TEST_SUITE_REGISTRATION( MeasurementTest );

MeasurementTest::MeasurementTest() :
        wns::TestFixture(),
	useCout(true),
	observationVector(NULL),
	measurement(NULL)
{
}

MeasurementTest::~MeasurementTest()
{
	if(measurement) delete measurement;
	if(observationVector) delete observationVector;
}

wns::pyconfig::View
MeasurementTest::makePyconfigString()
{
    wns::pyconfig::Parser pyconfig;
    pyconfig.loadString(
	    "import openwns.logger\n"
	    "import constanze.node\n"
	    "MMPPestimationResultFileName=\"output/test\"\n"
	    "estimatorNumberOfStates=5\n"
	    "meanRateList=[10000,30000,50000,70000,90000]\n"
	    "logger = constanze.node.Logger(\"MeasurementTest\",True,None)\n"
	    "probeWindow = 0.1\n"
	    "measurement = constanze.node.Measurement(\n"
	    "   MMPPestimationResultFileName=MMPPestimationResultFileName,\n"
	    "   estimatorNumberOfStates=5,\n"
	    "   meanRateList=meanRateList,\n"
	    "   probeWindow=probeWindow,\n"
	    "   logger=logger)\n"
	    "\n"
	    "\n"
	    );
    return pyconfig;
}

void MeasurementTest::prepare()
{
	wns::pyconfig::View config = makePyconfigString();

	measurement = new Measurement(config);
	observationVector = new std::vector<int>();
}

void MeasurementTest::cleanup()
{
	delete measurement; measurement=NULL;
	delete observationVector; observationVector = NULL;
}

void MeasurementTest::testConstructorDestructor()
{
	CPPUNIT_ASSERT(measurement != NULL);
}

void MeasurementTest::testMeasurement()
{
	wns::distribution::StandardUniform rng;

	unsigned int maxNumberInRandom = 500;

	for(unsigned long int i = 0; i < maxNumberInRandom; i++){
		bool notProperStateLevel = true;
		int stateLevel = 0;

		double randomValue = rng();
		// 100000 is variable value, only for test here
		double bitRate = randomValue * 100000;

		for(unsigned int j = 0; notProperStateLevel == true && j < measurement->boundaryVector->size(); j++){
			if(bitRate <= measurement->boundaryVector->at(j)){
				stateLevel = j;
				notProperStateLevel = false;
			}
		}
		if(notProperStateLevel)
			stateLevel = measurement->boundaryVector->size();

		observationVector->push_back(stateLevel);
	}

	HMM* hmm = measurement->calculate(measurement->getEstimatorNumberOfStates(),observationVector);

	if(useCout){
		//std::cout << "transition matrix after use of baum-welch:" << std::endl;
		measurement->output_python(hmm->getTransitionsMatrix(),hmm->getNumberOfStates(),measurement->getMMPPestimationResultFileName());
		//CPPUNIT_ASSERT(); // make tests
	}

	delete hmm;
}



