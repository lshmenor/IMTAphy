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

#include <WNS/ldk/tests/DelayedInterfaceTest.hpp>
#include <WNS/ldk/sar/Soft.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/ldk/tools/Stub.hpp>
#include <WNS/ldk/tools/PERProviderStub.hpp>
#include <WNS/ldk/helper/FakePDU.hpp>
#include <WNS/ldk/fun/Main.hpp>

#include <WNS/pyconfig/Parser.hpp>

#include <WNS/speetcl/inifile.hpp>
#include <WNS/speetcl/db.hpp>
#include <WNS/speetcl/probe.hpp>

#include <cppunit/extensions/HelperMacros.h>

#include <iostream>

namespace wns { namespace ldk { namespace sar { namespace tests {

    class SoftTest :
        public wns::ldk::tests::DelayedInterfaceTest
    {
        CPPUNIT_TEST_SUB_SUITE( SoftTest, DelayedInterfaceTest );
        CPPUNIT_TEST( noSegmentation );
        CPPUNIT_TEST( MTU );
        CPPUNIT_TEST( segmentation );
        CPPUNIT_TEST( heavySegmentation );
        CPPUNIT_TEST( noReassembly );
        CPPUNIT_TEST( MTUReassembly );
        CPPUNIT_TEST( reassembly );
        CPPUNIT_TEST( heavyReassembly );
        CPPUNIT_TEST( multipleMTU );
        CPPUNIT_TEST_SUITE_END();

    public:
        void
        noSegmentation();

        void
        MTU();

        void
        segmentation();

        void
        heavySegmentation();

        void
        noReassembly();

        void
        MTUReassembly();

        void
        reassembly();

        void
        heavyReassembly();

        void
        multipleMTU();

        void
        missingFragment();

    private:
        virtual void
        prepare();

        virtual void
        cleanup();

        virtual Soft*
        newTestee();

        virtual void
        tearDownTestee(DelayedInterface*)
        {
        } // tearDownTestee

        wns::ldk::FunctionalUnit*
        getLowerTestFU() const;

        Bit
        getTotalSize(CommandPool* commandPool, FunctionalUnit* questioner = NULL);

        helper::FakePDUPtr innerPDU;
        CommandPool* commandPool;
        tools::PERProviderStub* perProvider;
    }; // SoftTest

    CPPUNIT_TEST_SUITE_REGISTRATION( SoftTest );

    void
    SoftTest::prepare()
    {
        static bool probeCreated = false;

        if(!probeCreated)
        {
            probeCreated = true;

            { // simple
                IniFile ini;
                std::stringstream ss;
                ss
                    << "[test.SoftSAR]\n"
                    << "name = test.SoftSAR\n"
                    << "debug = true\n"
                    << "description = me\n"
                    << "ignore = false\n"
                    << "suffix = foo\n"
                    << "outputPF = true\n"
                    << "suffixPF = foopf\n"

                    << "type = PPDF\n"

                    << "minXValue = 0\n"
                    << "maxXValue = 1\n"
                    << "resolution = 5\n"
                    << "xScaleType = linear\n"

                    << "format =  fixed\n"
                    << "skipInterval = 0\n"

                    << "groupOutputFormat = n\n";

                ini.read(ss);

                PProbe *probe = new PProbe(&ini, String("test.SoftSAR"));
                PDataBase::getInstance()->registerProbe(probe);
                delete probe;
                // noone keeps a copy?
            }
        }

        wns::ldk::tests::DelayedInterfaceTest::prepare();

        innerPDU = helper::FakePDUPtr(new helper::FakePDU());
        commandPool = getFUN()->createCommandPool();

    } // prepare


    void
    SoftTest::cleanup()
    {
        innerPDU = helper::FakePDUPtr();
    } // cleanup


    Soft*
    SoftTest::newTestee()
    {
        wns::pyconfig::Parser all = wns::pyconfig::Parser();
        all.loadString(
                       "from openwns.SAR import Soft\n"
                       "sar = Soft(\n"
                       "  42,\n"
                       "  \"perProviderStub\",\n"
                       "  probeName = 'test.SoftSAR'\n"
                       ")\n"
                       );

        wns::pyconfig::Parser allPER = wns::pyconfig::Parser();
        allPER.loadString("fixedPER = 0.0\n");
        this->perProvider = new tools::PERProviderStub(getFUN(), allPER);

        wns::pyconfig::View pyco(all, "sar");
        Soft* soft = new Soft(getFUN(), pyco);
        soft->connect(this->perProvider);
        this->getFUN()->addFunctionalUnit(this->perProvider->getName(), this->perProvider);
        return soft;

    } // newTestee

    wns::ldk::FunctionalUnit*
    SoftTest::getLowerTestFU() const
    {
        return perProvider;
    } // getLowerTestFU

    Bit
    SoftTest::getTotalSize(CommandPool* commandPool, FunctionalUnit* questioner)
    {
        Bit dataSize = 0;
        Bit commandPoolSize = 0;

        getFUN()->calculateSizes(commandPool, commandPoolSize, dataSize, questioner);

        return commandPoolSize+dataSize;
    } // getTotalSize

    void
    SoftTest::noSegmentation()
    {
        innerPDU->setLengthInBits(1);
        CompoundPtr compound(new Compound(commandPool, innerPDU));

        CPPUNIT_ASSERT_EQUAL(1, getTotalSize(commandPool));

        CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(0), compoundsSent());
        getUpperStub()->sendData(compound);
        CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), compoundsSent());

