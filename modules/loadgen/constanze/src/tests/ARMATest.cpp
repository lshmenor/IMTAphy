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
// ./wns-core -T "constanze::tests::ARMATest"

#include <CONSTANZE/tests/GeneratorTest.hpp>

#include <CONSTANZE/GeneratorMMPP.hpp>
#include <CONSTANZE/Generator.hpp>
#include <CONSTANZE/SubGenerator.hpp>
#include <CONSTANZE/Binding.hpp>

#include <WNS/pyconfig/Parser.hpp>
#include <WNS/pyconfig/helper/Functions.hpp>
#include <WNS/node/tests/Stub.hpp>
#include <WNS/CppUnit.hpp>
#include <cppunit/extensions/HelperMacros.h>

#include <iostream>
#include <iomanip>


namespace constanze { namespace tests {

	class ARMATest
		: public wns::TestFixture
	{
 		CPPUNIT_TEST_SUITE( ARMATest );
		CPPUNIT_TEST( testConstructorDestructor );
		CPPUNIT_TEST( testFreeRun );
		CPPUNIT_TEST( testStopTime );
		CPPUNIT_TEST( testMMPP );
		CPPUNIT_TEST( testMPEG2 );
		CPPUNIT_TEST( testData );
		CPPUNIT_TEST( testTargetRate );
		CPPUNIT_TEST_SUITE_END();
	public:
		ARMATest();
		~ARMATest();
                void prepare();
                void cleanup();
		void testConstructorDestructor();
		void testFreeRun();
		void testStopTime();
		void testMMPP();
		void testMPEG2();
		void testData();
		void testTargetRate();
		bool useCout;
	private:
		wns::pyconfig::View makePyconfigString(std::string extension);
		wns::events::scheduler::Interface* scheduler;
		GeneratorMMPP* testGeneratorMMPP;
		constanze::tests::BindingStub* binding;
	};

} // tests
} // constanze

using namespace constanze;
using namespace constanze::tests;

/********************************* MMPPTest *****************************/

CPPUNIT_TEST_SUITE_REGISTRATION( ARMATest );

ARMATest::ARMATest() :
        wns::TestFixture(),
	useCout(false),
	testGeneratorMMPP(NULL),
	binding(NULL)
{
}

ARMATest::~ARMATest()
{
	//if (scheduler) delete scheduler; // scheduler is a pointer to a global instance
	if (testGeneratorMMPP) delete testGeneratorMMPP;
	if (binding) delete binding;
}

wns::pyconfig::View
ARMATest::makePyconfigString(std::string extension)
{
    wns::pyconfig::Parser pyconfig;
    pyconfig.loadString(
	    "import openwns.distribution\n"
	    "import constanze.traffic\n"
	    "import constanze.node\n"
	    "\n"
	    + extension +
	    "\n"
	    );
    return pyconfig;
}

void ARMATest::prepare()
{
	// for Doxygen:
	// begin example "constanze::Generator::method2.example"
	// do this and that
	// end example
	scheduler = wns::simulator::getEventScheduler();
	scheduler->reset();
	wns::pyconfig::View pyconfig = makePyconfigString(
		"mmppParams = constanze.traffic.MMPPexampleONOFF()\n"
		"traffic = constanze.traffic.MMPP(mmppParams, rateScale=1.0, transitionScale=1.0, offset = 0.0, duration=1.0)"
		);
	wns::pyconfig::View generatorView = pyconfig.get("traffic");
	testGeneratorMMPP = new GeneratorMMPP(generatorView);
	binding = new constanze::tests::BindingStub();
} // prepare

void ARMATest::cleanup()
{
	delete testGeneratorMMPP; testGeneratorMMPP=NULL;
	delete binding; binding=NULL;
}

void ARMATest::testConstructorDestructor()
{
	CPPUNIT_ASSERT(testGeneratorMMPP != NULL);
	CPPUNIT_ASSERT(binding != NULL);
	CPPUNIT_ASSERT_EQUAL(0U,binding->initBindingCounter);
	CPPUNIT_ASSERT_EQUAL(0U,binding->releaseBindingCounter);
	//CPPUNIT_ASSERT(bool);
	//CPPUNIT_ASSERT_EQUAL(soll,ist);
	//CPPUNIT_ASSERT_DOUBLES_EQUAL(soll,ist,errorTol)
	//CPPUNIT_ASSERT_MESSAGE("msg",bool)
	//CPPUNIT_ASSERT_THROW(foo(-1), wns::Assure::Exception)
	//WNS_ASSERT_MAX_REL_ERROR(soll,ist, 1E-3);
}

