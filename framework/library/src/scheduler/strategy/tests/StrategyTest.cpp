/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2009
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
// ./wns-core -T "wns::scheduler::strategy::tests::StrategyTest"

//#include <WNS/scheduler/strategy/tests/StrategyTest.hpp>

//#include <cppunit/extensions/HelperMacros.h>

#include <WNS/scheduler/strategy/Strategy.hpp>
#include <WNS/scheduler/grouper/SpatialGrouper.hpp>
#include <WNS/scheduler/queue/QueueInterface.hpp>
#include <WNS/scheduler/CallBackInterface.hpp>
#include <WNS/scheduler/MapInfoEntry.hpp>
#include <WNS/scheduler/queue/SimpleQueue.hpp>
#include <WNS/scheduler/strategy/tests/ResultsContainer.hpp>
#include <WNS/scheduler/tests/RegistryProxyStub.hpp>

#include <WNS/pyconfig/Parser.hpp>
#include <WNS/StaticFactory.hpp>
#include <WNS/ldk/helper/FakePDU.hpp>
#include <WNS/ldk/Compound.hpp>
//#include <WNS/ldk/tools/Stub.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/node/tests/Stub.hpp>
#include <WNS/CppUnit.hpp>
#include <cppunit/extensions/HelperMacros.h>

#include <string>
#include <set>
#include <iostream>

#include <WNS/scheduler/strategy/Strategy.hpp>
#include <WNS/scheduler/tests/RegistryProxyStub.hpp>
#include <WNS/scheduler/grouper/tests/GrouperStub.hpp>

#define FRAMEPLOTTING 1


namespace wns { namespace scheduler { namespace strategy { namespace tests {

	class StrategyTest :
		public wns::TestFixture
	{
		CPPUNIT_TEST_SUITE( StrategyTest );
		CPPUNIT_TEST( testConstructorDestructor );
		CPPUNIT_TEST( oneFrame );
		CPPUNIT_TEST( severalFrames );
		CPPUNIT_TEST( testEmptyQueues );
		CPPUNIT_TEST( testPriority );
		CPPUNIT_TEST_SUITE_END();
	public:
		StrategyTest();
		~StrategyTest();
		void prepare();
		void cleanup();

		// tests
		void testConstructorDestructor();
		void oneFrame();
		void severalFrames();
		void testEmptyQueues();
		void testPriority();

	private:
		void setupSubStrategy(std::string strateyName);
		void setupSubStrategyForPriority(std::string strateyName);
		void checkIfAllScheduled(wns::scheduler::strategy::StrategyResult strategyResult);
		bool pduBeenScheduled(wns::ldk::CompoundPtr, wns::scheduler::strategy::StrategyResult result);
		void createPDUsToFillOneFrame();

		bool useCout;
		wns::scheduler::grouper::GroupingProviderInterface* grouper;
		wns::scheduler::queue::QueueInterface* queue;
		wns::scheduler::strategy::StrategyInterface* strategy;
		wns::scheduler::tests::RegistryProxyStub* registry;
		wns::scheduler::strategy::tests::ResultsContainer* results;
		wns::ldk::fun::FUN* fuNet;

		std::set<wns::ldk::CompoundPtr> compoundsToSchedule;
		std::string substrategyNames[3];
		std::string strategyName;
	};

} // tests
} // strategy
} // scheduler
} // wns


using namespace wns::ldk;
using namespace wns::scheduler;
using namespace wns::scheduler::strategy::tests;

/********************************* Test *****************************/

//CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( StrategyTest, "StrategyTest");
// Mue: Disabled since these are the old startegies. This must be changed to test StaticPriority
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( StrategyTest, wns::testsuite::Disabled() );

StrategyTest::StrategyTest() : /* 1. */
	wns::TestFixture(),
	useCout(false),
	grouper(NULL),
	queue(NULL),
	strategy(NULL),
	registry(NULL),
	results(NULL),
	fuNet(NULL)
{
	if (useCout) std::cout << "StrategyTest()" << std::endl;
}

