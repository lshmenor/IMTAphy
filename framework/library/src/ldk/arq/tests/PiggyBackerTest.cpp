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

#include "PiggyBackerTest.hpp"

#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/ldk/tools/Consumer.hpp>
#include <WNS/pyconfig/Parser.hpp>

#include <vector>

using namespace wns::ldk::arq;

CPPUNIT_TEST_SUITE_REGISTRATION( PiggyBackerTest );

const int
PiggyBackerTest::bitsPerIFrame = 2;

const int
PiggyBackerTest::bitsPerRRFrame = 3;

const int
PiggyBackerTest::bitsIfPiggyBacked = 23;

const int
PiggyBackerTest::bitsIfNotPiggyBacked = 42;


void
PiggyBackerTest::setUp()
{
    layer = new tests::LayerStub();
    fuNet = new fun::Main(layer);

    wns::pyconfig::Parser emptyConfig;

    {
        wns::pyconfig::Parser pyco;
        pyco.loadString("from openwns.Buffer import Bounded\n"
                        "buffer = Bounded(size = 100)\n"
                        );
        wns::pyconfig::View view(pyco, "buffer");
        buffer = new buffer::Bounded(fuNet, view);
    }

    upper = new tools::Stub(fuNet, emptyConfig);

    {
        std::stringstream ss;

        ss << "arq = 'arq'\n"
           << "bitsIfPiggyBacked = " << bitsIfPiggyBacked << "\n"
           << "bitsIfNotPiggyBacked = " << bitsIfNotPiggyBacked << "\n"
           << "addACKPDUSize = True\n";

        wns::pyconfig::Parser pyco;
        pyco.loadString(ss.str());

        piggy = new PiggyBacker(fuNet, pyco);
    }

    lower = new tools::Stub(fuNet, emptyConfig);


    {
        std::stringstream ss;

        ss << "from openwns.logger import Logger\n"
           << "from openwns.ARQ import NoStatusCollection\n"
           << "bitsPerIFrame = " << bitsPerIFrame << "\n"
           << "bitsPerRRFrame = " << bitsPerRRFrame << "\n"
           << "resendTimeout = 1.0\n"
           << "useSuspendProbe = False\n"
           << "suspendProbeName = \"timeBufferEmpty\"\n"
           << "logger = Logger('TEST','PiggyBackerTest-StopAndWait',True)\n"
           << "arqStatusCollector = NoStatusCollection(logger)\n";

        wns::pyconfig::Parser pyco;
        pyco.loadString(ss.str());

        arq = new StopAndWait(fuNet, pyco);
    }

    consumer = new tools::Consumer(fuNet);
    consumer->consumeIncoming();

    consumer
        ->connect(buffer)
        ->connect(upper)
        ->connect(piggy)
        ->connect(lower);

    fuNet->addFunctionalUnit("consumer", consumer);
    fuNet->addFunctionalUnit("buffer", buffer);
    fuNet->addFunctionalUnit("upper", upper);
    fuNet->addFunctionalUnit("MissPiggy", piggy);
    fuNet->addFunctionalUnit("lower", lower);
    fuNet->addFunctionalUnit("arq", arq);
    fuNet->onFUNCreated();

    iCompound = CompoundPtr(fuNet->createCompound());
    arq->activateCommand(iCompound->getCommandPool())->peer.type = StopAndWaitCommand::I;

    ackCompound = CompoundPtr(fuNet->createCompound());
    arq->activateCommand(ackCompound->getCommandPool())->peer.type = StopAndWaitCommand::RR;

    wns::simulator::getEventScheduler()->reset();
} // setUp


void
PiggyBackerTest::tearDown()
{
    iCompound = CompoundPtr();
    ackCompound = CompoundPtr();

    delete fuNet;
    delete layer;
} // tearDown

