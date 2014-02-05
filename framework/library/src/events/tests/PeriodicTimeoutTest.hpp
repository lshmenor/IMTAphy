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

#ifndef WNS_EVNTS_TESTS_PERIODICTIMEOUTTEST_HPP
#define WNS_EVNTS_TESTS_PERIODICTIMEOUTTEST_HPP

#include <WNS/events/PeriodicTimeout.hpp>

#include <WNS/TestFixture.hpp>


namespace wns { namespace events { namespace tests {

    class PeriodicTimeoutTest :
        public wns::TestFixture
    {
        CPPUNIT_TEST_SUITE( PeriodicTimeoutTest );
        CPPUNIT_TEST( create );
        CPPUNIT_TEST( configureTimeout );
        CPPUNIT_TEST( testPeriod );
        CPPUNIT_TEST( testDelay );
        CPPUNIT_TEST( testCopyConstructor );
        CPPUNIT_TEST_SUITE_END();
    public:
        void prepare();
        void cleanup();

        void create();
        void configureTimeout();
        void testPeriod();
        void testDelay();
        void testCopyConstructor();
    private:

        class derivedPeriodicTimeout :
            public PeriodicTimeout
        {
        public:
            derivedPeriodicTimeout()
                :PeriodicTimeout(),
                 test_counter(0)
                {
                };

            void periodically()
                {
                    test_counter++;
                };
            int test_counter;
        };

    };
} // tests
} // events
} // wns
#endif //NOT defined WNS_EVNTS_TESTS_PERIODICTIMEOUTTEST_HPP