StrategyTest::~StrategyTest()
{
	if (useCout) std::cout << "~StrategyTest()" << std::endl;
}

void StrategyTest::prepare()
{
	if (useCout) std::cout << "StrategyTest::prepare()" << std::endl;

	strategyName = std::string("StaticPriority");

	substrategyNames[0] = std::string("ProportionalFair");
	substrategyNames[1] = std::string("ExhaustiveRoundRobin");
	substrategyNames[2] = std::string("RoundRobin");

	fuNet = new wns::ldk::fun::Main(new wns::ldk::tests::LayerStub());

	wns::pyconfig::Parser emptyConfig;

	// create the queues as a real Scheduler FU would do
	wns::scheduler::queue::QueueCreator* queueCreator = wns::scheduler::queue::QueueFactory::creator("SimpleQueue");

	wns::pyconfig::Parser queueConfig;
	std::stringstream ss;
	ss << "from openwns.Scheduler import SimpleQueue\n"
	   << "queue = SimpleQueue()\n";
	queueConfig.loadString(ss.str());

	queue = queueCreator->create(0, queueConfig.getView("queue"));
	assure(queue, "Queue creation failed");

	// create ResultsContainer to store scheduled compounds
	results = new ResultsContainer();

	// setup RegistryProxyStub
	registry = new wns::scheduler::tests::RegistryProxyStub();
	// set queueSizeLimit so that 40 PDUs of size 3000 Bits fit
	registry->setQueueSizeLimitPerConnection(125000);
	// set only 1 priority at first
	registry->setNumberOfPriorities(1);

	// setup GrouperStub
	grouper = new wns::scheduler::grouper::tests::GrouperStub();

	// setup colleagues
	queue->setColleagues(registry);
	grouper->setColleagues(registry);
	if (useCout) std::cout << "StrategyTest::prepare() ready" << std::endl;
}

void StrategyTest::testConstructorDestructor() /* 3. */
{
	if (useCout) std::cout << "StrategyTest::testConstructorDestructor()" << std::endl;

	CPPUNIT_ASSERT(fuNet != NULL);
	CPPUNIT_ASSERT(queue != NULL);
	CPPUNIT_ASSERT(results != NULL);
	CPPUNIT_ASSERT(registry != NULL);
	CPPUNIT_ASSERT(grouper != NULL);

}

void StrategyTest::cleanup()
{
	if (useCout) std::cout << "StrategyTest::cleanup()" << std::endl;
	delete queue;
	delete results;
	delete registry;
}

void
StrategyTest::setupSubStrategy(std::string substrategyName)
{
	if (useCout) std::cout << "StrategyTest::setupStrategy()" << std::endl;
	// create PyConfig for Strategy module creation
	wns::pyconfig::Parser pyCoParser;
	std::stringstream ss;
	ss << "import openwns.logger\n"
	   << "import openwns.scheduler.APCStrategy\n"
	   << "import openwns.scheduler.DSAStrategy\n"
	   << "import wns \n"
	   << "symbolDuration = 0.00001389\n"
	   << "txMode = True\n"
	   << "logger = openwns.logger.Logger(\"WNS\", \"Scheduling Strategy Test\", True)\n"
	   << "historyWeight = 0.9\n"
	   << "maxBursts = 100\n"
	   << "scalingBetweenMaxTPandPFair = 0.0\n" // 0.0=MaxThroughput; 1.0=ProportionalFair
	   << "allowReGrouping = False\n"
	   << "powerControlSlave = False\n"
	   << "excludeTooLowSINR = True\n"
	   << "dsastrategy = openwns.scheduler.DSAStrategy.LinearFFirst(oneUserOnOneSubChannel = True)\n"
	   << "dsafbstrategy = openwns.scheduler.DSAStrategy.LinearFFirst(oneUserOnOneSubChannel = True)\n"
	   << "apcstrategy = openwns.scheduler.APCStrategy.UseNominalTxPower()\n"
	   << "subStrategies = [wns.Scheduler."<<substrategyName<<"()] \n";
	pyCoParser.loadString(ss.str());

	// create Strategy module
	wns::scheduler::strategy::StrategyCreator* strategyCreator = wns::scheduler::strategy::StrategyFactory::creator(strategyName);
	strategy = strategyCreator->create(pyCoParser);
	assure(strategy, "Strategy module creation failed");

	// configure Strategy module
	strategy->setColleagues(queue, grouper, registry, NULL);
	strategy->getPowerCapabilities(wns::scheduler::UserID()); // trigger scheduler to ask RegistryProxy(Stub)
} // setupStrategy

