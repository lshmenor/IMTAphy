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

#include <GLUE/arqfsm/selectiverepeat/FSMFU.hpp>

#include <WNS/ldk/Layer.hpp>
#include <WNS/ldk/fun/FUN.hpp>
#include <WNS/ldk/buffer/Bounded.hpp>
#include <WNS/ldk/tools/Stub.hpp>
#include <WNS/ldk/tools/Bridge.hpp>
#include <WNS/ldk/tools/Producer.hpp>
#include <WNS/ldk/tools/Consumer.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>

#include <WNS/Exception.hpp>
#include <WNS/CppUnit.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <iostream>

namespace glue { namespace arqfsm { namespace selectiverepeat {

	class SelectiveRepeatFSMTest :
		public wns::TestFixture
	{
		CPPUNIT_TEST_SUITE( SelectiveRepeatFSMTest );
		CPPUNIT_TEST( testFillWindow );
		CPPUNIT_TEST( testSendOnACK );
		CPPUNIT_TEST( testRetransmissionIFrame );
		CPPUNIT_TEST( testRetransmissionIFramePending );
		CPPUNIT_TEST( testRetransmissionIFrame2 );
		CPPUNIT_TEST( testDuplicateACKFrame );
		CPPUNIT_TEST( testSequenceNumberACKFrameNotInWindow );
		CPPUNIT_TEST( testSequenceNumberACKFrameOutOfBound );
		CPPUNIT_TEST( testSendACKFrame );
		CPPUNIT_TEST( testOutOfSequenceIFrames );
		CPPUNIT_TEST( testDuplicateIFrame );
		CPPUNIT_TEST( testSendACKFramePending );
		CPPUNIT_TEST( testDuplicateOutOfSequenceIFrames );
		CPPUNIT_TEST( testSequenceNumberIFrameOutOfBound );
		CPPUNIT_TEST( testSpeedWithLoss );
		CPPUNIT_TEST_SUITE_END();

	public:
 		void prepare();
		void cleanup();

		void testFillWindow();
		void testSendOnACK();
		void testRetransmissionIFrame();
		void testRetransmissionIFramePending();
		void testRetransmissionIFrame2();
		void testDuplicateACKFrame();
		void testSequenceNumberACKFrameNotInWindow();
		void testSequenceNumberACKFrameOutOfBound();
		void testSendACKFrame();
		void testOutOfSequenceIFrames();
		void testDuplicateIFrame();
		void testSendACKFramePending();
		void testDuplicateOutOfSequenceIFrames();
		void testSequenceNumberIFrameOutOfBound();
		void testSpeedWithLoss();

		wns::ldk::CompoundPtr
		createIFrame(int number);

		wns::ldk::CompoundPtr
		createACKFrame(wns::ldk::CompoundPtr compound);

	private:
		wns::ldk::Layer* layer;
		wns::ldk::fun::FUN* fun;

		wns::ldk::tools::Stub* upper;
		wns::ldk::buffer::Bounded* buffer;
		FSMFU* arq;
		wns::ldk::tools::Stub* lower;

		static const int windowSize;
		static const int sequenceNumberSize;

		wns::events::scheduler::Interface* scheduler;
};

	CPPUNIT_TEST_SUITE_REGISTRATION( SelectiveRepeatFSMTest );

	const int
	SelectiveRepeatFSMTest::windowSize = 3;

	const int
	SelectiveRepeatFSMTest::sequenceNumberSize = 2*windowSize;

