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

#include <CONSTANZE/tests/GeneratorTest.hpp>

#include <CONSTANZE/GeneratorPP.hpp>
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

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    constanze::tests::BindingStub,
    constanze::Binding,
    "BindingStub",
    wns::PyConfigViewCreator);

namespace constanze { namespace tests {

	class GeneratorTest
		: public wns::TestFixture
	{
 		CPPUNIT_TEST_SUITE( GeneratorTest );
		CPPUNIT_TEST( testConstructorDestructor );
		CPPUNIT_TEST( testFreeRun );
		CPPUNIT_TEST( testStopTime );
		CPPUNIT_TEST( testPoisson );
		CPPUNIT_TEST( testABR );
		CPPUNIT_TEST_SUITE_END();
	public:
		GeneratorTest();
		~GeneratorTest();
                void prepare();
                void cleanup();
		void testConstructorDestructor();
		void testFreeRun();
		void testStopTime();
		void testPoisson();
		void testABR();
	private:
		wns::pyconfig::View makePyconfigString(std::string extension);
		wns::events::scheduler::Interface* scheduler;
		GeneratorPP* testGeneratorPP;
		constanze::tests::BindingStub* binding;
	};

} // tests
} // constanze

using namespace constanze;
using namespace constanze::tests;



BindingStub::BindingStub(const wns::pyconfig::View&) :
    SentDataCounter(0UL),
    initBindingCounter(0U),
    releaseBindingCounter(0U)
{
}

BindingStub::BindingStub() :
    SentDataCounter(0UL),
    initBindingCounter(0U),
    releaseBindingCounter(0U)
{
}

BindingStub::~BindingStub()
{
}

void BindingStub::registerComponent(wns::node::component::Component* /* _component */)
{
}

void BindingStub::sendData(const wns::osi::PDUPtr& /* _data */)
{
	SentDataCounter++;
}

// called in StartTrigger::onTimeout():
void BindingStub::initBinding(constanze::StartTrigger* _startTrigger)
{
	_startTrigger->bindingReady();
	// calls generator->bindingReady();
	// which calls generator->start();
	++initBindingCounter;
}

// called in StopTrigger::onTimeout()
void BindingStub::releaseBinding(constanze::StopTrigger* _stopTrigger)
{
	_stopTrigger->bindingReady();
	// calls generator->bindingReleased();
	// which calls generator->stop();
	++releaseBindingCounter;
}

void BindingStub::registerListener(constanze::Listener*)
{
	// intentionally left empty
}

std::string BindingStub::printAddress() const
{
	return "I do not have an address";
}

/********************************* GeneratorTest *****************************/

CPPUNIT_TEST_SUITE_REGISTRATION( GeneratorTest );

GeneratorTest::GeneratorTest() :
        wns::TestFixture(),
	scheduler(NULL),
	testGeneratorPP(NULL),
	binding(NULL)
{
}

GeneratorTest::~GeneratorTest()
{
}

wns::pyconfig::View
GeneratorTest::makePyconfigString(std::string extension)
{
    wns::pyconfig::Parser pyconfig;
    pyconfig.loadString(
	    "import openwns.distribution\n"
        "import constanze.traffic\n"
        "import constanze.node\n"
        "import constanze.distribution.CDFTables\n"

	    "\n"
	    + extension +
	    "\n"
	    );
    return pyconfig;
}

void GeneratorTest::prepare()
{
	// for Doxygen:
	// begin example "constanze::Generator::method.example"
	// do this_and_that
	// end example
	scheduler = wns::simulator::getEventScheduler();
	scheduler->reset();
	wns::pyconfig::View pyconfig = makePyconfigString(
		"traffic = constanze.traffic.CBR(0.0, 25600.0, 256.0, duration=1.0)"
		);
	wns::pyconfig::View generatorView = pyconfig.get("traffic");
	testGeneratorPP = new GeneratorPP(generatorView);
	binding = new constanze::tests::BindingStub();
} // prepare

void GeneratorTest::cleanup()
{
	delete testGeneratorPP; testGeneratorPP=NULL;
	delete binding; binding=NULL;
}

void GeneratorTest::testConstructorDestructor()
{
	CPPUNIT_ASSERT(testGeneratorPP != NULL);
	CPPUNIT_ASSERT(binding != NULL);
	CPPUNIT_ASSERT_EQUAL(0U,binding->initBindingCounter);
	CPPUNIT_ASSERT_EQUAL(0U,binding->releaseBindingCounter);
	//wns::pyconfig::Parser pyconfig;
	//pyconfig.loadString("\n" );
	//constanze::Binding* binding = new constanze::IpBinding(pyconfig);
	//binding->registerComponent(wns::node::component::Component*
	//_component);
	//initBinding(constanze::StartTrigger* startTrigger);
	//releaseBinding(constanze::StopTrigger* stopTrigger);
	//registerListener(constanze::Listener* _listener);
	//CPPUNIT_ASSERT(bool);
	//CPPUNIT_ASSERT_EQUAL(soll,ist);
	//CPPUNIT_ASSERT_DOUBLES_EQUAL(soll,ist,errorTol)
	//CPPUNIT_ASSERT_MESSAGE("msg",bool)
	//CPPUNIT_ASSERT_THROW(foo(-1), wns::Assure::Exception)
	//WNS_ASSERT_MAX_REL_ERROR(soll,ist, 1E-3);
}