        CPPUNIT_ASSERT_EQUAL(2, getTotalSize(commandPool));
    } // noSegmentation


    void
    SoftTest::MTU()
    {
        innerPDU->setLengthInBits(41);
        CompoundPtr compound(new Compound(commandPool, innerPDU));

        CPPUNIT_ASSERT_EQUAL(41, getTotalSize(commandPool));

        CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(0), compoundsSent());
        getUpperStub()->sendData(compound);
        CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), compoundsSent());

        CPPUNIT_ASSERT_EQUAL(42, getTotalSize(getLowerStub()->sent[0]->getCommandPool()));
    } // MTU


    void
    SoftTest::segmentation()
    {
        innerPDU->setLengthInBits(43);
        CompoundPtr compound(new Compound(commandPool, innerPDU));

        CPPUNIT_ASSERT_EQUAL(43, getTotalSize(commandPool));

        CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(0), compoundsSent());
        getUpperStub()->sendData(compound);
        CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(2), compoundsSent());

        CPPUNIT_ASSERT_EQUAL(42, getTotalSize(getLowerStub()->sent[0]->getCommandPool()));
        CPPUNIT_ASSERT_EQUAL(3, getTotalSize(getLowerStub()->sent[1]->getCommandPool()));
    } // segmentation


    void
    SoftTest::heavySegmentation()
    {
        innerPDU->setLengthInBits(400);
        CompoundPtr compound(new Compound(commandPool, innerPDU));

        CPPUNIT_ASSERT_EQUAL(400, getTotalSize(commandPool));

        CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(0), compoundsSent());
        getUpperStub()->sendData(compound);
        CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(10), compoundsSent());

        for(int i = 0; i < 9; ++i)
            CPPUNIT_ASSERT_EQUAL(42, getTotalSize(getLowerStub()->sent[i]->getCommandPool()));
            CPPUNIT_ASSERT_EQUAL(32, getTotalSize(getLowerStub()->sent[9]->getCommandPool()));
    } // heavySegmentation


    void
    SoftTest::noReassembly()
    {
        innerPDU->setLengthInBits(1);
        CompoundPtr compound(new Compound(commandPool, innerPDU));
        getUpperStub()->sendData(compound);

        getLowerStub()->onData(getLowerStub()->sent[0]);
        getLowerStub()->sent.clear();
        CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), compoundsDelivered());
        CPPUNIT_ASSERT_EQUAL(1, getTotalSize(getUpperStub()->received[0]->getCommandPool(), getTestee<Soft>()));
    } // noReassembly


    void
    SoftTest::MTUReassembly()
    {
        innerPDU->setLengthInBits(41);
        CompoundPtr compound(new Compound(commandPool, innerPDU));
        getUpperStub()->sendData(compound);

        getLowerStub()->onData(getLowerStub()->sent[0]);
        getLowerStub()->sent.clear();
        CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), compoundsDelivered());
        CPPUNIT_ASSERT_EQUAL(41, getTotalSize(getUpperStub()->received[0]->getCommandPool(), getTestee<Soft>()));
    } // MTUReassembly


    void
    SoftTest::reassembly()
    {
        innerPDU->setLengthInBits(43);
        CompoundPtr compound(new Compound(commandPool, innerPDU));
        getUpperStub()->sendData(compound);
        CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(2), compoundsSent());

        getLowerStub()->onData(getLowerStub()->sent[0]);
        getLowerStub()->onData(getLowerStub()->sent[1]);
        getLowerStub()->sent.clear();
        CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), compoundsDelivered());
        CPPUNIT_ASSERT_EQUAL(43, getTotalSize(getUpperStub()->received[0]->getCommandPool(), getTestee<Soft>()));
    } // reassembly


    void
    SoftTest::heavyReassembly()
    {
        innerPDU->setLengthInBits(410);
        CompoundPtr compound(new Compound(commandPool, innerPDU));
        getUpperStub()->sendData(compound);

        for (tools::PERProviderStub::ContainerType::iterator i = getLowerStub()->sent.begin();
             i != getLowerStub()->sent.end();
             ++i)
            {
                getLowerStub()->onData(*i);
            }
        getLowerStub()->sent.clear();
        CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), compoundsDelivered());
        CPPUNIT_ASSERT_EQUAL(410, getTotalSize(getUpperStub()->received[0]->getCommandPool(), getTestee<Soft>()));
    } // heavyReassembly


    void
    SoftTest::multipleMTU()
    {
        innerPDU->setLengthInBits(82);
        CompoundPtr compound(new Compound(commandPool, innerPDU));

        CPPUNIT_ASSERT_EQUAL(82, getTotalSize(commandPool));

        CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(0), compoundsSent());
        getUpperStub()->sendData(compound);
        CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(2), compoundsSent());

        CPPUNIT_ASSERT_EQUAL(42, getTotalSize(getLowerStub()->sent[0]->getCommandPool()));
        CPPUNIT_ASSERT_EQUAL(42, getTotalSize(getLowerStub()->sent[1]->getCommandPool()));
    } // multipleMTU


    void
    SoftTest::missingFragment()
    {
        innerPDU->setLengthInBits(60);
        CompoundPtr compound(new Compound(commandPool, innerPDU));
        getUpperStub()->sendData(compound);

        CPPUNIT_ASSERT_THROW(getLowerStub()->onData(getLowerStub()->sent[1]), wns::Exception);
    } // missingFragment


}
}
}
}


