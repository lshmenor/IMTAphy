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

#include <WNS/ldk/arq/GoBackN.hpp>

#include <WNS/ldk/tools/Bridge.hpp>
#include <WNS/ldk/tools/Consumer.hpp>
#include <WNS/ldk/tools/Producer.hpp>
#include <WNS/ldk/tools/Stub.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>

#include <WNS/ldk/buffer/Bounded.hpp>

#include <WNS/ldk/fun/Main.hpp>

#include <WNS/pyconfig/Parser.hpp>

#include <cppunit/extensions/HelperMacros.h>

#include <vector>

namespace wns { namespace ldk { namespace arq { namespace tests {

	class GoBackNTest :
		public wns::ldk::tests::DelayedInterfaceTest
	{
		CPPUNIT_TEST_SUB_SUITE( GoBackNTest, wns::ldk::tests::DelayedInterfaceTest );
		CPPUNIT_TEST( fillWindow );
		CPPUNIT_TEST( sendOnAck );
		CPPUNIT_TEST( multipleFillWindow );
		CPPUNIT_TEST( sendAck );
		CPPUNIT_TEST( sendNak );
		CPPUNIT_TEST( retransmission );
		CPPUNIT_TEST( retransmission2 );
		CPPUNIT_TEST( outOfSeqRetransmission );
		CPPUNIT_TEST( duplicateIFrames );
		CPPUNIT_TEST( duplicateOutOfSequenceIFrames );
		CPPUNIT_TEST( totallyOutOfSequenceIFrames );
		CPPUNIT_TEST( retransmissionState );
		CPPUNIT_TEST( ACKPending );
		CPPUNIT_TEST( outOfSeqIFrames );
		CPPUNIT_TEST( speedWithLoss );
		CPPUNIT_TEST( delayedDelivery );
		CPPUNIT_TEST_SUITE_END();
	public:
		void
		fillWindow();

		void
		sendOnAck();

		void
		multipleFillWindow();

		void
		sendAck();

		void
		sendNak();

		void
		retransmission();

		void
		retransmission2();

		void
		outOfSeqRetransmission();

		void
		duplicateIFrames();

		void
		duplicateOutOfSequenceIFrames();

		void
		totallyOutOfSequenceIFrames();

		void
		retransmissionState();

		void
		ACKPending();

		void
		outOfSeqIFrames();

		void
		speedWithLoss();

		void
		delayedDelivery();

	private:
		virtual void
		prepare();

		virtual void
		setUpTestFUs();

		virtual GoBackN*
		newTestee();

		virtual void
		tearDownTestee(DelayedInterface*)
		{
		}

		virtual FunctionalUnit*
		getUpperTestFU() const;

		CompoundPtr
		createACKFrame(const CompoundPtr& receivedCompound);

		CompoundPtr
		createNAKFrame(const CompoundPtr& receivedCompound);

		CompoundPtr
		createIFrame(const ARQCommand::SequenceNumber& number);

		buffer::Bounded* buffer;

		int windowSize;
		int sequenceNumberSize;
	};

	CPPUNIT_TEST_SUITE_REGISTRATION( GoBackNTest );

	void
	GoBackNTest::prepare()
	{
		wns::ldk::CommandProxy::clearRegistries();

		windowSize = 4;
		sequenceNumberSize = 2*windowSize;

		wns::ldk::tests::DelayedInterfaceTest::prepare();

		wns::simulator::getEventScheduler()->reset();
	} // prepare

	void
	GoBackNTest::setUpTestFUs()
	{
		wns::ldk::tests::DelayedInterfaceTest::setUpTestFUs();

		wns::pyconfig::Parser pyco;
		pyco.loadString("from openwns.Buffer import Bounded\n"
						"buffer = Bounded(size = 100, probingEnabled=False)\n"
						);
		wns::pyconfig::View view(pyco, "buffer");
		buffer = new buffer::Bounded(getFUN(), view);
		getFUN()->addFunctionalUnit("buffer", buffer);
		buffer->connect(getTestee<FunctionalUnit>());
	} // setUpTestFUs

