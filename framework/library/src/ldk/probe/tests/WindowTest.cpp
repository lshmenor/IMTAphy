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

#include <WNS/ldk/probe/Window.hpp>
#include <WNS/ldk/tools/Stub.hpp>
#include <WNS/ldk/helper/FakePDU.hpp>
#include <WNS/CppUnit.hpp>
#include <WNS/testing/TestTool.hpp>

namespace wns { namespace ldk { namespace probe { namespace tests {

    using namespace wns::ldk;

    class WindowTest :
        public wns::TestFixture
    {
        CPPUNIT_TEST_SUITE( WindowTest );
        CPPUNIT_TEST( testThroughput );
        CPPUNIT_TEST_SUITE_END();
    public:
        void prepare();
        void cleanup();

        void testThroughput();
        void testSize();

    private:
        ILayer* layer;
        fun::FUN* fuNet;

        wns::ldk::tools::Stub* upper;
        Window* probe;
        wns::ldk::tools::Stub* lower;
        wns::ldk::helper::FakePDUPtr innerPDU;
    };

    CPPUNIT_TEST_SUITE_REGISTRATION( WindowTest );

    void
    WindowTest::prepare()
    {
        wns::simulator::getEventScheduler()->reset();

        std::stringstream ss;
        ss
            << "[test.window.incoming.bitThroughput]\n"
            << "name = test.window.incoming.bitThroughput\n"
            << "description = windowed bit throughput\n"
            << "ignore = false\n"
            << "suffix = .foo\n"
            << "outputPF = true\n"
            << "suffixPF = .pf\n"
            << "type = Log\n"
            << "suffixLog = _log\n"
            << "appendFlag = false\n"
            << "format =  fixed\n"
            << "skipInterval = 0\n"
            << "groupOutputFormat = n\n"


            << "[test.window.incoming.compoundThroughput]\n"
            << "name = test.window.incoming.compoundThroughput\n"
            << "description = windowed compound throughput\n"
            << "ignore = false\n"
            << "suffix = .foo\n"
            << "outputPF = true\n"
            << "suffixPF = .pf\n"
            << "type = Log\n"
            << "suffixLog = _log\n"
            << "appendFlag = false\n"
            << "format =  fixed\n"
            << "skipInterval =  0\n"
            << "groupOutputFormat = n\n"

            << "[test.window.outgoing.bitThroughput]\n"
            << "name = test.window.outgoing.bitThroughput\n"
            << "description = windowed bit throughput\n"
            << "ignore = false\n"
            << "suffix = .foo\n"
            << "outputPF = true\n"
            << "suffixPF = .pf\n"
            << "type = Log\n"
            << "suffixLog = _log\n"
            << "appendFlag = false\n"
            << "format =  fixed\n"
            << "skipInterval = 0\n"
            << "groupOutputFormat = n\n"


            << "[test.window.outgoing.compoundThroughput]\n"
            << "name = test.window.outgoing.compoundThroughput\n"
            << "description = windowed compound throughput\n"
            << "ignore = false\n"
            << "suffix = .foo\n"
            << "outputPF = true\n"
            << "suffixPF = .pf\n"
            << "type = Log\n"
            << "suffixLog = _log\n"
            << "appendFlag = false\n"
            << "format =  fixed\n"
            << "skipInterval =  0\n"
            << "groupOutputFormat = n\n"

            << "[test.window.aggregated.bitThroughput]\n"
            << "name = test.window.aggregated.bitThroughput\n"
            << "description = windowed bit throughput\n"
            << "ignore = false\n"
            << "suffix = .foo\n"
            << "outputPF = true\n"
            << "suffixPF = .pf\n"
            << "type = Log\n"
            << "suffixLog = _log\n"
            << "appendFlag = false\n"
            << "format =  fixed\n"
            << "skipInterval =  0\n"
            << "groupOutputFormat = n\n"

            << "[test.window.aggregated.compoundThroughput]\n"
            << "name = test.window.aggregated.compoundThroughput\n"
            << "description = windowed bit throughput\n"
            << "ignore = false\n"
            << "suffix = .foo\n"
            << "outputPF = true\n"
            << "suffixPF = .pf\n"
            << "type = Log\n"
            << "suffixLog = _log\n"
            << "appendFlag = false\n"
            << "format =  fixed\n"
            << "skipInterval =  0\n"
            << "groupOutputFormat = n\n"

            ;

        layer = new wns::ldk::tests::LayerStub();
        fuNet = new wns::ldk::fun::Main(layer);

        pyconfig::Parser emptyConfig;
        upper = new wns::ldk::tools::Stub(fuNet, emptyConfig);
        lower = new wns::ldk::tools::Stub(fuNet, emptyConfig);

        pyconfig::Parser all;
        all.loadString(
            "from openwns.Probe import Window\n"
            "probe = Window('test', 'test', windowSize = 1.0, sampleInterval = 1.0)\n"
            );
        pyconfig::View config(all, "probe");
        probe = new Window(fuNet, config);

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
            "node = openwns.evaluation.createSourceNode(sim, 'test.window.incoming.bitThroughput')\n"
            "node.appendChildren(TimeSeries())\n"
            "node = openwns.evaluation.createSourceNode(sim, 'test.window.incoming.compoundThroughput')\n"
            "node.appendChildren(TimeSeries())\n"
            "node = openwns.evaluation.createSourceNode(sim, 'test.window.outgoing.bitThroughput')\n"
            "node.appendChildren(TimeSeries())\n"
            "node = openwns.evaluation.createSourceNode(sim, 'test.window.outgoing.compoundThroughput')\n"
            "node.appendChildren(TimeSeries())\n"
            "node = openwns.evaluation.createSourceNode(sim, 'test.window.aggregated.bitThroughput')\n"
            "node.appendChildren(TimeSeries())\n"
            "node = openwns.evaluation.createSourceNode(sim, 'test.window.aggregated.compoundThroughput')\n"
            "node.appendChildren(TimeSeries())\n";

        wns::pyconfig::Parser p;
        p.loadString(configstring);

        wns::probe::bus::ProbeBusRegistry* pbr = wns::simulator::getInstance()->getProbeBusRegistry();
        pbr->spawnProbeBusses(p.get<wns::pyconfig::View>("sim.environment.probeBusRegistry"));
    } // setUp


