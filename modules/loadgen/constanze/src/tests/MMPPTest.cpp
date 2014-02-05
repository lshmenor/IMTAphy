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
// ./wns-core -T "constanze::tests::MMPPTest"

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

// unix system performance measurements:
#include <sys/time.h>
#include <sys/resource.h>

namespace constanze { namespace tests {

	/** @brief stringify: converts a double to a string */
	std::string stringify(double x)
	{
		std::ostringstream o;
		o << x;
		return o.str();
	}

	/** @brief stringify: converts a long int to a string */
	std::string stringify(long x)
	{
		std::ostringstream o;
		o << x;
		return o.str();
	}

	/** @brief stringify: converts a struct timeval to a string */
	std::string stringify(struct timeval& t)
	{
		std::ostringstream o;
		o << std::fixed << std::setprecision( 6 ) << static_cast<double>(t.tv_sec) + static_cast<double>(t.tv_usec)/1000000.0;
		return o.str();
	}

	/** @brief this structure holds values of processor run time and memory usage */
	struct systemStatType {
		struct rusage rusageStats;
		int size;
		int resident;
		int share;
		int text;
		int lib;
		int data;
	};

	/** @brief class for measuring  processor run time and memory usage */
	class SystemUsageStats
	{
	public:
		/** @brief constructs a new object and measures current system stats */
		SystemUsageStats();
		~SystemUsageStats();
		/** @brief measure current system stats (again, after construction) and return result */
		struct systemStatType* getCurrentSystemUsageStats();
		/** @brief return stats measured before (at construction time) */
		struct systemStatType* getSavedSystemUsageStats();
		std::string printSystemUsageStats();
		/** @brief return run time difference between measurements of
		 * current object compared to other object argument */
		double compareTime(SystemUsageStats* other);
		int compareSize(SystemUsageStats* other);
		int compareRSS(SystemUsageStats* other);
		int compareDataSize(SystemUsageStats* other);
		std::string compareSystemUsageStats(SystemUsageStats* other);
	private:
		// man 5 proc
		// /proc/[number]/statm
		//      Provides information about memory status in pages.  The columns are:
		//       size       total program size
		//       resident   resident set size
		//       share      shared pages
		//       text       text (code)
		//       lib        library   // 0 always
		//       data       data/stack
		//       dt         dirty pages (unused in Linux 2.6)   // 0 always
		struct systemStatType systemStat;
	};

	class MMPPTest
		: public wns::TestFixture
	{
 		CPPUNIT_TEST_SUITE( MMPPTest );
		CPPUNIT_TEST( testConstructorDestructor );
		CPPUNIT_TEST( testFreeRun );
		CPPUNIT_TEST( testStopTime );
		CPPUNIT_TEST( testMMPP );
		CPPUNIT_TEST( testMPEG2 );
		CPPUNIT_TEST( testData );
		CPPUNIT_TEST( testTargetRate );
		CPPUNIT_TEST_SUITE_END();
	public:
		MMPPTest();
		~MMPPTest();
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
		SystemUsageStats* systemStatsBefore;
		SystemUsageStats* systemStatsAfter;
	};

} // tests
} // constanze

using namespace constanze;
using namespace constanze::tests;

/********************************* RusageStats *****************************/

SystemUsageStats::SystemUsageStats()
{
	getCurrentSystemUsageStats(); // make first measurement
}

SystemUsageStats::~SystemUsageStats()
{
}

struct systemStatType* SystemUsageStats::getCurrentSystemUsageStats()
{
	// man 2 getrusage
	// int getrusage(int who, struct rusage *usage);
	CPPUNIT_ASSERT( getrusage(RUSAGE_SELF, &systemStat.rusageStats) == 0 );
	std::ifstream in( "/proc/self/statm");
	CPPUNIT_ASSERT(in.good());
	in >> systemStat.size >> systemStat.resident >> systemStat.share >> systemStat.text >> systemStat.lib >> systemStat.data;
	in.close();
	unsigned long int pagesize = getpagesize(); // values are in "pages" which are usually 4KB of size:
	systemStat.size*=pagesize; systemStat.resident*=pagesize; systemStat.share*=pagesize; systemStat.text*=pagesize; systemStat.lib*=pagesize; systemStat.data*=pagesize;
	return &systemStat;
}

struct systemStatType* SystemUsageStats::getSavedSystemUsageStats()
{
	return &systemStat;
}

std::string SystemUsageStats::printSystemUsageStats()
{
	std::ostringstream o;
	o << "resource usage:" << std::endl;
	o << "ru_utime = " << stringify(systemStat.rusageStats.ru_utime) << std::endl;
	o << "ru_stime = " << stringify(systemStat.rusageStats.ru_stime) << std::endl;
	o << "total program size = " << systemStat.size << std::endl;
	o << "resident set size = " << systemStat.resident << std::endl;
	o << "shared pages = " << systemStat.share << std::endl;
	o << "text (code) = " << systemStat.text << std::endl;
	o << "library = " << systemStat.lib << std::endl;
	o << "data/stack = " << systemStat.data << std::endl << std::endl;
//	o << " = " << systemStats. << std::endl;
	return o.str();
}

double SystemUsageStats::compareTime(SystemUsageStats* other)
{
	struct timeval difference;
	timersub(&systemStat.rusageStats.ru_utime, &(other->getSavedSystemUsageStats()->rusageStats.ru_utime), &difference);
	return static_cast<double>(difference.tv_sec) + static_cast<double>(difference.tv_usec)/1000000.0;
}