	GoBackN*
	GoBackNTest::newTestee()
	{
		std::stringstream ss;
		ss << "from openwns.ARQ import GoBackN\n"
		   << "arq = GoBackN(\n"
		   << "  windowSize = " << windowSize << ",\n"
		   << "  sequenceNumberSize = " << sequenceNumberSize << ",\n"
		   << "  resendTimeout = 1.0,\n"
		   << "  useProbe = False\n"
		   << ")\n";

		wns::pyconfig::Parser all;
		all.loadString(ss.str());

		wns::pyconfig::View config(all, "arq");

		GoBackN* arq = new GoBackN(getFUN(), config);
		return arq;
	} // newTestee

	FunctionalUnit*
	GoBackNTest::getUpperTestFU() const
	{
		return buffer;
	}

	void
	GoBackNTest::fillWindow()
	{
		for (int i = 0; i< (windowSize+1) ; i++)
			getUpperStub()->sendData(getFUN()->createCompound());

		CPPUNIT_ASSERT_EQUAL(1, (int) buffer->getSize() );
		CPPUNIT_ASSERT(compoundsSent() == (unsigned long int)(windowSize));
	} // fillWindow


	void
	GoBackNTest::sendOnAck()
	{
		for (int i = 0; i< (windowSize+1) ; i++)
			getUpperStub()->sendData(getFUN()->createCompound());

		CPPUNIT_ASSERT_EQUAL(1, (int) buffer->getSize() );
		CPPUNIT_ASSERT(compoundsSent() == (unsigned long int)(windowSize));

		CompoundPtr ackFrame = createACKFrame(getLowerStub()->sent[0]);
		getLowerStub()->onData(ackFrame);

		CPPUNIT_ASSERT_EQUAL(0, (int) buffer->getSize() );
		CPPUNIT_ASSERT(compoundsSent() == (unsigned long int)(windowSize + 1));
	} // sendOnAck


	void
	GoBackNTest::multipleFillWindow()
	{
		for (int i = 0; i < 3*windowSize+1 ; i++)
			getUpperStub()->sendData(getFUN()->createCompound());

		CPPUNIT_ASSERT_EQUAL(static_cast<unsigned long int>(2*windowSize+1), buffer->getSize());
		CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(windowSize), compoundsSent());

		for (int i = 0; i < windowSize; i++){
			CompoundPtr ackFrame = createACKFrame(getLowerStub()->sent[i]);
			getLowerStub()->onData(ackFrame);
		}

		CPPUNIT_ASSERT_EQUAL(static_cast<unsigned long int>(windowSize+1), buffer->getSize());
		CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(2*windowSize), compoundsSent());

		for (int i = 0; i < windowSize; i++){
			CompoundPtr ackFrame = createACKFrame(getLowerStub()->sent[windowSize+i]);
			getLowerStub()->onData(ackFrame);
		}

		CPPUNIT_ASSERT_EQUAL(static_cast<unsigned long int>(1), buffer->getSize());
		CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(3*windowSize), compoundsSent());
	} // multipleFillWindow


	void
	GoBackNTest::sendAck()
	{
		CompoundPtr compound = createIFrame(ARQCommand::SequenceNumber(0));
		getLowerStub()->onData(compound);

		CPPUNIT_ASSERT_EQUAL(1, (int) compoundsDelivered() );
		CPPUNIT_ASSERT(getUpperStub()->received[0] == compound);

		CPPUNIT_ASSERT_EQUAL(1, (int) compoundsSent() );

		compound = getLowerStub()->sent[0];
		GoBackNCommand* command = getTestee<GoBackN>()->getCommand(compound->getCommandPool());
		CPPUNIT_ASSERT(command->peer.type == GoBackNCommand::ACK);
		CPPUNIT_ASSERT_EQUAL(0, (int) command->getNS() );
	} // sendAck