    void
    WindowTest::cleanup()
    {
        delete fuNet;
        delete layer;
        // Make sure that after deletion of the probes, they have
        // cancelled their timeouts
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), wns::simulator::getEventScheduler()->size());
    } // tearDown


    void
    WindowTest::testThroughput()
    {
        // Make sure there are is 1 periodic event in the queue
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), wns::simulator::getEventScheduler()->size());

        CompoundPtr compound1(fuNet->createCompound(innerPDU));
        CompoundPtr compound2(fuNet->createCompound(innerPDU));
        CompoundPtr compound3(fuNet->createCompound(innerPDU));

        // send first compound at 0.0
        upper->sendData((compound1));

        // send second compound at 0.5
        wns::simulator::getEventScheduler()->schedule(events::NoOp(), 0.5);
        wns::simulator::getEventScheduler()->processOneEvent();
        upper->sendData((compound2));

        // send third compound at 0.6
        wns::simulator::getEventScheduler()->schedule(events::NoOp(), 0.6);
        wns::simulator::getEventScheduler()->processOneEvent();
        upper->sendData((compound3));

        // we should have sent 3 compounds by now
        CPPUNIT_ASSERT(lower->sent.size() == 3);
        CPPUNIT_ASSERT(lower->sent[0] == compound1);
        CPPUNIT_ASSERT(lower->sent[1] == compound2);
        CPPUNIT_ASSERT(lower->sent[2] == compound3);

        // process the periodic event of the probe (i.e. make it
        // actually put)
        wns::simulator::getEventScheduler()->processOneEvent();

        // receiving phase. the incoming and aggregated throughput should now be logged
        wns::simulator::getEventScheduler()->schedule(events::NoOp(), 1.1);
        wns::simulator::getEventScheduler()->processOneEvent(); // 2 [bit/s]
        lower->onData((compound1));

        wns::simulator::getEventScheduler()->schedule(events::NoOp(), 1.5);
        wns::simulator::getEventScheduler()->processOneEvent(); // 2 [bit/s]
        lower->onData((compound2));

        // we should have received 2 compounds by now
        CPPUNIT_ASSERT(upper->received.size() == 2);
        CPPUNIT_ASSERT(upper->received[0] == compound1);
        CPPUNIT_ASSERT(upper->received[1] == compound2);

        // process the next periodi events of the probe (i.e. make it
        // put again)
        wns::simulator::getEventScheduler()->processOneEvent();

        // we should have reached 2.0s by now and have the next
        // periodic event (of the probe) pending
        CPPUNIT_ASSERT(wns::simulator::getEventScheduler()->getTime() == 2.0);
        CPPUNIT_ASSERT(wns::simulator::getEventScheduler()->size() == 1);

        wns::simulator::getInstance()->getProbeBusRegistry()->forwardOutput();

        CPPUNIT_ASSERT(wns::testing::compareFile(
                       "output/test.window.outgoing.bitThroughput_TimeSeries.dat",
                       "(#.*\n)*"
                       "1.0000.*3.0000.*\n"
                       "2.0000.*0.0000.*\n"
                       "(.*\n)*"
                       ));

        CPPUNIT_ASSERT(wns::testing::compareFile(
                       "output/test.window.outgoing.compoundThroughput_TimeSeries.dat",
                       "(#.*\n)*"
                       "1.0000.*3.0000.*\n"
                       "2.0000.*0.0000.*\n"
                       "(.*\n)*"
                       ));

        CPPUNIT_ASSERT(wns::testing::compareFile(
                       "output/test.window.aggregated.bitThroughput_TimeSeries.dat",
                       "(#.*\n)*"
                       "1.0000.*0.0000.*\n"
                       "2.0000.*2.0000.*\n"
                       "(.*\n)*"
                       ));

        CPPUNIT_ASSERT(wns::testing::compareFile(
                       "output/test.window.aggregated.compoundThroughput_TimeSeries.dat",
                       "(#.*\n)*"
                       "1.0000.*0.0000.*\n"
                       "2.0000.*2.0000.*\n"
                       "(.*\n)*"
                       ));

        CPPUNIT_ASSERT(wns::testing::compareFile(
                       "output/test.window.incoming.bitThroughput_TimeSeries.dat",
                       "(#.*\n)*"
                       "1.0000.*0.0000.*\n"
                       "2.0000.*2.0000.*\n"
                       "(.*\n)*"
                       ));

        CPPUNIT_ASSERT(wns::testing::compareFile(
                       "output/test.window.incoming.compoundThroughput_TimeSeries.dat",
                       "(#.*\n)*"
                       "1.0000.*0.0000.*\n"
                       "2.0000.*2.0000.*\n"
                       "(.*\n)*"
                       ));

    } // testThroughput


} // tests
} // probe
} // ldk
} // wns


