/******************************************************************************
 * Glue                                                                       *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2005-2006                                                    *
 * Lehrstuhl fuer Kommunikationsnetze (ComNets)                               *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnets.rwth-aachen.de                                          *
 * www: http://wns.comnets.rwth-aachen.de                                     *
 ******************************************************************************/

#include <GLUE/Pilot.hpp>

#include <WNS/ldk/tools/Stub.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/ldk/Layer.hpp>
#include <WNS/pyconfig/helper/Functions.hpp>
#include <WNS/pyconfig/Parser.hpp>
#include <WNS/CppUnit.hpp>
#include <WNS/events/scheduler/Interface.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace glue { namespace tests {

    class PilotTest :
	public wns::TestFixture
    {
	CPPUNIT_TEST_SUITE( PilotTest );
	CPPUNIT_TEST( beacon );
	CPPUNIT_TEST( timeout );
	CPPUNIT_TEST( delayedBeacon );
	CPPUNIT_TEST( observers );
	CPPUNIT_TEST_SUITE_END();
    public:
	PilotTest();

	virtual void
	prepare();

	virtual void
	cleanup();

	void
	beacon();

	void
	timeout();

	void
	delayedBeacon();

	void
	observers();

    private:
	wns::ldk::ILayer* layer;
	wns::ldk::fun::Main* fun;

	Pilot* pilot;
	wns::ldk::tools::Stub* lowerFU;
    };

    class PilotObserver :
	public Pilot::Observer
    {
    public:
	using Pilot::Observer::Information;

	PilotObserver()
	    : counter(0)
	{}

	virtual void
	receivedBeacon(const Information&)
	{
	    ++counter;
	}

	unsigned int counter;
    };

    CPPUNIT_TEST_SUITE_REGISTRATION( PilotTest );

    PilotTest::PilotTest()
	: layer(NULL),
	  fun(NULL),
	  pilot(NULL),
	  lowerFU(NULL)
    {}

    void
    PilotTest::prepare()
    {
	wns::simulator::getInstance()->reset();

	layer = new wns::ldk::tests::LayerStub();
	fun = new wns::ldk::fun::Main(layer);

	wns::pyconfig::View pilotConfig = wns::pyconfig::helper::createViewFromDropInConfig("glue.Pilot", "PilotTest");
	pilot = new Pilot(fun, pilotConfig);
	pilot->disable();

	wns::pyconfig::Parser emptyConfig;
	lowerFU = new wns::ldk::tools::Stub(fun, emptyConfig);
	lowerFU->close();

	pilot->connect(lowerFU);

	fun->addFunctionalUnit("RedBaron", pilot);
	fun->addFunctionalUnit("FokkerDrI", lowerFU);
    }

    void
    PilotTest::cleanup()
    {
	if (fun != NULL) delete fun;
	if (layer != NULL) delete layer;
    }

    void
    PilotTest::beacon()
    {
	lowerFU->open(false);
	pilot->sendBeacon();
	lowerFU->close();
	CPPUNIT_ASSERT_EQUAL(std::size_t(1), lowerFU->sent.size());
    }

    void
    PilotTest::timeout()
    {
	lowerFU->open(false);
	pilot->enable();

	CPPUNIT_ASSERT(wns::simulator::getEventScheduler()->processOneEvent());
	CPPUNIT_ASSERT_EQUAL(std::size_t(1), lowerFU->sent.size());

	CPPUNIT_ASSERT(wns::simulator::getEventScheduler()->processOneEvent());
	CPPUNIT_ASSERT_EQUAL(std::size_t(2), lowerFU->sent.size());
    }

    void
    PilotTest::delayedBeacon()
    {
	pilot->sendBeacon();
	
	CPPUNIT_ASSERT_EQUAL(std::size_t(0), lowerFU->sent.size());

	lowerFU->open(true);

	CPPUNIT_ASSERT_EQUAL(std::size_t(1), lowerFU->sent.size());
    }

    void
    PilotTest::observers()
    {
	PilotObserver o1, o2, o3;
	pilot->attach(&o1);
	pilot->attach(&o2);
	pilot->attach(&o3);
	lowerFU->open(false);
	pilot->sendBeacon();
	CPPUNIT_ASSERT_EQUAL(std::size_t(1), lowerFU->sent.size());
	lowerFU->onData(lowerFU->sent[0]);

	CPPUNIT_ASSERT_EQUAL((unsigned int)1, o1.counter);
	CPPUNIT_ASSERT_EQUAL((unsigned int)1, o2.counter);
	CPPUNIT_ASSERT_EQUAL((unsigned int)1, o3.counter);

	pilot->detach(&o2);
	lowerFU->onData(lowerFU->sent[0]);

	CPPUNIT_ASSERT_EQUAL((unsigned int)2, o1.counter);
	CPPUNIT_ASSERT_EQUAL((unsigned int)1, o2.counter);
	CPPUNIT_ASSERT_EQUAL((unsigned int)2, o3.counter);
    }

}}