	void
	GoBackNTest::sendNak()
	{
		CompoundPtr compound = createIFrame(ARQCommand::SequenceNumber(1));
		getLowerStub()->onData(compound);
		// frame with seqnr=1 (frame 0 missing) must be discarded
		CPPUNIT_ASSERT_EQUAL(0, (int) compoundsDelivered() );
		//CPPUNIT_ASSERT(getUpperStub()->received[0] == compound);
		CPPUNIT_ASSERT_EQUAL(1, (int) compoundsSent() );

		compound = getLowerStub()->sent[0];
		GoBackNCommand* command = getTestee<GoBackN>()->getCommand(compound->getCommandPool());
		CPPUNIT_ASSERT(command->peer.type == GoBackNCommand::NAK);
		CPPUNIT_ASSERT_EQUAL(0, (int) command->getNS() );
	} // sendAck


	void
	GoBackNTest::retransmission()
	{
		wns::events::scheduler::Interface *scheduler = wns::simulator::getEventScheduler();
		CPPUNIT_ASSERT(!scheduler->processOneEvent()); // assure clean scheduler

		getUpperStub()->sendData(getFUN()->createCompound());
		CPPUNIT_ASSERT_EQUAL(1, (int) compoundsSent() );

		// there should be only one event: the retransmission timer
		scheduler->processOneEvent();
		CPPUNIT_ASSERT_EQUAL(2, (int) compoundsSent() );

		// just to be sure... :)
		scheduler->processOneEvent();
		CPPUNIT_ASSERT_EQUAL(3, (int) compoundsSent() );

		// send an ACK
		CompoundPtr ackFrame = createACKFrame(getLowerStub()->sent[0]);
		getLowerStub()->onData(ackFrame);

		// now the scheduler shouldn't contain any events and there
		// shouldn't be any new compounds in the lower layer.
		CPPUNIT_ASSERT(!scheduler->processOneEvent());
		CPPUNIT_ASSERT_EQUAL(3, (int) compoundsSent() );
	} // retransmission


	void
	GoBackNTest::retransmission2()
	{
		wns::events::scheduler::Interface *scheduler = wns::simulator::getEventScheduler();
		CPPUNIT_ASSERT(!scheduler->processOneEvent()); // assure clean scheduler

		getUpperStub()->sendData(getFUN()->createCompound());
		getUpperStub()->sendData(getFUN()->createCompound());
		getUpperStub()->sendData(getFUN()->createCompound());
		getUpperStub()->sendData(getFUN()->createCompound());
		CPPUNIT_ASSERT_EQUAL(4, (int) compoundsSent() );

		//MESSAGE_BEGIN(NORMAL, logger, m, "GoBackNTest::testRetransmission2");
		//m << " sending an ACK for 2nd frame";
		//MESSAGE_END();
		//cout << " sending an ACK for 2nd frame";

		// send an ACK for the 2nd Frame
		// GoBackN: this means 1st frame is ACKed, too
		CompoundPtr ackFrame = createACKFrame(getLowerStub()->sent[1]);
		getLowerStub()->onData(ackFrame);
		// -> LA=2

		CPPUNIT_ASSERT_EQUAL(4, (int) compoundsSent() );

		// send an NAK for the 3rd Frame
		// means that [2] was received correctly
		CompoundPtr nakFrame = createNAKFrame(getLowerStub()->sent[3]); // NAK!
		// -> ackedNS=2, LA=2

		getLowerStub()->onData(nakFrame);

		// retransmission frame 2
		CPPUNIT_ASSERT_EQUAL(6, (int) compoundsSent() );

		// there should be only one event: the retransmission timer
		scheduler->processOneEvent();
		CPPUNIT_ASSERT_EQUAL(8, (int) compoundsSent() );

		// just to be sure... :)
		scheduler->processOneEvent();
		CPPUNIT_ASSERT_EQUAL(10, (int) compoundsSent() );

		// send an ACK for the 3rd Frame
		ackFrame = createACKFrame(getLowerStub()->sent[2]);
		getLowerStub()->onData(ackFrame);

		CPPUNIT_ASSERT_EQUAL(10, (int) compoundsSent() );

		// send an ACK for the 4th Frame
		ackFrame = createACKFrame(getLowerStub()->sent[3]);
		getLowerStub()->onData(ackFrame);

		CPPUNIT_ASSERT_EQUAL(10, (int) compoundsSent() );

		// now the scheduler shouldn't contain any events and there
		// shouldn't be any new compounds in the lower layer.
		CPPUNIT_ASSERT(!scheduler->processOneEvent());
		CPPUNIT_ASSERT_EQUAL(10, (int) compoundsSent() );
	} // retransmission2


