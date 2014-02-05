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

#include <WNS/logger/tests/LoggerTestHelper.hpp>
#include <WNS/logger/tests/LoggerTest.hpp>
#include <WNS/pyconfig/Parser.hpp>
#include <iostream>

using namespace wns::logger;
using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( LoggerTest );

void LoggerTest::setUp()
{
    TestOutput::result = "";
}

void LoggerTest::tearDown()
{}

void LoggerTest::stringConstructor()
{
#ifndef WNS_NO_LOGGING
    Logger l("WNS", "LoggerTest", NULL);
    CPPUNIT_ASSERT( l.enabled == true );
    CPPUNIT_ASSERT( l.lName == "LoggerTest" );
    CPPUNIT_ASSERT( l.mName == "WNS" );
#else
    Logger l("", "", NULL);
#endif
}

void LoggerTest::pyConfigViewConstructorWithMasterLogger()
{
#ifndef WNS_NO_LOGGING
    pyconfig::Parser p;
    stringstream s;
    s << "from openwns.logger import Logger\n"
      << "logger = Logger('WNS', 'LoggerTest', True)\n";
    p.loadString(s.str());
    Logger l(p.getView("logger"), NULL);
    CPPUNIT_ASSERT( l.enabled == true );
    CPPUNIT_ASSERT( l.lName == "LoggerTest" );
    CPPUNIT_ASSERT( l.getLoggerName() == "LoggerTest" );
    CPPUNIT_ASSERT( l.mName == "WNS" );
    CPPUNIT_ASSERT( l.getModuleName() == "WNS" );

    pyconfig::Parser p2;
    stringstream s2;
    s2 << "from openwns.logger import Logger\n"
       << "logger = Logger('WNS', 'LoggerTest2', False)\n";
    p2.loadString(s2.str());
    Logger l2(p2.getView("logger"), NULL);
    CPPUNIT_ASSERT( l2.enabled == false );
    CPPUNIT_ASSERT( l2.lName == "LoggerTest2" );
    CPPUNIT_ASSERT( l2.getLoggerName() == "LoggerTest2" );
    CPPUNIT_ASSERT( l2.mName == "WNS" );
    CPPUNIT_ASSERT( l2.getModuleName() == "WNS" );
    CPPUNIT_ASSERT( l2.level == 2 );
    CPPUNIT_ASSERT( l2.getLevel() == 2 );
#endif
}

void LoggerTest::pyConfigViewConstructor()
{
#ifndef WNS_NO_LOGGING
    pyconfig::Parser p;
    stringstream s;
    s << "from openwns.logger import Logger\n"
      << "logger = Logger('WNS', 'LoggerTest', True)\n";
    p.loadString(s.str());
    Logger l(p.getView("logger"));
    CPPUNIT_ASSERT( l.enabled == true );
    CPPUNIT_ASSERT( l.lName == "LoggerTest" );
    CPPUNIT_ASSERT( l.getLoggerName() == "LoggerTest" );
    CPPUNIT_ASSERT( l.mName == "WNS" );
    CPPUNIT_ASSERT( l.getModuleName() == "WNS" );

    pyconfig::Parser p2;
    stringstream s2;
    s2 << "from openwns.logger import Logger\n"
       << "logger = Logger('WNS', 'LoggerTest2', False)\n";
    p2.loadString(s2.str());
    Logger l2(p2.getView("logger"));
    CPPUNIT_ASSERT( l2.enabled == false );
    CPPUNIT_ASSERT( l2.lName == "LoggerTest2" );
    CPPUNIT_ASSERT( l2.getLoggerName() == "LoggerTest2" );
    CPPUNIT_ASSERT( l2.mName == "WNS" );
    CPPUNIT_ASSERT( l2.getModuleName() == "WNS" );
    CPPUNIT_ASSERT( l2.level == 2 );
    CPPUNIT_ASSERT( l2.getLevel() == 2 );
#endif
}

