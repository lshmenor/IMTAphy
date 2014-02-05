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

#include <COPPER/Transmission.hpp>
#include <COPPER/Transceiver.hpp>
#include <COPPER/Receiver.hpp>
#include <COPPER/Wire.hpp>
#include <COPPER/tests/TransmitterDataSentMock.hpp>
#include <COPPER/tests/ReceiverMock.hpp>

#include <WNS/pyconfig/helper/Functions.hpp>
#include <WNS/events/NoOp.hpp>
#include <WNS/ldk/helper/FakePDU.hpp>
#include <WNS/CppUnit.hpp>
#include <WNS/service/phy/copper/Handler.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace  copper { namespace tests {

	class WireTest :
		public wns::TestFixture
	{

		CPPUNIT_TEST_SUITE( WireTest );
		CPPUNIT_TEST( collision );
		CPPUNIT_TEST( blockedSince );
		CPPUNIT_TEST( blockedSinceDoubleTransmission );
		CPPUNIT_TEST( onCopperFree );
		CPPUNIT_TEST( onCopperBusy );
		CPPUNIT_TEST( sendBroadcastData );
		CPPUNIT_TEST( cancelBroadcastData );
		CPPUNIT_TEST( sendUnicastData );
		CPPUNIT_TEST( cancelUnicastData );
		CPPUNIT_TEST_SUITE_END();
	public:
		void prepare();
		void cleanup();
		void collision();
		void blockedSince();
		void blockedSinceDoubleTransmission();
		void onCopperFree();
		void onCopperBusy();
		void sendBroadcastData();
		void cancelBroadcastData();
		void sendUnicastData();
		void cancelUnicastData();

	private:
		WireInterface* wire;
		ReceiverMock* receiver1;
		ReceiverMock* receiver2;
		TransmitterDataSentMock* transmitter;
	};

	CPPUNIT_TEST_SUITE_REGISTRATION( WireTest );

	void
	WireTest::prepare()
	{
		wns::simulator::getEventScheduler()->reset();

		wns::pyconfig::View config =
			wns::pyconfig::helper::createViewFromString(
				"from copper.Copper import Wire, Transmitter, Receiver\n"
				"wire = Wire('theWire')\n"
				);

		wire = new Wire(config.get<wns::pyconfig::View>("wire"));
		receiver1 = new ReceiverMock(wns::service::dll::UnicastAddress(1));
		receiver2 = new ReceiverMock(wns::service::dll::UnicastAddress(2));
		transmitter = new TransmitterDataSentMock();

		wire->addReceiver(receiver1, wns::service::dll::UnicastAddress(1));
		wire->addReceiver(receiver2, wns::service::dll::UnicastAddress(2));
	}

	void
	WireTest::cleanup()
	{
		delete wire;
		delete receiver1;
		delete receiver2;
		delete transmitter;
	}

	void
	WireTest::blockedSince()
	{
		wns::osi::PDUPtr pdu(new wns::ldk::helper::FakePDU(100));
		CPPUNIT_ASSERT(wire->blockedSince() < 0);

		wire->sendData(BroadcastTransmissionPtr(new BroadcastTransmission(pdu, transmitter)), 0.1);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, wire->blockedSince(), 1E-9);

		wns::simulator::getEventScheduler()->scheduleDelay(wns::events::NoOp(), 0.05);
		wns::simulator::getEventScheduler()->processOneEvent();
		WNS_ASSERT_MAX_REL_ERROR( simTimeType(0.05), wire->blockedSince(), 1E-9);
		CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), transmitter->pdus.size());

		wns::simulator::getEventScheduler()->processOneEvent();
		CPPUNIT_ASSERT( wire->blockedSince() < 0);
		CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), transmitter->pdus.size());
	}

	void
	WireTest::blockedSinceDoubleTransmission()
	{
		wns::osi::PDUPtr pdu(new wns::ldk::helper::FakePDU(100));
		wns::osi::PDUPtr pdu2(new wns::ldk::helper::FakePDU(100));

		// first transmission
		wire->sendData(BroadcastTransmissionPtr(new BroadcastTransmission(pdu, transmitter)), 0.2);

		// move forward 0.05s
		wns::simulator::getEventScheduler()->scheduleDelay(wns::events::NoOp(), 0.05);
 		wns::simulator::getEventScheduler()->processOneEvent();

		// second transmission
		wire->sendData(BroadcastTransmissionPtr(new BroadcastTransmission(pdu2, transmitter)), 0.3);
		WNS_ASSERT_MAX_REL_ERROR( simTimeType(0.05), wire->blockedSince(), 1E-9);

		// end of first transmission
 		wns::simulator::getEventScheduler()->processOneEvent();
		WNS_ASSERT_MAX_REL_ERROR( simTimeType(0.2), wire->blockedSince(), 1E-9);
		CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), transmitter->pdus.size());

		// move forward 0.05s
		wns::simulator::getEventScheduler()->scheduleDelay(wns::events::NoOp(), 0.05);
		wns::simulator::getEventScheduler()->processOneEvent();
		WNS_ASSERT_MAX_REL_ERROR( simTimeType(0.25), wire->blockedSince(), 1E-9);

		// end of second transmission
		wns::simulator::getEventScheduler()->processOneEvent();
		CPPUNIT_ASSERT( wire->blockedSince() < 0);
		CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), transmitter->pdus.size());
	}

	void
	WireTest::onCopperFree()
	{
		wns::osi::PDUPtr pdu(new wns::ldk::helper::FakePDU(100));
		wire->sendData(BroadcastTransmissionPtr(new BroadcastTransmission(pdu, transmitter)), 0.1);

		CPPUNIT_ASSERT_EQUAL(0, receiver1->cOnCopperFree);
		CPPUNIT_ASSERT_EQUAL(0, receiver2->cOnCopperFree);

		wns::simulator::getEventScheduler()->processOneEvent();

		CPPUNIT_ASSERT_EQUAL(1, receiver1->cOnCopperFree);
		CPPUNIT_ASSERT_EQUAL(1, receiver2->cOnCopperFree);
	}

	void
	WireTest::onCopperBusy()
	{
		CPPUNIT_ASSERT_EQUAL(0, receiver1->cOnCopperBusy);
		CPPUNIT_ASSERT_EQUAL(0, receiver2->cOnCopperBusy);

		wns::osi::PDUPtr pdu(new wns::ldk::helper::FakePDU(100));
		wire->sendData(BroadcastTransmissionPtr(new BroadcastTransmission(pdu, transmitter)), 0.1);

		CPPUNIT_ASSERT_EQUAL(1, receiver1->cOnCopperBusy);
		CPPUNIT_ASSERT_EQUAL(1, receiver2->cOnCopperBusy);
	}

	void
	WireTest::collision()
	{
		wns::osi::PDUPtr pdu(new wns::ldk::helper::FakePDU(100));
		wns::osi::PDUPtr pdu2(new wns::ldk::helper::FakePDU(100));

		wire->sendData(BroadcastTransmissionPtr(new BroadcastTransmission(pdu, transmitter)), 0.1);
		wire->sendData(BroadcastTransmissionPtr(new BroadcastTransmission(pdu2, transmitter)), 0.1);

		// two PDUs -> two Events
		wns::simulator::getEventScheduler()->processOneEvent();
		// First transmission is over
		CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), transmitter->pdus.size());

		wns::simulator::getEventScheduler()->processOneEvent();
		// Second transmission is over
		CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), transmitter->pdus.size());

		CPPUNIT_ASSERT(receiver1->pdu == pdu2);
		CPPUNIT_ASSERT(receiver2->pdu == pdu2);
		CPPUNIT_ASSERT(receiver1->collision == true);
		CPPUNIT_ASSERT(receiver2->collision == true);

		CPPUNIT_ASSERT_EQUAL(1, receiver1->cOnCollision);
		CPPUNIT_ASSERT_EQUAL(1, receiver2->cOnCollision);

	}

	void
	WireTest::sendBroadcastData()
	{
		wns::osi::PDUPtr pdu(new wns::ldk::helper::FakePDU(100));

		wire->sendData(BroadcastTransmissionPtr(new BroadcastTransmission(pdu, transmitter)), 0.1);

		wns::simulator::getEventScheduler()->processOneEvent();
		// Transmission is over
		CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), transmitter->pdus.size());

		CPPUNIT_ASSERT(receiver1->pdu == pdu);
		CPPUNIT_ASSERT_EQUAL(0, receiver1->cOnCollision);

		CPPUNIT_ASSERT(receiver2->pdu == pdu);
		CPPUNIT_ASSERT_EQUAL(0, receiver2->cOnCollision);
	}

	void
	WireTest::cancelBroadcastData()
	{
		wns::osi::PDUPtr pdu(new wns::ldk::helper::FakePDU(100));

		wire->sendData(BroadcastTransmissionPtr(new BroadcastTransmission(pdu, transmitter)), 0.1);

		wns::simulator::getEventScheduler()->schedule(wns::events::NoOp(), 0.00005);
		wns::simulator::getEventScheduler()->schedule(wns::events::NoOp(), 2.0);

		CPPUNIT_ASSERT(receiver1->pdu == wns::osi::PDUPtr());
		CPPUNIT_ASSERT(receiver2->pdu == wns::osi::PDUPtr());

		// process to 0.00005
		wns::simulator::getEventScheduler()->processOneEvent();

		// stop packet
		wire->stopTransmission(pdu);

		// cancled data should not arrive
		CPPUNIT_ASSERT(receiver1->pdu == wns::osi::PDUPtr());
		CPPUNIT_ASSERT(receiver2->pdu == wns::osi::PDUPtr());

		wns::simulator::getEventScheduler()->processOneEvent();

		WNS_ASSERT_MAX_REL_ERROR( simTimeType(2.0), wns::simulator::getEventScheduler()->getTime(), 1E-10 );

		// cancled data should not arrive
		CPPUNIT_ASSERT(receiver1->pdu == wns::osi::PDUPtr());
		CPPUNIT_ASSERT(receiver2->pdu == wns::osi::PDUPtr());

 		// Since transmission has been cancled no event onDataSent
		// should be generated
		CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), transmitter->pdus.size());
	}

	void
	WireTest::sendUnicastData()
	{
		wns::osi::PDUPtr pdu(new wns::ldk::helper::FakePDU(100));

		wire->sendData(
			UnicastTransmissionPtr(
				new UnicastTransmission(
					wns::service::dll::UnicastAddress(1),
					pdu,
					transmitter)), 0.1);

		wns::simulator::getEventScheduler()->processOneEvent();
		// Data is sent
		CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), transmitter->pdus.size());
		CPPUNIT_ASSERT(pdu == transmitter->pdus.at(0));

		CPPUNIT_ASSERT(receiver1->pdu == pdu);
		CPPUNIT_ASSERT_EQUAL(0, receiver1->cOnCollision);

		CPPUNIT_ASSERT(receiver2->pdu == wns::osi::PDUPtr());
	}

	void
	WireTest::cancelUnicastData()
	{
		wns::osi::PDUPtr pdu(new wns::ldk::helper::FakePDU(100));

		wire->sendData(
			UnicastTransmissionPtr(
				new UnicastTransmission(
					wns::service::dll::UnicastAddress(1),
					pdu,
					transmitter)), 0.1);

		wns::simulator::getEventScheduler()->schedule(wns::events::NoOp(), 0.00005);
		wns::simulator::getEventScheduler()->schedule(wns::events::NoOp(), 2.0);

		CPPUNIT_ASSERT(receiver1->pdu == wns::osi::PDUPtr());
		CPPUNIT_ASSERT(receiver2->pdu == wns::osi::PDUPtr());

		// process to 0.00005
		wns::simulator::getEventScheduler()->processOneEvent();

		// stop packet
		wire->stopTransmission(pdu);

 		// Since transmission has been cancled no event onDataSent
		// should be generated
		CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), transmitter->pdus.size());

		// cancled data should not arrive
		CPPUNIT_ASSERT(receiver1->pdu == wns::osi::PDUPtr());
		CPPUNIT_ASSERT(receiver2->pdu == wns::osi::PDUPtr());

		wns::simulator::getEventScheduler()->processOneEvent();

		WNS_ASSERT_MAX_REL_ERROR( simTimeType(2.0), wns::simulator::getEventScheduler()->getTime(), 1E-10 );

		// cancled data should not arrive
		CPPUNIT_ASSERT(receiver1->pdu == wns::osi::PDUPtr());
		CPPUNIT_ASSERT(receiver2->pdu == wns::osi::PDUPtr());

 		// Since transmission has been cancled no event onDataSent
		// should be generated
		CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), transmitter->pdus.size());
	}

} // tests
} // copper


