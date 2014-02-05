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

#ifndef WNS_LOGGER_TESTS_MASTERTEST_HPP
#define WNS_LOGGER_TESTS_MASTERTEST_HPP

#include <WNS/pyconfig/Parser.hpp>
#include <WNS/logger/Master.hpp>
#include <cppunit/extensions/HelperMacros.h>

namespace wns { namespace logger {
    class MasterTest :
        public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE( MasterTest );
        CPPUNIT_TEST( testWriting );
        CPPUNIT_TEST( testBacktrace );
        CPPUNIT_TEST_SUITE_END();
    public:
        void setUp();
        void tearDown();
        void testWriting();
        void testBacktrace();
    private:
    };
}
}
#endif // NOT defined WNS_LOGGER_TESTS_MASTERTEST_HPP
