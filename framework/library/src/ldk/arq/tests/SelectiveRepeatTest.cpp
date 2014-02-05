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
#include <WNS/ldk/arq/SelectiveRepeat.hpp>
#include <WNS/ldk/buffer/Bounded.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>

#include <WNS/ldk/tools/Bridge.hpp>
#include <WNS/ldk/tools/Consumer.hpp>
#include <WNS/ldk/tools/Producer.hpp>

#include <WNS/events/NoOp.hpp>
#include <WNS/pyconfig/Parser.hpp>

#include <cppunit/extensions/HelperMacros.h>

#include <vector>

namespace wns { namespace ldk { namespace arq { namespace tests {

	class SelectiveRepeatTest :
		public wns::ldk::tests::DelayedInterfaceTest
	{
		CPPUNIT_TEST_SUB_SUITE( SelectiveRepeatTest, wns::ldk::tests::DelayedInterfaceTest );
		CPPUNIT_TEST( fillWindow );
		CPPUNIT_TEST( sendOnAck );
		CPPUNIT_TEST( multipleFillWindow );
		CPPUNIT_TEST( sendAck );
		CPPUNIT_TEST( retransmission );
		CPPUNIT_TEST( retransmission2 );
		CPPUNIT_TEST( outOfSeqRetransmission );
		CPPUNIT_TEST( duplicateIFrames );
		CPPUNIT_TEST( duplicateOutOfSequenceIFrames );
		CPPUNIT_TEST( totallyOutOfSequenceIFrames );
		CPPUNIT_TEST( retransmissionState );
		CPPUNIT_TEST( retransmissionState2 );
		CPPUNIT_TEST( ACKPending );
		CPPUNIT_TEST( outOfSeqIFrames );
		CPPUNIT_TEST( speedWithLoss );
		CPPUNIT_TEST( keepSorted );
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
		retransmissionState2();

		void
		ACKPending();

		void
		outOfSeqIFrames();

		void
		speedWithLoss();

		void
		keepSorted();

	private:
		virtual void
		prepare();

		virtual void
		setUpTestFUs();

		virtual SelectiveRepeat*
		newTestee();

		virtual void
		tearDownTestee(DelayedInterface*)
		{
		}

		virtual FunctionalUnit*
		getUpperTestFU() const;

		wns::ldk::CompoundPtr
		createACKFrame(const CompoundPtr& receivedCompound);

		wns::ldk::CompoundPtr
		createIFrame(const ARQCommand::SequenceNumber& number);

		wns::ldk::buffer::Bounded* buffer;

		static const int windowSize;
		static const int sequenceNumberSize;
	};

	CPPUNIT_TEST_SUITE_REGISTRATION( SelectiveRepeatTest );

	const int
	SelectiveRepeatTest::windowSize = 4;

	const int
	SelectiveRepeatTest::sequenceNumberSize = 2*windowSize;

	void
	SelectiveRepeatTest::prepare()
	{
		wns::ldk::CommandProxy::clearRegistries();

		wns::ldk::tests::DelayedInterfaceTest::prepare();

		wns::simulator::getEventScheduler()->reset();
	} // prepare

	void
	SelectiveRepeatTest::setUpTestFUs()
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

	SelectiveRepeat*
	SelectiveRepeatTest::newTestee()
	{
		std::stringstream ss;
		ss << "from openwns.ARQ import SelectiveRepeat\n"
		   << "arq = SelectiveRepeat(\n"
		   << "  probeName = 'unused',\n"
		   << "  windowSize = " << windowSize << ",\n"
		   << "  sequenceNumberSize = " << sequenceNumberSize << ",\n"
		   << "  resendTimeout = 1.0,\n"
		   << "  useProbe = False\n"
		   << ")\n";

		wns::pyconfig::Parser all;
		all.loadString(ss.str());

		wns::pyconfig::View config(all, "arq");

		return new SelectiveRepeat(getFUN(), config);
	} // newTestee

	wns::ldk::FunctionalUnit*
	SelectiveRepeatTest::getUpperTestFU() const
	{
		return buffer;
	} // getUpperTestFU