// duration=0.0
void GeneratorTest::testFreeRun()
{
	CPPUNIT_ASSERT_EQUAL(0U,binding->initBindingCounter);
	CPPUNIT_ASSERT_EQUAL(0U,binding->releaseBindingCounter);
	delete testGeneratorPP; // we construct a new one with different pyconfig
	wns::pyconfig::View pyconfig = makePyconfigString(
	    "traffic = constanze.traffic.CBR(0.0, 25600.0, 256.0, duration=0.0)"
	    );
	wns::pyconfig::View generatorView = pyconfig.get("traffic");
	testGeneratorPP = new GeneratorPP(generatorView);
	testGeneratorPP->registerBinding(binding);
	//testGeneratorPP->bindingReady();
	CPPUNIT_ASSERT(0 == binding->SentDataCounter);
	CPPUNIT_ASSERT_EQUAL(0UL, testGeneratorPP->countedPackets());
	CPPUNIT_ASSERT_EQUAL(0UL, testGeneratorPP->countedBits());
	CPPUNIT_ASSERT_EQUAL(0U,binding->initBindingCounter);
	CPPUNIT_ASSERT_EQUAL(0U,binding->releaseBindingCounter);
	int it=0;
	simTimeType now;
	do {
		it++;
		CPPUNIT_ASSERT(scheduler->processOneEvent());
		now = scheduler->getTime();
		//std::cout << std::fixed << std::setprecision( 6 ); // Nachkommastellen
		//std::cout << "i="<<it<<": t="<<now<<std::endl;
	} while ((now < 1.0) && (it < 100));
	CPPUNIT_ASSERT_EQUAL(binding->SentDataCounter, testGeneratorPP->countedPackets());
	//testGeneratorPP->bindingReleased(); // duration=0.0 => failed assertion (stopTrigger!=NULL)
	WNS_ASSERT_ASSURE_EXCEPTION(testGeneratorPP->bindingReleased());
	CPPUNIT_ASSERT_EQUAL(1U,binding->initBindingCounter);
	CPPUNIT_ASSERT_EQUAL(0U,binding->releaseBindingCounter);
}

void GeneratorTest::testStopTime()
{
	delete testGeneratorPP; // we construct a new one with different pyconfig
	wns::pyconfig::View pyconfig = makePyconfigString(
	    "traffic = constanze.traffic.CBR(0.5, 25600.0, 256.0, duration=1.0)"
	    );
	wns::pyconfig::View generatorView = pyconfig.get("traffic");
	testGeneratorPP = new GeneratorPP(generatorView);
	testGeneratorPP->registerBinding(binding);
	int it=0;
	simTimeType now;
	CPPUNIT_ASSERT(scheduler->processOneEvent());
	now = scheduler->getTime(); // correct start time (= first event):
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5, now, 1e-6);
	do {
		it++;
		CPPUNIT_ASSERT(scheduler->processOneEvent());
		now = scheduler->getTime();
	} while ((now < 1.5) && (it < 200));
	//std::cout << "duration exceeded" << std::endl;
	CPPUNIT_ASSERT(!scheduler->processOneEvent()); // No more events in queue
	CPPUNIT_ASSERT_EQUAL(binding->SentDataCounter, testGeneratorPP->countedPackets());
	//testGeneratorPP->bindingReleased(); // done automatically after stopTime
	CPPUNIT_ASSERT_EQUAL(1U,binding->initBindingCounter);
	CPPUNIT_ASSERT_EQUAL(1U,binding->releaseBindingCounter);
}

void GeneratorTest::testPoisson()
{
	delete testGeneratorPP; // we construct a new one with different pyconfig
	wns::pyconfig::View pyconfig = makePyconfigString(
	    "traffic = constanze.traffic.Poisson(0.5, 25600.0, 256.0, duration=1.0)"
	    );
	wns::pyconfig::View generatorView = pyconfig.get("traffic");
	testGeneratorPP = new GeneratorPP(generatorView);
	testGeneratorPP->registerBinding(binding);
	int it=0;
	simTimeType now;
	do {
		it++;
		CPPUNIT_ASSERT(scheduler->processOneEvent());
		now = scheduler->getTime();
	} while ((now < 1.5) && (it < 200));
	//std::cout << "duration exceeded" << std::endl;
	CPPUNIT_ASSERT(!scheduler->processOneEvent()); // No more events in queue
	CPPUNIT_ASSERT_EQUAL(binding->SentDataCounter, testGeneratorPP->countedPackets());
}

void GeneratorTest::testABR()
{
	delete testGeneratorPP; // we construct a new one with different pyconfig
	wns::pyconfig::View pyconfig = makePyconfigString(
	    "iatDistribution = openwns.distribution.NegExp(0.1)\n"
            "packetSizeDistribution = constanze.distribution.CDFTables.IPPacketSizeDataTraffic()\n"
	    "traffic = constanze.traffic.ABR(iatDistribution, packetSizeDistribution, offset = 0.5, duration = 1.0)"
	    );
	wns::pyconfig::View generatorView = pyconfig.get("traffic");
	testGeneratorPP = new GeneratorPP(generatorView);
	testGeneratorPP->registerBinding(binding);
	int it=0;
	simTimeType now;
	do {
		it++;
		CPPUNIT_ASSERT(scheduler->processOneEvent());
		now = scheduler->getTime();
	} while ((now < 1.5) && (it < 200));
	//std::cout << "duration exceeded" << std::endl;
	CPPUNIT_ASSERT(!scheduler->processOneEvent()); // No more events in queue
	CPPUNIT_ASSERT_EQUAL(binding->SentDataCounter, testGeneratorPP->countedPackets());
}