// duration=0.0
void ARMATest::testFreeRun()
{
	delete testGeneratorMMPP; // we construct a new one with different pyconfig
	wns::pyconfig::View pyconfig = makePyconfigString(
	    "mmppParams = constanze.traffic.MMPPexampleONOFF()\n"
            "traffic = constanze.traffic.MMPP(mmppParams, rateScale=0.1, transitionScale=1.0, offset = 0.0, duration=0.0)"
	    );
	wns::pyconfig::View generatorView = pyconfig.get("traffic");
	testGeneratorMMPP = new GeneratorMMPP(generatorView);
	testGeneratorMMPP->registerBinding(binding);
	//testGeneratorMMPP->bindingReady();
	CPPUNIT_ASSERT(0 == binding->SentDataCounter);
	CPPUNIT_ASSERT_EQUAL(0UL, testGeneratorMMPP->countedPackets());
	CPPUNIT_ASSERT_EQUAL(0UL, testGeneratorMMPP->countedBits());
	unsigned int it=0;
	simTimeType now;
	do {
		it++;
		CPPUNIT_ASSERT(scheduler->processOneEvent());
		now = scheduler->getTime();
	} while ((now < 30.0) && (it < 30000)); // limited by it<3000 here
	if (useCout)
		std::cout << "loop end: now="<<now<<"s, it="<<it<< std::endl;
	double trafficDuration = now - 0.0;
	double minTrafficRate  = testGeneratorMMPP->getMinRate();
	double meanTrafficRate = testGeneratorMMPP->getMeanRate();
	double maxTrafficRate  = testGeneratorMMPP->getMaxRate();
	if (useCout)
	    std::cout << "count=" << testGeneratorMMPP->countedBits()
		      << ", EXPECTED:"
		      << " min="  << static_cast<long int>(minTrafficRate  * trafficDuration)
		      << " mean=" << static_cast<long int>(meanTrafficRate * trafficDuration)
		      << " max="  << static_cast<long int>(maxTrafficRate  * trafficDuration)
		      << " relErr=" << (static_cast<double>(testGeneratorMMPP->countedBits())-meanTrafficRate*trafficDuration) / (meanTrafficRate*trafficDuration)
		      << std::endl;
	// around 90 packets sent (100 events - ~10 state changes):
	CPPUNIT_ASSERT_EQUAL(binding->SentDataCounter, testGeneratorMMPP->countedPackets());
	//testGeneratorMMPP->bindingReleased(); // duration=0.0 => failed assertion (stopTrigger!=NULL)
	WNS_ASSERT_ASSURE_EXCEPTION(testGeneratorMMPP->bindingReleased());
	CPPUNIT_ASSERT(testGeneratorMMPP->countedPackets() > 80U);
	CPPUNIT_ASSERT(testGeneratorMMPP->countedPackets() < it);
	CPPUNIT_ASSERT(testGeneratorMMPP->countedBits() > minTrafficRate  * trafficDuration);
	CPPUNIT_ASSERT(testGeneratorMMPP->countedBits() < maxTrafficRate  * trafficDuration);
	// very few events, check needs tolerance:
	WNS_ASSERT_MAX_REL_ERROR(meanTrafficRate * trafficDuration, static_cast<double>(testGeneratorMMPP->countedBits()), 0.1);
}

