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

#include <WNS/ldk/tests/DelayedInterfaceTest.hpp>
#include <WNS/ldk/buffer/Bounded.hpp>
#include <WNS/ldk/arq/StopAndWait.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>

#include <WNS/ldk/tools/Bridge.hpp>
#include <WNS/ldk/tools/Consumer.hpp>
#include <WNS/ldk/tools/Producer.hpp>

#include <WNS/pyconfig/Parser.hpp>

#include <cppunit/extensions/HelperMacros.h>

#include <vector>

namespace wns { namespace ldk { namespace arq { namespace tests {

	class StopAndWaitTest :
		public wns::ldk::tests::DelayedInterfaceTest
	{
		CPPUNIT_TEST_SUB_SUITE( StopAndWaitTest, wns::ldk::tests::DelayedInterfaceTest );
		CPPUNIT_TEST( fillWindow );
		CPPUNIT_TEST( sendOnAck );
		CPPUNIT_TEST( sendAck );
		CPPUNIT_TEST( retransmission );
		CPPUNIT_TEST( IFrameSize );
		CPPUNIT_TEST( RRFrameSize );
		CPPUNIT_TEST( speedWithLoss );
		CPPUNIT_TEST_SUITE_END();
	public:
		void
		fillWindow();

		void
		sendOnAck();

		void
		sendAck();

		void
		retransmission();

		void
		IFrameSize();

		void
		RRFrameSize();

		void
		speedWithLoss();

	private:
		virtual void
		prepare();

		virtual void
		setUpTestFUs();

		virtual StopAndWait*
		newTestee();

		virtual void
		tearDownTestee(DelayedInterface*)
		{
		}

		virtual FunctionalUnit*
		getUpperTestFU() const;

		buffer::Bounded* buffer;

		static const int bitsPerIFrame;
		static const int bitsPerRRFrame;
	};

	CPPUNIT_TEST_SUITE_REGISTRATION( StopAndWaitTest );

	const int
	StopAndWaitTest::bitsPerIFrame = 2;

	const int
	StopAndWaitTest::bitsPerRRFrame = 3;

	void
	StopAndWaitTest::prepare()
	{
		wns::ldk::CommandProxy::clearRegistries();

		wns::ldk::tests::DelayedInterfaceTest::prepare();

		wns::simulator::getEventScheduler()->reset();
	} // prepare

	void
	StopAndWaitTest::setUpTestFUs()
	{
		wns::ldk::tests::DelayedInterfaceTest::setUpTestFUs();

		wns::pyconfig::Parser pyco;
		pyco.loadString("from openwns.Buffer import Bounded\n"
						"buffer = Bounded(size = 100, probingEnabled = False)\n"
						);
		wns::pyconfig::View view(pyco, "buffer");
		buffer = new buffer::Bounded(getFUN(), view);
		getFUN()->addFunctionalUnit("buffer", buffer);
		buffer->connect(getTestee<FunctionalUnit>());
	} // setUpTestFUs

	StopAndWait*
	StopAndWaitTest::newTestee()
	{
		std::stringstream ss;

		ss << "from openwns.ARQ import StopAndWait\n"
		   << "arq = StopAndWait(\n"
		   << "  bitsPerIFrame = " << bitsPerIFrame << ",\n"
		   << "  bitsPerRRFrame = " << bitsPerRRFrame << ",\n"
		   << "  resendTimeout = 1.0\n"
		   << ")\n";

		wns::pyconfig::Parser all;
		all.loadString(ss.str());
		wns::pyconfig::View config(all, "arq");
		return new StopAndWait(getFUN(), config);
	} // newTestee

	wns::ldk::FunctionalUnit*
	StopAndWaitTest::getUpperTestFU() const
	{
		return buffer;
	} // getUpperTestFU

	void
	StopAndWaitTest::fillWindow()
	{
		getUpperStub()->sendData(getFUN()->createCompound());
		getUpperStub()->sendData(getFUN()->createCompound());

		CPPUNIT_ASSERT(buffer->getSize() == 1);
		CPPUNIT_ASSERT(compoundsSent() == 1);
	} // fillWindow


	void
	StopAndWaitTest::sendOnAck()
	{
		getUpperStub()->sendData(getFUN()->createCompound());
		getUpperStub()->sendData(getFUN()->createCompound());

		CPPUNIT_ASSERT(compoundsSent() == 1);
		StopAndWaitCommand* receivedPCI = getTestee<StopAndWait>()->getCommand(getLowerStub()->sent[0]->getCommandPool());

		CompoundPtr compound(getFUN()->createCompound());
		StopAndWaitCommand* ackPCI = getTestee<StopAndWait>()->activateCommand(compound->getCommandPool());

		ackPCI->peer.type = StopAndWaitCommand::RR;
		ackPCI->peer.NS = receivedPCI->peer.NS + 1;

		getLowerStub()->onData(compound);

		CPPUNIT_ASSERT_EQUAL(static_cast<unsigned long int>(0), buffer->getSize());
		CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(2), compoundsSent());
	} // sendOnAck