	void
	SelectiveRepeatTest::fillWindow()
	{
		for (int i = 0; i< (windowSize+1) ; i++)
			getUpperStub()->sendData(getFUN()->createCompound());

		CPPUNIT_ASSERT_EQUAL(static_cast<unsigned long int>(1), buffer->getSize());
		CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(windowSize), compoundsSent());
	} // fillWindow


	void
	SelectiveRepeatTest::sendOnAck()
	{
		for (int i = 0; i< (windowSize+1) ; i++)
			getUpperStub()->sendData(getFUN()->createCompound());

		CPPUNIT_ASSERT(buffer->getSize() == 1);
		CPPUNIT_ASSERT(compoundsSent() == (unsigned long int)(windowSize));

		CompoundPtr ackFrame = createACKFrame(getLowerStub()->sent[0]);
		getLowerStub()->onData(ackFrame);

		CPPUNIT_ASSERT(buffer->getSize() == 0);
		CPPUNIT_ASSERT(compoundsSent() == (unsigned long int)(windowSize + 1));
	} // sendOnAck


	void
	SelectiveRepeatTest::multipleFillWindow()
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
	SelectiveRepeatTest::sendAck()
	{
		CompoundPtr compound = createIFrame(0);
		getLowerStub()->onData(compound);

		CPPUNIT_ASSERT(compoundsDelivered() == 1);
		CPPUNIT_ASSERT(getUpperStub()->received[0] == compound);

		CPPUNIT_ASSERT(compoundsSent() == 1);

		compound = getLowerStub()->sent[0];
		SelectiveRepeatCommand* command = getTestee<SelectiveRepeat>()->getCommand(compound->getCommandPool());
		CPPUNIT_ASSERT(command->peer.type == SelectiveRepeatCommand::ACK);
		CPPUNIT_ASSERT(command->getNS()   == 0);
	} // sendAck


	void
	SelectiveRepeatTest::retransmission()
	{
		wns::events::scheduler::Interface *scheduler = wns::simulator::getEventScheduler();
		CPPUNIT_ASSERT(!scheduler->processOneEvent()); // assure clean scheduler

		getUpperStub()->sendData(getFUN()->createCompound());
		CPPUNIT_ASSERT(compoundsSent() == 1);

		// there should be only one event: the retransmission timer
		scheduler->processOneEvent();
		CPPUNIT_ASSERT(compoundsSent() == 2);

		// just to be sure... :)
		scheduler->processOneEvent();
		CPPUNIT_ASSERT(compoundsSent() == 3);

		// send an ACK
		CompoundPtr ackFrame = createACKFrame(getLowerStub()->sent[0]);
		getLowerStub()->onData(ackFrame);

		// now the scheduler shouldn't contain any events and there
		// shouldn't be any new compounds in the lower layer.
		CPPUNIT_ASSERT(!scheduler->processOneEvent());
		CPPUNIT_ASSERT(compoundsSent() == 3);
	} // retransmission


	void
	SelectiveRepeatTest::retransmission2()
	{
		wns::events::scheduler::Interface *scheduler = wns::simulator::getEventScheduler();
		CPPUNIT_ASSERT(!scheduler->processOneEvent()); // assure clean scheduler

		getUpperStub()->sendData(getFUN()->createCompound());
		getUpperStub()->sendData(getFUN()->createCompound());
		getUpperStub()->sendData(getFUN()->createCompound());
		getUpperStub()->sendData(getFUN()->createCompound());
		CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(4), compoundsSent());

		// send an ACK for the 2nd Frame (out of sequence)
		CompoundPtr ackFrame = createACKFrame(getLowerStub()->sent[1]);
		getLowerStub()->onData(ackFrame);

		CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(5), compoundsSent());

		// send an ACK for the 4th Frame
		ackFrame = createACKFrame(getLowerStub()->sent[3]);
		getLowerStub()->onData(ackFrame);

		CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(7), compoundsSent());

