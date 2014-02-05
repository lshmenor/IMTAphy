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

#include "SubTest.hpp"

#include <WNS/pyconfig/Parser.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/ldk/Command.hpp>

#include <WNS/ldk/buffer/Bounded.hpp>
#include <WNS/ldk/tools/Synchronizer.hpp>

#include <WNS/ldk/utils.hpp>

#include <iostream>

using namespace wns::ldk;
using namespace wns::ldk::fun;

CPPUNIT_TEST_SUITE_REGISTRATION( SubFUNInterfaceTest );
CPPUNIT_TEST_SUITE_REGISTRATION( SubTest );


void
SubTest::setUp()
{
    layer = new ldk::tests::LayerStub();
    mainNet = new Main(layer);
    subNet = new Sub(mainNet);

    pyconfig::Parser emptyConfig;
    mainFU = new tools::Stub(mainNet, emptyConfig);
    subFU = new tools::Stub(subNet, emptyConfig);
    otherSubFU = new tools::Stub(subNet, emptyConfig);
} // setUp


void
SubTest::tearDown()
{
    delete subNet;
    delete mainNet;

    if(mainFU)
        delete mainFU;

    if(subFU)
        delete subFU;

    if(otherSubFU)
        delete otherSubFU;

    delete layer;
} // tearDown


void
SubTest::testFindParent()
{
    mainNet->addFunctionalUnit("main", mainFU);
    mainFU = NULL;
    subNet->addFunctionalUnit("sub", subFU);
    subFU = NULL;

    CPPUNIT_ASSERT(mainNet->knowsFunctionalUnit("main"));
    CPPUNIT_ASSERT(subNet->knowsFunctionalUnit("main"));

    CPPUNIT_ASSERT(!mainNet->knowsFunctionalUnit("sub"));
    CPPUNIT_ASSERT(subNet->knowsFunctionalUnit("sub"));

    CPPUNIT_ASSERT(subNet->getFunctionalUnit("main") == mainNet->getFunctionalUnit("main"));
} // testFindParent


// void
// SubTest::testShadowParent()
// {
// 	mainNet->addFunctionalUnit("dada", mainFU);
//  	mainFU = NULL;
//  	subNet->addFunctionalUnit("dada", subFU);
//  	subFU = NULL;

//  	CPPUNIT_ASSERT(mainNet->knowsFunctionalUnit("dada"));
//  	CPPUNIT_ASSERT(subNet->knowsFunctionalUnit("dada"));

//  	CPPUNIT_ASSERT(subNet->getFunctionalUnit("dada") != mainNet->getFunctionalUnit("dada"));
// } // testShadowParent


void
SubTest::testCloneFUs()
{
    mainNet->addFunctionalUnit("main", mainFU);
    mainFU = NULL;
    subNet->addFunctionalUnit("sub", subFU);
    subFU = NULL;
    subNet->addFunctionalUnit("other", otherSubFU);
    otherSubFU = NULL;

    FUN* otherSubNet = subNet->clone();

    CPPUNIT_ASSERT(otherSubNet->knowsFunctionalUnit("sub"));
    CPPUNIT_ASSERT(otherSubNet->knowsFunctionalUnit("other"));

    delete otherSubNet;
} // testCloneFUs


void
SubTest::testCloneConnectionsExist()
{
    mainNet->addFunctionalUnit("main", mainFU);
    subNet->addFunctionalUnit("sub", subFU);
    subNet->addFunctionalUnit("other", otherSubFU);

    subFU->connect(otherSubFU);
    mainFU = NULL;
    subFU = NULL;
    otherSubFU = NULL;

    FUN* otherSubNet = subNet->clone();

    tools::Stub* a = otherSubNet->findFriend<tools::Stub*>("sub");
    tools::Stub* b = otherSubNet->findFriend<tools::Stub*>("other");
    CPPUNIT_ASSERT_EQUAL(unsigned long int(0), a->getReceptor()->size());
    CPPUNIT_ASSERT_EQUAL(unsigned long int(0), a->getDeliverer()->size());
    CPPUNIT_ASSERT_EQUAL(unsigned long int(1), a->getConnector()->size());

    CPPUNIT_ASSERT_EQUAL(unsigned long int(1), b->getReceptor()->size());
    CPPUNIT_ASSERT_EQUAL(unsigned long int(1), b->getDeliverer()->size());
    CPPUNIT_ASSERT_EQUAL(unsigned long int(0), b->getConnector()->size());

    delete otherSubNet;
} // testCloneConnectionsExist


void
SubTest::testCloneConnections()
{
    mainNet->addFunctionalUnit("main", mainFU);
    subNet->addFunctionalUnit("sub", subFU);
    subNet->addFunctionalUnit("other", otherSubFU);

    subFU->connect(otherSubFU);
    mainFU = NULL;
    subFU = NULL;
    otherSubFU = NULL;

    FUN* otherSubNet = subNet->clone();

    CompoundPtr compound(otherSubNet->createCompound());

    tools::Stub* a = otherSubNet->findFriend<tools::Stub*>("sub");
    tools::Stub* b = otherSubNet->findFriend<tools::Stub*>("other");

    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), a->received.size());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), a->sent.size());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), b->received.size());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), b->sent.size());

    a->sendData(compound);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), a->received.size());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), a->sent.size());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), b->received.size());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), b->sent.size());

    b->onData(compound);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), a->received.size());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), a->sent.size());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), b->received.size());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), b->sent.size());

    delete otherSubNet;
} // testCloneConnections


void
SubTest::testCommand()
{
    subNet->addFunctionalUnit("sub", subFU);
    CompoundPtr compound(subNet->createCompound());
    Command* command = subFU->activateCommand(compound->getCommandPool());
    subFU = NULL;

    FUN* otherSubNet = subNet->clone();
    tools::Stub* copyOfSubFU = otherSubNet->findFriend<tools::Stub*>("sub");

    CPPUNIT_ASSERT(command == copyOfSubFU->getCommand(compound->getCommandPool()));
} // testCommand


