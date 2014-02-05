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

#include "OpcodeTest.hpp"

#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/pyconfig/Parser.hpp>

#include <iostream>

using namespace wns::ldk::tools;
using namespace wns::ldk::multiplexer;

CPPUNIT_TEST_SUITE_REGISTRATION( OpcodeTest );

void
OpcodeTest::setUp()
{
    layer = new tests::LayerStub();
    fuNet = new fun::Main(layer);

    pyconfig::Parser emptyConfig;
    upper = new Stub(fuNet, emptyConfig);

    {
        pyconfig::Parser config;
        config.loadString(
            "import openwns.ldk\n"
            "setter = openwns.ldk.Multiplexer.OpcodeSetter(42, 'provider')\n"
            );
        pyconfig::View view(config, "setter");
        setter = new OpcodeSetter(fuNet, view);
    }

    {
        pyconfig::Parser config;
        config.loadString(
            "import openwns.ldk\n"
            "provider = openwns.ldk.Multiplexer.OpcodeProvider(23)\n"
            );
        pyconfig::View view(config, "provider");
        provider = new OpcodeProvider(fuNet, view);
    }

    lower = new Stub(fuNet, emptyConfig);

    fuNet->addFunctionalUnit("provider", provider);
    fuNet->addFunctionalUnit("setter", setter);
    fuNet->onFUNCreated();

    upper->connect(setter)
         ->connect(lower);
} // setUp


void
OpcodeTest::tearDown()
{
    delete upper;
    delete lower;

    delete layer;
} // tearDown


void
OpcodeTest::testOutgoing()
{
    CompoundPtr compound(new Compound(fuNet->getProxy()->createCommandPool()));
    upper->sendData(compound);

    CPPUNIT_ASSERT(lower->sent.size() == 1);

    OpcodeCommand* command = provider->getCommand(lower->sent[0]->getCommandPool());
    CPPUNIT_ASSERT(command->peer.opcode == 42);
} // testOutgoing


void
OpcodeTest::testIncoming()
{
    CompoundPtr compound(fuNet->createCompound());
    upper->sendData(compound);

    CPPUNIT_ASSERT(upper->received.size() == 0);
    lower->onData(lower->sent.at(0));
    CPPUNIT_ASSERT(upper->received.size() == 1);
} // testIncoming


void
OpcodeTest::testSizes()
{
    CompoundPtr compound(fuNet->createCompound());

    CPPUNIT_ASSERT_EQUAL(Bit(0), compound->getLengthInBits());
    upper->sendData(compound);
    CPPUNIT_ASSERT_EQUAL(Bit(23), compound->getLengthInBits());
} // testIncoming