void
StrategyTest::setupSubStrategyForPriority(std::string substrategyName)
{
	if (useCout) std::cout << "StrategyTest::setupStrategy()" << std::endl;
	// create PyConfig for Strategy module creation
	wns::pyconfig::Parser pyCoParser;
	std::stringstream ss;
	ss << "import openwns.logger\n"
	   << "import openwns.scheduler.APCStrategy\n"
	   << "import openwns.scheduler.DSAStrategy\n"
	   << "import wns \n"
	   << "symbolDuration = 0.00001389\n"
	   << "txMode = True\n"
	   << "logger = openwns.logger.Logger(\"WNS\", \"Scheduling Strategy Test\", True)\n"
	   << "historyWeight = 0.9\n"
	   << "maxBursts = 100\n"
	   << "scalingBetweenMaxTPandPFair = 0.0\n" // 0.0=MaxThroughput; 1.0=ProportionalFair
	   << "allowReGrouping = False\n"
	   << "powerControlSlave = False\n"
	   << "excludeTooLowSINR = True\n"
	   << "dsastrategy = openwns.scheduler.DSAStrategy.LinearFFirst(oneUserOnOneSubChannel = True)\n"
	   << "dsafbstrategy = openwns.scheduler.DSAStrategy.LinearFFirst(oneUserOnOneSubChannel = True)\n"
	   << "apcstrategy = openwns.scheduler.APCStrategy.UseNominalTxPower()\n"
	   << "subStrategies = [wns.Scheduler."<<substrategyName<<"(),wns.Scheduler."<<substrategyName<<"()] \n";
	pyCoParser.loadString(ss.str());

	// create Strategy module
	wns::scheduler::strategy::StrategyCreator* strategyCreator = wns::scheduler::strategy::StrategyFactory::creator(strategyName);
	strategy = strategyCreator->create(pyCoParser);
	assure(strategy, "Strategy module creation failed");

	// configure Strategy module
	strategy->setColleagues(queue, grouper, registry, NULL);
	strategy->getPowerCapabilities(wns::scheduler::UserID()); // trigger scheduler to ask RegistryProxy(Stub)
} // setupStrategy

void StrategyTest::createPDUsToFillOneFrame()
{
	if (useCout) std::cout << "StrategyTest::createPDUsToFillOneFrame()" << std::endl;
	for (int cid = 1; cid < 13; ++cid)
	{
		// create a dummy user ID
		UserID newUser = wns::scheduler::UserID();
		registry->associateCIDandUser(ConnectionID(cid), newUser);

		for (int i = 0; i < 33; ++i)
		{
			// create a dummy PDU of size 3000 bits and log it
			helper::FakePDUPtr inner(new  helper::FakePDU(3000));
			CompoundPtr compound(fuNet->createCompound(inner));
			compoundsToSchedule.insert(compound);

			// tell the RegistryProxyStub which CID belongs to it
			registry->setCIDforPDU(compound, ConnectionID(cid));

			// feed it to the queue
			assure(queue->isAccepting(compound), "Queue is not accepting compound");
			queue->put(compound);
		}
	}
}

