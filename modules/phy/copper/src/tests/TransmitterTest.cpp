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
#include <COPPER/Wire.hpp>
#include <COPPER/Transmitter.hpp>
#include <COPPER/tests/ReceiverMock.hpp>

#include <WNS/service/phy/copper/Handler.hpp>
#include <WNS/service/phy/copper/DataTransmissionFeedback.hpp>
#include <WNS/CppUnit.hpp>
#include <WNS/pyconfig/helper/Functions.hpp>
#include <WNS/ldk/helper/FakePDU.hpp>
#include <WNS/Observer.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace copper { namespace tests {

	/**
	 * @brief Test for copper::Transmitter
	 */
	class TransmitterTest :
		public wns::TestFixture
	{
		class WireMock :
			virtual public WireInterface
		{
		public:
			WireMock() :
				blockedDuration(0.0),
				cSendUnicast(0),
				cSendBroadcast(0),
				cStopTransmission(0)
			{}

			virtual void
			sendData(const UnicastTransmissionPtr& /*ut*/, simTimeType /*duration*/)
			{
				++cSendUnicast;
			}

			virtual void
			sendData(const BroadcastTransmissionPtr& /*bt*/, simTimeType /*duration*/)
			{
				++cSendBroadcast;
			}

			virtual void
			stopTransmission(const wns::osi::PDUPtr& /*pdu*/)
			{
				++cStopTransmission;
			}

			virtual simTimeType
			blockedSince() const
			{
				return blockedDuration;
			}

			virtual void
			addReceiver(
				ReceiverInterface* /*r*/,
				const wns::service::dll::UnicastAddress& /*macAddress*/)
			{}

			simTimeType blockedDuration;
			int cSendUnicast;
			int cSendBroadcast;
			int cStopTransmission;
		};

		class TransmitterFeedbackMock :
			public wns::Observer<wns::service::phy::copper::DataTransmissionFeedbackInterface>
		{
		public:
			virtual void
			onDataSent(wns::osi::PDUPtr pdu)
			{
				pdus.push_back(pdu);
			}

			std::vector<wns::osi::PDUPtr> pdus;
		};

		CPPUNIT_TEST_SUITE( TransmitterTest );
		CPPUNIT_TEST( sendDataUnicast );
		CPPUNIT_TEST( sendDataBroadcast );
		CPPUNIT_TEST( cancelData );
		CPPUNIT_TEST( sendDataUnicastOnRealWire );
		CPPUNIT_TEST( sendDataBroadcastOnRealWire );
		CPPUNIT_TEST( cancelDataOnRealWire );
		CPPUNIT_TEST( isFree );
		CPPUNIT_TEST( isFreeDelayedTransmissionDetection );
		CPPUNIT_TEST_SUITE_END();
	public:
		void prepare();
		void cleanup();
		void sendDataUnicast();
		void sendDataBroadcast();
		void cancelData();
		void sendDataUnicastOnRealWire();
		void sendDataBroadcastOnRealWire();
		void cancelDataOnRealWire();
		void isFree();
		void isFreeDelayedTransmissionDetection();

	private:
		WireMock* wire;
		Wire* realWire;
		Transmitter* transmitter;
		ReceiverMock* receiverOnRealWire;
		Transmitter* transmitterOnRealWire;
		TransmitterFeedbackMock* transmitterFeedback;
	};

	CPPUNIT_TEST_SUITE_REGISTRATION( TransmitterTest );

	void
	TransmitterTest::prepare()
	{
		wire = new WireMock();

		wns::pyconfig::View config =
			wns::pyconfig::helper::createViewFromString(
				"from copper.Copper import Transmitter, Wire\n"
				"wire = Wire('theWire')\n"
				"transmitter = Transmitter(1E6, 0.0, None)\n"
				);

		realWire = new Wire(config.get<wns::pyconfig::View>("wire"));
		receiverOnRealWire = new ReceiverMock(wns::service::dll::UnicastAddress(1));
		realWire->addReceiver(receiverOnRealWire, wns::service::dll::UnicastAddress(1));
		transmitterFeedback = new TransmitterFeedbackMock();

		transmitter = new Transmitter(config.get("transmitter"), wire);
		transmitterOnRealWire = new Transmitter(config.get("transmitter"), realWire);
		// observer for onDataSent
		transmitterFeedback->startObserving(transmitterOnRealWire);
	}

	void
	TransmitterTest::cleanup()
	{
		delete transmitter;
		delete wire;
		delete transmitterFeedback;
		delete transmitterOnRealWire;
		delete receiverOnRealWire;
		delete realWire;
	}

	void
	TransmitterTest::sendDataUnicast()
	{
		transmitter->sendData(
			wns::service::dll::UnicastAddress(1), wns::osi::PDUPtr(
				new wns::ldk::helper::FakePDU(100)));

		CPPUNIT_ASSERT_EQUAL(1, wire->cSendUnicast );
		CPPUNIT_ASSERT_EQUAL(0, wire->cSendBroadcast );
	}

	void
	TransmitterTest::sendDataBroadcast()
	{
		transmitter->sendData(
			wns::service::dll::BroadcastAddress(), wns::osi::PDUPtr(
				new wns::ldk::helper::FakePDU(100)));

		CPPUNIT_ASSERT_EQUAL(0, wire->cSendUnicast );
		CPPUNIT_ASSERT_EQUAL(1, wire->cSendBroadcast );
	}

	void
	TransmitterTest::cancelData()
	{
		transmitter->cancelData(wns::osi::PDUPtr());
		CPPUNIT_ASSERT_EQUAL(1, wire->cStopTransmission );
	}

	void
	TransmitterTest::sendDataUnicastOnRealWire()
	{
		transmitterOnRealWire->sendData(
			wns::service::dll::UnicastAddress(1), wns::osi::PDUPtr(
				new wns::ldk::helper::FakePDU(100)));

		wns::simulator::getEventScheduler()->processOneEvent();
		CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), transmitterFeedback->pdus.size());
	}

	void
	TransmitterTest::sendDataBroadcastOnRealWire()
	{
		transmitterOnRealWire->sendData(
			wns::service::dll::BroadcastAddress(), wns::osi::PDUPtr(
				new wns::ldk::helper::FakePDU(100)));

		wns::simulator::getEventScheduler()->processOneEvent();
		CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), transmitterFeedback->pdus.size());
	}

	void
	TransmitterTest::cancelDataOnRealWire()
	{
		transmitter->cancelData(wns::osi::PDUPtr());

		wns::simulator::getEventScheduler()->processOneEvent();
		CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), transmitterFeedback->pdus.size());
	}

	void
	TransmitterTest::isFree()
	{
		wire->blockedDuration = 0.0;
		CPPUNIT_ASSERT( !transmitter->isFree() );

		wire->blockedDuration = 1.0;
		CPPUNIT_ASSERT( !transmitter->isFree() );

		wire->blockedDuration = -1.0;
		CPPUNIT_ASSERT( transmitter->isFree() );
	}

	void
	TransmitterTest::isFreeDelayedTransmissionDetection()
	{
		delete transmitter;

		wns::pyconfig::View config =
			wns::pyconfig::helper::createViewFromString(
				"from copper.Copper import Transmitter\n"
				"transmitter = Transmitter(1E6, 0.1, None)\n"
				);
		transmitter = new Transmitter(config.get("transmitter"), wire);

		wire->blockedDuration = 0.0;
		CPPUNIT_ASSERT( transmitter->isFree() );

		wire->blockedDuration = 0.05;
		CPPUNIT_ASSERT( transmitter->isFree() );

		wire->blockedDuration = 0.1;
		CPPUNIT_ASSERT( !transmitter->isFree() );

		wire->blockedDuration = 1.0;
		CPPUNIT_ASSERT( !transmitter->isFree() );

		wire->blockedDuration = -1.0;
		CPPUNIT_ASSERT( transmitter->isFree() );
	}


} // tests
} // copper