void LoggerTest::configure()
{
#ifndef WNS_NO_LOGGING
    pyconfig::Parser p;
    stringstream s;
    s << "from openwns.logger import Logger\n"
      << "logger = Logger('WNS', 'LoggerTest', True)\n";
    p.loadString(s.str());
    Logger l;
    l.configure(p.getView("logger"));
    CPPUNIT_ASSERT( l.enabled == true );
    CPPUNIT_ASSERT( l.lName == "LoggerTest" );
    CPPUNIT_ASSERT( l.getLoggerName() == "LoggerTest" );
    CPPUNIT_ASSERT( l.mName == "WNS" );
    CPPUNIT_ASSERT( l.getModuleName() == "WNS" );

    pyconfig::Parser p2;
    stringstream s2;
    s2 << "from openwns.logger import Logger\n"
       << "logger = Logger('WNS', 'LoggerTest2', False)\n";
    p2.loadString(s2.str());
    Logger l2;
    l2.configure(p2.getView("logger"));
    CPPUNIT_ASSERT( l2.enabled == false );
    CPPUNIT_ASSERT( l2.lName == "LoggerTest2" );
    CPPUNIT_ASSERT( l2.getLoggerName() == "LoggerTest2" );
    CPPUNIT_ASSERT( l2.mName == "WNS" );
    CPPUNIT_ASSERT( l2.getModuleName() == "WNS" );
    CPPUNIT_ASSERT( l2.level == 2 );
    CPPUNIT_ASSERT( l2.getLevel() == 2 );
#endif
}


void LoggerTest::send()
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

    string mod("WNS");
    string loc("LoggerTest");
    string msg("foobar");
    Logger l(mod, loc, &t);
    CPPUNIT_ASSERT( l.getLevel() == 2 );

    Message m("foobar");

    // Send a normal message
    l.send(m);
#ifndef WNS_NO_LOGGING
    CPPUNIT_ASSERT( TestOutput::result == mod+loc+msg );
#else
    CPPUNIT_ASSERT( TestOutput::result == "" );
#endif

    // send a normal message, logger switched off
    TestOutput::result = "";
    l.switchOff();
    l.send(m);
    CPPUNIT_ASSERT( TestOutput::result == "" );

    // send a normal message, logger switched on again
    TestOutput::result = "";
    l.switchOn();
    l.send(m);
#ifndef WNS_NO_LOGGING
    CPPUNIT_ASSERT( TestOutput::result == mod+loc+msg );
#else
    CPPUNIT_ASSERT( TestOutput::result == "" );
#endif


    // send a normal message with verbosity level 3, logger will be silent,
    // because it has the default level of 2 set
    Message m3("foobar",3);
    TestOutput::result = "";
    l.send(m3);
    CPPUNIT_ASSERT( TestOutput::result == "" );

    // send a normal message with verbosity level 2, logger will log
    Message m2("foobar",2);
    TestOutput::result = "";
    l.send(m2);
#ifndef WNS_NO_LOGGING
    CPPUNIT_ASSERT( TestOutput::result == mod+loc+msg );
#else
    CPPUNIT_ASSERT( TestOutput::result == "" );
#endif

    // send a normal message, Logger has null pointer as MasterLogger
    // -> Logger will not log
    TestOutput::result = "";
    l = Logger(mod, loc, NULL);
    l.send(m);
    CPPUNIT_ASSERT( TestOutput::result == "" );
}

void LoggerTest::macro()
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

    string mod("WNS");
    string loc("LoggerTest");
    string msg("foobar");
    Logger l(mod, loc, &t);

    // try the macro
    MESSAGE_BEGIN(NORMAL, l, m, "foobar");
    MESSAGE_END();

#ifndef WNS_NO_LOGGING
    CPPUNIT_ASSERT_MESSAGE( TestOutput::result, TestOutput::result == mod+loc+msg );
#else
    CPPUNIT_ASSERT_MESSAGE( TestOutput::result, TestOutput::result == "" );
#endif
}

void LoggerTest::singleLineMacro()
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

    string mod("WNS");
    string loc("LoggerTest");
    string msg("foobar 8");
    Logger l(mod, loc, &t);

    // try the macro
    MESSAGE_SINGLE(NORMAL, l, "foobar " << 8);

#ifndef WNS_NO_LOGGING
    CPPUNIT_ASSERT_MESSAGE( TestOutput::result, TestOutput::result == mod+loc+msg );
#else
    CPPUNIT_ASSERT_MESSAGE( TestOutput::result, TestOutput::result == "" );
#endif
}
