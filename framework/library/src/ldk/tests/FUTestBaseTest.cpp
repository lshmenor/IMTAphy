/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
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

#include <WNS/ldk/tests/FUTestBase.hpp>
#include <WNS/ldk/tools/Stub.hpp>
#include <WNS/pyconfig/Parser.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace wns { namespace ldk { namespace tests {

    class FUTestBaseTest :
	public FUTestBase
    {
	CPPUNIT_TEST_SUITE( FUTestBaseTest );
	CPPUNIT_TEST( counters );
	CPPUNIT_TEST_SUITE_END();
    public:

	void
	counters();

    private:
	virtual void
	setUpTestFUs();

	virtual void
	tearDownTestFUs();

	virtual FunctionalUnit*
	getUpperTestFU() const;

	virtual FunctionalUnit*
	getLowerTestFU() const;

	tools::Stub* stub;
    };

    CPPUNIT_TEST_SUITE_REGISTRATION( FUTestBaseTest );

    void
    FUTestBaseTest::setUpTestFUs()
    {
	pyconfig::Parser emptyconfig;
	stub = new tools::Stub(getFUN(), emptyconfig);
	getFUN()->addFunctionalUnit("testFU", stub);
    }

    void
    FUTestBaseTest::tearDownTestFUs()
    {
    }

    FunctionalUnit*
    FUTestBaseTest::getUpperTestFU() const
    {
	return stub;
    }

    FunctionalUnit*
    FUTestBaseTest::getLowerTestFU() const
    {
	return getUpperTestFU();
    }

    void
    FUTestBaseTest::counters()
    {
	CPPUNIT_ASSERT_EQUAL((unsigned int)0, compoundsAccepted());
	CPPUNIT_ASSERT_EQUAL((unsigned int)0, compoundsSent());
	CPPUNIT_ASSERT_EQUAL((unsigned int)0, compoundsReceived());
	CPPUNIT_ASSERT_EQUAL((unsigned int)0, compoundsDelivered());

	sendCompound(newFakeCompound());

	CPPUNIT_ASSERT_EQUAL((unsigned int)1, compoundsAccepted());
	CPPUNIT_ASSERT_EQUAL((unsigned int)1, compoundsSent());
	CPPUNIT_ASSERT_EQUAL((unsigned int)0, compoundsReceived());
	CPPUNIT_ASSERT_EQUAL((unsigned int)0, compoundsDelivered());

	stub->close();
	sendCompound(newFakeCompound());
	stub->open();

	CPPUNIT_ASSERT_EQUAL((unsigned int)2, compoundsAccepted());
	CPPUNIT_ASSERT_EQUAL((unsigned int)1, compoundsSent());
	CPPUNIT_ASSERT_EQUAL((unsigned int)0, compoundsReceived());
	CPPUNIT_ASSERT_EQUAL((unsigned int)0, compoundsDelivered());

	receiveCompound(sendCompound(newFakeCompound()));

	CPPUNIT_ASSERT_EQUAL((unsigned int)3, compoundsAccepted());
	CPPUNIT_ASSERT_EQUAL((unsigned int)2, compoundsSent());
	CPPUNIT_ASSERT_EQUAL((unsigned int)1, compoundsReceived());
	CPPUNIT_ASSERT_EQUAL((unsigned int)1, compoundsDelivered());
    }

}}}
