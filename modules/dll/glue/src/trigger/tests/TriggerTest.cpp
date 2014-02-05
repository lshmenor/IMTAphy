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

#include <GLUE/trigger/Trigger.hpp>

#include <GLUE/BERProvider.hpp>
#include <GLUE/trigger/FunctionalUnitLight.hpp>
#include <WNS/ldk/Layer.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/sar/Fixed.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>

#include <WNS/pyconfig/Parser.hpp>
#include <WNS/Exception.hpp>
#include <WNS/CppUnit.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace glue { namespace trigger {

	class TriggerTest :
		public wns::TestFixture
	{
		CPPUNIT_TEST_SUITE( TriggerTest );
		CPPUNIT_TEST( testSegmentSizeCalculation );
		CPPUNIT_TEST_SUITE_END();

	public:
		class DemoBERProvider :
			public FunctionalUnitLight,
			public BERProvider
		{
		public:
			DemoBERProvider(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config) :
				FunctionalUnitLight(fun, config)
			{} // DemoBERProvider
		};

		void prepare();
		void cleanup();

		void testSegmentSizeCalculation();

	private:
		wns::ldk::ILayer* layer;
		wns::ldk::fun::Main* fun;

		Trigger* trigger;
		DemoBERProvider* demoBERProvider;
		wns::ldk::sar::Fixed* sar;
};

	CPPUNIT_TEST_SUITE_REGISTRATION( TriggerTest );

	STATIC_FACTORY_REGISTER_WITH_CREATOR(TriggerTest::DemoBERProvider,
					     wns::ldk::FunctionalUnit,
					     "glue.trigger.tests.DemoBERProvider",
					     wns::ldk::FUNConfigCreator);

	void
	TriggerTest::prepare()
	{
		layer = new wns::ldk::tests::LayerStub();
		fun = new wns::ldk::fun::Main(layer);

		wns::pyconfig::Parser demoBERProviderPyCo;
		demoBERProviderPyCo.loadString("class DemoBERProvider:\n"
					       "  __plugin__ = 'glue.trigger.tests.DemoBERProvider'\n"
					       "  name = \"DemoBERProvider\"\n"
					       "demoBERProvider = DemoBERProvider\n");
		wns::pyconfig::View DemoBERProviderView(demoBERProviderPyCo, "demoBERProvider");
		demoBERProvider = new DemoBERProvider(fun, DemoBERProviderView);

		wns::pyconfig::Parser SARPyCo;
		SARPyCo.loadString("import openwns.SAR\n"
				   "sar = openwns.SAR.Fixed(100)\n");
		wns::pyconfig::View SARView(SARPyCo, "sar");
		sar = new wns::ldk::sar::Fixed(fun, SARView);

		wns::pyconfig::Parser triggerPyCo;
		triggerPyCo.loadString("import glue.Trigger\n"
				       "trigger = glue.Trigger.Trigger(\"demoBERProvider\", \"sar\")\n");
		wns::pyconfig::View TriggerView(triggerPyCo, "trigger");
		trigger = new Trigger(fun, TriggerView);

		fun->addFunctionalUnit("demoBERProvider", demoBERProvider);
		fun->addFunctionalUnit("sar", sar);
		fun->addFunctionalUnit("trigger", trigger);

		// find friends
		fun->onFUNCreated();

	} // prepare

	void
	TriggerTest::cleanup()
	{
		// deleted by fun (FUN)
		// delete demoBERProvider;
		// delete sar;
		// delete trigger;

		delete fun;
		delete layer;
	} // cleanup

	void
	TriggerTest::testSegmentSizeCalculation()
	{
		// BER 1e-6, opt. payload size (int)7968.062 = 7968
		demoBERProvider->notifyBERConsumers(1e-6, 1000);
		CPPUNIT_ASSERT_EQUAL(Bit(7968), sar->getSegmentSize());

		// BER 1e-5, opt. payload size (int)2498.018 = 2498
		demoBERProvider->notifyBERConsumers(1e-5, 1000);
		CPPUNIT_ASSERT_EQUAL(Bit(2498), sar->getSegmentSize());

		// BER 1e-4, opt. payload size (int)768.620 = 768
		demoBERProvider->notifyBERConsumers(1e-4, 1000);
		CPPUNIT_ASSERT_EQUAL(Bit(768), sar->getSegmentSize());

		// BER 1e-3, opt. payload size (int)222.935 = 222
		demoBERProvider->notifyBERConsumers(1e-3, 1000);
		CPPUNIT_ASSERT_EQUAL(Bit(222), sar->getSegmentSize());

		// BER 1e-2, opt. payload size (int)53.976 = 53
		demoBERProvider->notifyBERConsumers(1e-2, 1000);
		CPPUNIT_ASSERT_EQUAL(Bit(53), sar->getSegmentSize());
	} // testSegmentSizeCalculation
} // trigger
} // glue


