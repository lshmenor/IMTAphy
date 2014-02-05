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
#include <WNS/ldk/probe/TickTack.hpp>

#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/tools/Stub.hpp>
#include <WNS/ldk/tools/ConstantDelay.hpp>
#include <WNS/pyconfig/Parser.hpp>
#include <WNS/CppUnit.hpp>
#include <WNS/ldk/helper/FakePDU.hpp>
#include <WNS/probe/bus/ProbeBus.hpp>
#include <WNS/simulator/ISimulator.hpp>

namespace wns { namespace ldk { namespace probe { namespace tests {

                class TickTackConstDelayTest :
                    public wns::TestFixture,
                    public wns::probe::bus::ProbeBus
                {

                    CPPUNIT_TEST_SUITE( TickTackConstDelayTest );
                    CPPUNIT_TEST( testDelay );
                    //CPPUNIT_TEST( testIncoming );
                    CPPUNIT_TEST_SUITE_END();

                public:
                    void prepare()
                    {
                        this->startObserving(
                            wns::simulator::getProbeBusRegistry()->getMeasurementSource("test.delay"));

                        layerTX.reset(new wns::ldk::tests::LayerStub());
                        fuNetTX.reset(new fun::Main(layerTX.get()));
                        layerRX.reset(new wns::ldk::tests::LayerStub());
                        fuNetRX.reset(new fun::Main(layerRX.get()));

                        pyconfig::Parser emptyConfig;
                        pyconfig::View config =
                            pyconfig::Parser::fromString(
                                "import openwns.ldk\n"
                                "tick = openwns.ldk.Probe.Tick(\"test\")\n"
                                "delay1 = openwns.ldk.Tools.ConstantDelay(0.1)\n"
                                "delay2 = openwns.ldk.Tools.ConstantDelay(0.25)\n"
                                "tack = openwns.ldk.Probe.Tack(\"test\")\n");

                        tickTX = new Tick(fuNetTX.get(), config.get("tick"));
                        delay1 = new tools::ConstantDelay(fuNetTX.get(), config.get("delay1"));
                        tackTX = new Tack(fuNetTX.get(), config.get("tack"));
                        delay2 = new tools::ConstantDelay(fuNetTX.get(), config.get("delay2"));
                        lower = new tools::Stub(fuNetTX.get(), emptyConfig);

                        upper = new tools::Stub(fuNetRX.get(), emptyConfig);
                        tickRX = new Tick(fuNetRX.get(), config.get("tick"));
                        tackRX = new Tack(fuNetRX.get(), config.get("tack"));

                        tickTX->connect(delay1);
                        delay1->connect(tackTX);
                        tackTX->connect(delay2);
                        delay2->connect(lower);

                        upper->connect(tackRX);
                        tackRX->connect(tickRX);

                        fuNetTX->addFunctionalUnit("test", "tickTX", tickTX);
                        fuNetTX->addFunctionalUnit("delay1", delay1);
                        fuNetTX->addFunctionalUnit("tackTX", tackTX);
                        fuNetTX->addFunctionalUnit("delay2", delay2);

                        fuNetRX->addFunctionalUnit("test", "tickRX", tickRX);
                        fuNetRX->addFunctionalUnit("tackRX", tackRX);

                        fuNetTX->onFUNCreated();
                        fuNetRX->onFUNCreated();

                        probedDelay = -1.0;
                        nProbed = 0;
                    }

                    // Probe bus interface
                    virtual bool
                    accepts(const wns::simulator::Time&, const wns::probe::bus::IContext&)
                    {return true;};

                    virtual void
                    onMeasurement(const wns::simulator::Time&,
                        const double& measurement,
                        const wns::probe::bus::IContext&)
                    {
                        probedDelay = measurement;
                        nProbed++;
                    };

                    virtual void
                    output() {};

                    void
                    testDelay()
                    {
                        CPPUNIT_ASSERT(nProbed == 0);

                        wns::ldk::helper::FakePDUPtr innerPDU =
                                wns::ldk::helper::FakePDUPtr(new wns::ldk::helper::FakePDU(1));

                        CompoundPtr compound1(fuNetTX->createCompound(innerPDU));
                        CompoundPtr compound2(fuNetTX->createCompound(innerPDU));
                        CompoundPtr compound3(fuNetTX->createCompound(innerPDU));
                        CompoundPtr compound4(fuNetTX->createCompound(innerPDU));
                        CompoundPtr compound5(fuNetTX->createCompound(innerPDU));

                        tackTX->probeIncoming();
                        tackRX->probeOutgoing();

                        tickTX->sendData(compound1);
                        wns::simulator::getEventScheduler()->processOneEvent();
                        wns::simulator::getEventScheduler()->processOneEvent();
                        tickRX->onData(compound1);

                        // Wrong direction in RX and TX, we do not probe
                        CPPUNIT_ASSERT(nProbed == 0);

                        tackTX->probeOutgoing();

                        tickTX->sendData(compound2);
                        wns::simulator::getEventScheduler()->processOneEvent();
                        wns::simulator::getEventScheduler()->processOneEvent();
                        tickRX->onData(compound2);

                        // Now we probe outgoing in TX
                        CPPUNIT_ASSERT(nProbed == 1);
                        WNS_ASSERT_MAX_REL_ERROR(probedDelay, 0.1, 1E-6);

                        tackRX->probeIncoming();
                        tickTX->sendData(compound3);
                        wns::simulator::getEventScheduler()->processOneEvent();
                        wns::simulator::getEventScheduler()->processOneEvent();
                        tickRX->onData(compound3);

                        // TX and RX now probe, but compound is only probed once
                        CPPUNIT_ASSERT(nProbed == 2);
                        WNS_ASSERT_MAX_REL_ERROR(probedDelay, 0.1, 1E-6);

                        tackRX->probeIncoming();
                        tackTX->probeIncoming();

                        tickTX->sendData(compound4);
                        wns::simulator::getEventScheduler()->processOneEvent();

                        // TickTX should not probe
                        CPPUNIT_ASSERT(nProbed == 2);

                        wns::simulator::getEventScheduler()->processOneEvent();
                        tickRX->onData(compound4);

                        // RX probes
                        CPPUNIT_ASSERT(nProbed == 3);
                        WNS_ASSERT_MAX_REL_ERROR(probedDelay, 0.35, 1E-6);

                        // TX side does not probe, RX does
                        tickTX->probeIncoming();
                        tackTX->probeIncoming();
                        tickRX->probeIncoming();
                        tackRX->probeIncoming();

                        tickTX->sendData(compound5);
                        wns::simulator::getEventScheduler()->processOneEvent();
                        wns::simulator::getEventScheduler()->processOneEvent();

                        // No probing in TX
                        CPPUNIT_ASSERT(nProbed == 3);
                        tickRX->onData(compound5);

                        // Probed zero delay in RX 
                        CPPUNIT_ASSERT(nProbed == 4);
                        WNS_ASSERT_MAX_REL_ERROR(probedDelay, 0.0, 1E-6);
                    }

                    void cleanup() 
                    {
                    }

                private:
                    std::auto_ptr<ILayer> layerTX;
                    std::auto_ptr<ILayer> layerRX;
                    std::auto_ptr<fun::Main> fuNetTX;
                    std::auto_ptr<fun::Main> fuNetRX;

                    Tick* tickTX;
                    tools::ConstantDelay* delay1;
                    Tack* tackTX;
                    tools::ConstantDelay* delay2;
                    tools::Stub* lower;

                    tools::Stub* upper;
                    Tick* tickRX;
                    Tack* tackRX;

                    wns::simulator::Time probedDelay;
                    unsigned int nProbed;

                };

                CPPUNIT_TEST_SUITE_REGISTRATION( TickTackConstDelayTest );

            }
        }
    }
}
