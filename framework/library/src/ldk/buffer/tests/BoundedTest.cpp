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

#include <WNS/ldk/buffer/tests/BoundedTest.hpp>

#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/ldk/helper/FakePDU.hpp>
#include <WNS/ldk/fun/Main.hpp>

#include <iostream>


using namespace wns::ldk::buffer;


CPPUNIT_TEST_SUITE_REGISTRATION( BoundedTest );

void
BoundedTest::setUp()
{
    layer = new tests::LayerStub();
    fuNet = new fun::Main(layer);

    wns::pyconfig::Parser emptyConfig;
    upper = new tools::Stub(fuNet, emptyConfig);

    {
        wns::pyconfig::Parser pyco;
        pyco.loadString("from openwns.Buffer import Bounded\n"
                "buffer = Bounded(size = 2)\n"
            );
        wns::pyconfig::View view(pyco, "buffer");
        buffer = new buffer::Bounded(fuNet, view);
    }

    lower = new tools::Stub(fuNet, emptyConfig);

    upper
        ->connect(buffer)
        ->connect(lower);
} // setUp

void
BoundedTest::tearDown()
{
    delete upper;
    delete buffer;
    delete lower;

    delete fuNet;
    delete layer;
} // tearDown


void
BoundedTest::testFill()
{
    CompoundPtr compound(fuNet->createCompound());
    lower->close();

    CPPUNIT_ASSERT(buffer->isAccepting(compound));

    upper->sendData((compound));
    CPPUNIT_ASSERT(buffer->isAccepting(compound));

    upper->sendData((compound));
    CPPUNIT_ASSERT(!buffer->isAccepting(compound));
} // testProxy


void
BoundedTest::testWakeup()
{
    CompoundPtr compound(fuNet->createCompound());
    lower->close();
    upper->sendData((compound));
    upper->sendData((compound));
    lower->open();

    CPPUNIT_ASSERT(buffer->isAccepting(compound));
    CPPUNIT_ASSERT(lower->sent.size() == 2);

    lower->close();
    upper->sendData((compound));
    CPPUNIT_ASSERT(buffer->isAccepting(compound));

    upper->sendData((compound));
    CPPUNIT_ASSERT(!buffer->isAccepting(compound));
} // testProxy



CPPUNIT_TEST_SUITE_REGISTRATION( BoundedBitTest );

void
BoundedBitTest::setUp()
{
    wns::pyconfig::Parser pyco;
    pyco.loadString("from openwns.Buffer import Bounded\n"
            "buffer = Bounded(size = 20)\n"
            "buffer.sizeUnit = 'Bit'\n"
        );

    layer = new tests::LayerStub();
    fuNet = new fun::Main(layer);

    wns::pyconfig::Parser emptyConfig;
    upper = new tools::Stub(fuNet, emptyConfig);
    buffer = new Bounded(fuNet, pyco.getView("buffer"));
    lower = new tools::Stub(fuNet, emptyConfig);

    upper
        ->connect(buffer)
        ->connect(lower);
} // setUp

void
BoundedBitTest::tearDown()
{
    delete upper;
    delete buffer;
    delete lower;

    delete fuNet;
    delete layer;
} // tearDown


void
BoundedBitTest::testFill()
{
    CompoundPtr compound1(fuNet->createCompound(helper::FakePDUPtr(new helper::FakePDU(1))));
    CompoundPtr compound2(fuNet->createCompound(helper::FakePDUPtr(new helper::FakePDU(2))));
    CompoundPtr compound19(fuNet->createCompound(helper::FakePDUPtr(new helper::FakePDU(19))));
    lower->close();

    CPPUNIT_ASSERT(buffer->isAccepting(compound19));

    upper->sendData((compound19));
    CPPUNIT_ASSERT(buffer->isAccepting(compound1));
    CPPUNIT_ASSERT(!buffer->isAccepting(compound2));

    upper->sendData((compound1));
    CPPUNIT_ASSERT(!buffer->isAccepting(compound1));
    CPPUNIT_ASSERT(!buffer->isAccepting(compound2));
} // testFill


void
BoundedBitTest::testEmpty()
{
    CompoundPtr compound1(fuNet->createCompound(helper::FakePDUPtr(new helper::FakePDU(1))));
    CompoundPtr compound2(fuNet->createCompound(helper::FakePDUPtr(new helper::FakePDU(2))));
    CompoundPtr compound18(fuNet->createCompound(helper::FakePDUPtr(new helper::FakePDU(18))));

    lower->close();

    upper->sendData((compound1));
    upper->sendData((compound1));
    upper->sendData((compound18));
    CPPUNIT_ASSERT(!buffer->isAccepting(compound1));
    CPPUNIT_ASSERT(!buffer->isAccepting(compound2));

    lower->step();
    CPPUNIT_ASSERT(buffer->isAccepting(compound1));
    CPPUNIT_ASSERT(!buffer->isAccepting(compound2));

    lower->step();
    CPPUNIT_ASSERT(buffer->isAccepting(compound1));
    CPPUNIT_ASSERT(buffer->isAccepting(compound2));
} // testEmpty



