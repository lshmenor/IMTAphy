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

#include <WNS/pyconfig/View.hpp>
#include <WNS/pyconfig/helper/Functions.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/tools/Stub.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/CppUnit.hpp>

#include <TCP/CumulativeACK.hpp>



namespace tcp { namespace tests {

	class CumulativeACKTest :
		public wns::TestFixture
	{
		CPPUNIT_TEST_SUITE(CumulativeACKTest);
		CPPUNIT_TEST(sendDataStream);
		CPPUNIT_TEST(dropPacket);
		CPPUNIT_TEST(delayPacket);
		CPPUNIT_TEST(timeOut);
		CPPUNIT_TEST_SUITE_END();

	public:
		void prepare();
		void sendDataStream();
		void dropPacket();
		void delayPacket();
		void timeOut();
		void cleanup();

	private:
		std::stringstream ss;

		wns::pyconfig::Parser all;

		struct testSetup {
			wns::ldk::ILayer* layer;

			wns::ldk::fun::Main* fun;
			
			wns::ldk::tools::Stub* upper;
			CumulativeACK* cumACK;
			wns::ldk::tools::Stub* lower;
		} sender, receiver;

	};
} //tests
} //tcp


using namespace tcp::tests;

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(CumulativeACKTest, wns::testsuite::Default());


void
CumulativeACKTest::prepare()
{
	wns::pyconfig::Parser emptyConfig;

	ss << "import tcp.TCP\n"
	   << "cumACK = tcp.TCP.CumulativeACK()\n"
	   << "cumACK.congestionControl.slowStart.congestionWindow = 10\n"
	   << "cumACK.probing = False";

	all.loadString(ss.str());

	wns::pyconfig::View config(all, "cumACK");

	sender.layer = new wns::ldk::tests::LayerStub();
	sender.fun = new wns::ldk::fun::Main(sender.layer);
	sender.cumACK = new CumulativeACK(sender.fun, config);
	sender.lower = new wns::ldk::tools::Stub(sender.fun, emptyConfig);
	sender.upper = new wns::ldk::tools::Stub(sender.fun, emptyConfig);

	receiver.layer = new wns::ldk::tests::LayerStub();
	receiver.fun = new wns::ldk::fun::Main(receiver.layer);
	receiver.cumACK = new CumulativeACK(receiver.fun, config);
	receiver.lower = new wns::ldk::tools::Stub(receiver.fun, emptyConfig);
	receiver.upper = new wns::ldk::tools::Stub(receiver.fun, emptyConfig);

	sender.fun->addFunctionalUnit("sender.upper", sender.upper);
	sender.fun->addFunctionalUnit("sender.cumACK", sender.cumACK);
	sender.fun->addFunctionalUnit("sender.lower", sender.lower);

	receiver.fun->addFunctionalUnit("receiver.upper", receiver.upper);
	receiver.fun->addFunctionalUnit("sender.cumACK", receiver.cumACK);
	receiver.fun->addFunctionalUnit("receiver.lower", receiver.lower);

	sender.upper->connect(sender.cumACK);
	sender.cumACK->connect(sender.lower);

	receiver.upper->connect(receiver.cumACK);
	receiver.cumACK->connect(receiver.lower);

	wns::simulator::getEventScheduler()->reset();
	CPPUNIT_ASSERT(!wns::simulator::getEventScheduler()->processOneEvent());
}


void
CumulativeACKTest::sendDataStream()
{
	//set windowSize (implying increase of sendCredit())
	unsigned long int segments = 10;

	wns::ldk::CompoundPtr compound;

	for (unsigned long int i = 0; i < segments; i++)
	{
		compound = sender.fun->createCompound();

		sender.cumACK->doSendData(compound);
		receiver.lower->onData(compound);

		wns::ldk::CompoundPtr comp = sender.lower->sent.back();
		CumulativeACKCommand* cmd = sender.cumACK->getCommand(comp->getCommandPool());

		unsigned long int sequenceNumber = cmd->peer.sequenceNumber;

		comp = receiver.lower->sent.back();
		cmd = receiver.cumACK->getCommand(comp->getCommandPool());

		unsigned long int ackNumber = cmd->peer.ACKNumber;

		CPPUNIT_ASSERT_EQUAL(ackNumber, sequenceNumber + 1);
	}

	CPPUNIT_ASSERT_EQUAL(segments, sender.lower->sent.size());

	for (unsigned long int i = 0; i < segments; i++)
	{
		unsigned long int windowSize = sender.cumACK->sendCredit();

		wns::ldk::CompoundPtr comp = receiver.lower->sent[i];

		// sender receives acknowledgements
		sender.lower->onData(comp);

		// window size is being increased and
		// compound being sent is removed
		CPPUNIT_ASSERT_EQUAL(sender.cumACK->sendCredit(), windowSize + 2);
	}
}


