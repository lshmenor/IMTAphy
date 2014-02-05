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

#include <WNS/ldk/fun/Main.hpp>
#include <WNS/events/NoOp.hpp>

#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/pyconfig/Parser.hpp>

#include <WNS/ldk/probe/Packet.hpp>
#include <WNS/ldk/tools/Stub.hpp>
#include <WNS/ldk/helper/FakePDU.hpp>
#include <WNS/CppUnit.hpp>
#include <WNS/testing/TestTool.hpp>

namespace wns { namespace ldk { namespace probe { namespace tests {

    using namespace wns::ldk;

    class PacketTest :
        public wns::TestFixture
    {
        CPPUNIT_TEST_SUITE( PacketTest );
        CPPUNIT_TEST( testThroughput );
        CPPUNIT_TEST( testDelay );
        CPPUNIT_TEST( testSize );
        CPPUNIT_TEST_SUITE_END();
    public:
        void prepare();
        void cleanup();

        void testThroughput();
        void testDelay();
        void testSize();

    private:
        ILayer* layer;
        fun::FUN* fuNet;

        wns::ldk::tools::Stub* upper;
        Packet* probe;
        wns::ldk::tools::Stub* lower;
        wns::ldk::helper::FakePDUPtr innerPDU;
    };

    CPPUNIT_TEST_SUITE_REGISTRATION( PacketTest );

    void
    PacketTest::prepare()
    {
        layer = new wns::ldk::tests::LayerStub();
        fuNet = new wns::ldk::fun::Main(layer);

        pyconfig::Parser emptyConfig;
        upper = new wns::ldk::tools::Stub(fuNet, emptyConfig);
        lower = new wns::ldk::tools::Stub(fuNet, emptyConfig);

        pyconfig::Parser all;
        all.loadString(
            "from openwns.Probe import Packet\n"
            "probe = Packet('test', 'test')\n"
            );
        pyconfig::View config(all, "probe");
        probe = new Packet(fuNet, config);

        upper
            ->connect(probe)
            ->connect(lower);

        fuNet->addFunctionalUnit("upperStub", upper);
        fuNet->addFunctionalUnit("ernie", probe);
        fuNet->addFunctionalUnit("lowerStub", lower);

        innerPDU = wns::ldk::helper::FakePDUPtr((new wns::ldk::helper::FakePDU(1)));

        std::string configstring =
            "import openwns\n"
            "from openwns.evaluation import *\n"
            "sim = openwns.Simulator()\n"
            "node = openwns.evaluation.createSourceNode(sim, 'test.packet.incoming.delay')\n"
            "node.appendChildren(PDF(minXValue = 0,\n"
            "                        maxXValue = 4,\n"
            "                        resolution = 40))\n"
            "node = openwns.evaluation.createSourceNode(sim, 'test.packet.outgoing.delay')\n"
            "node.appendChildren(PDF(minXValue = 0,\n"
            "                        maxXValue = 4,\n"
            "                        resolution = 40))\n"
            "node = openwns.evaluation.createSourceNode(sim, 'test.packet.incoming.bitThroughput')\n"
            "node.appendChildren(PDF(minXValue = 0,\n"
            "                        maxXValue = 4,\n"
            "                        resolution = 40))\n"
            "node = openwns.evaluation.createSourceNode(sim, 'test.packet.incoming.size')\n"
            "node.appendChildren(PDF(minXValue = 0,\n"
            "                        maxXValue = 12000.0,\n"
            "                        resolution = 100))\n"
            "node = openwns.evaluation.createSourceNode(sim, 'test.packet.outgoing.size')\n"
            "node.appendChildren(PDF(minXValue = 0,\n"
            "                        maxXValue = 12000.0,\n"
            "                        resolution = 100))\n";

        wns::pyconfig::Parser p;
        p.loadString(configstring);

        wns::probe::bus::ProbeBusRegistry* pbr = wns::simulator::getInstance()->getProbeBusRegistry();
        pbr->spawnProbeBusses(p.get<wns::pyconfig::View>("sim.environment.probeBusRegistry"));
    } // setUp


    void
    PacketTest::cleanup()
    {
        delete fuNet;
        delete layer;
    } // tearDown


