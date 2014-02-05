/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 16, D-52074 Aachen, Germany
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

#ifndef WNS_TESTFIXTURE_HPP
#define WNS_TESTFIXTURE_HPP

#include <WNS/Assure.hpp>
#include <WNS/Conversion.hpp>

#include <WNS/events/scheduler/Interface.hpp>
#include <WNS/rng/RNGen.hpp>

#include <cppunit/TestAssert.h>
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cmath>

// Helper macros and functions for CPPUNIT

namespace wns {
    /**
     * @brief Derive from this class instead of CppUnit::TestFixture
     *
     * @author Marc Schinnenburg <marc@schinnenburg.com>
     * @ingroup group_main_classes
     *
     * This class sets up the WNS environment to run unit tests. Many units
     * which are under test in WNS need an EventScheduler or
     * RandomNumberGenerator to work. These elements (EventScheduler, RNG)
     * are Singletons within the simulation, e.g. because the EventScheduler
     * needs to be accessible from anywhere. Hence, the EventScheduler must
     * be reset to be in a (defined) pristine state (no events pending,
     * etc.) before a test is run. Similar things hold for the
     * RandomNumberGenerator. Setting up the WNS environment is performed in
     * the method setUp(). Tearing down (destroy objects, etc.) is taken
     * care of in tearDown(). To give the test writer the ability to prepare
     * and cleanup his test too, the wns::TestFixture provides two methods:
     *
     * <ul>
     *   <li> prepare()
     *   <li> cleanup()
     * </ul>
     *
     * prepare() is called after setUp() and cleanup() is called before the
     * tearDown()
     */
    class TestFixture :
        public CppUnit::TestFixture
    {
    public:
        /**
         * @brief Don't call manually!!!
         *
         * @warning This method is called by the testing framework. Don't call
         * it manually!!!!
         *
         * @todo (Marc Schinnenburg <marc@schinnenburg.com>) I wanted to
         * make this private, but the CPPUNIT implementation inhibits
         * it.
         */
        virtual void
        setUp();

        /**
         * @brief Don't call manually!!!
         *
         * @warning This method is called by the testing framework. Don't call
         * it manually!!!!
         *
         * @todo (Marc Schinnenburg <marc@schinnenburg.com>) I wanted to
         * make this private, but the CPPUNIT implementation inhibits
         * it.
         */
        virtual void
        tearDown();

    protected:
        /**
         * @brief Overload to prepare your test fixture before each
         * test
         */
        virtual void
        prepare() = 0;

        /**
         * @brief Overload to cleanup your test fixture after each
         * test
         */
        virtual void
        cleanup() = 0;
    };


    /**
     * @brief Construct a failure message containing file, line, expected
     * and actual value.
     */
    std::string
    failureMessage(
        const std::string& sourceFile,
        const std::string& sourceLine,
        const std::string& expected,
        const std::string& actual);

    /**
     * @brief Asserts that the relative error between "expected" and
     * "actual" is not greater than "maxRelativeError" (used for unit tests)
     *
     * @author Marc Schinnenburg <marc@schinnenburg.com>
     */
    template <typename T>
    void assertMaxRelError(
        const T& expected,
        const T& actual,
        double maxRelativeError,
        const std::string sourceFile,
        const std::string sourceLine)
    {
        CPPUNIT_ASSERT_MESSAGE( failureMessage(sourceFile,
                                               sourceLine,
                                               wns::to<std::string>(expected),
                                               wns::to<std::string>(actual)),
                                std::fabs(expected - actual) <= std::abs(expected * maxRelativeError) );
    }


    /**
     * @brief Keeps the names for the different test registries
     *
     * @author Marc Schinnenburg <marc@schinnenburg.com>
     *
     * WNS has different means to execute a number of unit tests:
     *
     *  - "./wns-core -t" will run all tests registered with
     *    CPPUNIT_TEST_SUITE_REGISTRATION( TestName ) or
     *    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( TestName, wns::testsuite::Default() )
     *
     *  - "./wns-core -T TestName" will run all tests registerd with at one of
     *    the registries specified in this namespace
     */
    namespace testsuite
    {
        /**
         * @brief CppUnit default registry
         */
        std::string
        Default();

        /**
         * @brief Registry for disabled tests
         */
        std::string
        Disabled();

        /**
         * @brief Registry for performance tests
         */
        std::string
        Performance();

        /**
         * @brief Registry for spikes
         */
        std::string
        Spike();
    }

}

/**
 * @brief Advanced version of CPPUNIT_ASSERT_DOUBLES_EQUAL
 * @author Marc Schinnenburg <marc@schinnenburg.com>
 *
 * CPPUNIT_ASSERT_DOUBLES_EQUAL lets you test if
 * @f[ |expected-actual|<error @f]
 * But normally you want to test for a realtive error:
 * @f[ \left|\frac{expected-actual}{expected}\right|<maxRelativeError @f]
 * This is performed if this macro is used.
 */
#define WNS_ASSERT_MAX_REL_ERROR(expected, actual, maxRelativeError) wns::assertMaxRelError((expected), (actual), (maxRelativeError), __FILE__, ::wns::to<std::string>(__LINE__))

#endif // NOT defined WNS_CPPUNIT_HPP

#ifdef WNS_ASSURE_THROWS_EXCEPTION

/**
 * @brief Assert that assure throws an exception.
 *
 * You should use this macro to check, if an assure throws an exception.
 * Assures are disabled on opt builds, as is this macro. Using this macro
 * your test won't fail for opt runs.
 */
#define WNS_ASSERT_ASSURE_EXCEPTION(expression) CPPUNIT_ASSERT_THROW(expression, wns::Assure::Exception)

/**
 * @brief Assert that assureNotNull throws an exception.
 *
 * @author Marc Schinnenburg <marc@schinnenburg.com>
 *
 * You should use this macro to check, if an assureNotNull throws an exception.
 * Assures are disabled on opt builds, as is this macro. Using this macro
 * your test won't fail for opt runs.
 */
#define WNS_ASSERT_ASSURE_NOT_NULL_EXCEPTION(expression) CPPUNIT_ASSERT_THROW(expression, wns::NullException)

#else

#define WNS_ASSERT_ASSURE_EXCEPTION(expression)
#define WNS_ASSERT_ASSURE_NOT_NULL_EXCEPTION(expression)

#endif // NOT defined WNS_TESTFIXTURE_HPP