void StrategyTest::checkIfAllScheduled(wns::scheduler::strategy::StrategyResult strategyResult)
{
	if (useCout) std::cout << "StrategyTest::checkIfAllScheduled()" << std::endl;
	// check if every pdu created has actually been scheduled
	for (std::set<CompoundPtr>::const_iterator iter = compoundsToSchedule.begin();
		 iter != compoundsToSchedule.end(); ++iter)
	{
		CPPUNIT_ASSERT(pduBeenScheduled(*iter, strategyResult));
	}

}

bool StrategyTest::pduBeenScheduled(wns::ldk::CompoundPtr pdu , StrategyResult strategyResult)
{
	wns::scheduler::SchedulingMapPtr schedulingMap = strategyResult.schedulingMap;
	// search in each subchannel
	for (int fChannel = 0; fChannel < 15; ++fChannel)
	{
		// search in each time slot
		for (int timeSlot = 0; timeSlot < 1; ++timeSlot)
		{
			// search in each spatial layer
			for (int spatialIndex = 0; spatialIndex < 1; ++spatialIndex)
			{
				// search in scheduled compounds
				for(wns::scheduler::ScheduledCompoundsList::const_iterator iterComps = schedulingMap->subChannels[fChannel].temporalResources[timeSlot]->physicalResources[spatialIndex].scheduledCompoundsBegin(); iterComps != schedulingMap->subChannels[fChannel].temporalResources[timeSlot]->physicalResources[spatialIndex].scheduledCompoundsEnd(); ++iterComps)
				{
					if(iterComps->compoundPtr->getBirthmark() == pdu->getBirthmark())
					{
						// find pdu which was scheduled
						return true;
					}
				}
			}
		}
	}
	return false;
}


void StrategyTest::oneFrame()
{
	if (useCout) std::cout << "StrategyTest::oneFrame()" << std::endl;
	// loop over all available strategyNames
	for (int i = 0; i < 3; ++i)
	{
		std::string substrategyName = substrategyNames[i];

		results->reset(); //deletes scheduled PDUs from previous runs
		queue->resetAllQueues();

		setupSubStrategy(substrategyName);

		// create some PDUs to schedule, log them, and feed them to the queue
		compoundsToSchedule.clear();
		createPDUsToFillOneFrame();


		// ****************************************************
		// now to the actual scheduling:

		int fChannels = 15;
		int maxSpatialLayers = 1;
		simTimeType slotLength = 0.005;
		int numberOfTimeSlots = 1;

		wns::scheduler::strategy::StrategyInput strategyInput(fChannels, slotLength, numberOfTimeSlots, maxSpatialLayers, results);

		if (FRAMEPLOTTING)
			results->plotNextFrame(fChannels,
					       maxSpatialLayers,
					       slotLength,
					       strategyName
				);

		// let the Strategy module do its work
		wns::scheduler::strategy::StrategyResult strategyResult = strategy->startScheduling(strategyInput);

		if (FRAMEPLOTTING)
			results->finishPlotting();

		checkIfAllScheduled(strategyResult);

		delete strategy;
	}
}

void StrategyTest::severalFrames()
{
	if (useCout) std::cout << "StrategyTest::severalFrames()" << std::endl;
        // loop over all available strategyNames
	for (int i = 0; i < 3; ++i)
	{
		std::string substrategyName = substrategyNames[i];

		results->reset(); //deletes scheduled PDUs from previous runs
		queue->resetAllQueues();

		setupSubStrategy(substrategyName);

		int fChannels = 15;
		int maxSpatialLayers = 1;
		simTimeType slotLength = 0.005;
		int numberOfTimeSlots = 1;

		// create pdus that should fill more or less five frames
		for (int j = 0; j < 5; ++j)
		{
			compoundsToSchedule.clear();
			createPDUsToFillOneFrame();

			std::stringstream ss;
			ss.clear();
			ss << substrategyName
			   << "-"
			   << j;


			if (FRAMEPLOTTING)
				results->plotNextFrame(fChannels,
						       maxSpatialLayers,
						       slotLength,
						       ss.str()
					);

			wns::scheduler::strategy::StrategyInput strategyInput(fChannels, slotLength, numberOfTimeSlots, maxSpatialLayers, results);

			// let the Strategy module do its work
			wns::scheduler::strategy::StrategyResult strategyResult = strategy->startScheduling(strategyInput);

			if (FRAMEPLOTTING)
				results->finishPlotting();

			checkIfAllScheduled(strategyResult);
		}

		delete strategy;
	}
}