    void
    PacketTest::testThroughput()
    {
        wns::simulator::getEventScheduler()->reset();

        wns::simulator::getEventScheduler()->schedule(events::NoOp(), 2);
        wns::simulator::getEventScheduler()->processOneEvent();

        CompoundPtr compound1(fuNet->createCompound(innerPDU));
        CompoundPtr compound2(fuNet->createCompound(innerPDU));
        CompoundPtr compound3(fuNet->createCompound(innerPDU));

        upper->sendData((compound1));
        upper->sendData((compound2));
        upper->sendData((compound3));

        wns::simulator::getEventScheduler()->schedule(events::NoOp(), 2.5);
        wns::simulator::getEventScheduler()->processOneEvent(); // 2 [bit/s]
        lower->onData((compound1));

        wns::simulator::getEventScheduler()->schedule(events::NoOp(), 3.0);
        wns::simulator::getEventScheduler()->processOneEvent(); // 2 [bit/s]
        lower->onData((compound2));

        wns::simulator::getEventScheduler()->schedule(events::NoOp(), 4.0);
        wns::simulator::getEventScheduler()->processOneEvent(); // 0.5 [bit/s]
        lower->onData((compound3));

        CPPUNIT_ASSERT(lower->sent.size() == 3);
        CPPUNIT_ASSERT(lower->sent[0] == compound1);
        CPPUNIT_ASSERT(lower->sent[1] == compound2);
        CPPUNIT_ASSERT(lower->sent[2] == compound3);

        wns::simulator::getInstance()->getProbeBusRegistry()->forwardOutput();
        CPPUNIT_ASSERT(wns::testing::compareFile(
                       "output/test.packet.incoming.bitThroughput_PDF.dat",
                       "(#.*\n)*"
                       "#.*Minimum: 0\\.5.*\n"
                       "#.*Maximum: 2\\.0.*\n"
                       "#.*Trials: 3\n"
                       "#.*Mean: 1\\.166.*\n"
                       "(.*\n)*"
                       ));
    } // testThroughput


    void
    PacketTest::testDelay()
    {
        CompoundPtr compound1(fuNet->createCompound(innerPDU));

        wns::simulator::getEventScheduler()->reset();

        wns::simulator::getEventScheduler()->schedule(events::NoOp(), 0.0);
        wns::simulator::getEventScheduler()->processOneEvent();
        upper->sendData((compound1));
        wns::simulator::getEventScheduler()->schedule(events::NoOp(), 1.0);
        wns::simulator::getEventScheduler()->processOneEvent();
        lower->onData((compound1));

        CompoundPtr compound2(fuNet->createCompound(innerPDU));
        wns::simulator::getEventScheduler()->reset();
        wns::simulator::getEventScheduler()->schedule(events::NoOp(), 0.0);
        wns::simulator::getEventScheduler()->processOneEvent();
        upper->sendData((compound2));
        wns::simulator::getEventScheduler()->schedule(events::NoOp(), 2.0);
        wns::simulator::getEventScheduler()->processOneEvent();
        lower->onData((compound2));

        CPPUNIT_ASSERT(upper->received.size() == 2);
        CPPUNIT_ASSERT(upper->received[0] == compound1);
        CPPUNIT_ASSERT(upper->received[1] == compound2);

        wns::simulator::getInstance()->getProbeBusRegistry()->forwardOutput();
        CPPUNIT_ASSERT(wns::testing::compareFile(
                       "output/test.packet.incoming.delay_PDF.dat",
                       "(#.*\n)*"
                       "#.*Minimum: 1\\.0.*\n"
                       "#.*Maximum: 2\\.0.*\n"
                       "#.*Trials: 2\n"
                       "#.*Mean: 1\\.5.*\n"
                       "(.*\n)*"
                       ));
        CPPUNIT_ASSERT(wns::testing::compareFile(
                       "output/test.packet.outgoing.delay_PDF.dat",
                       "(#.*\n)*"
                       "#.*Minimum: 1\\.0.*\n"
                       "#.*Maximum: 2\\.0.*\n"
                       "#.*Trials: 2\n"
                       "#.*Mean: 1\\.5.*\n"
                       "(.*\n)*"
                       ));
    } // testDelay


    void
    PacketTest::testSize()
    {
        wns::simulator::getEventScheduler()->reset();

        wns::simulator::getEventScheduler()->schedule(events::NoOp(), 23.0);
        wns::simulator::getEventScheduler()->processOneEvent();
        CompoundPtr compound1(fuNet->createCompound(innerPDU));
        CompoundPtr compound2(fuNet->createCompound(innerPDU));

        upper->sendData((compound1));
        upper->sendData((compound2));

        wns::simulator::getEventScheduler()->schedule(events::NoOp(), 42.0);
        wns::simulator::getEventScheduler()->processOneEvent();
        lower->onData((compound1));
        innerPDU->setLengthInBits(2);
        lower->onData((compound2));

        wns::simulator::getInstance()->getProbeBusRegistry()->forwardOutput();
        CPPUNIT_ASSERT(wns::testing::compareFile(
                       "output/test.packet.incoming.size_PDF.dat",
                       "(#.*\n)*"
                       "#.*Minimum: 1\\.0.*\n"
                       "#.*Maximum: 2\\.0.*\n"
                       "#.*Trials: 2\n"
                       "#.*Mean: 1\\.5.*\n"
                       "(.*\n)*"
                       ));

        CPPUNIT_ASSERT(wns::testing::compareFile(
                       "output/test.packet.outgoing.size_PDF.dat",
                       "(#.*\n)*"
                       "#.*Minimum: 1\\.0.*\n"
                       "#.*Maximum: 1\\.0.*\n"
                       "#.*Trials: 2\n"
                       "#.*Mean: 1\\.0.*\n"
                       "(.*\n)*"
                       ));
    } // testSize

} // tests
} // probe
} // ldk
} // wns