	void
	SelectiveRepeatFSMTest::prepare()
	{
		wns::ldk::CommandProxy::clearRegistries();

		scheduler = wns::simulator::getEventScheduler();
		scheduler->reset();

		layer = new wns::ldk::tests::LayerStub();

		wns::pyconfig::Parser FUNPyCo;
		FUNPyCo.loadString("import wns.Logger\n"
				"class LinkHandler:\n"
				"  type = \"wns.ldk.SequentlyCallingLinkHandler\"\n"
				"  isAcceptingLogger = wns.Logger.Logger(\"W-NS\", \"LinkHandler\", True)\n"
				"  sendDataLogger = wns.Logger.Logger(\"W-NS\", \"LinkHandler\", True)\n"
				"  wakeupLogger = wns.Logger.Logger(\"W-NS\", \"LinkHandler\", True)\n"
				"  onDataLogger = wns.Logger.Logger(\"W-NS\", \"LinkHandler\", True)\n"
				"  traceCompoundJourney = True\n"
				"linkHandler = LinkHandler()\n"
				"class FUNConfig:\n"
				"  logger = wns.Logger.Logger(\"W-NS\",\"TestFUN\",True)\n"
				"  commandProxy = wns.FUN.CommandProxy(logger)\n"
				"fun = FUNConfig()");
		fun = new wns::ldk::fun::Main(layer, FUNPyCo);

		wns::pyconfig::Parser emptyConfig;
		upper = new wns::ldk::tools::Stub(fun, emptyConfig);

		wns::pyconfig::Parser BufferPyCo;
		BufferPyCo.loadString("import wns.Buffer\n"
				      "buffer = wns.Buffer.Bounded(size = 100, probingEnabled = False)\n");
		wns::pyconfig::View BufferView(BufferPyCo, "buffer");
		buffer = new wns::ldk::buffer::Bounded(fun, BufferView);

		std::stringstream ss;
		ss << "import glue.ARQFSM\n"
		   << "arq = glue.ARQFSM.SelectiveRepeat(\n"
		   << "  windowSize = " << windowSize << ",\n"
		   << "  sequenceNumberSize = " << sequenceNumberSize << ",\n"
		   << "  resendTimeout = 1.0\n"
		   << ")\n";
		wns::pyconfig::Parser ARQPyCo;
		ARQPyCo.loadString(ss.str());
		wns::pyconfig::View ARQView(ARQPyCo, "arq");
		arq = new FSMFU(fun, ARQView);

		lower = new wns::ldk::tools::Stub(fun, emptyConfig);

		upper
			->connect(buffer)
			->connect(arq)
			->connect(lower);

		fun->addFunctionalUnit("ARQ", arq);

		// assure clean scheduler
		CPPUNIT_ASSERT(!scheduler->processOneEvent());
	} // prepare

	void
	SelectiveRepeatFSMTest::cleanup()
	{
		delete upper;
		delete buffer;
		// deleted by fun (FUN)
		// delete arq;
		delete lower;

		delete fun;
		delete layer;
	} // cleanup

	void
	SelectiveRepeatFSMTest::testFillWindow()
	{
		for (int i = 0; i < (windowSize+1); ++i)
			upper->sendData(fun->createCompound());

		CPPUNIT_ASSERT_EQUAL(unsigned long int(1), buffer->getSize());
		CPPUNIT_ASSERT_EQUAL(size_t(windowSize), lower->sent.size());
		CPPUNIT_ASSERT_EQUAL(std::string("glue_arqfsm_selectiverepeat_WaitingForACKsBufferFull"), arq->getStateName());

		for (int i = 0; i < windowSize; ++i)
		{
			SelectiveRepeatCommand* command = arq->getCommand(lower->sent[i]->getCommandPool());
			CPPUNIT_ASSERT_EQUAL(SelectiveRepeatCommand::I, command->peer.type);
			CPPUNIT_ASSERT_EQUAL(i, command->peer.NS);
		}
	} // testFillWindow

	void
	SelectiveRepeatFSMTest::testSendOnACK()
	{
		for (int i = 0; i < (3*windowSize); ++i)
			upper->sendData(fun->createCompound());

		CPPUNIT_ASSERT_EQUAL(unsigned long int(2*windowSize), buffer->getSize());
		CPPUNIT_ASSERT_EQUAL(size_t(windowSize), lower->sent.size());
		CPPUNIT_ASSERT_EQUAL(std::string("glue_arqfsm_selectiverepeat_WaitingForACKsBufferFull"), arq->getStateName());

		for (int i = 0; i < windowSize; ++i)
			lower->onData(createACKFrame(lower->sent[i]));

		CPPUNIT_ASSERT_EQUAL(unsigned long int(windowSize), buffer->getSize());
		CPPUNIT_ASSERT_EQUAL(size_t(2*windowSize), lower->sent.size());
		CPPUNIT_ASSERT_EQUAL(std::string("glue_arqfsm_selectiverepeat_WaitingForACKsBufferFull"), arq->getStateName());

		for (int i = 0; i < windowSize; ++i)
			lower->onData(createACKFrame(lower->sent[i+windowSize]));

		CPPUNIT_ASSERT_EQUAL(unsigned long int(0), buffer->getSize());
		CPPUNIT_ASSERT_EQUAL(size_t(3*windowSize), lower->sent.size());
		CPPUNIT_ASSERT_EQUAL(std::string("glue_arqfsm_selectiverepeat_WaitingForACKsBufferFull"), arq->getStateName());

		for (int i = 0; i < (3*windowSize); ++i)
		{
			SelectiveRepeatCommand* command = arq->getCommand(lower->sent[i]->getCommandPool());
			CPPUNIT_ASSERT_EQUAL(SelectiveRepeatCommand::I, command->peer.type);
			CPPUNIT_ASSERT_EQUAL((i % sequenceNumberSize), command->peer.NS);
		}
	} // testSendOnACK