		// there should be only one event: the retransmission timer
		scheduler->processOneEvent();
		CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(9), compoundsSent());

		// just to be sure... :)
		scheduler->processOneEvent();
		CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(11), compoundsSent());

		// send an ACK for the 1st Frame
		ackFrame = createACKFrame(getLowerStub()->sent[0]);
		getLowerStub()->onData(ackFrame);

		CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(11), compoundsSent());

		// send an ACK for the 3rd Frame
		ackFrame = createACKFrame(getLowerStub()->sent[2]);
		getLowerStub()->onData(ackFrame);

		CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(11), compoundsSent());

		// now the scheduler shouldn't contain any events and there
		// shouldn't be any new compounds in the lower layer.
		CPPUNIT_ASSERT(!scheduler->processOneEvent());
		CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(11), compoundsSent());
	} // retransmission2


	void
	SelectiveRepeatTest::outOfSeqRetransmission()
	{
		for (int i = 0; i<(windowSize+1); i++)
		{
			getUpperStub()->sendData(getFUN()->createCompound());
		}

		CPPUNIT_ASSERT(buffer->getSize() == 1);
		CPPUNIT_ASSERT(compoundsSent() == (unsigned long int)(windowSize));

		// send an out-of-sequence ACK (the third, index 2)
		CompoundPtr ackFrame = createACKFrame(getLowerStub()->sent[2]);
		getLowerStub()->onData(ackFrame);

		CPPUNIT_ASSERT_EQUAL((unsigned long int)(1), buffer->getSize());
		CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(windowSize + 2), compoundsSent());

		// Check if the first and second frame (index 0 and 1)  have been repeated
		ARQCommand::SequenceNumber A =
			getTestee<SelectiveRepeat>()->getCommand(
				getLowerStub()->sent[ windowSize + 1 ]->getCommandPool()  )->getNS();

		ARQCommand::SequenceNumber B =
			getTestee<SelectiveRepeat>()->getCommand(
				getLowerStub()->sent[ 1              ]->getCommandPool()  )->getNS();

		ARQCommand::SequenceNumber C =
			getTestee<SelectiveRepeat>()->getCommand(
				getLowerStub()->sent[ windowSize     ]->getCommandPool()  )->getNS();

		ARQCommand::SequenceNumber D =
			getTestee<SelectiveRepeat>()->getCommand(
				getLowerStub()->sent[ 0              ]->getCommandPool()  )->getNS();

		CPPUNIT_ASSERT( A == B);
		CPPUNIT_ASSERT( C == D);
	} // outOfSeqRetransmission


	void
	SelectiveRepeatTest::duplicateIFrames()
	{
		ARQCommand::SequenceNumber mySN = 0;

		// send 5 subsequent I-Frames
		for (int i=0; i < 5; i++)
			{
				getLowerStub()->onData(createIFrame(mySN++));
			}
		CPPUNIT_ASSERT( compoundsSent()     == 5); // 5 ACKs sent?
		CPPUNIT_ASSERT( compoundsDelivered() == 5); // 5 Frames delivered?

		// resend the third Frame
		ARQCommand::SequenceNumber duplicateSN = 2;
		getLowerStub()->onData(createIFrame(duplicateSN));

		CPPUNIT_ASSERT( compoundsSent()     == 6); // 6 ACKs sent?
		CPPUNIT_ASSERT( compoundsDelivered() == 5); // 5 Frames delivered?
	} // duplicateIFrames


	void
	SelectiveRepeatTest::duplicateOutOfSequenceIFrames()
	{
		// send 2 subsequent out of sequence I-Frames
		for (int i=1; i < 3; i++)
		{
			getLowerStub()->onData(createIFrame(i));
		}
		CPPUNIT_ASSERT_EQUAL( 2, (int)compoundsSent() ); // 2 ACKs sent?
		CPPUNIT_ASSERT_EQUAL( 0, (int)compoundsDelivered() ); // No Frames delivered?

		// resend the third Frame
		ARQCommand::SequenceNumber duplicateSN = 2;
		getLowerStub()->onData(createIFrame(duplicateSN));

		CPPUNIT_ASSERT_EQUAL( 3, (int)compoundsSent() ); // 3 ACKs sent?
		CPPUNIT_ASSERT_EQUAL( 0, (int)compoundsDelivered() ); // Still no Frames
		// delivered?

		getLowerStub()->onData(createIFrame(0));

		CPPUNIT_ASSERT_EQUAL( 4, (int)compoundsSent() ); // 4 ACKs sent?
		CPPUNIT_ASSERT_EQUAL( 3, (int)compoundsDelivered() ); // 3 Frames delivered
	} // duplicateOutOfSequenceIFrames


	void
	SelectiveRepeatTest::totallyOutOfSequenceIFrames()
	{
		getLowerStub()->onData(createIFrame(3));

		CPPUNIT_ASSERT_EQUAL( 1, (int)compoundsSent() ); // 1 ACK sent?
		CPPUNIT_ASSERT_EQUAL( 0, (int)compoundsDelivered() ); // No Frames delivered?


		// send the second frame
		getLowerStub()->onData(createIFrame(1));

		CPPUNIT_ASSERT_EQUAL( 2, (int)compoundsSent() ); // 2 ACKs sent?
		CPPUNIT_ASSERT_EQUAL( 0, (int)compoundsDelivered() ); // No Frames delivered?

		// send the first frame
		getLowerStub()->onData(createIFrame(0));

		CPPUNIT_ASSERT_EQUAL( 3, (int)compoundsSent() ); // 3 ACKs sent?
		CPPUNIT_ASSERT_EQUAL( 2, (int)compoundsDelivered() ); // 2 frames received?

		// send the third frame
		getLowerStub()->onData(createIFrame(2));

		CPPUNIT_ASSERT_EQUAL( 4, (int)compoundsSent() ); // 4 ACKs sent?
		CPPUNIT_ASSERT_EQUAL( 4, (int)compoundsDelivered() ); // 4 Frames delivered
	} // totallyOutOfSequenceIFrames


	void
	SelectiveRepeatTest::retransmissionState()
	{
		wns::events::scheduler::Interface *scheduler = wns::simulator::getEventScheduler();
		CPPUNIT_ASSERT(!scheduler->processOneEvent()); // assure clean scheduler

		getLowerStub()->close();

		// send three packets
		for (int i = 0; i < 3; i++)
			getUpperStub()->sendData(getFUN()->createCompound());

		CPPUNIT_ASSERT_EQUAL( 0, (int)compoundsSent() ); // No Packets sent?

		for (int i = 0; i < 3; i++)
			getLowerStub()->step();

		CPPUNIT_ASSERT_EQUAL( 3, (int)compoundsSent() ); // 3 Packets sent?

		// acknowledge the receipt of the 3rd packet
		getLowerStub()->onData(createACKFrame(getLowerStub()->sent[2]));

		CPPUNIT_ASSERT_EQUAL( 3, (int)compoundsSent() ); // Nothing happened yet?

		// are we in retransmission state?
		CPPUNIT_ASSERT( getTestee<SelectiveRepeat>()->retransmissionState() );
		// with 2 frames to retransmit
		CPPUNIT_ASSERT_EQUAL( 2,  (int) getTestee<SelectiveRepeat>()->toRetransmit.size());

		// allow to send 1 retransmission
		getLowerStub()->step();
		// we  should still be in retransmission state
		CPPUNIT_ASSERT( getTestee<SelectiveRepeat>()->retransmissionState() );
		// and have one packet left
		CPPUNIT_ASSERT_EQUAL( 1,  (int) getTestee<SelectiveRepeat>()->toRetransmit.size());
		// 1 retransmission sent?
		CPPUNIT_ASSERT_EQUAL( 4,  (int) compoundsSent() );

		// acknowledge the receipt of the packet in the retransmission queue
		getLowerStub()->onData( createACKFrame(getTestee<SelectiveRepeat>()->toRetransmit.front()) );
		// accept to send one packet
		getLowerStub()->step();
		// we should have left retransmission state
		CPPUNIT_ASSERT( !getTestee<SelectiveRepeat>()->retransmissionState()  );
		getLowerStub()->open();

		// this should be the same ACK as before
		getLowerStub()->onData( createACKFrame( getLowerStub()->sent[1] ) );

		// we should not be in retransmission state
		CPPUNIT_ASSERT( !getTestee<SelectiveRepeat>()->retransmissionState()  );
		// but have 1 frame in the sentPDUs  buffer
		CPPUNIT_ASSERT_EQUAL( 1,  (int) getTestee<SelectiveRepeat>()->sentPDUs.size());
		// send the missing ACK
		getLowerStub()->onData( createACKFrame( getLowerStub()->sent[0] ) );

		// there should be nothing pending
		CPPUNIT_ASSERT( !scheduler->processOneEvent() );
		CPPUNIT_ASSERT( !getTestee<SelectiveRepeat>()->retransmissionState() );
		CPPUNIT_ASSERT( !getTestee<SelectiveRepeat>()->hasSomethingToSend() );
		CPPUNIT_ASSERT_EQUAL( 6,  (int) compoundsSent() );
	} // retransmissionState

	void
	SelectiveRepeatTest::retransmissionState2()
	{
		wns::events::scheduler::Interface *scheduler = wns::simulator::getEventScheduler();
		CPPUNIT_ASSERT(!scheduler->processOneEvent()); // assure clean scheduler

		getLowerStub()->close();

		// send three packets
		for (int i = 0; i < 4; i++)
			getUpperStub()->sendData(getFUN()->createCompound());

		CPPUNIT_ASSERT_EQUAL( 0, (int)compoundsSent() ); // No Packets sent?

		for (int i = 0; i < 4; i++)
			getLowerStub()->step();

		CPPUNIT_ASSERT_EQUAL( 4, (int)compoundsSent() ); // 4 Packets sent?

		// make one millisecond pass
		scheduler->scheduleDelay(wns::events::NoOp(), 1.0e-3);
		scheduler->processOneEvent();

		// acknowledge the receipt of the 3rd packet and thus trigger a
		// retransmission of the first two
		getLowerStub()->onData(createACKFrame(getLowerStub()->sent[2]));
		getLowerStub()->step();
		getLowerStub()->step();
		CPPUNIT_ASSERT_EQUAL( 6, (int)compoundsSent() );
		// have we left retransmission state?
		CPPUNIT_ASSERT( ! getTestee<SelectiveRepeat>()->retransmissionState() );

		// acknowledge the receipt of the 4th packet, should not trigger
		// any more retransmissions, since we haven't advanced in time
		getLowerStub()->onData(createACKFrame(getLowerStub()->sent[3]));
		getLowerStub()->step();
		getLowerStub()->step();
		CPPUNIT_ASSERT_EQUAL( 6, (int)compoundsSent() ); // Nothing happened

		// make another two milliseconds pass
		scheduler->scheduleDelay(wns::events::NoOp(), 2.0e-3);
		scheduler->processOneEvent();

		// acknowledge the receipt of the 2nd packet, should trigger
		// a retransmission of the first one, since more than 2*minRTT
		// have passsed since the last retransmission
		getLowerStub()->onData(createACKFrame(getLowerStub()->sent[1]));
		getLowerStub()->step();
		CPPUNIT_ASSERT_EQUAL( 7, (int)compoundsSent() ); // Nothing happened

		// have we left retransmission state?
		CPPUNIT_ASSERT( ! getTestee<SelectiveRepeat>()->retransmissionState() );
	} // retransmissionState2


	void
	SelectiveRepeatTest::ACKPending()
	{
		// stop automatic processing
		getLowerStub()->close();

		// send three packets
		getLowerStub()->onData(createIFrame(2));
		getLowerStub()->onData(createIFrame(0));
		getLowerStub()->onData(createIFrame(2)); // duplicated intentionally
		getLowerStub()->onData(createIFrame(1));

		getLowerStub()->step();
		CPPUNIT_ASSERT_EQUAL( 1, (int)compoundsSent() ); // 1 ACK sent?
		getLowerStub()->open();
		CPPUNIT_ASSERT_EQUAL( 4, (int)compoundsSent() ); // all ACKs sent?
		CPPUNIT_ASSERT_EQUAL( 3, (int)compoundsDelivered() ); // all frames delivered?
	} // ACKPending


	void
	SelectiveRepeatTest::outOfSeqIFrames()
	{
		// send windowSize-1 subsequent I-Frames
		for (int i=(windowSize-1); i > 0; i--) {
			getLowerStub()->onData(createIFrame(i));
		}
		CPPUNIT_ASSERT( compoundsSent()     == windowSize-1); // all ACKs sent?
		CPPUNIT_ASSERT( compoundsDelivered() == 0);            // 0 Frames delivered?

		// send the zeroe-th Frame
		ARQCommand::SequenceNumber missingSN = 0;
		getLowerStub()->onData(createIFrame(missingSN));

		CPPUNIT_ASSERT( compoundsSent()     == (unsigned long int)(windowSize)); // all ACKs sent?
		CPPUNIT_ASSERT( compoundsDelivered() == (unsigned long int)(windowSize)); // all Frames delivered?
	} // outOfSeqIFrames


	void
	SelectiveRepeatTest::speedWithLoss()
	{
		const long maxIterations = 75;

		ILayer *leftLayer = new wns::ldk::tests::LayerStub();
		fun::FUN* leftFUN = new fun::Main(leftLayer);
		ILayer *rightLayer = new wns::ldk::tests::LayerStub();
		fun::FUN* rightFUN = new fun::Main(rightLayer);
		wns::pyconfig::Parser arqConfig;

		std::stringstream ss;

		ss << "from openwns.ARQ import SelectiveRepeat\n"
		   << "arq = SelectiveRepeat(\n"
		   << "  probeName = 'TestTransmissionCounter',\n"
		   << "  windowSize = " << windowSize << ",\n"
		   << "  sequenceNumberSize = " << sequenceNumberSize << ",\n"
		   << "  resendTimeout = 1.0\n"
		   << ")\n";

		arqConfig.loadString(ss.str());
		pyconfig::View pcv = arqConfig.getView("arq");

		arq::SelectiveRepeat *leftARQ = new arq::SelectiveRepeat(leftFUN,  pcv);
		arq::SelectiveRepeat *rightARQ = new arq::SelectiveRepeat(rightFUN, pcv);

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
	SelectiveRepeatTest::keepSorted()
	{
		// CAUTION -- some hard-coded numbers in this
		// Test are directly related to the windowsize

		std::list<CompoundPtr> myList;

		const int v[4] = { 1, 0, 3, 2 };

		myList.clear();

		for (int i=0; i<4; i++)
		{
			getTestee<SelectiveRepeat>()->keepSorted(createIFrame(v[i]), myList);
		}

		int counter=0;
		std::list<CompoundPtr>::iterator i;

		for (i = myList.begin(); i != myList.end() ; i++)
		{
			CPPUNIT_ASSERT( getTestee<SelectiveRepeat>()->getCommand(((*i)->getCommandPool()))->getNS() == counter++ );
		}

		myList.clear();
	} // keepSorted


	wns::ldk::CompoundPtr
	SelectiveRepeatTest::createACKFrame(const CompoundPtr& receivedCompound)
	{
		SelectiveRepeatCommand* receivedPCI = getTestee<SelectiveRepeat>()->getCommand(receivedCompound->getCommandPool());
		CompoundPtr compound(getFUN()->createCompound());
		SelectiveRepeatCommand* command = getTestee<SelectiveRepeat>()->activateCommand(compound->getCommandPool());

		command->peer.type = SelectiveRepeatCommand::ACK;
		command->setNS( receivedPCI->getNS() );
		return compound;
	} // createACKFrame


	wns::ldk::CompoundPtr
	SelectiveRepeatTest::createIFrame(const ARQCommand::SequenceNumber& number)
	{
		CompoundPtr compound(getFUN()->createCompound());
		SelectiveRepeatCommand* command = getTestee<SelectiveRepeat>()->activateCommand(compound->getCommandPool());

		command->peer.type = SelectiveRepeatCommand::I;
		command->setNS( number );
		return compound;
	} // createIFrame

}}}}


