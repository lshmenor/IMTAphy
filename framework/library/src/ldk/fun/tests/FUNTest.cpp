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

#include <WNS/ldk/fun/tests/FUNTest.hpp>

#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/pyconfig/Parser.hpp>
#include <WNS/ldk/Layer.hpp>
#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/ldk/Command.hpp>
#include <WNS/ldk/tools/Stub.hpp>

#include <WNS/ldk/buffer/Bounded.hpp>
#include <WNS/ldk/tools/Synchronizer.hpp>

#include <WNS/ldk/utils.hpp>

#include <iostream>

using namespace wns::ldk::fun;

void
FUNTest::setUp()
{
    pyconfig::Parser config;
    config.loadString(
        "from openwns.FUN import FUN\n"
        "fuNet = FUN()\n"
        );

    this->layer = new tests::LayerStub();
    this->fuNet = newCandidate(this->layer);

    pyconfig::Parser emptyConfig;
    this->fu1 = new wns::ldk::tools::Stub(fuNet, emptyConfig);
    this->fu2 = new wns::ldk::tools::Stub(fuNet, emptyConfig);
} // setUp


void
FUNTest::tearDown()
{
    if(this->fu1)
        delete this->fu1;

    if(this->fu2)
        delete this->fu2;

    deleteCandidate(this->fuNet);

    delete this->layer;
} // tearDown


void
FUNTest::testLayerDelegation()
{
    CPPUNIT_ASSERT( this->layer == this->fuNet->getLayer());
    CPPUNIT_ASSERT( this->layer->getName() == this->fuNet->getName());
} // testLayerDelegation


void
FUNTest::testAdd()
{
    this->fuNet->addFunctionalUnit("mon", this->fu1);
    this->fuNet->addFunctionalUnit("dieu", this->fu2);

    CPPUNIT_ASSERT( this->fuNet->knowsFunctionalUnit("mon") );
    CPPUNIT_ASSERT( this->fuNet->knowsFunctionalUnit("dieu") );
    CPPUNIT_ASSERT( !this->fuNet->knowsFunctionalUnit("olala") );

    CPPUNIT_ASSERT( this->fuNet->getFunctionalUnit("mon") == this->fu1 );
    CPPUNIT_ASSERT( this->fuNet->getFunctionalUnit("dieu") == this->fu2 );

    this->fu1 = NULL;
    this->fu2 = NULL;
} // testAdd


void
FUNTest::testAddAlready1()
{
    this->fuNet->addFunctionalUnit("dieu", this->fu1);
    this->fu1 = NULL;
    this->fuNet->addFunctionalUnit("dieu", this->fu2); // BANG
} // testAddAlready1


void
FUNTest::testAddAlready2()
{
    this->fuNet->addFunctionalUnit("mon", this->fu1);
    this->fu1 = NULL;
    this->fuNet->addFunctionalUnit("dieu", this->fu1); // BANG
} // testAddAlready2


void
FUNTest::testConnect()
{
    this->fuNet->addFunctionalUnit("mon", this->fu1);
    this->fuNet->addFunctionalUnit("dieu", this->fu2);
    this->fu1 = NULL;
    this->fu2 = NULL;
    this->fuNet->connectFunctionalUnit("mon", "dieu");

    wns::ldk::tools::Stub* mon =
        dynamic_cast<wns::ldk::tools::Stub*>(this->fuNet->getFunctionalUnit("mon"));
    wns::ldk::tools::Stub* dieu =
        dynamic_cast<wns::ldk::tools::Stub*>(this->fuNet->getFunctionalUnit("dieu"));

    CPPUNIT_ASSERT( mon->getReceptor()->size() == 0 );
    CPPUNIT_ASSERT( mon->getConnector()->size() == 1 );
    CPPUNIT_ASSERT( mon->getDeliverer()->size() == 0 );

    CPPUNIT_ASSERT( dieu->getReceptor()->size() == 1 );
    CPPUNIT_ASSERT( dieu->getConnector()->size() == 0 );
    CPPUNIT_ASSERT( dieu->getDeliverer()->size() == 1 );

    CPPUNIT_ASSERT( mon->getConnector()->getAcceptor(CompoundPtr()) == dieu );

    CPPUNIT_ASSERT( mon->wakeupCalled == 0);
    dieu->getReceptor()->wakeup();
    CPPUNIT_ASSERT( mon->wakeupCalled == 1);

    CPPUNIT_ASSERT( dieu->getDeliverer()->getAcceptor(CompoundPtr()) == mon );
} // testConnect


void
FUNTest::testUnknownConnect1()
{
    this->fuNet->addFunctionalUnit("mon", this->fu1);
    this->fuNet->addFunctionalUnit("dieu", this->fu2);
    this->fu1 = NULL;
    this->fu2 = NULL;

    this->fuNet->connectFunctionalUnit("un", "dieu");
} // testUnknownConnect1