void ARMATest::testStopTime()
{
	delete testGeneratorMMPP; // we construct a new one with different pyconfig
	wns::pyconfig::View pyconfig = makePyconfigString(
                "mmppParams = constanze.traffic.MMPPexampleONOFF()\n"
                "traffic = constanze.traffic.MMPP(mmppParams, rateScale=0.1, transitionScale=1.0, offset = 0.5, duration=1.0)\n"
	    );
	wns::pyconfig::View generatorView = pyconfig.get("traffic");
	testGeneratorMMPP = new GeneratorMMPP(generatorView);
	testGeneratorMMPP->registerBinding(binding);
	unsigned int it=0;
	simTimeType now;
	CPPUNIT_ASSERT(scheduler->processOneEvent());
	now = scheduler->getTime(); // correct start time (= first event):
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5, now, 1e-6);
	do {
		it++;
		CPPUNIT_ASSERT(scheduler->processOneEvent());
		now = scheduler->getTime();
	} while ((now < 1.5) && (it < 1000)); // limited by time here
	if (useCout)
	    std::cout << "loop end: now="<<now<<"s, it="<<it<< std::endl;
	double trafficDuration = now - 0.5;
	double minTrafficRate  = testGeneratorMMPP->getMinRate();
	double meanTrafficRate = testGeneratorMMPP->getMeanRate();
	double maxTrafficRate  = testGeneratorMMPP->getMaxRate();
	if (useCout)
	    std::cout << "count=" << testGeneratorMMPP->countedBits()
		      << ", EXPECTED:"
		      << " min="  << static_cast<long int>(minTrafficRate  * trafficDuration)
		      << " mean=" << static_cast<long int>(meanTrafficRate * trafficDuration)
		      << " max="  << static_cast<long int>(maxTrafficRate  * trafficDuration)
		      << " relErr=" << (static_cast<double>(testGeneratorMMPP->countedBits())-meanTrafficRate*trafficDuration) / (meanTrafficRate*trafficDuration)
		      << std::endl;
	CPPUNIT_ASSERT(!scheduler->processOneEvent()); // No more events in queue
	// around 540 packets sent (~637 events):
	CPPUNIT_ASSERT_EQUAL(binding->SentDataCounter, testGeneratorMMPP->countedPackets());
	CPPUNIT_ASSERT(testGeneratorMMPP->countedPackets() > 390);
	CPPUNIT_ASSERT(testGeneratorMMPP->countedPackets() < it);
	CPPUNIT_ASSERT(testGeneratorMMPP->countedBits() > minTrafficRate  * trafficDuration);
	CPPUNIT_ASSERT(testGeneratorMMPP->countedBits() < maxTrafficRate  * trafficDuration);
	// traffic is bursty and only few events, so big tolerance needed:
	// (this test should only test stopTime, so very few events occur.
	// WNS_ASSERT_MAX_REL_ERROR(meanTrafficRate * trafficDuration, static_cast<double>(testGeneratorMMPP->countedBits()), 0.5);
	//testGeneratorMMPP->bindingReleased(); // done automatically after stopTime
}

void ARMATest::testMMPP()
{
	delete testGeneratorMMPP; // we construct a new one with different pyconfig
	// Markov model with 5 states:
	wns::pyconfig::View pyconfig = makePyconfigString(
		"mmppParams = constanze.traffic.MMPPexample01()\n"
		"traffic = constanze.traffic.MMPP(mmppParams, rateScale=1.0, transitionScale=1.0, offset = 0.5, duration=10.0)\n"
	    );
	wns::pyconfig::View generatorView = pyconfig.get("traffic");
	testGeneratorMMPP = new GeneratorMMPP(generatorView);
	testGeneratorMMPP->registerBinding(binding);
	unsigned int it=0;
	simTimeType now;
	do {
		it++;
		CPPUNIT_ASSERT(scheduler->processOneEvent());
		now = scheduler->getTime();
	} while ((now < 5.5) && (it < 10000)); // limited by it<10000 here, at t=?
	if (useCout)
	    std::cout << "loop end: now="<<now<<"s, it="<<it<< std::endl;
	double trafficDuration = now - 0.5;
	double minTrafficRate  = testGeneratorMMPP->getMinRate();
	double meanTrafficRate = testGeneratorMMPP->getMeanRate();
	double maxTrafficRate  = testGeneratorMMPP->getMaxRate();
	if (useCout)
	    std::cout << "count=" << testGeneratorMMPP->countedBits()
		      << ", EXPECTED:"
		      << " min="  << static_cast<long int>(minTrafficRate  * trafficDuration)
		      << " mean=" << static_cast<long int>(meanTrafficRate * trafficDuration)
		      << " max="  << static_cast<long int>(maxTrafficRate  * trafficDuration)
		      << " relErr=" << (static_cast<double>(testGeneratorMMPP->countedBits())-meanTrafficRate*trafficDuration) / (meanTrafficRate*trafficDuration)
		      << std::endl;
	CPPUNIT_ASSERT(scheduler->processOneEvent()); // still events in queue
	// around 4826 packets sent (5000 events):
	CPPUNIT_ASSERT_EQUAL(binding->SentDataCounter, testGeneratorMMPP->countedPackets());
	CPPUNIT_ASSERT(testGeneratorMMPP->countedPackets() > 4000);
	CPPUNIT_ASSERT(testGeneratorMMPP->countedPackets() < it);
	CPPUNIT_ASSERT(testGeneratorMMPP->countedBits() > minTrafficRate  * trafficDuration);
	CPPUNIT_ASSERT(testGeneratorMMPP->countedBits() < maxTrafficRate  * trafficDuration);
	// traffic is very bursty and only few events, so 20% tolerance needed:
	WNS_ASSERT_MAX_REL_ERROR(meanTrafficRate * trafficDuration, static_cast<double>(testGeneratorMMPP->countedBits()), 0.2);
}