	void
	GoBackNTest::outOfSeqRetransmission()
	{
		for (int i = 0; i<(windowSize+1) ; i++)
			getUpperStub()->sendData(getFUN()->createCompound());
		// ^ one frame too many for arq
		// buffer is above arq and stores frames that don't fit into arq
		CPPUNIT_ASSERT_EQUAL(1, (int) buffer->getSize() );
		CPPUNIT_ASSERT(compoundsSent() == (unsigned long int)(windowSize));

		// send an out-of-sequence ACK (the third, index 2)
		// this implicitely acks 0+1+2
		CompoundPtr ackFrame = createACKFrame(getLowerStub()->sent[2]);
		getLowerStub()->onData(ackFrame);
		// -> LA=2

		CPPUNIT_ASSERT_EQUAL(0, (int) buffer->getSize() );
		//CPPUNIT_ASSERT(compoundsSent() == unsigned long int(windowSize) + 1);
		CPPUNIT_ASSERT_EQUAL( (int)(windowSize) + 1, (int) compoundsSent() );
		// no retransmission for GoBackN
	} // outOfSeqRetransmission


	void
	GoBackNTest::duplicateIFrames()
	{
		ARQCommand::SequenceNumber mySN(0);

		// send 5 subsequent I-Frames
		for (int i=0; i < 5; i++)
			{
				getLowerStub()->onData(createIFrame(mySN++));
			}
		CPPUNIT_ASSERT_EQUAL( 5, (int) compoundsSent()  ); // 5 ACKs sent?
		CPPUNIT_ASSERT_EQUAL( 5, (int) compoundsDelivered() ); // 5 Frames delivered?

		// resend the third Frame
		ARQCommand::SequenceNumber duplicateSN(2);
		getLowerStub()->onData(createIFrame(duplicateSN));

		CPPUNIT_ASSERT_EQUAL( 6, (int) compoundsSent()     ); // 6 ACKs sent?
		CPPUNIT_ASSERT_EQUAL( 5, (int) compoundsDelivered() ); // 5 Frames delivered?
	} // duplicateIFrames


	void
	GoBackNTest::duplicateOutOfSequenceIFrames()
	{
		// send 2 subsequent out of sequence I-Frames from lower to upper
		for (int i=1; i < 3; i++)
		{
			getLowerStub()->onData(createIFrame(ARQCommand::SequenceNumber(i)));
		}
		CPPUNIT_ASSERT_EQUAL( 2, (int)compoundsSent() ); // 1 NAK sent?
		CPPUNIT_ASSERT_EQUAL( 0, (int)compoundsDelivered() ); // No Frames delivered?

		// resend the third Frame
		ARQCommand::SequenceNumber duplicateSN(2);
		getLowerStub()->onData(createIFrame(duplicateSN));
		// new NAKs here
		CPPUNIT_ASSERT_EQUAL( 3, (int)compoundsSent() ); // 3 NAKs sent?
		CPPUNIT_ASSERT_EQUAL( 0, (int)compoundsDelivered() ); // Still no Frames
		// delivered?

		getLowerStub()->onData(createIFrame(ARQCommand::SequenceNumber(0)));

		CPPUNIT_ASSERT_EQUAL( 4, (int)compoundsSent() ); // 4 ACK/NAKs sent?
		CPPUNIT_ASSERT_EQUAL( 1, (int)compoundsDelivered() ); // 1 Frames delivered
	} // duplicateOutOfSequenceIFrames


