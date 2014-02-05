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

#include <WNS/testing/DetailedListener.hpp>

#include <iostream>
#include <iomanip>

using namespace wns::testing;

void
DetailedListener::startTest(CppUnit::Test* test)
{
    failed = false;
    std::cout << "\033[01;35m[TST]\033[00m ";;
    std::string testName = test->getName();
    int length = testName.length();
    std::string padding = "";
    if (length < 90)
    {
        std::cout << testName;
        padding.append(90 - length, ' ');
        std::cout << padding;
    }
    else
    {
        testName.erase(0, length-86);
        std::cout << "..." << testName << " ";
    }
    std::cout.flush();

    sw = wns::StopWatch();
    sw.start();
}

void
DetailedListener::addFailure(const CppUnit::TestFailure&)
{
    failed = true;
}

void
DetailedListener::endTest(CppUnit::Test* /*test*/)
{
    sw.stop();

    if (failed)
    {
        std::cout << "\033[01;31m[FAILED]\033[00m";
    }
    else
    {
        std::cout << "\033[01;32m[OK]\033[00m";
    }
    std::cout << " "
              << std::setiosflags(std::ios::right)
              << std::setiosflags(std::ios::fixed)
              << std::setiosflags(std::ios::dec)
              << std::setprecision(6)
              << std::setw(10)
              << sw.getInSeconds() << " s" << std::endl;
}

