/******************************************************************************
 * WNS (Wireless Network Simulator)                                           *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2004-2006                                                    *
 * Chair of Communication Networks (ComNets)                                  *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnets.rwth-aachen.de                                          *
 * www: http://wns.comnets.rwth-aachen.de                                     *
 ******************************************************************************/

#include "StopAndWaitTest.hpp"

#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/pyconfig/Parser.hpp>
#include <WNS/simulator/ISimulator.hpp>
#include <WNS/events/scheduler/Interface.hpp>

#include <vector>

using namespace glue::arqfsm::stopandwait;
using namespace wns::ldk;

CPPUNIT_TEST_SUITE_REGISTRATION( StopAndWaitFSMTest );

const int
StopAndWaitFSMTest::bitsPerIFrame = 2;

const int
StopAndWaitFSMTest::bitsPerRRFrame = 3;

void
StopAndWaitFSMTest::prepare()
{
	layer = new tests::LayerStub();
	fuNet = new fun::Main(layer);

	wns::pyconfig::Parser emptyConfig;
	upper = new tools::Stub(fuNet, emptyConfig);

	{
		wns::pyconfig::Parser pyco;
		pyco.loadString("from wns.Buffer import Bounded\n"
						"buffer = Bounded(size = 100, probingEnabled=False)\n"
						);
		wns::pyconfig::View view(pyco, "buffer");
		buffer = new buffer::Bounded(fuNet, view);
	}

	{
		std::stringstream ss;

		ss << "from glue.ARQFSM import StopAndWait\n"
		   << "arq = StopAndWait(\n"
		   << "  bitsPerIFrame = " << bitsPerIFrame << ",\n"
		   << "  bitsPerRRFrame = " << bitsPerRRFrame << ",\n"
		   << "  resendTimeout = 1.0\n"
		   << ")\n";

		wns::pyconfig::Parser all;
		all.loadString(ss.str()
						);
		wns::pyconfig::View config(all, "arq");
		arq = new FSMFU(fuNet, config);
	}

	lower = new tools::Stub(fuNet, emptyConfig);

	upper
		->connect(buffer)
		->connect(arq)
		->connect(lower);

	fuNet->addFunctionalUnit("ernie", arq);

	wns::simulator::getEventScheduler()->reset();
} // setUp


void
StopAndWaitFSMTest::cleanup()
{
	delete upper;
	delete buffer;
	delete lower;

	delete fuNet;
	delete layer;
} // tearDown


void
StopAndWaitFSMTest::testFillWindow()
{
	upper->sendData(fuNet->createCompound());
	upper->sendData(fuNet->createCompound());

	CPPUNIT_ASSERT_EQUAL(static_cast<unsigned long int>(1), buffer->getSize());
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), lower->sent.size());
} // testFillQWindow


void
StopAndWaitFSMTest::testSendOnAck()
{
	upper->sendData(fuNet->createCompound());
	upper->sendData(fuNet->createCompound());

	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), lower->sent.size());
	StopAndWaitCommand* receivedPCI = arq->getCommand(lower->sent[0]->getCommandPool());

	CompoundPtr compound(fuNet->createCompound());
	StopAndWaitCommand* ackPCI = arq->activateCommand(compound->getCommandPool());

	ackPCI->peer.type = StopAndWaitCommand::ACK;
	ackPCI->peer.NS = (receivedPCI->peer.NS + 1) % 2;

	lower->onData(compound);
	CPPUNIT_ASSERT_EQUAL(static_cast<unsigned long int>(0), buffer->getSize());
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), lower->sent.size());
} // testSendOnAck


void
StopAndWaitFSMTest::testSendAck()
{
	CompoundPtr compound(fuNet->createCompound());
	StopAndWaitCommand* command = arq->activateCommand(compound->getCommandPool());

	command->peer.type = StopAndWaitCommand::I;
	command->peer.NS = 0;

	lower->onData(compound);

	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), upper->received.size());
	CPPUNIT_ASSERT_EQUAL(compound, upper->received[0]);
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), lower->sent.size());

	compound = lower->sent[0];
	command = arq->getCommand(compound->getCommandPool());
	CPPUNIT_ASSERT_EQUAL(StopAndWaitCommand::ACK, command->peer.type);
	CPPUNIT_ASSERT_EQUAL(1, command->peer.NS);
} // testSendAck


void
StopAndWaitFSMTest::testRetransmission()
{
	wns::events::scheduler::Interface *scheduler = wns::simulator::getEventScheduler();
	CPPUNIT_ASSERT(!scheduler->processOneEvent()); // assure clean scheduler

	upper->sendData(fuNet->createCompound());
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), lower->sent.size());

	// there should be only one event: the retransmission timer
	scheduler->processOneEvent();
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), lower->sent.size());

	// just to be shure... :)
	scheduler->processOneEvent();
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), lower->sent.size());

	// send an ACK
	StopAndWaitCommand* receivedPCI = arq->getCommand(lower->sent[0]->getCommandPool());
	CompoundPtr compound(fuNet->createCompound());
	StopAndWaitCommand* command = arq->activateCommand(compound->getCommandPool());

	command->peer.type = StopAndWaitCommand::ACK;
	command->peer.NS = (receivedPCI->peer.NS + 1) % 2;

	lower->onData(compound);

	// now the scheduler shouldn't contain any events and there
	// shouldn't be any new compounds in the lower layer.
	CPPUNIT_ASSERT(!scheduler->processOneEvent());
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), lower->sent.size());
} // testRetransmission


void
StopAndWaitFSMTest::testIFrameSize()
{
	upper->sendData(fuNet->createCompound());

	int commandPoolSize;
	int dataSize;
	fuNet->calculateSizes(lower->sent[0]->getCommandPool(), commandPoolSize, dataSize);
	CPPUNIT_ASSERT_EQUAL(2, commandPoolSize);
	CPPUNIT_ASSERT_EQUAL(0, dataSize);
} // testIFrameSize


void
StopAndWaitFSMTest::testRRFrameSize()
{
	CompoundPtr compound(fuNet->createCompound());
	StopAndWaitCommand* command = arq->activateCommand(compound->getCommandPool());

	command->peer.type = StopAndWaitCommand::I;
	command->peer.NS = 0;

	lower->onData(compound);

	int commandPoolSize;
	int dataSize;
	fuNet->calculateSizes(lower->sent[0]->getCommandPool(), commandPoolSize, dataSize);
	CPPUNIT_ASSERT_EQUAL(3, commandPoolSize);
	CPPUNIT_ASSERT_EQUAL(0, dataSize);
} // testRRFrameSize



