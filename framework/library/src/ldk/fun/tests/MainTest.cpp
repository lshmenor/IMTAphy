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

#include <WNS/ldk/fun/tests/MainTest.hpp>

#include <WNS/pyconfig/Parser.hpp>
#include <WNS/ldk/Layer.hpp>
#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/ldk/Command.hpp>
#include <WNS/ldk/Compound.hpp>
#include <WNS/ldk/tools/Stub.hpp>

#include <WNS/ldk/buffer/Bounded.hpp>
#include <WNS/ldk/tools/Synchronizer.hpp>

#include <WNS/ldk/utils.hpp>

#include <iostream>

using namespace wns::ldk::fun;

CPPUNIT_TEST_SUITE_REGISTRATION( MainFUNInterfaceTest );

void
MainFUNInterfaceTest::testReconfigureFUN()
{
    pyconfig::Parser emptyConfig;
    FunctionalUnit* oldFU = new tools::Stub(getFUN(), emptyConfig);

    getFUN()->addFunctionalUnit("upperStub", fu1);
    getFUN()->addFunctionalUnit("lowerStub", fu2);
    getFUN()->addFunctionalUnit("oldStub", oldFU);

    getFUN()->connectFunctionalUnit("upperStub", "oldStub");
    getFUN()->connectFunctionalUnit("oldStub", "lowerStub");

    fu1->sendData(getFUN()->createCompound());
    CPPUNIT_ASSERT_EQUAL(size_t(1), dynamic_cast<tools::Stub*>(oldFU)->sent.size());
    CPPUNIT_ASSERT_EQUAL(size_t(1), dynamic_cast<tools::Stub*>(fu2)->sent.size());

    fu2->onData(getFUN()->createCompound());
    CPPUNIT_ASSERT_EQUAL(size_t(1), dynamic_cast<tools::Stub*>(oldFU)->received.size());
    CPPUNIT_ASSERT_EQUAL(size_t(1), dynamic_cast<tools::Stub*>(fu1)->received.size());

    fu2->wakeup();
    CPPUNIT_ASSERT_EQUAL(long(1), dynamic_cast<tools::Stub*>(oldFU)->wakeupCalled);
    CPPUNIT_ASSERT_EQUAL(long(1), dynamic_cast<tools::Stub*>(fu1)->wakeupCalled);

    pyconfig::Parser config;
    config.loadString(
        "from openwns.Tools import Stub\n"
        "class ReconfigureFUN:\n"
        "  replaceFU = \"oldStub\"\n"
        "  newFUName = \"newStub\"\n"
        "  newFUConfig = Stub()\n"
        "reconfigureFUN = ReconfigureFUN()\n");
    pyconfig::View reconfig(config, "reconfigureFUN");

    getFUN()->reconfigureFUN(reconfig);

    fu1->sendData(getFUN()->createCompound());
    CPPUNIT_ASSERT_EQUAL(size_t(1), dynamic_cast<tools::Stub*>(getFUN()->getFunctionalUnit("newStub"))->sent.size());
    CPPUNIT_ASSERT_EQUAL(size_t(2), dynamic_cast<tools::Stub*>(fu2)->sent.size());

    fu2->onData(getFUN()->createCompound());
    CPPUNIT_ASSERT_EQUAL(size_t(1), dynamic_cast<tools::Stub*>(getFUN()->getFunctionalUnit("newStub"))->received.size());
    CPPUNIT_ASSERT_EQUAL(size_t(2), dynamic_cast<tools::Stub*>(fu1)->received.size());

    fu2->wakeup();
    CPPUNIT_ASSERT_EQUAL(long(1), dynamic_cast<tools::Stub*>(getFUN()->getFunctionalUnit("newStub"))->wakeupCalled);
    CPPUNIT_ASSERT_EQUAL(long(2), dynamic_cast<tools::Stub*>(fu1)->wakeupCalled);

    fu1 = NULL;
    fu2 = NULL;
} // testReconfigureFUN



