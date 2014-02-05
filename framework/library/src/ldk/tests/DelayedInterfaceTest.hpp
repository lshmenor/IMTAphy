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

#ifndef WNS_LDK_TESTS_DELAYEDINTERFACETEST_HPP
#define WNS_LDK_TESTS_DELAYEDINTERFACETEST_HPP

#include <WNS/ldk/tests/FUTestBase.hpp>
#include <WNS/ldk/Delayed.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace wns { namespace ldk { namespace tests {

    /**
     * @brief Abstract test suite for DelayedInterface FUs.
     */
    class DelayedInterfaceTest :
	public FUTestBase
    {
	CPPUNIT_TEST_SUITE( DelayedInterfaceTest );
	CPPUNIT_TEST( nothingToSend );
	CPPUNIT_TEST( somethingToSend );
	CPPUNIT_TEST( compoundsSameSize );
	CPPUNIT_TEST( compoundsSameCommands );
	CPPUNIT_TEST( compoundsSameJourney );
	CPPUNIT_TEST( compoundsSameData );
	CPPUNIT_TEST_SUITE_END_ABSTRACT();

    public:
	DelayedInterfaceTest();

	void
	nothingToSend();

	void
	somethingToSend();

	void
	compoundsSameSize();

	void
	compoundsSameCommands();

	void
	compoundsSameJourney();

	void
	compoundsSameData();

    protected:
	/**
	 * @brief Return the testee.
	 *
	 * Use this to access the testee. getTestee() will do a
	 * dynamic_cast to CAST*.
	 */
	template<class CAST>
	CAST* getTestee()
	{
	    return dynamic_cast<CAST*>(testee);
	} // getTestee

	virtual void
	setUpTestFUs();

	virtual void
	tearDownTestFUs();

    private:
	/**
	 * @brief Return a new testee.
	 *
	 * Deriving test suites must implement this.
	 */
	virtual DelayedInterface*
	newTestee() = 0;

	/**
	 * @brief Tear down a testee.
	 *
	 * @note The testee doesn't need to be deleted; this will be done
	 * by the FUN.
	 */
	virtual void
	tearDownTestee(DelayedInterface* testee) = 0;

	virtual FunctionalUnit*
	getUpperTestFU() const;

	virtual FunctionalUnit*
	getLowerTestFU() const;

	DelayedInterface* testee;
    }; // DelayedInterfaceTest

}}}

#endif // WNS_LDK_TESTS_DELAYEDTEST_HPP
