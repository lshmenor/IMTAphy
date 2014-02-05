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

#include <WNS/TestFixture.hpp>
#include <WNS/Backtrace.hpp>

#include <features.h>

namespace wns { namespace tests {

	/**
	 * @brief test for wns::Backtrace
	 * @author Marc Schinnenburg <marc@schinnenburg.com>
	 */
	class BacktraceTest :
		public wns::TestFixture
	{
        CPPUNIT_TEST_SUITE( BacktraceTest );
        CPPUNIT_TEST( testConstructor );
		CPPUNIT_TEST( testFunctionCall );
        CPPUNIT_TEST( testClear );
		CPPUNIT_TEST_SUITE_END();

    public:

		void
		prepare()
		{
		}

		void
		cleanup()
		{
		}

        void
        testConstructor()
        {
            Backtrace backtrace;
            Backtrace::FunctionCalls functionCalls = backtrace.getFunctionCalls();
            CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(0) , functionCalls.size());
        }

		void
		testFunctionCall()
		{
            // begin example "wns.Backtrace.example"
            Backtrace backtrace;

            // make a snapshot of the current stack
            backtrace.snapshot();

            // backtrace can be used in streams
            std::stringstream tmpStream;
            tmpStream << backtrace;

            // retrive a vector with all function calls
            Backtrace::FunctionCalls functionCalls = backtrace.getFunctionCalls();

#ifdef __GLIBC__
            // this works only with glibc

            // at least this function and the snapshot function must be on the stack
            CPPUNIT_ASSERT(functionCalls.size() >= static_cast<size_t>(2));

            // copy the first element
            Backtrace::FunctionCall functionCall = functionCalls.at(0);

            // the elements in the vector (Backtrace::FunctionCall) provide the
            // following methods
            std::string tmpString = "";
            tmpString = functionCall.getOrigin();
            tmpString = functionCall.getOffset();
            tmpString = functionCall.getReturnAddress();

            // most interesting is probably the method getName() which returns
            // the name of the function at the respective position on the stack
            CPPUNIT_ASSERT_MESSAGE( functionCall.getName(), functionCall.getName() == "wns::Backtrace::snapshot()");

            CPPUNIT_ASSERT_MESSAGE( functionCall.getOrigin(), functionCall.getOrigin() != "unknown");
            CPPUNIT_ASSERT_MESSAGE( functionCall.getOffset(), functionCall.getOffset() != "unknown");
            CPPUNIT_ASSERT_MESSAGE( functionCall.getReturnAddress(), functionCall.getReturnAddress() != "unknown");
#else
            // no backtrace available
	    CPPUNIT_ASSERT(functionCalls.size() == static_cast<size_t>(0));
#endif // __GLIBC__

            // end example
        }

        void
        testClear()
        {
            Backtrace backtrace;

            backtrace.snapshot();
            backtrace.clear();
            Backtrace::FunctionCalls functionCalls = backtrace.getFunctionCalls();
            CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(0) , functionCalls.size());
        }
    };

	CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( BacktraceTest, wns::testsuite::Default() );

} // namespace tests
} // namespace wns
