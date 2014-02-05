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

#include <WNS/logger/tests/MasterTest.hpp>
#include <WNS/logger/tests/LoggerTestHelper.hpp>

using namespace wns::logger;
using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( MasterTest );

void MasterTest::setUp()
{
    TestOutput::result = "";
}

void MasterTest::tearDown()
{}

void MasterTest::testWriting()
{
    std::stringstream s;
    s << "from openwns.logger import *\n"
      << "masterLogger = Master()\n"
      << "class Test:\n"
      << "  __plugin__ = 'Test' \n"
      << "masterLogger.loggerChain = [FormatOutputPair(Test(), Test())] ";
    pyconfig::Parser p;
    p.loadString(s.str());

    Master t(p.getView("masterLogger"));

    TestOutput::result = "";

    t.write("A", "B", "C");
    CPPUNIT_ASSERT_MESSAGE( TestOutput::result, TestOutput::result=="ABC" );

    TestOutput::result = "";

    t.write(string("A"), string("B"), string("C") );
    CPPUNIT_ASSERT_MESSAGE( TestOutput::result, TestOutput::result=="ABC" );

    std::stringstream s2;
    s2 << "from openwns.logger import *\n"
       << "masterLogger = Master()\n"
       << "class Test:\n"
       << "  __plugin__ = 'Test' \n"
       << "masterLogger.loggerChain = [FormatOutputPair(Test(), Test())]\n"
       << "masterLogger.enabled = False";
    pyconfig::Parser p2;
    p2.loadString(s2.str());

    TestOutput::result = "";
    Master t2(p2.getView("masterLogger"));

    CPPUNIT_ASSERT_MESSAGE( TestOutput::result, TestOutput::result=="" );
}

void MasterTest::testBacktrace()
{
    stringstream s;
    s << "from openwns.logger import *\n"
      << "masterLogger = Master()\n"
      << "class Test:\n"
      << "  __plugin__ = 'Test' \n"
      << "masterLogger.loggerChain = [FormatOutputPair(Test(), Test())]\n"
      << "masterLogger.backtrace.enabled = True\n"
      << "masterLogger.backtrace.length = 3\n";
    Master t(pyconfig::Parser::fromString(s.str()).getView("masterLogger"));
    t.write("A", "B", "1");
    t.write("A", "B", "2");
    t.write("A", "B", "3");
    t.write("A", "B", "4");
    t.write("A", "B", "5");
    TestOutput::result = "";
    t.outputBacktrace();
    CPPUNIT_ASSERT_MESSAGE( TestOutput::result, std::string("AB3AB4AB5") == TestOutput::result );
}