	void
	GoBackNTest::totallyOutOfSequenceIFrames()
	{
		getLowerStub()->onData(createIFrame(ARQCommand::SequenceNumber(3)));

		CPPUNIT_ASSERT_EQUAL( 1, (int)compoundsSent() ); // 1 NAK sent?
		CPPUNIT_ASSERT_EQUAL( 0, (int)compoundsDelivered() ); // No Frames delivered?

		// send the second frame
		getLowerStub()->onData(createIFrame(ARQCommand::SequenceNumber(1)));

		CPPUNIT_ASSERT_EQUAL( 2, (int)compoundsSent() ); // 2 NAKs sent?
		CPPUNIT_ASSERT_EQUAL( 0, (int)compoundsDelivered() ); // No Frames delivered?

		// send the first frame
		getLowerStub()->onData(createIFrame(ARQCommand::SequenceNumber(0)));

		CPPUNIT_ASSERT_EQUAL( 3, (int)compoundsSent() ); // 3 ACK/NAKs sent?
		CPPUNIT_ASSERT_EQUAL( 1, (int)compoundsDelivered() ); // 1 frames received?

		// send the third frame
		getLowerStub()->onData(createIFrame(ARQCommand::SequenceNumber(2)));

		CPPUNIT_ASSERT_EQUAL( 4, (int)compoundsSent() ); // 4 ACK/NAKs sent?
		CPPUNIT_ASSERT_EQUAL( 1, (int)compoundsDelivered() ); // 1 Frames delivered
	} // totallyOutOfSequenceIFrames


	void
	GoBackNTest::retransmissionState()
	{
		wns::events::scheduler::Interface *scheduler = wns::simulator::getEventScheduler();
		CPPUNIT_ASSERT(!scheduler->processOneEvent()); // assure clean scheduler

		getLowerStub()->close();

		// send three packets
		for (int i = 0; i < 3; i++)
		{
			getUpperStub()->sendData(getFUN()->createCompound());
		}

		CPPUNIT_ASSERT_EQUAL( 0, (int)compoundsSent() ); // No Packets sent?

		for (int i = 0; i < 3; i++)
		{
			getLowerStub()->step();
		}

		CPPUNIT_ASSERT_EQUAL( 3, (int)compoundsSent() ); // 3 Packets sent?

		// NAK: neg.acknowledge the receipt of the 2nd packet (3rd was ok)
		// => ack frame 0
		getLowerStub()->onData(createNAKFrame(getLowerStub()->sent[2]));

		CPPUNIT_ASSERT_EQUAL( 3, (int)compoundsSent() ); // Nothing happened yet?

		// are we in retransmission state?
		CPPUNIT_ASSERT( getTestee<GoBackN>()->retransmissionState() );
		// with 2 frames to retransmit
		CPPUNIT_ASSERT_EQUAL( 2,  (int) getTestee<GoBackN>()->toRetransmit.size());

		// allow to send 1 retransmission
		getLowerStub()->step();
		// we should still be in retransmission state
		CPPUNIT_ASSERT( getTestee<GoBackN>()->retransmissionState() );
		// allow to send 1 retransmission
		getLowerStub()->step();
		// "Leaving retransmission state"
		// we  should not be in retransmission state
		CPPUNIT_ASSERT( !getTestee<GoBackN>()->retransmissionState() );
		// and have no packet left
		CPPUNIT_ASSERT_EQUAL( 0,  (int) getTestee<GoBackN>()->toRetransmit.size());
		// 2 retransmissions sent?
		CPPUNIT_ASSERT_EQUAL( 5,  (int) compoundsSent() );

		// acknowledge the receipt of the packet in the retransmission queue
		// ^ nothing in (getTestee<GoBackN>()->toRetransmit.front()) !!!
		//getLowerStub()->onData( createACKFrame(getTestee<GoBackN>()->toRetransmit.front()) );
		// accept to send one packet
		//getLowerStub()->step();
		// we should have left retransmission state
		//CPPUNIT_ASSERT( !getTestee<GoBackN>()->retransmissionState() );
		getLowerStub()->open();

		// this should be the same ACK as before
		getLowerStub()->onData( createACKFrame( getLowerStub()->sent[1] ) );

		// we should not be in retransmission state
		CPPUNIT_ASSERT( !getTestee<GoBackN>()->retransmissionState()  );
		// but have 1 frame in the sentPDUs  buffer
		CPPUNIT_ASSERT_EQUAL( 1,  (int) getTestee<GoBackN>()->sentPDUs.size());
		// send the missing ACK
		getLowerStub()->onData( createACKFrame( getLowerStub()->sent[2] ) );

		// there should be nothing pending
		CPPUNIT_ASSERT( !scheduler->processOneEvent() );
		CPPUNIT_ASSERT( !getTestee<GoBackN>()->retransmissionState() );
		CPPUNIT_ASSERT( !getTestee<GoBackN>()->hasSomethingToSend() );
		CPPUNIT_ASSERT_EQUAL( 5,  (int) compoundsSent() );
	} // retransmissionState