void
CumulativeACKTest::dropPacket()
{
	unsigned long int segments = 10;

	unsigned long int droppedPacket = 6;

	for(unsigned long int i = 0; i < segments; ++i)
	{
		wns::ldk::CompoundPtr compound = sender.fun->createCompound();

		sender.cumACK->doSendData(compound);

		if (i != droppedPacket)
		{
			// simulate a packet loss
			receiver.lower->getDeliverer()->getAcceptor(compound)->onData(compound);

			// return the ack
			wns::ldk::CompoundPtr ackCompound = receiver.lower->sent.back();
			CPPUNIT_ASSERT(ackCompound);
			sender.lower->getDeliverer()->getAcceptor(ackCompound)->onData(ackCompound);
		}

		wns::ldk::CompoundPtr comp = sender.lower->sent.back();
		CumulativeACKCommand* cmd = sender.cumACK->getCommand(comp->getCommandPool());

		unsigned long int sequenceNumber = cmd->peer.sequenceNumber;

		comp = receiver.lower->sent.back();
		cmd = receiver.cumACK->getCommand(comp->getCommandPool());

		unsigned long int ackNumber = cmd->peer.ACKNumber;

		if (i >= (droppedPacket -1))
		{
			CPPUNIT_ASSERT_EQUAL(ackNumber, droppedPacket);
		}
		else
		{
			CPPUNIT_ASSERT_EQUAL(ackNumber, sequenceNumber + 1);
		}
	}
}


void
CumulativeACKTest::delayPacket()
{
	unsigned long int segments = 20;

	unsigned long int delayedPacket = 8;
	// 4 packets behind schedule
	unsigned long int delay = delayedPacket + 4;

	wns::ldk::CompoundPtr compound, ackCompound, delayedCompound;

	for(unsigned long int i = 0; i < segments; i++)
	{
		compound = sender.fun->createCompound();

		sender.cumACK->doSendData(compound);

		CumulativeACKCommand* cmd = sender.cumACK->getCommand(compound->getCommandPool());
		unsigned long int sequenceNumber = cmd->peer.sequenceNumber;

		if(i != delayedPacket)
		{
			// skip receiving the delayed packet
			receiver.lower->getDeliverer()->getAcceptor(compound)->onData(compound);
		}
		if(i == delayedPacket + delay)
		{
			delayedCompound = compound;

			receiver.lower->getDeliverer()->getAcceptor(delayedCompound)->onData(delayedCompound);
		}
		// return the ack
		ackCompound = receiver.lower->sent.back();
		sender.lower->getDeliverer()->getAcceptor(ackCompound)->onData(ackCompound);

		cmd = sender.cumACK->getCommand(ackCompound->getCommandPool());
		unsigned long int ackNumber = cmd->peer.ACKNumber;

		// until packets arrive out of order ack with the next packet
		// number expected.
		// when packet loss occurs ack the number of next expected packet in
		// order.
		// when delayed packet arrives ack with the next packet number
		// expected according to all received packets out of order
		// during delay
		if ((i >= delayedPacket) && (i < delayedPacket + delay))
		{
			CPPUNIT_ASSERT_EQUAL(ackNumber, delayedPacket);
		}
		else
		{
			CPPUNIT_ASSERT_EQUAL(ackNumber, sequenceNumber + 1);
		}
	}

	// all segments/acks sent
//	CPPUNIT_ASSERT_EQUAL(segments, sender.lower->sent.size());
	// one packet didn't arrive at receiver
	CPPUNIT_ASSERT_EQUAL(segments - 1, receiver.lower->sent.size());
}


void
CumulativeACKTest::timeOut()
{
	simTimeType beforeTimeOut = wns::simulator::getEventScheduler()->getTime();

	wns::ldk::CompoundPtr compound = sender.fun->createCompound();

	sender.cumACK->doSendData(compound);

	wns::ldk::CompoundPtr comp = sender.lower->sent.back();
	CumulativeACKCommand* cmd = sender.cumACK->getCommand(comp->getCommandPool());
	// remember the sequence number
	unsigned long int first = cmd->peer.sequenceNumber;
	// dequeue the compound
	sender.lower->sent.pop_back();

	wns::simulator::getEventScheduler()->processOneEvent();

	simTimeType timeOut = sender.cumACK->getRetransmissionTimeout();

	// get the resent compound
	comp = sender.lower->sent.back();
	cmd = sender.cumACK->getCommand(comp->getCommandPool());
	unsigned long int second = cmd->peer.sequenceNumber;
	sender.lower->sent.pop_back();

	// first compound being retransmitted?
	CPPUNIT_ASSERT_EQUAL(first, second);

	simTimeType afterTimeOut = wns::simulator::getEventScheduler()->getTime();
	CPPUNIT_ASSERT_EQUAL(afterTimeOut, beforeTimeOut + timeOut);
}


void
CumulativeACKTest::cleanup()
{
	delete receiver.fun;
	delete receiver.layer;

	delete sender.fun;
	delete sender.layer;
}




