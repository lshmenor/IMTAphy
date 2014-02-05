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

#include <GLUE/BERMeasurementReporting.hpp>
#include <GLUE/BERProvider.hpp>
#include <GLUE/BERConsumer.hpp>
#include <GLUE/trigger/FunctionalUnitLight.hpp>

#include <WNS/ldk/Layer.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/sar/Fixed.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/ldk/tools/Stub.hpp>

#include <WNS/pyconfig/Parser.hpp>
#include <WNS/Exception.hpp>
#include <WNS/CppUnit.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace glue {

	class BERMeasurementReportingTest :
		public wns::TestFixture
	{
		CPPUNIT_TEST_SUITE( BERMeasurementReportingTest );
		CPPUNIT_TEST( testReporting );
		CPPUNIT_TEST( testReportingInterval );
		CPPUNIT_TEST( testReportingNoMeasurementsAvailable );
		CPPUNIT_TEST( testMeasurementAveraging );
		CPPUNIT_TEST( testMeasurementReset );
		CPPUNIT_TEST( testWakeupNothingToSend );
		CPPUNIT_TEST( testWakeupMeasurementReportToSend );
		CPPUNIT_TEST( testWakeupTwiceMeasurementReportToSend );
		CPPUNIT_TEST( testMeasurementAveragingOnBusyConnection );
		CPPUNIT_TEST( testHeaderSize );
		CPPUNIT_TEST( testInvalidBER );
		CPPUNIT_TEST( testInvalidPacketSize );
		CPPUNIT_TEST_SUITE_END();

	public:
		class DemoBERProvider :
			public glue::trigger::FunctionalUnitLight,
			public BERProvider
		{
		public:
			DemoBERProvider(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config) :
				FunctionalUnitLight(fun, config)
			{} // DemoBERProvider

			virtual
			~DemoBERProvider()
			{} // ~DemoBERProvider
		};

		class DemoBERConsumer :
			public glue::trigger::FunctionalUnitLight,
			public BERConsumer
		{
		public:
			DemoBERConsumer(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& _config);

			virtual
			~DemoBERConsumer()
			{} // ~DemoBERConsumer

			virtual void
			onBERProviderDeleted();

			virtual void
			onNewMeasurement(double _ber, int _packetSize);

			virtual void
			onFUNCreated();

			int berProviderDeleted;

			double ber;
			int packetSize;

		private:
			struct Friends
			{
				BERProvider* berProvider;
			} friends;
			wns::pyconfig::View config;
		};

		void prepare();
		void cleanup();

		void testReporting();
		void testReportingInterval();
		void testReportingNoMeasurementsAvailable();
		void testMeasurementAveraging();
		void testMeasurementReset();
		void testWakeupNothingToSend();
		void testWakeupMeasurementReportToSend();
		void testWakeupTwiceMeasurementReportToSend();
		void testMeasurementAveragingOnBusyConnection();
		void testHeaderSize();
		void testInvalidBER();
		void testInvalidPacketSize();

	private:
		wns::ldk::ILayer* layer;
		wns::ldk::fun::Main* fun;

		BERMeasurementReporting* berMeasurementReporting;
		DemoBERProvider* demoBERProvider;
		DemoBERConsumer* demoBERConsumer;
		wns::ldk::tools::Stub* stub;

		wns::events::scheduler::Interface* scheduler;
};

	CPPUNIT_TEST_SUITE_REGISTRATION( BERMeasurementReportingTest );

	STATIC_FACTORY_REGISTER_WITH_CREATOR(BERMeasurementReportingTest::DemoBERProvider,
					     wns::ldk::FunctionalUnit,
					     "glue.tests.DemoBERProvider",
					     wns::ldk::FUNConfigCreator);

	STATIC_FACTORY_REGISTER_WITH_CREATOR(BERMeasurementReportingTest::DemoBERConsumer,
					     wns::ldk::FunctionalUnit,
					     "glue.tests.DemoBERConsumer",
					     wns::ldk::FUNConfigCreator);

	BERMeasurementReportingTest::DemoBERConsumer::DemoBERConsumer(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& _config) :
		glue::trigger::FunctionalUnitLight(fun, _config),
		BERConsumer(),
		berProviderDeleted(0),
		ber(0.0),
		packetSize(0),
		config(_config)
	{
		friends.berProvider = NULL;
	} // BERConsumer

	void
	BERMeasurementReportingTest::DemoBERConsumer::onBERProviderDeleted()
	{
		++berProviderDeleted;
	} // BERProviderDeleted

	void
	BERMeasurementReportingTest::DemoBERConsumer::onNewMeasurement(double _ber, int _packetSize)
	{
		ber = _ber;
		packetSize = _packetSize;
	} // reportBER

	void
	BERMeasurementReportingTest::DemoBERConsumer::onFUNCreated()
	{
		friends.berProvider = getFUN()->findFriend<BERProvider*>(config.get<std::string>("BERProvider"));
		friends.berProvider->attachBERConsumer(this);
	} // onFUNCreated

	void
	BERMeasurementReportingTest::prepare()
	{
		scheduler = wns::simulator::getEventScheduler();
		scheduler->reset();

		layer = new wns::ldk::tests::LayerStub();
		fun = new wns::ldk::fun::Main(layer);

		wns::pyconfig::Parser demoBERProviderPyCo;
		demoBERProviderPyCo.loadString("class DemoBERProvider:\n"
					       "  __plugin__ = 'glue.tests.DemoBERProvider'\n"
					       "  name = \"DemoBERProvider\"\n"
					       "demoBERProvider = DemoBERProvider\n");
		wns::pyconfig::View demoBERProviderView(demoBERProviderPyCo, "demoBERProvider");
		demoBERProvider = new DemoBERProvider(fun, demoBERProviderView);

		wns::pyconfig::Parser demoBERConsumerPyCo;
		demoBERConsumerPyCo.loadString("class DemoBERConsumer:\n"
					       "  __plugin__ = 'glue.tests.DemoBERConsumer'\n"
					       "  name = \"DemoBERConsumer\"\n"
					       "  BERProvider = \"demoBERProvider\"\n"
					       "demoBERConsumer = DemoBERConsumer\n");
		wns::pyconfig::View demoBERConsumerView(demoBERConsumerPyCo, "demoBERConsumer");
		demoBERConsumer = new DemoBERConsumer(fun, demoBERConsumerView);

		wns::pyconfig::Parser berMeasurementReportingPyCo;
		berMeasurementReportingPyCo.loadString("import glue.BERMeasurementReporting\n"
						       "berMeasurementReporting = glue.BERMeasurementReporting.BERMeasurementReporting("
						       "\"demoBERProvider\", "
						       "transmissionInterval = 0.01, "
						       "headerSize = 32)\n");
		wns::pyconfig::View berMeasurementReportingView(berMeasurementReportingPyCo, "berMeasurementReporting");
		berMeasurementReporting = new BERMeasurementReporting(fun, berMeasurementReportingView);

		wns::pyconfig::Parser emptyPyCo;
		stub = new wns::ldk::tools::Stub(fun, emptyPyCo);

		fun->addFunctionalUnit("demoBERProvider", demoBERProvider);
		fun->addFunctionalUnit("demoBERConsumer", demoBERConsumer);
		fun->addFunctionalUnit("berMeasurementReporting", berMeasurementReporting);
		fun->addFunctionalUnit("stub", stub);

		berMeasurementReporting->connect(stub);

		// find friends
		fun->onFUNCreated();
	} // prepare

	void
	BERMeasurementReportingTest::cleanup()
	{
		// deleted by fun (FUN)
		// delete demoBERProvider;
		// delete demoBERConsumer;
		// delete berMeasurementReporting
		// delete stub;

		delete fun;
		delete layer;
	} // cleanup

	void
	BERMeasurementReportingTest::testReporting()
	{
		demoBERProvider->notifyBERConsumers(0.1, 1);

		CPPUNIT_ASSERT(scheduler->processOneEvent());
		CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), stub->sent.size());

		BERMeasurementReportingCommand* berCommand =
			dynamic_cast<BERMeasurementReportingCommand*>
			(fun->getProxy()->getCommand(stub->sent[0]->getCommandPool(), berMeasurementReporting));

		CPPUNIT_ASSERT_EQUAL(0.1, berCommand->peer.BER);
		CPPUNIT_ASSERT_EQUAL(1, berCommand->magic.packetSize);
	} // testReporting

	void
	BERMeasurementReportingTest::testReportingInterval()
	{
		simTimeType startTime = wns::simulator::getEventScheduler()->getTime();

		demoBERProvider->notifyBERConsumers(0.1, 1);
		CPPUNIT_ASSERT(scheduler->processOneEvent());

		CPPUNIT_ASSERT_EQUAL(startTime + 0.01, wns::simulator::getEventScheduler()->getTime());
	} // testReportingInterval

	void
	BERMeasurementReportingTest::testReportingNoMeasurementsAvailable()
	{
		CPPUNIT_ASSERT(scheduler->processOneEvent());
		CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), stub->sent.size());
	} // testReportingNoMeasurementsAvailable

	void
	BERMeasurementReportingTest::testMeasurementAveraging()
	{
		demoBERProvider->notifyBERConsumers(0.12, 1);
		demoBERProvider->notifyBERConsumers(0.03, 2);
		demoBERProvider->notifyBERConsumers(0.02, 3);

		CPPUNIT_ASSERT(scheduler->processOneEvent());
		CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), stub->sent.size());

		BERMeasurementReportingCommand* berCommand =
			dynamic_cast<BERMeasurementReportingCommand*>
			(fun->getProxy()->getCommand(stub->sent[0]->getCommandPool(), berMeasurementReporting));

		CPPUNIT_ASSERT_EQUAL(0.04, berCommand->peer.BER);
		CPPUNIT_ASSERT_EQUAL(2, berCommand->magic.packetSize);
	} // testMeasurementAveraging

	void
	BERMeasurementReportingTest::testMeasurementReset()
	{
		demoBERProvider->notifyBERConsumers(0.1, 1);
		demoBERProvider->notifyBERConsumers(0.2, 2);
		demoBERProvider->notifyBERConsumers(0.3, 3);

		CPPUNIT_ASSERT(scheduler->processOneEvent());

		demoBERProvider->notifyBERConsumers(0.12, 1);
		demoBERProvider->notifyBERConsumers(0.03, 2);
		demoBERProvider->notifyBERConsumers(0.02, 3);

		CPPUNIT_ASSERT(scheduler->processOneEvent());
		CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), stub->sent.size());

		BERMeasurementReportingCommand* berCommand =
			dynamic_cast<BERMeasurementReportingCommand*>
			(fun->getProxy()->getCommand(stub->sent[1]->getCommandPool(), berMeasurementReporting));

		CPPUNIT_ASSERT_EQUAL(0.04, berCommand->peer.BER);
		CPPUNIT_ASSERT_EQUAL(2, berCommand->magic.packetSize);
	} // testMeasurementReset


	void
	BERMeasurementReportingTest::testWakeupNothingToSend()
	{
		stub->wakeup();

		CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), stub->sent.size());
	} // testWakeupNothingToSend

	void
	BERMeasurementReportingTest::testWakeupMeasurementReportToSend()
	{
		stub->close();

		demoBERProvider->notifyBERConsumers(0.01, 8);
		demoBERProvider->notifyBERConsumers(0.04, 6);
		demoBERProvider->notifyBERConsumers(0.04, 10);

		CPPUNIT_ASSERT(scheduler->processOneEvent());

		stub->open(true);

		CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), stub->sent.size());

		BERMeasurementReportingCommand* berCommand =
			dynamic_cast<BERMeasurementReportingCommand*>
			(fun->getProxy()->getCommand(stub->sent[0]->getCommandPool(), berMeasurementReporting));

		CPPUNIT_ASSERT_EQUAL(0.03, berCommand->peer.BER);
		CPPUNIT_ASSERT_EQUAL(8, berCommand->magic.packetSize);
	} // testWakeupMeasurementReportToSend

	void
	BERMeasurementReportingTest::testWakeupTwiceMeasurementReportToSend()
	{
		stub->close();

		demoBERProvider->notifyBERConsumers(0.01, 8);
		demoBERProvider->notifyBERConsumers(0.04, 6);
		demoBERProvider->notifyBERConsumers(0.04, 10);

		CPPUNIT_ASSERT(scheduler->processOneEvent());

		stub->open(true);

		stub->wakeup();

		CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), stub->sent.size());
	} // testWakeupTwiceMeasurementReportToSend

	void
	BERMeasurementReportingTest::testMeasurementAveragingOnBusyConnection()
	{
		stub->close();

		demoBERProvider->notifyBERConsumers(0.1, 1);
		demoBERProvider->notifyBERConsumers(0.4, 4);
		demoBERProvider->notifyBERConsumers(0.4, 4);

		CPPUNIT_ASSERT(scheduler->processOneEvent());

		demoBERProvider->notifyBERConsumers(0.01, 8);
		demoBERProvider->notifyBERConsumers(0.04, 6);
		demoBERProvider->notifyBERConsumers(0.04, 10);

		CPPUNIT_ASSERT(scheduler->processOneEvent());

		stub->open(true);

		CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), stub->sent.size());

		BERMeasurementReportingCommand* berCommand =
			dynamic_cast<BERMeasurementReportingCommand*>
			(fun->getProxy()->getCommand(stub->sent[0]->getCommandPool(), berMeasurementReporting));

		CPPUNIT_ASSERT_EQUAL(0.03, berCommand->peer.BER);
		CPPUNIT_ASSERT_EQUAL(8, berCommand->magic.packetSize);
	} // testMeasurementAveragingOnBusyConnection

	void
	BERMeasurementReportingTest::testHeaderSize()
	{
		demoBERProvider->notifyBERConsumers(0.1, 1);

		CPPUNIT_ASSERT(scheduler->processOneEvent());

		Bit headerSize = 0;
		Bit payloadSize = 0;
		stub->calculateSizes(stub->sent[0]->getCommandPool(), headerSize, payloadSize);

		CPPUNIT_ASSERT_EQUAL(Bit(32), headerSize);
		CPPUNIT_ASSERT_EQUAL(Bit(0), payloadSize);
	} // testHeaderSize

	void
	BERMeasurementReportingTest::testInvalidBER()
	{
		CPPUNIT_ASSERT_THROW(demoBERProvider->notifyBERConsumers(-0.1, 1), wns::Exception);
		CPPUNIT_ASSERT_THROW(demoBERProvider->notifyBERConsumers(0.6, 1), wns::Exception);
	} // testInvalidBER

	void
	BERMeasurementReportingTest::testInvalidPacketSize()
	{
		CPPUNIT_ASSERT_THROW(demoBERProvider->notifyBERConsumers(0.1, 0), wns::Exception);
	} // testInvalidPacketSize

} // glue