	void
	GoBackNTest::ACKPending()
	{
		// stop automatic processing
		getLowerStub()->close();

		// send three packets
		getLowerStub()->onData(createIFrame(ARQCommand::SequenceNumber(2)));
		getLowerStub()->onData(createIFrame(ARQCommand::SequenceNumber(0)));
		getLowerStub()->onData(createIFrame(ARQCommand::SequenceNumber(2))); // duplicated intentionally
		getLowerStub()->onData(createIFrame(ARQCommand::SequenceNumber(1)));

		getLowerStub()->step();
		CPPUNIT_ASSERT_EQUAL( 1, (int)compoundsSent() ); // 1 NAK sent?
		getLowerStub()->open();
		CPPUNIT_ASSERT_EQUAL( 4, (int)compoundsSent() ); // all ACKs sent?
		CPPUNIT_ASSERT_EQUAL( 2, (int)compoundsDelivered() ); // 2 frames delivered?
	} // ACKPending


	void
	GoBackNTest::outOfSeqIFrames()
	{
		// send windowSize-1 subsequent I-Frames
		for (int i=(windowSize-1); i > 0; i--)
		{
			getLowerStub()->onData(createIFrame(ARQCommand::SequenceNumber(i)));
		}
		CPPUNIT_ASSERT( compoundsSent()     == static_cast<size_t>(windowSize-1)); // all NAKs sent?
		CPPUNIT_ASSERT( compoundsDelivered() == 0);            // 0 Frames delivered?

		// send the zeroe-th Frame
		ARQCommand::SequenceNumber missingSN(0);
		getLowerStub()->onData(createIFrame(missingSN));

		CPPUNIT_ASSERT( compoundsSent()     == (unsigned long int)(windowSize)); // all NAK/ACKs sent?
		CPPUNIT_ASSERT( compoundsDelivered() == 1); // 1 Frames delivered?
	} // outOfSeqIFrames