void
FUNTest::testUnknownConnect2()
{
    this->fuNet->addFunctionalUnit("mon", this->fu1);
    this->fuNet->addFunctionalUnit("dieu", this->fu2);
    this->fu1 = NULL;
    this->fu2 = NULL;

    this->fuNet->connectFunctionalUnit("mon", "putain");
} // testUnknownConnect2


void
FUNTest::testUpConnect()
{
    this->fuNet->addFunctionalUnit("mon", this->fu1);
    this->fuNet->addFunctionalUnit("dieu", this->fu2);
    this->fu1 = NULL;
    this->fu2 = NULL;

    this->fuNet->upConnectFunctionalUnit("mon", "dieu");

    wns::ldk::tools::Stub* mon =
        dynamic_cast<wns::ldk::tools::Stub*>(this->fuNet->getFunctionalUnit("mon"));
    wns::ldk::tools::Stub* dieu =
        dynamic_cast<wns::ldk::tools::Stub*>(this->fuNet->getFunctionalUnit("dieu"));

    CPPUNIT_ASSERT( mon->getReceptor()->size() == 0 );
    CPPUNIT_ASSERT( mon->getConnector()->size() == 0 );
    CPPUNIT_ASSERT( mon->getDeliverer()->size() == 0 );

    CPPUNIT_ASSERT( dieu->getReceptor()->size() == 0 );
    CPPUNIT_ASSERT( dieu->getConnector()->size() == 0 );
    CPPUNIT_ASSERT( dieu->getDeliverer()->size() == 1 );

    CPPUNIT_ASSERT( dieu->getDeliverer()->getAcceptor(CompoundPtr()) == mon );
} // testUpConnect


void
FUNTest::testDownConnect()
{
    this->fuNet->addFunctionalUnit("mon", this->fu1);
    this->fuNet->addFunctionalUnit("dieu", this->fu2);
    this->fu1 = NULL;
    this->fu2 = NULL;

    this->fuNet->downConnectFunctionalUnit("mon", "dieu");

    wns::ldk::tools::Stub* mon =
        dynamic_cast<wns::ldk::tools::Stub*>(this->fuNet->getFunctionalUnit("mon"));
    wns::ldk::tools::Stub* dieu =
        dynamic_cast<wns::ldk::tools::Stub*>(this->fuNet->getFunctionalUnit("dieu"));

    CPPUNIT_ASSERT( mon->getReceptor()->size() == 0 );
    CPPUNIT_ASSERT( mon->getConnector()->size() == 1 );
    CPPUNIT_ASSERT( mon->getDeliverer()->size() == 0 );

    CPPUNIT_ASSERT( dieu->getReceptor()->size() == 1 );
    CPPUNIT_ASSERT( dieu->getConnector()->size() == 0 );
    CPPUNIT_ASSERT( dieu->getDeliverer()->size() == 0 );

    CPPUNIT_ASSERT( mon->getConnector()->getAcceptor(CompoundPtr()) == dieu );

    CPPUNIT_ASSERT( mon->wakeupCalled == 0 );
    dieu->getReceptor()->wakeup();
    CPPUNIT_ASSERT( mon->wakeupCalled == 1 );
} // testDownConnect


void
FUNTest::testFindFriend()
{
    this->fuNet->addFunctionalUnit("mon", this->fu1);
    this->fu1 = NULL;

    CPPUNIT_ASSERT( this->fuNet->findFriend<FunctionalUnit*>("mon") );
    CPPUNIT_ASSERT( this->fuNet->findFriend<wns::ldk::tools::Stub*>("mon") );

    pyconfig::Parser config;
    config.loadString(
        "from openwns.Buffer import Bounded\n"
        "foo = Bounded()\n"
        );

    this->fuNet->addFunctionalUnit("hermann",
                       configuredFunctionalUnit(this->fuNet, config.get("foo")));

    CPPUNIT_ASSERT( this->fuNet->findFriend<FunctionalUnit*>("hermann") );
    CPPUNIT_ASSERT( this->fuNet->findFriend<buffer::Buffer*>("hermann") );
    CPPUNIT_ASSERT( this->fuNet->findFriend<buffer::Bounded*>("hermann") );
} // testFindFriend


void
FUNTest::testFindFriendUnknown()
{
    this->fuNet->findFriend<FunctionalUnit*>("kakerlake"); // BANG!
} // testFindFriendUnknown


void
FUNTest::testFindFriendBad()
{
    this->fuNet->addFunctionalUnit("mon", this->fu1);
    this->fu1 = NULL;

    CPPUNIT_ASSERT_THROW(this->fuNet->findFriend<buffer::Buffer*>("mon"), wns::ldk::fun::FindFriendException );
} // testFindFriendBad


FUN*
FUNTest::getFUN()
{
    return fuNet;
}