	void
	SelectiveRepeatFSMTest::testRetransmissionIFrame()
	{
		upper->sendData(fun->createCompound());
		CPPUNIT_ASSERT_EQUAL(size_t(1), lower->sent.size());
		CPPUNIT_ASSERT_EQUAL(std::string("glue_arqfsm_selectiverepeat_ReadyForTransmissionBufferPartlyFilled"), arq->getStateName());
		SelectiveRepeatCommand* command = arq->getCommand(lower->sent[0]->getCommandPool());
		CPPUNIT_ASSERT_EQUAL(SelectiveRepeatCommand::I, command->peer.type);
		CPPUNIT_ASSERT_EQUAL(0, command->peer.NS);

		// there should be only one event: the retransmission timer
		CPPUNIT_ASSERT(scheduler->processOneEvent());
		CPPUNIT_ASSERT_EQUAL(size_t(2), lower->sent.size());
		CPPUNIT_ASSERT_EQUAL(std::string("glue_arqfsm_selectiverepeat_ReadyForTransmissionBufferPartlyFilled"), arq->getStateName());
		command = arq->getCommand(lower->sent[1]->getCommandPool());
		CPPUNIT_ASSERT_EQUAL(SelectiveRepeatCommand::I, command->peer.type);
		CPPUNIT_ASSERT_EQUAL(0, command->peer.NS);

		// just to be sure... :)
		CPPUNIT_ASSERT(scheduler->processOneEvent());
		CPPUNIT_ASSERT_EQUAL(size_t(3), lower->sent.size());
		CPPUNIT_ASSERT_EQUAL(std::string("glue_arqfsm_selectiverepeat_ReadyForTransmissionBufferPartlyFilled"), arq->getStateName());
		command = arq->getCommand(lower->sent[2]->getCommandPool());
		CPPUNIT_ASSERT_EQUAL(SelectiveRepeatCommand::I, command->peer.type);
		CPPUNIT_ASSERT_EQUAL(0, command->peer.NS);

		// send an ACK
		wns::ldk::CompoundPtr ackFrame = createACKFrame(lower->sent[0]);
		lower->onData(ackFrame);

		CPPUNIT_ASSERT_EQUAL(std::string("glue_arqfsm_selectiverepeat_ReadyForTransmissionBufferEmpty"), arq->getStateName());

		// now the scheduler shouldn't contain any events and there
		// shouldn't be any new compounds in the lower layer.
		CPPUNIT_ASSERT(!scheduler->processOneEvent());
		CPPUNIT_ASSERT_EQUAL(size_t(3), lower->sent.size());
	} // testRetransmissionIFrame

