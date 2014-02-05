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

#include <GLUE/Stamper.hpp>

#include <WNS/ldk/tests/FUTestBase.hpp>
#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/pyconfig/helper/Functions.hpp>
#include <WNS/CppUnit.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace glue { namespace tests {

    class StamperTestCommand :
	public wns::ldk::Command
    {
    public:
	struct {} local;
	struct {} peer;
	struct {
	    bool postPaid;
	} magic;
    };

    class StamperTest :
	public wns::ldk::tests::FUTestBase
    {
	CPPUNIT_TEST_SUITE( StamperTest );
	CPPUNIT_TEST( noStamp );
	CPPUNIT_TEST( stamp );
	CPPUNIT_TEST_SUITE_END();

    public:
	void
	noStamp();

	void
	stamp();

    private:

	virtual void
	setUpTestFUs();

	virtual void
	tearDownTestFUs();

	virtual wns::ldk::FunctionalUnit*
	getUpperTestFU() const;

	virtual wns::ldk::FunctionalUnit*
	getLowerTestFU() const;

	Stamper<StamperTestCommand>* stamper;
    };

    CPPUNIT_TEST_SUITE_REGISTRATION(StamperTest);

    void
    StamperTest::setUpTestFUs()
    {
	wns::pyconfig::View stamperConfig = wns::pyconfig::helper::createViewFromDropInConfig("glue.Stamper", "StamperTest");
	stamper = new Stamper<StamperTestCommand>(getFUN(), stamperConfig);
	getFUN()->addFunctionalUnit("stamper", stamper);
    }

    void
    StamperTest::tearDownTestFUs()
    {
    }

    wns::ldk::FunctionalUnit*
    StamperTest::getUpperTestFU() const
    {
	return stamper;
    }

    wns::ldk::FunctionalUnit*
    StamperTest::getLowerTestFU() const
    {
	return getUpperTestFU();
    }

    void
    StamperTest::noStamp()
    {
	WNS_ASSERT_ASSURE_EXCEPTION(sendCompound(newFakeCompound()));
    }

    void
    StamperTest::stamp()
    {
	// test if stamping works
	StamperTestCommand stamp;
	stamp.magic.postPaid = false;
	stamper->setStamp(stamp);
	wns::ldk::CompoundPtr compound;
	CPPUNIT_ASSERT_NO_THROW(compound = sendCompound(newFakeCompound()));
	CPPUNIT_ASSERT_EQUAL((unsigned int)1, compoundsSent());
	StamperTestCommand* command = stamper->getCommand(compound->getCommandPool());
	CPPUNIT_ASSERT(!command->magic.postPaid);

	// test if Stamper uses its own instance to stamp
	stamp.magic.postPaid = true;
	compound = sendCompound(newFakeCompound());
	CPPUNIT_ASSERT_EQUAL((unsigned int)2, compoundsSent());
	command = stamper->getCommand(compound->getCommandPool());
	CPPUNIT_ASSERT(!command->magic.postPaid);

	// test if updating the stamp works
	stamper->setStamp(stamp);
	compound = sendCompound(newFakeCompound());
	CPPUNIT_ASSERT_EQUAL((unsigned int)3, compoundsSent());
	command = stamper->getCommand(compound->getCommandPool());
	CPPUNIT_ASSERT(command->magic.postPaid);
    }
}}