void StrategyTest::testEmptyQueues()
{
	if (useCout) std::cout << "StrategyTest::testEmptyQueues()" << std::endl;
	// loop over all available strategyNames
	// we don't ASSERT anything here, we just check whether
	// assures/exceptions are thrown when the strategy is called with empty
	// queues and strange input parameters
	for (int i = 0; i < 3; ++i)
	{
		std::string substrategyName = substrategyNames[i];

		queue->resetAllQueues();

		setupSubStrategy(substrategyName);

		wns::scheduler::strategy::StrategyInput strategyInput1(15, 0.005, 1, 1, results);
		wns::scheduler::strategy::StrategyResult strategyResult = strategy->startScheduling(strategyInput1);

		wns::scheduler::strategy::StrategyInput strategyInput2(0, 0.005, 1, 1, results);
		CPPUNIT_ASSERT_THROW(strategyResult = strategy->startScheduling(strategyInput2), wns::Exception );

		wns::scheduler::strategy::StrategyInput strategyInput3(1, 0.005, 0, 0, results);
		CPPUNIT_ASSERT_THROW(strategyResult = strategy->startScheduling(strategyInput3), wns::Exception );

		wns::scheduler::strategy::StrategyInput strategyInput4(0, 0.005, 0, 0, results);
		CPPUNIT_ASSERT_THROW(strategyResult = strategy->startScheduling(strategyInput4), wns::Exception );

		wns::scheduler::strategy::StrategyInput strategyInput5(1, 0.0, 1, 1, results);
		CPPUNIT_ASSERT_THROW(strategyResult = strategy->startScheduling(strategyInput5), wns::Exception );

		delete strategy;
	}
}

void StrategyTest::testPriority()
{
	if (useCout) std::cout << "StrategyTest::testPriority()" << std::endl;

	//now test two priorities
	registry->setNumberOfPriorities(2);

	for (int i = 0; i < 3; ++i)
	{
		std::string substrategyName = substrategyNames[i];

		results->reset(); //deletes scheduled PDUs from previous runs
		queue->resetAllQueues();

		//two priorities need to set two substrategies
		setupSubStrategyForPriority(substrategyName);

		int fChannels = 15;
		int maxSpatialLayers = 1;
		simTimeType slotLength = 0.005;
		int numberOfTimeSlots = 1;

		// create pdus that should fill more or less five frames
		for (int j = 0; j < 5; ++j)
		{
			compoundsToSchedule.clear();
			createPDUsToFillOneFrame();

			std::stringstream ss;
			ss.clear();
			ss << substrategyName
			   << "-"
			   << j;

			if (FRAMEPLOTTING)
				results->plotNextFrame(fChannels,
						       maxSpatialLayers,
						       slotLength,
						       ss.str()
					);

			wns::scheduler::strategy::StrategyInput strategyInput(fChannels, slotLength, numberOfTimeSlots, maxSpatialLayers, results);

			// let the Strategy module do its work
			wns::scheduler::strategy::StrategyResult strategyResult = strategy->startScheduling(strategyInput);

			if (FRAMEPLOTTING)
				results->finishPlotting();

			checkIfAllScheduled(strategyResult);
		}

		delete strategy;
	}

}