	void
	SelectiveRepeatFSMTest::testRetransmissionIFramePending()
	{
		upper->sendData(fun->createCompound());
		CPPUNIT_ASSERT_EQUAL(size_t(1), lower->sent.size());
		CPPUNIT_ASSERT_EQUAL(std::string("glue_arqfsm_selectiverepeat_ReadyForTransmissionBufferPartlyFilled"), arq->getStateName());

		lower->close();

		CPPUNIT_ASSERT(scheduler->processOneEvent());
		CPPUNIT_ASSERT(!scheduler->processOneEvent());

		lower->open(true);

		CPPUNIT_ASSERT_EQUAL(size_t(2), lower->sent.size());

		SelectiveRepeatCommand* command = arq->getCommand(lower->sent[0]->getCommandPool());
		CPPUNIT_ASSERT_EQUAL(SelectiveRepeatCommand::I, command->peer.type);
		CPPUNIT_ASSERT_EQUAL(0, command->peer.NS);

		command = arq->getCommand(lower->sent[1]->getCommandPool());
		CPPUNIT_ASSERT_EQUAL(SelectiveRepeatCommand::I, command->peer.type);
		CPPUNIT_ASSERT_EQUAL(0, command->peer.NS);

		CPPUNIT_ASSERT(scheduler->processOneEvent());
		CPPUNIT_ASSERT_EQUAL(size_t(3), lower->sent.size());
		command = arq->getCommand(lower->sent[2]->getCommandPool());
		CPPUNIT_ASSERT_EQUAL(SelectiveRepeatCommand::I, command->peer.type);
		CPPUNIT_ASSERT_EQUAL(0, command->peer.NS);

		// send an ACK
		wns::ldk::CompoundPtr ackFrame = createACKFrame(lower->sent[0]);
		lower->onData(ackFrame);

		CPPUNIT_ASSERT_EQUAL(std::string("glue_arqfsm_selectiverepeat_ReadyForTransmissionBufferEmpty"), arq->getStateName());

		// now the scheduler shouldn't contain any events and there
		// shouldn't be any new compounds in the lower layer.
		CPPUNIT_ASSERT(!scheduler->processOneEvent());
		CPPUNIT_ASSERT_EQUAL(size_t(3), lower->sent.size());
	} // testRetransmissionIFramePending

	void
	SelectiveRepeatFSMTest::testRetransmissionIFrame2()
	{
		for (int i = 0; i < (windowSize+3); ++i)
			upper->sendData(fun->createCompound());

		CPPUNIT_ASSERT_EQUAL(unsigned long int(3), buffer->getSize());
		CPPUNIT_ASSERT_EQUAL(size_t(windowSize), lower->sent.size());
		CPPUNIT_ASSERT_EQUAL(std::string("glue_arqfsm_selectiverepeat_WaitingForACKsBufferFull"), arq->getStateName());

		// send an ACK for frame 2
		wns::ldk::CompoundPtr ackFrame = createACKFrame(lower->sent[2]);
		lower->onData(ackFrame);

		CPPUNIT_ASSERT_EQUAL(unsigned long int(3), buffer->getSize());
		CPPUNIT_ASSERT_EQUAL(size_t(windowSize), lower->sent.size());
		CPPUNIT_ASSERT_EQUAL(std::string("glue_arqfsm_selectiverepeat_WaitingForACKsBufferFull"), arq->getStateName());

		// send an ACK for frame 0
		ackFrame = createACKFrame(lower->sent[0]);
		lower->onData(ackFrame);

		CPPUNIT_ASSERT_EQUAL(unsigned long int(2), buffer->getSize());
		CPPUNIT_ASSERT_EQUAL(size_t(windowSize+1), lower->sent.size());
		CPPUNIT_ASSERT_EQUAL(std::string("glue_arqfsm_selectiverepeat_WaitingForACKsBufferFull"), arq->getStateName());

		CPPUNIT_ASSERT(scheduler->processOneEvent());
		CPPUNIT_ASSERT_EQUAL(size_t(windowSize+2), lower->sent.size());
		CPPUNIT_ASSERT_EQUAL(std::string("glue_arqfsm_selectiverepeat_WaitingForACKsBufferFull"), arq->getStateName());
		SelectiveRepeatCommand* command = arq->getCommand(lower->sent[windowSize+1]->getCommandPool());
		CPPUNIT_ASSERT_EQUAL(SelectiveRepeatCommand::I, command->peer.type);
		CPPUNIT_ASSERT_EQUAL(1, command->peer.NS);

		// send an ACK for frame 1
		ackFrame = createACKFrame(lower->sent[1]);
		lower->onData(ackFrame);

		CPPUNIT_ASSERT_EQUAL(unsigned long int(0), buffer->getSize());
		CPPUNIT_ASSERT_EQUAL(size_t(windowSize+4), lower->sent.size());
		CPPUNIT_ASSERT_EQUAL(std::string("glue_arqfsm_selectiverepeat_WaitingForACKsBufferFull"), arq->getStateName());
	} // testRetransmissionIFrame2

