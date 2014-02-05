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

#include <WNS/ldk/crc/CRCFilter.hpp>
#include <WNS/ldk/crc/CRC.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/multiplexer/Dispatcher.hpp>
#include <WNS/ldk/tools/Stub.hpp>
#include <WNS/ldk/tools/PERProviderStub.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>

#include <WNS/pyconfig/Parser.hpp>
#include <WNS/Exception.hpp>
#include <WNS/CppUnit.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace wns { namespace ldk {	namespace crc {

    class CRCFilterTest :
        public wns::TestFixture
    {
        CPPUNIT_TEST_SUITE( CRCFilterTest );
        CPPUNIT_TEST( testNotMarking );
        CPPUNIT_TEST( testNoErrorsCommandNotInList );
        CPPUNIT_TEST( testNoErrorsCommandInList );
        CPPUNIT_TEST( testErrorsCommandNotInList );
        CPPUNIT_TEST( testErrorsCommandInList );
        CPPUNIT_TEST_SUITE_END();

    public:
        void prepare();
        void cleanup();

        void testNotMarking();
        void testNoErrorsCommandNotInList();
        void testNoErrorsCommandInList();
        void testErrorsCommandNotInList();
        void testErrorsCommandInList();

    private:
        ILayer* layer;
        fun::Main* fuNet;

        tools::Stub* upperNotInList;
        tools::Stub* upperInList;
        multiplexer::Dispatcher* dispatcher;
        CRCFilter* crcFilter;
        CRC* crc;
        tools::PERProviderStub* lower;

        void setUpCRC(const bool dropping);
        void setUpPERProvider(const double _PER);
    };

    CPPUNIT_TEST_SUITE_REGISTRATION( CRCFilterTest );

    void
    CRCFilterTest::prepare()
    {
        layer = new wns::ldk::tests::LayerStub();
        fuNet = new fun::Main(layer);

        wns::pyconfig::Parser emptyConfig;
        upperNotInList = new tools::Stub(fuNet, emptyConfig);
        upperInList = new tools::Stub(fuNet, emptyConfig);

        wns::pyconfig::Parser dispatcherPyCo;
        dispatcherPyCo.loadString("import openwns.Multiplexer\n"
                          "dispatcher = openwns.Multiplexer.Dispatcher(1)\n");
        wns::pyconfig::View dispatcherView(dispatcherPyCo, "dispatcher");
        dispatcher = new multiplexer::Dispatcher(fuNet, dispatcherView);

        wns::pyconfig::Parser crcFilterPyCo;
        crcFilterPyCo.loadString("import openwns.CRC\n"
                        "crcFilter = openwns.CRC.CRCFilter(\"crc\", [ \"upperInList\" ])\n");
        wns::pyconfig::View crcFilterView(crcFilterPyCo, "crcFilter");
        crcFilter = new CRCFilter(fuNet, crcFilterView);

        fuNet->addFunctionalUnit("upperNotInList", upperNotInList);
        fuNet->addFunctionalUnit("upperInList", upperInList);
        fuNet->addFunctionalUnit("dispatcher", dispatcher);
        fuNet->addFunctionalUnit("crcFilter", crcFilter);

        upperNotInList->connect(dispatcher);
        upperInList->connect(dispatcher);

        dispatcher->connect(crcFilter);

    } // prepare


    void
    CRCFilterTest::setUpCRC(const bool dropping)
    {
        // Construct CRC FU
        wns::pyconfig::Parser crcPyCo;
        std::stringstream ss;
        ss << "import openwns.CRC\n"
           << "crc = openwns.CRC.CRC(\"PERstub\",\n"
           << "  isDropping = "<< (dropping ? "True" : "False") << ")\n";
        crcPyCo.loadString(ss.str());
        wns::pyconfig::View crcView(crcPyCo, "crc");
        crc = new CRC(fuNet, crcView);

        fuNet->addFunctionalUnit("crc", crc);
    } // setUpCRC


    void
    CRCFilterTest::setUpPERProvider(const double _PER)
    {
        // Construct fixed PER stub
        std::stringstream ss;
        ss << "fixedPER = "<< _PER <<"\n";
        wns::pyconfig::Parser perProviderPyCo;
        perProviderPyCo.loadString(ss.str());
        lower = new tools::PERProviderStub(fuNet, perProviderPyCo);

        fuNet->addFunctionalUnit("PERstub", lower);
    } // setUpPERProvider


    void
    CRCFilterTest::cleanup()
    {
        delete fuNet;
        delete layer;
    } // cleanup


    void
    CRCFilterTest::testNotMarking()
    {
        setUpCRC(true);
        setUpPERProvider(0.0);

        crcFilter
            ->connect(crc)
            ->connect(lower);

        CPPUNIT_ASSERT_THROW(fuNet->onFUNCreated(), wns::Exception);
    } // testNotMarking


    void
    CRCFilterTest::testNoErrorsCommandNotInList()
    {
        setUpCRC(false);
        setUpPERProvider(0.0);

        crcFilter
            ->connect(crc)
            ->connect(lower);

        fuNet->onFUNCreated();

        upperNotInList->sendData(fuNet->createCompound());

        CPPUNIT_ASSERT_EQUAL( size_t(1), lower->sent.size() );

        lower->onData(lower->sent[0]);

        CPPUNIT_ASSERT_EQUAL( size_t(1), upperNotInList->received.size() );
    } // testNoErrorsCommandNotInList


    void
    CRCFilterTest::testNoErrorsCommandInList()
    {
        setUpCRC(false);
        setUpPERProvider(0.0);

        crcFilter
            ->connect(crc)
            ->connect(lower);

        fuNet->onFUNCreated();

        upperInList->sendData(fuNet->createCompound());

        CPPUNIT_ASSERT_EQUAL( size_t(1), lower->sent.size() );

        lower->onData(lower->sent[0]);

        CPPUNIT_ASSERT_EQUAL( size_t(1), upperInList->received.size() );
    } // testNoErrorsCommandInList


    void
    CRCFilterTest::testErrorsCommandNotInList()
    {
        setUpCRC(false);
        setUpPERProvider(1.0);

        crcFilter
            ->connect(crc)
            ->connect(lower);

        fuNet->onFUNCreated();

        upperNotInList->sendData(fuNet->createCompound());

        CPPUNIT_ASSERT_EQUAL( size_t(1), lower->sent.size() );

        lower->onData(lower->sent[0]);

        CPPUNIT_ASSERT_EQUAL( size_t(0), upperNotInList->received.size() );
    } // testErrorsCommandNotInList


    void
    CRCFilterTest::testErrorsCommandInList()
    {
        setUpCRC(false);
        setUpPERProvider(1.0);

        crcFilter
            ->connect(crc)
            ->connect(lower);

        fuNet->onFUNCreated();

        upperInList->sendData(fuNet->createCompound());

        CPPUNIT_ASSERT_EQUAL( size_t(1), lower->sent.size() );

        lower->onData(lower->sent[0]);

        CPPUNIT_ASSERT_EQUAL( size_t(1), upperInList->received.size() );
    } // testErrorsCommandInList

} // crc
} // ldk
} // wns


