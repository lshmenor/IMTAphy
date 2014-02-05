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
#include <WNS/simulator/ISimulator.hpp>

#include <iostream>

using namespace wns;

void
TestFixture::setUp()
{
    try
    {
        wns::simulator::getInstance()->reset();
        this->prepare();
    }
    catch (const wns::Exception& e)
    {
        std::cerr << "\n"
                  << "A wns::Exception occured during prepare(): \n"
                  << e.what() << "\n";
        throw;
    }
    catch (const std::exception& e)
    {
        std::cerr << "\n"
                  << "An std::exception occured during prepare(): \n"
                  << e.what() << "\n";
        throw;
    }
}

void
TestFixture::tearDown()
{
    try
    {
        this->cleanup();
        // any global cleanup stuff must follow after local cleanup
        // stuff ...
    }
    catch (const wns::Exception& e)
    {
        std::cerr << "\n"
                  << "wns::Exception thrown in wns::TestFixture::cleanup(): \n"
                  << e.what() << "\n";
        throw;
    }
    catch (const std::exception& e)
    {
        std::cerr << "\n"
                  << "std::exception thrown in wns::TestFixture::cleanup(): \n"
                  << e.what() << "\n";
        throw;
    }
}

std::string
wns::failureMessage(const std::string& sourceFile, const std::string& sourceLine, const std::string& expected, const std::string& actual)
{
    return "Line " + sourceLine + " in " + sourceFile + "\n" +
        "- Expected: " + expected + "\n" +
        "- Actual  : " + actual + "\n";
}

std::string
wns::testsuite::Disabled()
{
    return "wns::testsuite::Disabled";
}

std::string
wns::testsuite::Default()
{
    return "wns::testsuite::Default";
}

std::string
wns::testsuite::Performance()
{
    return "wns::testsuite::Performance";
}

std::string
wns::testsuite::Spike()
{
    return "wns::testsuite::Spike";
}