	void
	SelectiveRepeatFSMTest::testDuplicateACKFrame()
	{
		for (int i = 0; i < (windowSize+1); ++i)
			upper->sendData(fun->createCompound());

		CPPUNIT_ASSERT_EQUAL(unsigned long int(1), buffer->getSize());
		CPPUNIT_ASSERT_EQUAL(size_t(windowSize), lower->sent.size());
		CPPUNIT_ASSERT_EQUAL(std::string("glue_arqfsm_selectiverepeat_WaitingForACKsBufferFull"), arq->getStateName());

		wns::ldk::CompoundPtr ackFrame = createACKFrame(lower->sent[1]);
		lower->onData(ackFrame);

		CPPUNIT_ASSERT_EQUAL(unsigned long int(1), buffer->getSize());
		CPPUNIT_ASSERT_EQUAL(size_t(windowSize), lower->sent.size());
		CPPUNIT_ASSERT_EQUAL(std::string("glue_arqfsm_selectiverepeat_WaitingForACKsBufferFull"), arq->getStateName());

		lower->onData(ackFrame);

		CPPUNIT_ASSERT_EQUAL(unsigned long int(1), buffer->getSize());
		CPPUNIT_ASSERT_EQUAL(size_t(windowSize), lower->sent.size());
		CPPUNIT_ASSERT_EQUAL(std::string("glue_arqfsm_selectiverepeat_WaitingForACKsBufferFull"), arq->getStateName());
	} // testDuplicateACKFrame

	void
	SelectiveRepeatFSMTest::testSequenceNumberACKFrameNotInWindow()
	{
		for (int i = 0; i < (windowSize+2); ++i)
			upper->sendData(fun->createCompound());

		CPPUNIT_ASSERT_EQUAL(unsigned long int(2), buffer->getSize());
		CPPUNIT_ASSERT_EQUAL(size_t(windowSize), lower->sent.size());
		CPPUNIT_ASSERT_EQUAL(std::string("glue_arqfsm_selectiverepeat_WaitingForACKsBufferFull"), arq->getStateName());

		wns::ldk::CompoundPtr ackFrame = createACKFrame(lower->sent[0]);
		lower->onData(ackFrame);

		CPPUNIT_ASSERT_EQUAL(unsigned long int(1), buffer->getSize());
		CPPUNIT_ASSERT_EQUAL(size_t(windowSize+1), lower->sent.size());
		CPPUNIT_ASSERT_EQUAL(std::string("glue_arqfsm_selectiverepeat_WaitingForACKsBufferFull"), arq->getStateName());

		lower->onData(ackFrame);

		CPPUNIT_ASSERT_EQUAL(unsigned long int(1), buffer->getSize());
		CPPUNIT_ASSERT_EQUAL(size_t(windowSize+1), lower->sent.size());
		CPPUNIT_ASSERT_EQUAL(std::string("glue_arqfsm_selectiverepeat_WaitingForACKsBufferFull"), arq->getStateName());
	} // testSequenceNumberACKFrameNotInWindow

	void
	SelectiveRepeatFSMTest::testSequenceNumberACKFrameOutOfBound()
	{
		upper->sendData(fun->createCompound());
		CPPUNIT_ASSERT_EQUAL(size_t(1), lower->sent.size());
		CPPUNIT_ASSERT_EQUAL(std::string("glue_arqfsm_selectiverepeat_ReadyForTransmissionBufferPartlyFilled"), arq->getStateName());

		wns::ldk::CompoundPtr compound(fun->createCompound());
		SelectiveRepeatCommand* command = arq->activateCommand(compound->getCommandPool());

		command->peer.type = SelectiveRepeatCommand::ACK;
		command->peer.NS = 2*sequenceNumberSize;

		CPPUNIT_ASSERT_THROW(lower->onData(compound), wns::Exception);
	} // testSequenceNumberACKFrameOutOfBound

