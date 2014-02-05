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

#include <GLUE/BERProvider.hpp>
#include <GLUE/BERConsumer.hpp>

#include <WNS/Exception.hpp>
#include <WNS/CppUnit.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace glue {

	class BERProviderConsumerTest :
		public wns::TestFixture
	{
		CPPUNIT_TEST_SUITE( BERProviderConsumerTest );
		CPPUNIT_TEST( testAttachBERConsumer );
		CPPUNIT_TEST( testAttachBERConsumerAlreadyAttached );
		CPPUNIT_TEST( testDetachBERConsumer );
		CPPUNIT_TEST( testDetachBERConsumerNotAttached );
		CPPUNIT_TEST( testNotifyBERConsumers );
		CPPUNIT_TEST( testDestroyBERConsumer );
		CPPUNIT_TEST( testDestroyBERProvider );
		CPPUNIT_TEST_SUITE_END();

	public:
		class BERConsumerTest :
			public BERConsumer
		{
		public:
			BERConsumerTest();

			virtual void
			onBERProviderDeleted();

			virtual void
			onNewMeasurement(double _ber, int _packetSize);

			int berProviderDeleted;

			double ber;
			int packetSize;
		};

		void prepare();
		void cleanup();

		void testAttachBERConsumer();
		void testAttachBERConsumerAlreadyAttached();
		void testDetachBERConsumer();
		void testDetachBERConsumerNotAttached();
		void testNotifyBERConsumers();
		void testDestroyBERConsumer();
		void testDestroyBERProvider();

	private:
		BERProvider* berProvider;
		BERConsumerTest* berConsumerTest;
		BERConsumerTest* secondBerConsumerTest;
	};

	CPPUNIT_TEST_SUITE_REGISTRATION( BERProviderConsumerTest );

	BERProviderConsumerTest::BERConsumerTest::BERConsumerTest() :
		BERConsumer(),
		berProviderDeleted(0),
		ber(0.0),
		packetSize(0)
	{} // BERConsumer

	void
	BERProviderConsumerTest::BERConsumerTest::onBERProviderDeleted()
	{
		++berProviderDeleted;
	} // BERProviderDeleted

	void
	BERProviderConsumerTest::BERConsumerTest::onNewMeasurement(double _ber, int _packetSize)
	{
		ber = _ber;
		packetSize = _packetSize;
	} // reportBER

	void
	BERProviderConsumerTest::prepare()
	{
		berProvider = new BERProvider();
		berConsumerTest = new BERConsumerTest();
		secondBerConsumerTest = new BERConsumerTest();
	} // prepare

	void
	BERProviderConsumerTest::cleanup()
	{
		if (berConsumerTest)
			delete berConsumerTest;
		if (secondBerConsumerTest)
			delete secondBerConsumerTest;
		if (berProvider)
			delete berProvider;
	} // catch ExceptionTearDown

	void
	BERProviderConsumerTest::testAttachBERConsumer()
	{
		berProvider->attachBERConsumer(berConsumerTest);

		CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), berProvider->getBERConsumersAttached().size());
		CPPUNIT_ASSERT_EQUAL(dynamic_cast<BERConsumer*>(berConsumerTest), berProvider->getBERConsumersAttached().front());

		berProvider->attachBERConsumer(secondBerConsumerTest);

		CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), berProvider->getBERConsumersAttached().size());
		CPPUNIT_ASSERT_EQUAL(dynamic_cast<BERConsumer*>(secondBerConsumerTest), berProvider->getBERConsumersAttached().back());
	} // testAttachBERConsumer

	void
	BERProviderConsumerTest::testAttachBERConsumerAlreadyAttached()
	{
		berProvider->attachBERConsumer(berConsumerTest);

		WNS_ASSERT_ASSURE_EXCEPTION(berProvider->attachBERConsumer(berConsumerTest));
	} // testAttachBERConsumerAlreadyAttached

	void
	BERProviderConsumerTest::testDetachBERConsumer()
	{
		berProvider->attachBERConsumer(berConsumerTest);
		berProvider->attachBERConsumer(secondBerConsumerTest);

		berProvider->detachBERConsumer(berConsumerTest);
		CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), berProvider->getBERConsumersAttached().size());
		CPPUNIT_ASSERT_EQUAL(dynamic_cast<BERConsumer*>(secondBerConsumerTest), berProvider->getBERConsumersAttached().front());

		berProvider->detachBERConsumer(secondBerConsumerTest);
		CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), berProvider->getBERConsumersAttached().size());
	} // testDetachBERConsumer

	void
	BERProviderConsumerTest::testDetachBERConsumerNotAttached()
	{
		WNS_ASSERT_ASSURE_EXCEPTION(berProvider->detachBERConsumer(berConsumerTest));
	} // testDetachBERConsumerNotAttached

	void
	BERProviderConsumerTest::testNotifyBERConsumers()
	{
		berProvider->attachBERConsumer(berConsumerTest);
		berProvider->attachBERConsumer(secondBerConsumerTest);

		berProvider->notifyBERConsumers(12.3, 456);

		CPPUNIT_ASSERT_EQUAL(12.3, berConsumerTest->ber);
		CPPUNIT_ASSERT_EQUAL(456, berConsumerTest->packetSize);

		CPPUNIT_ASSERT_EQUAL(12.3, secondBerConsumerTest->ber);
		CPPUNIT_ASSERT_EQUAL(456, secondBerConsumerTest->packetSize);
	} // testNotifyBERConsumers

	void
	BERProviderConsumerTest::testDestroyBERConsumer()
	{
		berProvider->attachBERConsumer(berConsumerTest);
		berProvider->attachBERConsumer(secondBerConsumerTest);

		delete berConsumerTest;
		berConsumerTest = NULL;

		CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), berProvider->getBERConsumersAttached().size());
		CPPUNIT_ASSERT_EQUAL(dynamic_cast<BERConsumer*>(secondBerConsumerTest), berProvider->getBERConsumersAttached().front());
	} // testDestroyBERConsumer

	void
	BERProviderConsumerTest::testDestroyBERProvider()
	{
		berProvider->attachBERConsumer(berConsumerTest);
		berProvider->attachBERConsumer(secondBerConsumerTest);

		delete berProvider;
		berProvider = NULL;

		CPPUNIT_ASSERT_EQUAL(1, berConsumerTest->berProviderDeleted);
		CPPUNIT_ASSERT_EQUAL(1, secondBerConsumerTest->berProviderDeleted);
	} // testDestroyBERProvider

} // glue