void
PiggyBackerTest::testOutgoingI()
{
    lower->close();
    buffer->sendData(iCompound);

    CPPUNIT_ASSERT(lower->sent.size() == 0);
    lower->open();
    CPPUNIT_ASSERT(lower->sent.size() == 1);
    CPPUNIT_ASSERT(lower->sent[0] == iCompound);
} // testOutgoingI


void
PiggyBackerTest::testOutgoingACK()
{
    lower->close();
    buffer->sendData((ackCompound));

    CPPUNIT_ASSERT(lower->sent.size() == 0);
    lower->open();
    CPPUNIT_ASSERT(lower->sent.size() == 1);
    CPPUNIT_ASSERT(lower->sent[0] == ackCompound);
} // testOutgoingACK


void
PiggyBackerTest::testOutgoingBoth()
{
    lower->close();
    buffer->sendData((ackCompound));
    buffer->sendData((iCompound));

    CPPUNIT_ASSERT(lower->sent.size() == 0);
    lower->open();
    CPPUNIT_ASSERT(lower->sent.size() == 1);
    CPPUNIT_ASSERT(lower->sent[0] == iCompound);
} // testOutgoingBoth


void
PiggyBackerTest::testIncomingI()
{
    buffer->sendData((iCompound));
    lower->onData((lower->sent[0]));

    CPPUNIT_ASSERT(upper->received.size() == 1);
    CPPUNIT_ASSERT(upper->received[0] == iCompound);
} // testI


void
PiggyBackerTest::testIncomingACK()
{
    buffer->sendData((ackCompound));
    lower->onData((lower->sent[0]));

    CPPUNIT_ASSERT(upper->received.size() == 1);
    CPPUNIT_ASSERT(upper->received[0] == ackCompound);
} // testIncomingACK


void
PiggyBackerTest::testIncomingBoth()
{
    lower->close();
    buffer->sendData((ackCompound));
    buffer->sendData((iCompound));
    lower->open();

    CPPUNIT_ASSERT(lower->sent.size() == 1);
    lower->onData((lower->sent[0]));

    CPPUNIT_ASSERT(upper->received.size() == 2);
    CPPUNIT_ASSERT(upper->received[0] == ackCompound);
    CPPUNIT_ASSERT(upper->received[1] == iCompound);
} // testIncomingBoth


void
PiggyBackerTest::testWakeupNone_None()
{
    lower->wakeup();
    CPPUNIT_ASSERT(lower->sent.size() == 0);
} // testWakeupNone_None


void
PiggyBackerTest::testWakeupNone_I()
{
    upper->close();
    buffer->sendData((iCompound));
    upper->open(false);

    lower->wakeup();
    CPPUNIT_ASSERT(lower->sent.size() == 1);
    CPPUNIT_ASSERT(lower->sent[0] == iCompound);
} // testWakeupNone_I


void
PiggyBackerTest::testWakeupNone_ACK()
{
    upper->close();
    buffer->sendData((ackCompound));
    upper->open(false);

    lower->wakeup();
    CPPUNIT_ASSERT(lower->sent.size() == 1);
    CPPUNIT_ASSERT(lower->sent[0] == ackCompound);
} // testWakeupNone_ACK


void
PiggyBackerTest::testWakeupNone_Both()
{
    upper->close();
    buffer->sendData((ackCompound));
    buffer->sendData((iCompound));
    upper->open(false);

    lower->wakeup();
    CPPUNIT_ASSERT(lower->sent.size() == 1);
    CPPUNIT_ASSERT(lower->sent[0] == iCompound);

    lower->onData((lower->sent[0]));
    CPPUNIT_ASSERT(upper->received.size() == 2);
    CPPUNIT_ASSERT(upper->received[0] == ackCompound);
    CPPUNIT_ASSERT(upper->received[1] == iCompound);
} // testWakeupNone_Both


void
PiggyBackerTest::testWakeupI_None()
{
    lower->close();
    buffer->sendData((iCompound));

    CPPUNIT_ASSERT(lower->sent.size() == 0);
    lower->open(false);
    lower->wakeup();

    CPPUNIT_ASSERT(lower->sent.size() == 1);
    CPPUNIT_ASSERT(lower->sent[0] == iCompound);
} // testWakeupI_None