	void
	GoBackNTest::speedWithLoss()
	{
		const long maxIterations = 75;

		ILayer* leftLayer = new wns::ldk::tests::LayerStub();
		//FUN* leftFUN = new FUN(leftLayer);
		fun::FUN* leftFUN = new fun::Main(leftLayer);
		ILayer* rightLayer = new wns::ldk::tests::LayerStub();
		//FUN* rightFUN = new FUN(rightLayer);
		fun::FUN* rightFUN = new fun::Main(rightLayer);
		wns::pyconfig::Parser arqConfig;

		std::stringstream ss;

		ss << "from openwns.ARQ import GoBackN\n"
		   << "arq = GoBackN(\n"
		   << "  windowSize = " << windowSize << ",\n"
		   << "  sequenceNumberSize = " << sequenceNumberSize << ",\n"
		   << "  resendTimeout = 1.0\n"
		   << ")\n";

		arqConfig.loadString(ss.str());
		pyconfig::View pcv = arqConfig.getView("arq");

		arq::GoBackN *leftARQ = new arq::GoBackN(leftFUN,  pcv);
		arq::GoBackN *rightARQ = new arq::GoBackN(rightFUN, pcv);

		wns::pyconfig::Parser emptyConfig;
		tools::Stub* stepper = new tools::Stub(rightFUN, emptyConfig);
		stepper->close();

		leftFUN->addFunctionalUnit("ernie", leftARQ);
		rightFUN->addFunctionalUnit("ernie", rightARQ);

		// the last parameter is the error probability
		tools::Bridge* bridge = new tools::Bridge(leftFUN, rightFUN, 0.4);
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
			if(stepper->received.size() > 14)
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


	void
	GoBackNTest::delayedDelivery()
	{
		getTestee<GoBackN>()->doDelayDelivery();

		for (int i = 0; i < 2*windowSize+1 ; i++)
			getUpperStub()->sendData(getFUN()->createCompound());

		for (int i = 0; i < windowSize; i++)
			getLowerStub()->onData(getLowerStub()->sent[i]);

		CPPUNIT_ASSERT_EQUAL((unsigned long int)(windowSize+1), buffer->getSize());
		CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), getUpperStub()->received.size());

		for (int i = 0; i < windowSize; i++)
			getLowerStub()->onData(getLowerStub()->sent[i+windowSize]);

		CPPUNIT_ASSERT_EQUAL((unsigned long int)(1), buffer->getSize());
		CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), getUpperStub()->received.size());

		getTestee<GoBackN>()->doDeliver();

		CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(windowSize), getUpperStub()->received.size());

		for (int i = 0; i < windowSize; i++)
			getLowerStub()->onData(getLowerStub()->sent[i+2*windowSize]);

		CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2*windowSize), getUpperStub()->received.size());
	} // delayedDelivery


	wns::ldk::CompoundPtr
	GoBackNTest::createACKFrame(const CompoundPtr& receivedCompound)
	{
		GoBackNCommand* receivedPCI = getTestee<GoBackN>()->getCommand(receivedCompound->getCommandPool());
		CompoundPtr compound(getFUN()->createCompound());
		GoBackNCommand* command = getTestee<GoBackN>()->activateCommand(compound->getCommandPool());

		command->peer.type = GoBackNCommand::ACK;
		command->setNS( receivedPCI->getNS() );
		return compound;
	} // createACKFrame

	wns::ldk::CompoundPtr
	GoBackNTest::createNAKFrame(const CompoundPtr& receivedCompound)
	{
		GoBackNCommand* receivedPCI = getTestee<GoBackN>()->getCommand(receivedCompound->getCommandPool());
		CompoundPtr compound(getFUN()->createCompound());
		GoBackNCommand* command = getTestee<GoBackN>()->activateCommand(compound->getCommandPool());

		command->peer.type = GoBackNCommand::NAK;
		// frame NS received, assuming NS-1 missing, so send NAK for NS-1:
		ARQCommand::SequenceNumber NS = receivedPCI->getNS();
		NS--; // (NS-1+sequenceNumberSize)%sequenceNumberSize
		command->setNS( NS );
		return compound;
	} // createNAKFrame


	wns::ldk::CompoundPtr
	GoBackNTest::createIFrame(const ARQCommand::SequenceNumber& number)
	{
		CompoundPtr compound(getFUN()->createCompound());
		GoBackNCommand* command = getTestee<GoBackN>()->activateCommand(compound->getCommandPool());

		command->peer.type = GoBackNCommand::I;
		command->setNS( number );
		return compound;
	} // createIFrame

}}}}