	void
	StopAndWaitTest::sendAck()
	{
		CompoundPtr compound(getFUN()->createCompound());
		StopAndWaitCommand* command = getTestee<StopAndWait>()->activateCommand(compound->getCommandPool());

		command->peer.type = StopAndWaitCommand::I;
		command->peer.NS = 0;

		getLowerStub()->onData(compound);

		CPPUNIT_ASSERT(compoundsDelivered() == 1);
		CPPUNIT_ASSERT(getUpperStub()->received[0] == compound);

		CPPUNIT_ASSERT(compoundsSent() == 1);

		compound = getLowerStub()->sent[0];
		command = getTestee<StopAndWait>()->getCommand(compound->getCommandPool());
		CPPUNIT_ASSERT(command->peer.type == StopAndWaitCommand::RR);
		CPPUNIT_ASSERT(command->peer.NS == 1);
	} // sendAck


	void
	StopAndWaitTest::retransmission()
	{
		wns::events::scheduler::Interface *scheduler = wns::simulator::getEventScheduler();
		CPPUNIT_ASSERT(!scheduler->processOneEvent()); // assure clean scheduler

		getUpperStub()->sendData(getFUN()->createCompound());
		CPPUNIT_ASSERT(compoundsSent() == 1);

		// there should be only one event: the retransmission timer
		scheduler->processOneEvent();
		CPPUNIT_ASSERT(compoundsSent() == 2);

		// just to be shure... :)
		scheduler->processOneEvent();
		CPPUNIT_ASSERT(compoundsSent() == 3);

		// send an ACK
		StopAndWaitCommand* receivedPCI = getTestee<StopAndWait>()->getCommand(getLowerStub()->sent[0]->getCommandPool());
		CompoundPtr compound(getFUN()->createCompound());
		StopAndWaitCommand* command = getTestee<StopAndWait>()->activateCommand(compound->getCommandPool());

		command->peer.type = StopAndWaitCommand::RR;
		command->peer.NS = receivedPCI->peer.NS + 1;

		getLowerStub()->onData(compound);

		// now the scheduler shouldn't contain any events and there
		// shouldn't be any new compounds in the lower layer.
		CPPUNIT_ASSERT(!scheduler->processOneEvent());
		CPPUNIT_ASSERT(compoundsSent() == 3);
	} // retransmission


	void
	StopAndWaitTest::IFrameSize()
	{
		getUpperStub()->sendData(getFUN()->createCompound());

		Bit commandPoolSize;
		Bit dataSize;
		getFUN()->calculateSizes(getLowerStub()->sent[0]->getCommandPool(), commandPoolSize, dataSize);
		CPPUNIT_ASSERT(commandPoolSize == 2);
		CPPUNIT_ASSERT(dataSize == 0);
	} // IFrameSize


	void
	StopAndWaitTest::RRFrameSize()
	{
		CompoundPtr compound(getFUN()->createCompound());
		StopAndWaitCommand* command = getTestee<StopAndWait>()->activateCommand(compound->getCommandPool());

		command->peer.type = StopAndWaitCommand::I;
		command->peer.NS = 0;

		getLowerStub()->onData(compound);

		Bit commandPoolSize;
		Bit dataSize;
		getFUN()->calculateSizes(getLowerStub()->sent[0]->getCommandPool(), commandPoolSize, dataSize);
		CPPUNIT_ASSERT(commandPoolSize == 3);
		CPPUNIT_ASSERT(dataSize == 0);
	} // RRFrameSize

	void
	StopAndWaitTest::speedWithLoss()
	{
		const long maxIterations = 75;

		ILayer* leftLayer = new wns::ldk::tests::LayerStub();
		fun::FUN* leftFUN = new fun::Main(leftLayer);
		ILayer* rightLayer = new wns::ldk::tests::LayerStub();
		fun::FUN* rightFUN = new fun::Main(rightLayer);
		wns::pyconfig::Parser arqConfig;

		std::stringstream ss;

		ss << "from openwns.ARQ import StopAndWait\n"
		   << "arq = StopAndWait(\n"
		   << "  bitsPerIFrame = " << bitsPerIFrame << ",\n"
		   << "  bitsPerRRFrame = " << bitsPerRRFrame << ",\n"
		   << "  resendTimeout = 1.0\n"
		   << ")\n";

		arqConfig.loadString(ss.str());
		pyconfig::View pcv = arqConfig.getView("arq");

		arq::StopAndWait* leftARQ = new arq::StopAndWait(leftFUN,  pcv);
		arq::StopAndWait* rightARQ = new arq::StopAndWait(rightFUN, pcv);

		wns::pyconfig::Parser emptyConfig;
		tools::Stub* stepper = new tools::Stub(rightFUN, emptyConfig);
		stepper->close();

		leftFUN->addFunctionalUnit("SAW-ARQ", leftARQ);
		rightFUN->addFunctionalUnit("SAW-ARQ", rightARQ);

		// the last parameter is the error probability
		tools::Bridge* bridge = new tools::Bridge(leftFUN, rightFUN, 0.3);
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
			if(stepper->received.size() == 15)
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
	} // speedWithLoss

}}}}


