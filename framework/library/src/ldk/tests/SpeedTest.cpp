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

#include "SpeedTest.hpp"

#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/ldk/fun/Main.hpp>

#include <WNS/rng/RNGen.hpp>

#include <iostream>

#include <ctime>


using namespace wns::ldk;


CPPUNIT_TEST_SUITE_REGISTRATION( SpeedTest );

void
SpeedTest::setUp()
{
	wns::simulator::getEventScheduler()->reset();
} // setUp


void
SpeedTest::tearDown()
{
} // tearDown


void
SpeedTest::testSpeed()
{
	wns::ldk::CommandProxy::clearRegistries();

	const long speedIterations = 10; // 1000000;

	ILayer* leftLayer = new tests::LayerStub();
	fun::FUN* leftFUN = new fun::Main(leftLayer);

	ILayer* rightLayer = new tests::LayerStub();
	fun::FUN* rightFUN = new fun::Main(rightLayer);

	wns::pyconfig::Parser arqConfig;
	arqConfig.loadString("from openwns.logger import Logger\n"
						 "from openwns.ARQ import NoStatusCollection\n"
						 "bitsPerIFrame = 1\n"
						 "bitsPerRRFrame = 1\n"
						 "resendTimeout = 1.0\n"
						 "useSuspendProbe = False\n"
						 "suspendProbeName = \"timeBufferEmpty\"\n"
						 "logger = Logger('TEST','SpeedTest-StopAndWait',True)\n"
						 "arqStatusCollector = NoStatusCollection(logger)\n"
						 );
	arq::StopAndWait *leftARQ = new arq::StopAndWait(leftFUN, arqConfig);
	arq::StopAndWait *rightARQ = new arq::StopAndWait(rightFUN, arqConfig);

	wns::pyconfig::Parser emptyConfig;
	tools::Stub* stepper = new tools::Stub(rightFUN, emptyConfig);
	stepper->close();

	leftFUN->addFunctionalUnit("ernie", leftARQ);
	rightFUN->addFunctionalUnit("ernie", rightARQ);

	tools::Bridge* bridge = new tools::Bridge(leftFUN, rightFUN);
	leftARQ
		->connect(stepper)
		->connect(bridge->getLeft());
	rightARQ
		->connect(bridge->getRight());

	FunctionalUnit* leftUpper = new tools::Producer(leftFUN);
	tools::Consumer* rightUpper = new tools::Consumer(rightFUN);
	rightUpper->consumeIncoming();

	leftUpper
		->connect(leftARQ);

	rightUpper
		->connect(rightARQ);

	CPPUNIT_ASSERT(stepper->received.size() == 0);
	leftUpper->wakeup();

	// to enable measuring, uncomment the following lines and link
	// with librt...
//	struct timespec t_start, t_end;
//	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t_start);
	for(long l = 0; l < speedIterations; ++l) {
		stepper->flush();
		stepper->step();
		CPPUNIT_ASSERT_EQUAL(size_t(1), stepper->received.size());
		CPPUNIT_ASSERT_EQUAL(size_t(1), stepper->sent.size());
	}
//	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t_end);
//	cerr << t_start.tv_sec << " " << t_start.tv_nsec << " "
//		 << t_end.tv_sec << " " << t_end.tv_nsec << endl;

	delete rightUpper;
	delete leftUpper;
	delete bridge;
	delete stepper;
	delete rightFUN;
	delete leftFUN;
	delete rightLayer;
	delete leftLayer;
} // testSpeed


void
SpeedTest::testWithLoss()
{
	const long maxIterations = 50;

	ILayer* leftLayer = new tests::LayerStub();
	fun::FUN* leftFUN = new fun::Main(leftLayer);
	ILayer* rightLayer = new tests::LayerStub();
	fun::FUN* rightFUN = new fun::Main(rightLayer);

	wns::pyconfig::Parser arqConfig;
	arqConfig.loadString("from openwns.logger import Logger\n"
			     "from openwns.ARQ import NoStatusCollection\n"
						 "bitsPerIFrame = 1\n"
						 "bitsPerRRFrame = 1\n"
						 "resendTimeout = 1.0\n"
						 "useSuspendProbe = False\n"
						 "suspendProbeName = \"timeBufferEmpty\"\n"
						 "logger = Logger('TEST','SpeedTest-StopAndWait',True)\n"
						 "arqStatusCollector = NoStatusCollection(logger)\n"
						 );
	arq::StopAndWait *leftARQ = new arq::StopAndWait(leftFUN, arqConfig);
	arq::StopAndWait *rightARQ = new arq::StopAndWait(rightFUN, arqConfig);

	wns::pyconfig::Parser emptyConfig;
	tools::Stub* stepper = new tools::Stub(rightFUN, emptyConfig);
	stepper->close();

	leftFUN->addFunctionalUnit("ernie", leftARQ);
	rightFUN->addFunctionalUnit("ernie", rightARQ);

	tools::Bridge* bridge = new tools::Bridge(leftFUN, rightFUN, 0.5);
	leftARQ
		->connect(stepper)
		->connect(bridge->getLeft());
	rightARQ
		->connect(bridge->getRight());

	FunctionalUnit* leftUpper = new tools::Producer(leftFUN);
	tools::Consumer* rightUpper = new tools::Consumer(rightFUN);
	rightUpper->consumeIncoming();

	leftUpper
		->connect(leftARQ);

	rightUpper
		->connect(rightARQ);

	CPPUNIT_ASSERT(stepper->received.size() == 0);
	leftUpper->wakeup();

	long l;
	for(l = 0; l < maxIterations; ++l) {
		stepper->step();
		if(stepper->received.size() == 10)
			break;
		wns::simulator::getEventScheduler()->processOneEvent();
	}
	CPPUNIT_ASSERT(l < maxIterations);

	delete rightUpper;
	delete leftUpper;
	delete bridge;
	delete stepper;
	delete rightFUN;
	delete leftFUN;
	delete rightLayer;
	delete leftLayer;
} // testWithLoss



