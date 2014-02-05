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

#include <WNS/ldk/sar/Fixed.hpp>

#include <WNS/ldk/tests/DelayedInterfaceTest.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/ldk/helper/FakePDU.hpp>
#include <WNS/pyconfig/Parser.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace wns { namespace ldk { namespace sar { namespace tests {

    class FixedTest :
        public wns::ldk::tests::DelayedInterfaceTest
    {
        CPPUNIT_TEST_SUB_SUITE( FixedTest, wns::ldk::tests::DelayedInterfaceTest );
        CPPUNIT_TEST( noSegmentation );
        CPPUNIT_TEST( segmentSize );
        CPPUNIT_TEST( segmentation );
        CPPUNIT_TEST( heavySegmentation );
        CPPUNIT_TEST( noReassembly );
        CPPUNIT_TEST( segmentSizeReassembly );
        CPPUNIT_TEST( reassembly );
        CPPUNIT_TEST( heavyReassembly );
        CPPUNIT_TEST( multipleSegmentSize );
        CPPUNIT_TEST( missingFragment );
        CPPUNIT_TEST( changingSegmentSizeOutgoing );
        CPPUNIT_TEST( changingSegmentSizeIncoming );
        CPPUNIT_TEST( testChangeSegmentSizeOnLastSegment );
        CPPUNIT_TEST_SUITE_END();
    public:
        void
        noSegmentation();

        void
        segmentSize();

        void
        segmentation();

        void
        heavySegmentation();

        void
        noReassembly();

        void
        segmentSizeReassembly();

        void
        reassembly();

        void
        heavyReassembly();

        void
        multipleSegmentSize();

        void
        missingFragment();

        void
        changingSegmentSizeOutgoing();

        void
        changingSegmentSizeIncoming();

        void
        testChangeSegmentSizeOnLastSegment();

    private:
        virtual void
        prepare();

        virtual void
        cleanup();

        virtual Fixed*
        newTestee();

        virtual void
        tearDownTestee(DelayedInterface*)
        {
        } // tearDownTestee

        Bit
        getTotalSize(CommandPool* commandPool, FunctionalUnit* questioner = NULL);

        helper::FakePDUPtr innerPDU;
        CommandPool* commandPool;
    }; // FixedTest


    CPPUNIT_TEST_SUITE_REGISTRATION( FixedTest );

    Fixed*
    FixedTest::newTestee()
    {
        wns::pyconfig::Parser all = wns::pyconfig::Parser();
        all.loadString(
                       "from openwns.SAR import Fixed\n"
                       "sar = Fixed(42)\n"
                       );
        wns::pyconfig::View pyco(all, "sar");
        return new Fixed(getFUN(), pyco);
    } // newTestee

    void
    FixedTest::prepare()
    {
        wns::ldk::tests::DelayedInterfaceTest::prepare();

        innerPDU = helper::FakePDUPtr(new helper::FakePDU());
        commandPool = getFUN()->createCommandPool();
    } // prepare


    void
    FixedTest::cleanup()
    {
        innerPDU = helper::FakePDUPtr();

        wns::ldk::tests::DelayedInterfaceTest::cleanup();
    } // cleanup


    Bit
    FixedTest::getTotalSize(CommandPool* commandPool, FunctionalUnit* questioner)
    {
        Bit dataSize = 0;
        Bit commandPoolSize = 0;

        getFUN()->calculateSizes(commandPool, commandPoolSize, dataSize, questioner);

        return commandPoolSize+dataSize;
    } // getTotalSize

    void
    FixedTest::noSegmentation()
    {
        innerPDU->setLengthInBits(1);
        CompoundPtr compound(new Compound(commandPool, innerPDU));

        CPPUNIT_ASSERT_EQUAL(Bit(1), getTotalSize(commandPool));

        CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(0), compoundsSent());
        getUpperStub()->sendData(compound);
        CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), compoundsSent());

        CPPUNIT_ASSERT_EQUAL(Bit(2), getTotalSize(commandPool));
    } // noSegmentation


    void
    FixedTest::segmentSize()
    {
        innerPDU->setLengthInBits(41);
        CompoundPtr compound(new Compound(commandPool, innerPDU));

        CPPUNIT_ASSERT_EQUAL(Bit(41), getTotalSize(commandPool));

        CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(0), compoundsSent());
        getUpperStub()->sendData(compound);
        CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), compoundsSent());

        CPPUNIT_ASSERT_EQUAL(Bit(42), getTotalSize(getLowerStub()->sent[0]->getCommandPool()));
    } // segmentSize


    void
    FixedTest::segmentation()
    {
        innerPDU->setLengthInBits(43);
        CompoundPtr compound(new Compound(commandPool, innerPDU));

        CPPUNIT_ASSERT_EQUAL(Bit(43), getTotalSize(commandPool));

        CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(0), compoundsSent());
        getUpperStub()->sendData(compound);
        CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(2), compoundsSent());

        CPPUNIT_ASSERT_EQUAL(Bit(42), getTotalSize(getLowerStub()->sent[0]->getCommandPool()));
        CPPUNIT_ASSERT_EQUAL(Bit(3), getTotalSize(getLowerStub()->sent[1]->getCommandPool()));
    } // segmentation


    void
    FixedTest::heavySegmentation()
    {
        innerPDU->setLengthInBits(400);
        CompoundPtr compound(new Compound(commandPool, innerPDU));

        CPPUNIT_ASSERT_EQUAL(Bit(400), getTotalSize(commandPool));

        CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(0), compoundsSent());
        getUpperStub()->sendData(compound);
        CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(10), compoundsSent());

        for(int i = 0; i < 9; ++i)
            CPPUNIT_ASSERT_EQUAL(Bit(42), getTotalSize(getLowerStub()->sent[i]->getCommandPool()));
            CPPUNIT_ASSERT_EQUAL(Bit(32), getTotalSize(getLowerStub()->sent[9]->getCommandPool()));
    } // heavySegmentation


    void
    FixedTest::noReassembly()
    {
        innerPDU->setLengthInBits(1);
        CompoundPtr compound(new Compound(commandPool, innerPDU));
        getUpperStub()->sendData(compound);

        getLowerStub()->onData(getLowerStub()->sent[0]);
        getLowerStub()->sent.clear();
        CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), compoundsDelivered());
        CPPUNIT_ASSERT_EQUAL(Bit(1), getTotalSize(getUpperStub()->received[0]->getCommandPool()));
    } // noReassembly


    void
    FixedTest::segmentSizeReassembly()
    {
        innerPDU->setLengthInBits(41);
        CompoundPtr compound(new Compound(commandPool, innerPDU));
        getUpperStub()->sendData(compound);

        getLowerStub()->onData(getLowerStub()->sent[0]);
        getLowerStub()->sent.clear();
        CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), compoundsDelivered());
        CPPUNIT_ASSERT_EQUAL(Bit(41), getTotalSize(getUpperStub()->received[0]->getCommandPool()));
    } // segmentSizeReassembly


    void
    FixedTest::reassembly()
    {
        innerPDU->setLengthInBits(43);
        CompoundPtr compound(new Compound(commandPool, innerPDU));
        getUpperStub()->sendData(compound);
        CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(2), compoundsSent());

        getLowerStub()->onData(getLowerStub()->sent[0]);
        getLowerStub()->onData(getLowerStub()->sent[1]);
        getLowerStub()->sent.clear();
        CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), compoundsDelivered());
        CPPUNIT_ASSERT_EQUAL(Bit(43), getTotalSize(getUpperStub()->received[0]->getCommandPool()));
    } // reassembly


    void
    FixedTest::heavyReassembly()
    {
        innerPDU->setLengthInBits(410);
        CompoundPtr compound(new Compound(commandPool, innerPDU));
        getUpperStub()->sendData(compound);

        for (tools::Stub::ContainerType::iterator i = getLowerStub()->sent.begin();
             i != getLowerStub()->sent.end();
             ++i)
            {
                getLowerStub()->onData(*i);
            }
        getLowerStub()->sent.clear();
        CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), compoundsDelivered());
        CPPUNIT_ASSERT_EQUAL(Bit(410), getTotalSize(getUpperStub()->received[0]->getCommandPool()));
    } // heavyReassembly


    void
    FixedTest::multipleSegmentSize()
    {
        innerPDU->setLengthInBits(82);
        CompoundPtr compound(new Compound(commandPool, innerPDU));

        CPPUNIT_ASSERT_EQUAL(Bit(82), getTotalSize(commandPool));

        CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(0), compoundsSent());
        getUpperStub()->sendData(compound);
        CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(2), compoundsSent());

        CPPUNIT_ASSERT_EQUAL(Bit(42), getTotalSize(getLowerStub()->sent[0]->getCommandPool()));
        CPPUNIT_ASSERT_EQUAL(Bit(42), getTotalSize(getLowerStub()->sent[1]->getCommandPool()));
    } // multipleSegmentSize


    void
    FixedTest::missingFragment()
    {
        innerPDU->setLengthInBits(60);
        CompoundPtr compound(new Compound(commandPool, innerPDU));
        getUpperStub()->sendData(compound);

        WNS_ASSERT_ASSURE_EXCEPTION(getLowerStub()->onData(getLowerStub()->sent[1]));
    } // missingFragment


    void
    FixedTest::changingSegmentSizeOutgoing()
    {
        innerPDU->setLengthInBits(84);
        CompoundPtr compound(new Compound(commandPool, innerPDU));
        getLowerStub()->setStepping(true);
        getUpperStub()->sendData(compound);
        CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), compoundsSent());

        getTestee<Fixed>()->setSegmentSize(21);
        getLowerStub()->step();
        CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(2), compoundsSent());
        getLowerStub()->step();
        CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(3), compoundsSent());
        getLowerStub()->step();
        CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(4), compoundsSent());
        getLowerStub()->step();
        CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(4), compoundsSent());
        CPPUNIT_ASSERT_EQUAL(Bit(42), getTotalSize(getLowerStub()->sent[0]->getCommandPool()));
        CPPUNIT_ASSERT_EQUAL(Bit(21), getTotalSize(getLowerStub()->sent[1]->getCommandPool()));
        CPPUNIT_ASSERT_EQUAL(Bit(21), getTotalSize(getLowerStub()->sent[2]->getCommandPool()));
        CPPUNIT_ASSERT_EQUAL(Bit(4), getTotalSize(getLowerStub()->sent[3]->getCommandPool()));
    } // changingSegmentSizeOutgoing

    void
    FixedTest::changingSegmentSizeIncoming()
    {
        innerPDU->setLengthInBits(84);
        CompoundPtr compound(new Compound(commandPool, innerPDU));
        getLowerStub()->setStepping(true);
        getUpperStub()->sendData(compound);
        getTestee<Fixed>()->setSegmentSize(21);
        getLowerStub()->setStepping(false);
        getLowerStub()->open();

        getLowerStub()->onData(getLowerStub()->sent[0]);
        getLowerStub()->onData(getLowerStub()->sent[1]);
        getLowerStub()->onData(getLowerStub()->sent[2]);
        getLowerStub()->onData(getLowerStub()->sent[3]);

        CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), compoundsDelivered());
        CPPUNIT_ASSERT_EQUAL(Bit(84), getTotalSize(getUpperStub()->received[0]->getCommandPool()));
    } // changingSegmentSizeIncoming

    void
    FixedTest::testChangeSegmentSizeOnLastSegment()
    {
        innerPDU->setLengthInBits(42+42+2);
        CompoundPtr compound(new Compound(commandPool, innerPDU));
        getLowerStub()->setStepping(true);
        getUpperStub()->sendData(compound);
        CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), compoundsSent());

        getLowerStub()->step();
        CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(2), compoundsSent());
        getTestee<Fixed>()->setSegmentSize(42);
        getLowerStub()->step();
        CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(3), compoundsSent());
        CPPUNIT_ASSERT_EQUAL(Bit(42), getTotalSize(getLowerStub()->sent[0]->getCommandPool()));
        CPPUNIT_ASSERT_EQUAL(Bit(42), getTotalSize(getLowerStub()->sent[1]->getCommandPool()));
        CPPUNIT_ASSERT_EQUAL(Bit(5), getTotalSize(getLowerStub()->sent[2]->getCommandPool()));
    }

}
}
}
}