void ARMATest::testMPEG2()
{
	delete testGeneratorMMPP; // we construct a new one with different pyconfig
	// Markov model with 5 states, deterministic transition every 1/30 s:
	wns::pyconfig::View pyconfig = makePyconfigString(
                "mmppParams = constanze.traffic.MMPPMPEG2()\n"
                "traffic = constanze.traffic.MMPP(mmppParams, rateScale=0.1, transitionScale=1.0, offset = 0.5, duration=20.0)\n"
	    );
	wns::pyconfig::View generatorView = pyconfig.get("traffic");
	testGeneratorMMPP = new GeneratorMMPP(generatorView);
	testGeneratorMMPP->registerBinding(binding);
	unsigned int it=0;
	simTimeType now;
	do {
		it++;
		CPPUNIT_ASSERT(scheduler->processOneEvent());
		now = scheduler->getTime();
	} while ((now < 5.5) && (it < 5000)); // limited by t<5.5 here, at it=4518
	if (useCout)
	    std::cout << "loop end: now="<<now<<"s, it="<<it<< std::endl;
	double trafficDuration = now - 0.5;
	double minTrafficRate  = testGeneratorMMPP->getMinRate();
	double meanTrafficRate = testGeneratorMMPP->getMeanRate();
	double maxTrafficRate  = testGeneratorMMPP->getMaxRate();
	if (useCout)
	    std::cout << "count=" << testGeneratorMMPP->countedBits()
		      << ", EXPECTED:"
		      << " min="  << static_cast<long int>(minTrafficRate  * trafficDuration)
		      << " mean=" << static_cast<long int>(meanTrafficRate * trafficDuration)
		      << " max="  << static_cast<long int>(maxTrafficRate  * trafficDuration)
		      << " relErr=" << (static_cast<double>(testGeneratorMMPP->countedBits())-meanTrafficRate*trafficDuration) / (meanTrafficRate*trafficDuration)
		      << std::endl;
	CPPUNIT_ASSERT(scheduler->processOneEvent()); // still events in queue
	CPPUNIT_ASSERT_EQUAL(binding->SentDataCounter, testGeneratorMMPP->countedPackets());
	CPPUNIT_ASSERT(testGeneratorMMPP->countedPackets() > 900);
	CPPUNIT_ASSERT(testGeneratorMMPP->countedPackets() < it);
	CPPUNIT_ASSERT(testGeneratorMMPP->countedBits() > minTrafficRate  * trafficDuration);
	CPPUNIT_ASSERT(testGeneratorMMPP->countedBits() < maxTrafficRate  * trafficDuration);
	// traffic is very bursty, so 10% tolerance needed:
	WNS_ASSERT_MAX_REL_ERROR(meanTrafficRate * trafficDuration, static_cast<double>(testGeneratorMMPP->countedBits()), 0.1);
}

