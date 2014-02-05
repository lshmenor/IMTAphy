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
#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/tools/Stub.hpp>
#include <WNS/ldk/tools/Compressor.hpp>
#include <WNS/pyconfig/Parser.hpp>
#include <WNS/CppUnit.hpp>
#include <WNS/ldk/helper/FakePDU.hpp>

namespace wns { namespace ldk { namespace tools { namespace tests {

                class CompressorTest :
                    public wns::TestFixture
                {

                    CPPUNIT_TEST_SUITE( CompressorTest );
                    CPPUNIT_TEST( testSize );
                    CPPUNIT_TEST_SUITE_END();

                public:
                    void prepare()
                    {
                        layer.reset( new wns::ldk::tests::LayerStub() );
                        fuNet.reset( new fun::Main(layer.get()) );

                        pyconfig::Parser emptyConfig;
                        pyconfig::View config =
                            pyconfig::Parser::fromString(
                                "import openwns.ldk\n"
                                "compressorFU = openwns.ldk.Tools.Compressor(42)\n");
                        upper = new Stub(fuNet.get(), emptyConfig);
                        compressor = new Compressor(fuNet.get(), config.get("compressorFU"));
                        lower = new Stub(fuNet.get(), emptyConfig);

                        upper
                            ->connect(compressor)
                            ->connect(lower);
                        fuNet->addFunctionalUnit("upperStub", upper);
                        fuNet->addFunctionalUnit("compressor", compressor);
                        fuNet->addFunctionalUnit("lowerStub", lower);
                    }

                    void
                    testSize()
                    {
                        CompoundPtr compound
                            (fuNet->createCompound(
                                wns::osi::PDUPtr( new wns::ldk::helper::FakePDU(84))));
                        upper->sendData(compound);
                        CPPUNIT_ASSERT(lower->sent.size() == 1);
                        CPPUNIT_ASSERT(lower->sent[0] == compound);
                        CPPUNIT_ASSERT_EQUAL(Bit(42), compound->getLengthInBits());
                    }

                    void cleanup() {}

                private:
                    std::auto_ptr<ILayer> layer;
                    std::auto_ptr<fun::FUN> fuNet;

                    Stub* upper;
                    Compressor* compressor;
                    Stub* lower;

                };

                CPPUNIT_TEST_SUITE_REGISTRATION( CompressorTest );

            }
        }
    }
}
