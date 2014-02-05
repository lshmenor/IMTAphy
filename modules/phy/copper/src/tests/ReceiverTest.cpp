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
#include <COPPER/Receiver.hpp>
#include <COPPER/tests/TransmitterDataSentMock.hpp>

#include <WNS/ldk/helper/FakePDU.hpp>
#include <WNS/service/phy/copper/Handler.hpp>
#include <WNS/service/phy/copper/CarrierSensing.hpp>
#include <WNS/Observer.hpp>
#include <WNS/CppUnit.hpp>
#include <WNS/pyconfig/helper/Functions.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace copper { namespace tests {

	/**
	 * @brief Test for copper::Receiver
	 *
	 * This is a very simple test since the Receiver must only forward
	 * calls to the Handler;
	 */
	class ReceiverTest :
		public wns::TestFixture
	{
		typedef wns::Subject<wns::service::phy::copper::Handler> HandlerSubject;
		typedef wns::Subject<wns::service::phy::copper::CarrierSensing> CarrierSensingSubject;

		class HandlerMock :
			public wns::Observer<wns::service::phy::copper::Handler>
		{
		public:
			HandlerMock() :
				cOnData(0),
				ber(0.0)
			{
			}

			virtual void
			onData(const wns::osi::PDUPtr&, double _ber, bool)
			{
				++cOnData;
				ber = _ber;
			}

			int cOnData;
			double ber;
		};

		class CarrierSensingMock :
			public wns::Observer<wns::service::phy::copper::CarrierSensing>
		{
		public:
			CarrierSensingMock() :
				cOnCarrierIdle(0),
				cOnCarrierBusy(0),
				cOnCollision(0)
			{
			}

			virtual void
			onCarrierIdle()
			{
				++cOnCarrierIdle;
			}

			virtual void
			onCarrierBusy()
			{
				++cOnCarrierBusy;
			}

			virtual void
			onCollision()
			{
				++cOnCollision;
			}

			int cOnCarrierIdle;
			int cOnCarrierBusy;
			int cOnCollision;
		};

		class WireMock :
			virtual public WireInterface
		{
		public:
			WireMock()
			{}

			virtual void
			sendData(const UnicastTransmissionPtr& /*ut*/, simTimeType /*duration*/)
			{
			}

			virtual void
			sendData(const BroadcastTransmissionPtr& /*bt*/, simTimeType /*duration*/)
			{
			}

			virtual void
			stopTransmission(const wns::osi::PDUPtr& /*pdu*/)
			{
			}

			virtual simTimeType
			blockedSince() const
			{
				return -1;
			}

			virtual void
			addReceiver(
				ReceiverInterface* /*r*/,
				const wns::service::dll::UnicastAddress& /*macAddress*/)
			{}
		};

		CPPUNIT_TEST_SUITE( ReceiverTest );
		CPPUNIT_TEST( onData );
		CPPUNIT_TEST( onCopperFree );
		CPPUNIT_TEST( onCopperBusy );
		CPPUNIT_TEST( onCollision );
		CPPUNIT_TEST_SUITE_END();
	public:
		void prepare();
		void cleanup();
		void onData();
		void onCopperFree();
		void onCopperBusy();
		void onCollision();

	private:
		HandlerMock* handler;
		CarrierSensingMock* carrierSensing;
		Receiver* receiver;
		WireMock* wire;
		TransmitterDataSentMock* transmitter;
	};

	CPPUNIT_TEST_SUITE_REGISTRATION( ReceiverTest );

	void
	ReceiverTest::prepare()
	{
		wns::simulator::getEventScheduler()->reset();

		wire = new WireMock();
		handler = new HandlerMock();
		carrierSensing = new CarrierSensingMock();
		transmitter = new TransmitterDataSentMock();

		wns::pyconfig::View config =
			wns::pyconfig::helper::createViewFromString(
				"from copper.Copper import Receiver\n"
				"from openwns.distribution import Fixed\n"
				"receiver = Receiver(Fixed(0.1), 0.01, None)\n"
				);
		receiver = new Receiver(config.get("receiver"), wire);
		handler->startObserving(receiver);
		receiver->setDLLUnicastAddress(wns::service::dll::UnicastAddress(1));
		carrierSensing->startObserving(receiver);
	}

	void
	ReceiverTest::cleanup()
	{
		delete handler;
		delete carrierSensing;
		delete receiver;
		delete wire;
		delete transmitter;
	}

	void
	ReceiverTest::onData()
	{
		bool forMe = false;

		// Broadcast
		BroadcastTransmissionPtr bt(
			new BroadcastTransmission(
				wns::osi::PDUPtr(new wns::ldk::helper::FakePDU(100)),
				transmitter));

		forMe = receiver->onData(bt);
		CPPUNIT_ASSERT_EQUAL( 1, handler->cOnData );
		WNS_ASSERT_MAX_REL_ERROR( 0.1, handler->ber, 1E-10);
		CPPUNIT_ASSERT( forMe );

		// Unicast
		UnicastTransmissionPtr ut(
			new UnicastTransmission(
				wns::service::dll::UnicastAddress(2),
				wns::osi::PDUPtr(new wns::ldk::helper::FakePDU(100)),
				transmitter));

		forMe = receiver->onData(ut);
		CPPUNIT_ASSERT_EQUAL( 1, handler->cOnData );
		CPPUNIT_ASSERT( !forMe );

		UnicastTransmissionPtr ut2(
			new UnicastTransmission(
				wns::service::dll::UnicastAddress(1),
				wns::osi::PDUPtr(new wns::ldk::helper::FakePDU(100)),
				transmitter));

		forMe = receiver->onData(ut2);
		CPPUNIT_ASSERT_EQUAL( 2, handler->cOnData );
		CPPUNIT_ASSERT( forMe );
	}

	void
	ReceiverTest::onCopperFree()
	{
		receiver->onCopperFree();
		wns::events::scheduler::Interface* es = wns::simulator::getEventScheduler();
		es->processOneEvent();
		WNS_ASSERT_MAX_REL_ERROR( simTimeType(0.01), es->getTime(), 1E-10);
		CPPUNIT_ASSERT_EQUAL( 1, carrierSensing->cOnCarrierIdle );
		receiver->onCopperFree();
		es->processOneEvent();
		WNS_ASSERT_MAX_REL_ERROR( simTimeType(0.02), es->getTime(), 1E-10);
		CPPUNIT_ASSERT_EQUAL( 2, carrierSensing->cOnCarrierIdle );
	}

	void
	ReceiverTest::onCopperBusy()
	{
		receiver->onCopperBusy();
		wns::events::scheduler::Interface* es = wns::simulator::getEventScheduler();
		es->processOneEvent();
		WNS_ASSERT_MAX_REL_ERROR( simTimeType(0.01), es->getTime(), 1E-10);
		CPPUNIT_ASSERT_EQUAL( 1, carrierSensing->cOnCarrierBusy );
		receiver->onCopperBusy();
		es->processOneEvent();
		WNS_ASSERT_MAX_REL_ERROR( simTimeType(0.02), es->getTime(), 1E-10);
		CPPUNIT_ASSERT_EQUAL( 2, carrierSensing->cOnCarrierBusy );
	}

	void
	ReceiverTest::onCollision()
	{
		receiver->onCollision();
		wns::events::scheduler::Interface* es = wns::simulator::getEventScheduler();
		es->processOneEvent();
		WNS_ASSERT_MAX_REL_ERROR( simTimeType(0.01), es->getTime(), 1E-10);
		CPPUNIT_ASSERT_EQUAL( 1, carrierSensing->cOnCollision );
		receiver->onCollision();
		es->processOneEvent();
		WNS_ASSERT_MAX_REL_ERROR( simTimeType(0.02), es->getTime(), 1E-10);
		CPPUNIT_ASSERT_EQUAL( 2, carrierSensing->cOnCollision );
	}


} // tests
} // copper