void ARMATest::testData()
{
	delete testGeneratorMMPP; // we construct a new one with different pyconfig
	// C=10, Pareto transition distribution here
	wns::pyconfig::View pyconfig = makePyconfigString(
                "mmppParams = constanze.traffic.MMPPdata()\n"
                "traffic = constanze.traffic.MMPP(mmppParams, rateScale=0.01, transitionScale=1.0, offset = 0.5, duration=10.0)\n"
	    );
	wns::pyconfig::View generatorView = pyconfig.get("traffic");
	testGeneratorMMPP = new GeneratorMMPP(generatorView);
	testGeneratorMMPP->registerBinding(binding);
	unsigned int it=0;
	simTimeType now;
	do {
		it++;
		CPPUNIT_ASSERT(scheduler->processOneEvent());
		now = scheduler->getTime();
	} while ((now < 5.5) && (it < 5000)); // limited by t<2.5 here, at it=828
	if (useCout)
	    std::cout << "loop end: now="<<now<<"s, it="<<it<< std::endl;
	double trafficDuration = now - 0.5;
	double minTrafficRate  = testGeneratorMMPP->getMinRate();
	double meanTrafficRate = testGeneratorMMPP->getMeanRate();
	double maxTrafficRate  = testGeneratorMMPP->getMaxRate();
	if (useCout)
	    std::cout << "count=" << testGeneratorMMPP->countedBits()
		      << ", EXPECTED:"
		      << " min="  << static_cast<long int>(minTrafficRate  * trafficDuration)
		      << " mean=" << static_cast<long int>(meanTrafficRate * trafficDuration)
		      << " max="  << static_cast<long int>(maxTrafficRate  * trafficDuration)
		      << " relErr=" << (static_cast<double>(testGeneratorMMPP->countedBits())-meanTrafficRate*trafficDuration) / (meanTrafficRate*trafficDuration)
		      << std::endl;
	CPPUNIT_ASSERT(scheduler->processOneEvent()); // still events in queue
	// around 553 packets sent (828 events):
	CPPUNIT_ASSERT_EQUAL(binding->SentDataCounter, testGeneratorMMPP->countedPackets());
	CPPUNIT_ASSERT(testGeneratorMMPP->countedPackets() > 400);
	CPPUNIT_ASSERT(testGeneratorMMPP->countedPackets() < it);
	CPPUNIT_ASSERT(testGeneratorMMPP->countedBits() > minTrafficRate  * trafficDuration);
	CPPUNIT_ASSERT(testGeneratorMMPP->countedBits() < maxTrafficRate  * trafficDuration);
	// traffic is very bursty (and self-similar), so 70% tolerance needed:
	WNS_ASSERT_MAX_REL_ERROR(meanTrafficRate * trafficDuration, static_cast<double>(testGeneratorMMPP->countedBits()), 0.7);
}

void ARMATest::testTargetRate()
{
	delete testGeneratorMMPP; // we construct a new one with different pyconfig
	// Markov model with 5 states, deterministic transition every 1/30 s:
	wns::pyconfig::View pyconfig = makePyconfigString(
                "mmppParams = constanze.traffic.MMPPMPEG2()\n"
                "traffic = constanze.traffic.MMPP(mmppParams, targetRate=1e6, transitionScale=1.0, offset = 0.5, duration=20.0)\n"
	    );
	wns::pyconfig::View generatorView = pyconfig.get("traffic");
	testGeneratorMMPP = new GeneratorMMPP(generatorView);
	testGeneratorMMPP->registerBinding(binding);
	unsigned int it=0;
	simTimeType now;
	do {
		it++;
		CPPUNIT_ASSERT(scheduler->processOneEvent());
		now = scheduler->getTime();
	} while ((now < 5.5) && (it < 10000)); // limited by t < 5.5 here
	if (useCout)
	    std::cout << "loop end: now="<<now<<"s, it="<<it<< std::endl;
	double trafficDuration = now - 0.5;
	double minTrafficRate  = testGeneratorMMPP->getMinRate();
	double meanTrafficRate = testGeneratorMMPP->getMeanRate();
	double maxTrafficRate  = testGeneratorMMPP->getMaxRate();
	if (useCout)
	    std::cout << "count=" << testGeneratorMMPP->countedBits()
		      << ", EXPECTED:"
		      << " min="  << static_cast<long int>(minTrafficRate  * trafficDuration)
		      << " mean=" << static_cast<long int>(meanTrafficRate * trafficDuration)
		      << " max="  << static_cast<long int>(maxTrafficRate  * trafficDuration)
		      << " relErr=" << (static_cast<double>(testGeneratorMMPP->countedBits())-meanTrafficRate*trafficDuration) / (meanTrafficRate*trafficDuration)
		      << std::endl;
	CPPUNIT_ASSERT(scheduler->processOneEvent()); // still events in queue
	// around ? packets sent (~? events):
	CPPUNIT_ASSERT_EQUAL(binding->SentDataCounter, testGeneratorMMPP->countedPackets());
	CPPUNIT_ASSERT(testGeneratorMMPP->countedPackets() > 4000);
	CPPUNIT_ASSERT(testGeneratorMMPP->countedPackets() < it);
	CPPUNIT_ASSERT(testGeneratorMMPP->countedBits() > minTrafficRate  * trafficDuration);
	CPPUNIT_ASSERT(testGeneratorMMPP->countedBits() < maxTrafficRate  * trafficDuration);
	// duration=multiple of MPEG2-frame => 10% tolerance is ok:
	WNS_ASSERT_MAX_REL_ERROR(meanTrafficRate * trafficDuration, static_cast<double>(testGeneratorMMPP->countedBits()), 0.10);
	//sleep(30);
}


