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

#ifndef WNS_LOGGER_TEST_LOGGERTEST_HPP
#define WNS_LOGGER_TEST_LOGGERTEST_HPP

#include <WNS/pyconfig/Parser.hpp>
#include <WNS/logger/Logger.hpp>
#include <cppunit/extensions/HelperMacros.h>

namespace wns { namespace logger {
    class LoggerTest :
        public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE( LoggerTest );
        CPPUNIT_TEST( stringConstructor );
        CPPUNIT_TEST( pyConfigViewConstructorWithMasterLogger );
        CPPUNIT_TEST( pyConfigViewConstructor );
        CPPUNIT_TEST( configure );
        CPPUNIT_TEST( send );
        CPPUNIT_TEST( macro );
        CPPUNIT_TEST( singleLineMacro );
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp();
        void tearDown();
        void stringConstructor();
        void pyConfigViewConstructorWithMasterLogger();
        void pyConfigViewConstructor();
        void configure();
        void send();
        void macro();
        void singleLineMacro();
    }; // LoggerTest
} // logger
} // wns
#endif // not defined WNS_LOGGER_TEST_LOGGERTEST_HPP