	void
	SelectiveRepeatFSMTest::testSendACKFrame()
	{
		for (int i = 0; i < windowSize; ++i)
		{
			wns::ldk::CompoundPtr iFrame = createIFrame(i);
			lower->onData(iFrame);
			wns::ldk::CompoundPtr compound = lower->sent[i];
			SelectiveRepeatCommand* command = arq->getCommand(compound->getCommandPool());
			CPPUNIT_ASSERT_EQUAL(size_t(i+1), lower->sent.size());
			CPPUNIT_ASSERT_EQUAL(SelectiveRepeatCommand::ACK, command->peer.type);
			CPPUNIT_ASSERT_EQUAL(i, command->peer.NS);
			CPPUNIT_ASSERT_EQUAL(size_t(i+1), upper->received.size());
			CPPUNIT_ASSERT_EQUAL(iFrame, upper->received[i]);
		}
	} // testSendACKFrame

	void
	SelectiveRepeatFSMTest::testOutOfSequenceIFrames()
	{
		wns::ldk::CompoundPtr iFrame0 = createIFrame(0);
		wns::ldk::CompoundPtr iFrame1 = createIFrame(1);
		wns::ldk::CompoundPtr iFrame2 = createIFrame(2);

		lower->onData(iFrame1);
		SelectiveRepeatCommand* command = arq->getCommand(lower->sent[0]->getCommandPool());
		CPPUNIT_ASSERT_EQUAL(SelectiveRepeatCommand::ACK, command->peer.type);
		CPPUNIT_ASSERT_EQUAL(1, command->peer.NS);
		CPPUNIT_ASSERT_EQUAL(size_t(0), upper->received.size());

		lower->onData(iFrame2);
		command = arq->getCommand(lower->sent[1]->getCommandPool());
		CPPUNIT_ASSERT_EQUAL(SelectiveRepeatCommand::ACK, command->peer.type);
		CPPUNIT_ASSERT_EQUAL(2, command->peer.NS);
		CPPUNIT_ASSERT_EQUAL(size_t(0), upper->received.size());

		lower->onData(iFrame0);
		command = arq->getCommand(lower->sent[2]->getCommandPool());
		CPPUNIT_ASSERT_EQUAL(SelectiveRepeatCommand::ACK, command->peer.type);
		CPPUNIT_ASSERT_EQUAL(0, command->peer.NS);
		CPPUNIT_ASSERT_EQUAL(size_t(3), upper->received.size());
		CPPUNIT_ASSERT_EQUAL(iFrame0, upper->received[0]);
		CPPUNIT_ASSERT_EQUAL(iFrame1, upper->received[1]);
		CPPUNIT_ASSERT_EQUAL(iFrame2, upper->received[2]);
	} // testOutOfSequenceIFrames

	void
	SelectiveRepeatFSMTest::testDuplicateIFrame()
	{
		wns::ldk::CompoundPtr iFrame = createIFrame(0);
		lower->onData(iFrame);

		CPPUNIT_ASSERT_EQUAL(size_t(1), lower->sent.size());
		CPPUNIT_ASSERT_EQUAL(size_t(1), upper->received.size());
		CPPUNIT_ASSERT_EQUAL(iFrame, upper->received[0]);

		lower->onData(iFrame);

		CPPUNIT_ASSERT_EQUAL(size_t(2), lower->sent.size());
		SelectiveRepeatCommand* command = arq->getCommand(lower->sent[1]->getCommandPool());
		CPPUNIT_ASSERT_EQUAL(SelectiveRepeatCommand::ACK, command->peer.type);
		CPPUNIT_ASSERT_EQUAL(0, command->peer.NS);
		CPPUNIT_ASSERT_EQUAL(size_t(1), upper->received.size());
	} // testDuplicateIFrame

	void
	SelectiveRepeatFSMTest::testSendACKFramePending()
	{
		lower->close();

		wns::ldk::CompoundPtr iFrame = createIFrame(0);
		lower->onData(iFrame);
		lower->onData(iFrame);
		lower->onData(iFrame);

		CPPUNIT_ASSERT_EQUAL(size_t(0), lower->sent.size());

		lower->open(true);

		CPPUNIT_ASSERT_EQUAL(size_t(1), lower->sent.size());
		SelectiveRepeatCommand* command = arq->getCommand(lower->sent[0]->getCommandPool());
		CPPUNIT_ASSERT_EQUAL(SelectiveRepeatCommand::ACK, command->peer.type);
		CPPUNIT_ASSERT_EQUAL(0, command->peer.NS);
	} // testSendACKFramePending