int SystemUsageStats::compareSize(SystemUsageStats* other)
{
	return systemStat.size - other->getSavedSystemUsageStats()->size;
}

int SystemUsageStats::compareRSS(SystemUsageStats* other)
{
	return systemStat.resident - other->getSavedSystemUsageStats()->resident;
}

int SystemUsageStats::compareDataSize(SystemUsageStats* other)
{
	return systemStat.data - other->getSavedSystemUsageStats()->data;
}

std::string SystemUsageStats::compareSystemUsageStats(SystemUsageStats* other)
{
	std::ostringstream o;
	o << "resource usage comparison:" << std::endl;
	struct timeval difference;
	timersub(&systemStat.rusageStats.ru_utime, &(other->getSavedSystemUsageStats()->rusageStats.ru_utime), &difference);
	o << "utime              += " << stringify(difference) << std::endl;
	o << "total program size += " << systemStat.size     - other->getSavedSystemUsageStats()->size     << std::endl;
	o << "resident set size  += " << systemStat.resident - other->getSavedSystemUsageStats()->resident << std::endl;
	//o << "shared pages     += " << systemStat.share    - other->getSavedSystemUsageStats()->share    << std::endl;
	//o << "text (code)      += " << systemStat.text     - other->getSavedSystemUsageStats()->text     << std::endl;
	//o << "library          += " << systemStat.lib      - other->getSavedSystemUsageStats()->lib      << std::endl;
	o << "data/stack         += " << systemStat.data     - other->getSavedSystemUsageStats()->data     << std::endl;
	return o.str();
}

/********************************* MMPPTest *****************************/

CPPUNIT_TEST_SUITE_REGISTRATION( MMPPTest );

MMPPTest::MMPPTest() :
        wns::TestFixture(),
	useCout(false),
	testGeneratorMMPP(NULL),
	binding(NULL),
	systemStatsBefore(NULL),
	systemStatsAfter(NULL)
{
}

MMPPTest::~MMPPTest()
{
	//if (scheduler) delete scheduler; // scheduler is a pointer to a global instance
	if (testGeneratorMMPP) delete testGeneratorMMPP;
	if (binding) delete binding;
	if (systemStatsBefore) delete systemStatsBefore;
	if (systemStatsAfter) delete systemStatsAfter;
}

wns::pyconfig::View
MMPPTest::makePyconfigString(std::string extension)
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

void MMPPTest::prepare()
{
	// for Doxygen:
	// begin example "constanze::Generator::method1.example"
	// do this and that
	// end example
	systemStatsBefore  = new SystemUsageStats();
	//if (useCout) std::cout << systemStatsBefore->printSystemUsageStats() << std::endl;
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

void MMPPTest::cleanup()
{
	delete testGeneratorMMPP; testGeneratorMMPP=NULL;
	delete binding; binding=NULL;
	systemStatsAfter = new SystemUsageStats();
	/*double utime =*/ systemStatsAfter->compareTime(systemStatsBefore);
	int memIncrease = systemStatsAfter->compareDataSize(systemStatsBefore);
	if (useCout)
		//std::cout << systemStatsAfter->printSystemUsageStats() << std::endl;
		std::cout << systemStatsAfter->compareSystemUsageStats(systemStatsBefore) << std::endl;
	//std::cout << "utime = " << utime << std::endl;
	// CPPUNIT_ASSERT( utime < 60.0 ); // should be ready within 60 seconds
	//CPPUNIT_ASSERT( memIncrease <= 10240*1024 ); // should not be more than 10MB
	if ( memIncrease > 10240*1024 )
		std::cout << "memIncrease = " << memIncrease/1024/1024 << "MB!" << std::endl;
	delete systemStatsBefore; systemStatsBefore=NULL;
	delete systemStatsAfter; systemStatsAfter=NULL;
}

void MMPPTest::testConstructorDestructor()
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
void MMPPTest::testFreeRun()
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
	} while ((now < 20.0) && (it < 2000)); // limited by it<2000 here
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
	//WNS_ASSERT_MAX_REL_ERROR(meanTrafficRate * trafficDuration, static_cast<double>(testGeneratorMMPP->countedBits()), 0.1);
}

void MMPPTest::testStopTime()
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
	CPPUNIT_ASSERT(testGeneratorMMPP->countedPackets() > 400);
	CPPUNIT_ASSERT(testGeneratorMMPP->countedPackets() < it);
	CPPUNIT_ASSERT(testGeneratorMMPP->countedBits() > minTrafficRate  * trafficDuration);
	CPPUNIT_ASSERT(testGeneratorMMPP->countedBits() < maxTrafficRate  * trafficDuration);
	// traffic is bursty and only few events, so big tolerance needed:
	// (this test should only test stopTime, so very few events occur.
	// WNS_ASSERT_MAX_REL_ERROR(meanTrafficRate * trafficDuration, static_cast<double>(testGeneratorMMPP->countedBits()), 0.5);
	//testGeneratorMMPP->bindingReleased(); // done automatically after stopTime
}

void MMPPTest::testMMPP()
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
	} while ((now < 5.5) && (it < 100000)); // limited by it<100000 here, at t=?
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

void MMPPTest::testMPEG2()
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

void MMPPTest::testData()
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
	} while ((now < 5.5) && (it < 10000)); // limited by t<2.5 here, at it=828
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
	// traffic is very bursty (and self-similar), so 50% tolerance needed:
	WNS_ASSERT_MAX_REL_ERROR(meanTrafficRate * trafficDuration, static_cast<double>(testGeneratorMMPP->countedBits()), 0.5);
}

void MMPPTest::testTargetRate()
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