void
PiggyBackerTest::testWakeupI_ACK()
{
    lower->close();
    buffer->sendData((iCompound));

    CPPUNIT_ASSERT(lower->sent.size() == 0);
    lower->open(false);

    upper->close();
    buffer->sendData((ackCompound));
    upper->open(false);

    lower->wakeup();
    CPPUNIT_ASSERT(lower->sent.size() == 1);
    CPPUNIT_ASSERT(lower->sent[0] == iCompound);

    lower->onData((lower->sent[0]));
    CPPUNIT_ASSERT(upper->received[0] == ackCompound);
    CPPUNIT_ASSERT(upper->received[1] == iCompound);
} // testWakeupI_ACK


void
PiggyBackerTest::testWakeupACK_None()
{
    lower->close();
    buffer->sendData((ackCompound));

    CPPUNIT_ASSERT(lower->sent.size() == 0);
    lower->open(false);
    lower->wakeup();

    CPPUNIT_ASSERT(lower->sent.size() == 1);
    CPPUNIT_ASSERT(lower->sent[0] == ackCompound);
} // testWakeupACK_None


void
PiggyBackerTest::testWakeupACK_I()
{
    lower->close();
    buffer->sendData((ackCompound));

    CPPUNIT_ASSERT(lower->sent.size() == 0);
    lower->open(false);

    upper->close();
    buffer->sendData((iCompound));
    upper->open(false);

    lower->wakeup();
    CPPUNIT_ASSERT(lower->sent.size() == 1);
    CPPUNIT_ASSERT(lower->sent[0] == iCompound);

    lower->onData((lower->sent[0]));
    CPPUNIT_ASSERT(upper->received[0] == ackCompound);
    CPPUNIT_ASSERT(upper->received[1] == iCompound);
} // testWakeupACK_I


void
PiggyBackerTest::testSizeI()
{
    lower->close();
    buffer->sendData((iCompound));

    CPPUNIT_ASSERT(lower->sent.size() == 0);
    lower->open();
    CPPUNIT_ASSERT(lower->sent.size() == 1);
    CPPUNIT_ASSERT(lower->sent[0] == iCompound);

    Bit commandPoolSize;
    Bit sduSize;
    fuNet->calculateSizes(iCompound->getCommandPool(), commandPoolSize, sduSize);
    CPPUNIT_ASSERT(commandPoolSize == bitsIfNotPiggyBacked + bitsPerIFrame);
} // testSizeI


void
PiggyBackerTest::testSizeACK()
{
    lower->close();
    buffer->sendData((ackCompound));

    CPPUNIT_ASSERT(lower->sent.size() == 0);
    lower->open();
    CPPUNIT_ASSERT(lower->sent.size() == 1);
    CPPUNIT_ASSERT(lower->sent[0] == ackCompound);

    Bit commandPoolSize;
    Bit sduSize;
    fuNet->calculateSizes(ackCompound->getCommandPool(), commandPoolSize, sduSize);
    CPPUNIT_ASSERT(commandPoolSize == bitsIfNotPiggyBacked + bitsPerRRFrame);
} // testSizeACK


void
PiggyBackerTest::testSizeBoth()
{
    lower->close();
    buffer->sendData((ackCompound));
    buffer->sendData((iCompound));

    CPPUNIT_ASSERT(lower->sent.size() == 0);
    lower->open();
    CPPUNIT_ASSERT(lower->sent.size() == 1);
    CPPUNIT_ASSERT(lower->sent[0] == iCompound);

    Bit commandPoolSize;
    Bit sduSize;
    fuNet->calculateSizes(iCompound->getCommandPool(), commandPoolSize, sduSize);
    CPPUNIT_ASSERT(commandPoolSize == bitsIfPiggyBacked + bitsPerIFrame + bitsPerRRFrame);
} // testSizeBoth