	void
	SelectiveRepeatFSMTest::testDuplicateOutOfSequenceIFrames()
	{
		wns::ldk::CompoundPtr iFrame0 = createIFrame(0);
		wns::ldk::CompoundPtr iFrame1 = createIFrame(1);
		wns::ldk::CompoundPtr iFrame2 = createIFrame(2);

		lower->onData(iFrame2);
		CPPUNIT_ASSERT_EQUAL(size_t(1), lower->sent.size());
		SelectiveRepeatCommand* command = arq->getCommand(lower->sent[0]->getCommandPool());
		CPPUNIT_ASSERT_EQUAL(SelectiveRepeatCommand::ACK, command->peer.type);
		CPPUNIT_ASSERT_EQUAL(2, command->peer.NS);
		CPPUNIT_ASSERT_EQUAL(size_t(0), upper->received.size());

		lower->onData(iFrame1);
		CPPUNIT_ASSERT_EQUAL(size_t(2), lower->sent.size());
		command = arq->getCommand(lower->sent[1]->getCommandPool());
		CPPUNIT_ASSERT_EQUAL(SelectiveRepeatCommand::ACK, command->peer.type);
		CPPUNIT_ASSERT_EQUAL(1, command->peer.NS);
		CPPUNIT_ASSERT_EQUAL(size_t(0), upper->received.size());

		lower->onData(iFrame2);
		CPPUNIT_ASSERT_EQUAL(size_t(3), lower->sent.size());
		command = arq->getCommand(lower->sent[2]->getCommandPool());
		CPPUNIT_ASSERT_EQUAL(SelectiveRepeatCommand::ACK, command->peer.type);
		CPPUNIT_ASSERT_EQUAL(2, command->peer.NS);
		CPPUNIT_ASSERT_EQUAL(size_t(0), upper->received.size());

		lower->onData(iFrame1);
		CPPUNIT_ASSERT_EQUAL(size_t(4), lower->sent.size());
		command = arq->getCommand(lower->sent[3]->getCommandPool());
		CPPUNIT_ASSERT_EQUAL(SelectiveRepeatCommand::ACK, command->peer.type);
		CPPUNIT_ASSERT_EQUAL(1, command->peer.NS);
		CPPUNIT_ASSERT_EQUAL(size_t(0), upper->received.size());

		lower->onData(iFrame0);
		CPPUNIT_ASSERT_EQUAL(size_t(5), lower->sent.size());
		command = arq->getCommand(lower->sent[4]->getCommandPool());
		CPPUNIT_ASSERT_EQUAL(SelectiveRepeatCommand::ACK, command->peer.type);
		CPPUNIT_ASSERT_EQUAL(0, command->peer.NS);
		CPPUNIT_ASSERT_EQUAL(size_t(3), upper->received.size());
		CPPUNIT_ASSERT_EQUAL(iFrame0, upper->received[0]);
		CPPUNIT_ASSERT_EQUAL(iFrame1, upper->received[1]);
		CPPUNIT_ASSERT_EQUAL(iFrame2, upper->received[2]);
	} // testDuplicateOutOfSequenceIFrames

	void
	SelectiveRepeatFSMTest::testSequenceNumberIFrameOutOfBound()
	{
		wns::ldk::CompoundPtr compound = createIFrame(2*sequenceNumberSize);
		CPPUNIT_ASSERT_THROW(lower->onData(compound), wns::Exception);
	} // testSequenceNumberIFrameOutOfBound

	void
	SelectiveRepeatFSMTest::testSpeedWithLoss()
	{
		wns::ldk::CommandProxy::clearRegistries();

		const int maxIterations = 50;

		wns::ldk::Layer *leftLayer = new wns::ldk::tests::LayerStub();
		wns::pyconfig::Parser FUNPyCo;
		FUNPyCo.loadString("import wns.Logger\n"
				   "class LinkHandler:\n"
				   "  type = \"wns.ldk.SimpleLinkHandler\"\n"
				   "  isAcceptingLogger = wns.Logger.Logger(\"W-NS\", \"LinkHandler\", True)\n"
				   "  sendDataLogger = wns.Logger.Logger(\"W-NS\", \"LinkHandler\", True)\n"
				   "  wakeupLogger = wns.Logger.Logger(\"W-NS\", \"LinkHandler\", True)\n"
				   "  onDataLogger = wns.Logger.Logger(\"W-NS\", \"LinkHandler\", True)\n"
				   "  traceCompoundJourney = True\n"
				   "linkHandler = LinkHandler()\n"
				   "class FUNConfig:\n"
				   "  logger = wns.Logger.Logger(\"W-NS\",\"TestFUN\",True)\n"
				   "  commandProxy = wns.FUN.CommandProxy(logger)\n"
				   "fun = FUNConfig()");
		wns::ldk::fun::FUN* leftFUN = new wns::ldk::fun::Main(leftLayer, FUNPyCo);

		wns::ldk::Layer *rightLayer = new wns::ldk::tests::LayerStub();
		wns::ldk::fun::FUN* rightFUN = new wns::ldk::fun::Main(rightLayer, FUNPyCo);

		std::stringstream ss;
		ss << "import glue.ARQFSM\n"
		   << "arq = glue.ARQFSM.SelectiveRepeat(\n"
		   << "  windowSize = " << windowSize << ",\n"
		   << "  sequenceNumberSize = " << sequenceNumberSize << ",\n"
		   << "  resendTimeout = 1.0\n"
		   << ")\n";
		wns::pyconfig::Parser ARQPyCo;
		ARQPyCo.loadString(ss.str());
		wns::pyconfig::View ARQView(ARQPyCo, "arq");

		FSMFU *leftARQ = new FSMFU(leftFUN,  ARQView);
		FSMFU *rightARQ = new FSMFU(rightFUN, ARQView);

		wns::pyconfig::Parser emptyConfig;
		wns::ldk::tools::Stub* stepper = new wns::ldk::tools::Stub(leftFUN, emptyConfig);
		stepper->setStepping(true);

		leftFUN->addFunctionalUnit("ernie", leftARQ);
		rightFUN->addFunctionalUnit("ernie", rightARQ);

		// the last parameter is the error probability
		wns::ldk::tools::Bridge* bridge = new wns::ldk::tools::Bridge(leftFUN, rightFUN, 0.0);
		leftARQ
			->connect(stepper)
			->connect(bridge->getLeft());

		rightARQ
			->connect(bridge->getRight());

		wns::ldk::tools::Producer* leftUpper = new wns::ldk::tools::Producer(leftFUN);
		wns::ldk::tools::Consumer* rightUpper = new wns::ldk::tools::Consumer(rightFUN);
		rightUpper->consumeIncoming();

		leftUpper
			->connect(leftARQ);

		rightUpper
			->connect(rightARQ);

		CPPUNIT_ASSERT_EQUAL(size_t(0), stepper->received.size());
		leftUpper->wakeup();

		for(int l = 0; l < maxIterations; ++l)
		{
			stepper->step();
			if(stepper->received.size() == 15)
				break;
			wns::simulator::getEventScheduler()->processOneEvent();
		}

		delete leftUpper;
		// deleted by leftFun (FUN)
		// delete leftARQ;
		delete stepper;

		delete rightUpper;
		// deleted by rightFun (FUN)
		// delete rightARQ;

		delete bridge;

		delete leftFUN;
		delete rightFUN;
		delete leftLayer;
		delete rightLayer;
	} // testSpeedWithLoss

	wns::ldk::CompoundPtr
	SelectiveRepeatFSMTest::createIFrame(int number)
	{
		wns::ldk::CompoundPtr compound(fun->createCompound());
		SelectiveRepeatCommand* command = arq->activateCommand(compound->getCommandPool());

		command->peer.type = SelectiveRepeatCommand::I;
		command->peer.NS = number;
		return compound;
	} // createIFrame

	wns::ldk::CompoundPtr
	SelectiveRepeatFSMTest::createACKFrame(wns::ldk::CompoundPtr receivedCompound)
	{
		SelectiveRepeatCommand* receivedPCI = arq->getCommand(receivedCompound->getCommandPool());
		wns::ldk::CompoundPtr compound(fun->createCompound());
		SelectiveRepeatCommand* command = arq->activateCommand(compound->getCommandPool());

		command->peer.type = SelectiveRepeatCommand::ACK;
		command->peer.NS = receivedPCI->peer.NS;
		return compound;
	} // createACKFrame

} // selectiverepeat
} // arqfsm
} // glue


