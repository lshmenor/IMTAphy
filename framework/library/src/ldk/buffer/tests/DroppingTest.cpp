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
#include <WNS/ldk/buffer/Dropping.hpp>
#include <WNS/ldk/helper/FakePDU.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>

#include <WNS/pyconfig/Parser.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace wns { namespace ldk { namespace buffer { namespace tests {

    class DroppingTest :
        public wns::ldk::tests::DelayedInterfaceTest
    {
        CPPUNIT_TEST_SUB_SUITE( DroppingTest, wns::ldk::tests::DelayedInterfaceTest );
        CPPUNIT_TEST( PDUDropTail );
        CPPUNIT_TEST( PDUDropFront );
        CPPUNIT_TEST( bitDropTail );
        CPPUNIT_TEST( bitDropFront );
        CPPUNIT_TEST_SUITE_END();

    public:
        void
        PDUDropTail();

        void
        PDUDropFront();

        void
        bitDropTail();

        void
        bitDropFront();

    private:
        virtual Dropping*
        newTestee();

        virtual void
        tearDownTestee(DelayedInterface*)
        {
        }
    };

    CPPUNIT_TEST_SUITE_REGISTRATION( DroppingTest );

    Dropping*
    DroppingTest::newTestee()
    {
        pyconfig::Parser all;
        all.loadString(
                       "from openwns.Buffer import *\n"
                       "foo = Dropping(\n"
                       "  size = 2,\n"
                       "  sizeUnit = 'PDU',\n"
                       "  drop = 'Tail'\n"
                       ")\n"
                       );
        wns::pyconfig::View config(all, "foo");

        return new Dropping(getFUN(), config);
    } // newTestee

    void
    DroppingTest::PDUDropTail()
    {
        pyconfig::Parser emptyConfig;
        tools::Stub* lower = new tools::Stub(getFUN(), emptyConfig);
        tools::Stub* upper = new tools::Stub(getFUN(), emptyConfig);

        pyconfig::Parser all;
        all.loadString(
                       "from openwns.Buffer import *\n"
                       "foo = Dropping(\n"
                       "  size = 2,\n"
                       "  sizeUnit = 'PDU',\n"
                       "  drop = 'Tail'\n"
                       ")\n"
                       );
        wns::pyconfig::View config(all, "foo");

        Dropping* buffer = new Dropping(getFUN(), config);
        upper
            ->connect(buffer)
            ->connect(lower);

        CompoundPtr compound1(getFUN()->createCompound());
        CompoundPtr compound2(getFUN()->createCompound());
        CompoundPtr compound3(getFUN()->createCompound());

        lower->close();
        // now the buffer is lossy and it can't deliver compounds to its lower
        // layer -> the 3. PDU will be lost.

        CPPUNIT_ASSERT(buffer->hasCapacity());
        CPPUNIT_ASSERT(buffer->isAccepting(compound1));

        upper->sendData((compound1));	// 1. compound
        CPPUNIT_ASSERT(buffer->hasCapacity());
        CPPUNIT_ASSERT(buffer->isAccepting(compound1));

        upper->sendData((compound2));	// 2. compound
        CPPUNIT_ASSERT(buffer->hasCapacity());
        CPPUNIT_ASSERT(buffer->isAccepting(compound1));

        upper->sendData((compound3));	// 3. compound (should get lost)
        CPPUNIT_ASSERT(buffer->hasCapacity());
        CPPUNIT_ASSERT(buffer->isAccepting(compound1));

        // finally open the lower layer again to see what has been captured
        // in the lossy buffer.
        lower->open();
        CPPUNIT_ASSERT(lower->sent.size() == 2);
        CPPUNIT_ASSERT(lower->sent[0] == compound1);
        CPPUNIT_ASSERT(lower->sent[1] == compound2);

        delete upper;
        delete buffer;
        delete lower;
    } // PDUDropTail


    void
    DroppingTest::PDUDropFront()
    {
        pyconfig::Parser emptyConfig;
        tools::Stub* lower = new tools::Stub(getFUN(), emptyConfig);
        tools::Stub* upper = new tools::Stub(getFUN(), emptyConfig);

        pyconfig::Parser all;
        all.loadString(
                       "from openwns.Buffer import *\n"
                       "foo = Dropping(\n"
                       "  size = 2,\n"
                       "  sizeUnit = 'PDU',\n"
                       "  drop = 'Front'\n"
                       ")\n"
                       );
        wns::pyconfig::View config(all, "foo");

        Dropping* buffer = new Dropping(getFUN(), config);
        upper
            ->connect(buffer)
            ->connect(lower);

        CompoundPtr compound1(getFUN()->createCompound(helper::FakePDUPtr(new helper::FakePDU(2))));
        CompoundPtr compound2(getFUN()->createCompound(helper::FakePDUPtr(new helper::FakePDU(2))));
        CompoundPtr compound3(getFUN()->createCompound(helper::FakePDUPtr(new helper::FakePDU(2))));

        lower->close();
        // now the buffer is lossy and it can't deliver compounds to its lower
        // layer -> the 3. PDU will introduce loss.

        CPPUNIT_ASSERT(buffer->hasCapacity());
        CPPUNIT_ASSERT(buffer->isAccepting(compound1));

        upper->sendData((compound1));	// 1. compound
        CPPUNIT_ASSERT(buffer->hasCapacity());
        CPPUNIT_ASSERT(buffer->isAccepting(compound1));

        upper->sendData((compound2));	// 2. compound
        CPPUNIT_ASSERT(buffer->hasCapacity());
        CPPUNIT_ASSERT(buffer->isAccepting(compound2));

        upper->sendData((compound3));	// 3. compound (-> 1. should get lsot)
        CPPUNIT_ASSERT(buffer->hasCapacity());
        CPPUNIT_ASSERT(buffer->isAccepting(compound3));

        // finally open the lower layer again to see what has been captured
        // in the lossy buffer.
        lower->open();
        CPPUNIT_ASSERT(lower->sent.size() == 2);
        CPPUNIT_ASSERT(lower->sent[0] == compound2);
        CPPUNIT_ASSERT(lower->sent[1] == compound3);

        delete upper;
        delete buffer;
        delete lower;
    } // testPDUDropFront


    void
    DroppingTest::bitDropTail()
    {
        pyconfig::Parser emptyConfig;
        tools::Stub* lower = new tools::Stub(getFUN(), emptyConfig);
        tools::Stub* upper = new tools::Stub(getFUN(), emptyConfig);

        pyconfig::Parser all;
        all.loadString(
                       "from openwns.Buffer import *\n"
                       "foo = Dropping(\n"
                       "  size = 5,\n"
                       "  sizeUnit = 'Bit',\n"
                       "  drop = 'Tail'\n"
                       ")\n"
                       );
        wns::pyconfig::View config(all, "foo");

        Dropping* buffer = new Dropping(getFUN(), config);
        upper
            ->connect(buffer)
            ->connect(lower);

        CompoundPtr compound1(getFUN()->createCompound(helper::FakePDUPtr(new helper::FakePDU(2))));
        CompoundPtr compound2(getFUN()->createCompound(helper::FakePDUPtr(new helper::FakePDU(2))));
        CompoundPtr compound3(getFUN()->createCompound(helper::FakePDUPtr(new helper::FakePDU(2))));

        lower->close();
        // now the buffer is lossy and it can't deliver compounds to its lower
        // layer -> the 3. PDU will be lost.

        CPPUNIT_ASSERT(buffer->hasCapacity());
        CPPUNIT_ASSERT(buffer->isAccepting(compound1));

        upper->sendData((compound1));	// 1. compound
        CPPUNIT_ASSERT(buffer->hasCapacity());
        CPPUNIT_ASSERT(buffer->isAccepting(compound1));

        upper->sendData((compound2));	// 2. compound
        CPPUNIT_ASSERT(buffer->hasCapacity());
        CPPUNIT_ASSERT(buffer->isAccepting(compound1));

        upper->sendData((compound3));	// 3. compound (should get lost)
        CPPUNIT_ASSERT(buffer->hasCapacity());
        CPPUNIT_ASSERT(buffer->isAccepting(compound1));

        // finally open the lower layer again to see what has been captured
        // in the lossy buffer.
        lower->open();
        CPPUNIT_ASSERT(lower->sent.size() == 2);
        CPPUNIT_ASSERT(lower->sent[0] == compound1);
        CPPUNIT_ASSERT(lower->sent[1] == compound2);

        delete upper;
        delete buffer;
        delete lower;
    } // bitDropTail


    void
    DroppingTest::bitDropFront()
    {
        pyconfig::Parser emptyConfig;
        tools::Stub* lower = new tools::Stub(getFUN(), emptyConfig);
        tools::Stub* upper = new tools::Stub(getFUN(), emptyConfig);

        pyconfig::Parser all;
        all.loadString(
                       "from openwns.Buffer import *\n"
                       "foo = Dropping(\n"
                       "  size = 5,\n"
                       "  sizeUnit = 'Bit',\n"
                       "  drop = 'Front'\n"
                       ")\n"
                       );
        wns::pyconfig::View config(all, "foo");

        Dropping* buffer = new Dropping(getFUN(), config);
        upper
            ->connect(buffer)
            ->connect(lower);

        CompoundPtr compound1(getFUN()->createCompound(helper::FakePDUPtr(new helper::FakePDU(2))));
        CompoundPtr compound2(getFUN()->createCompound(helper::FakePDUPtr(new helper::FakePDU(2))));
        CompoundPtr compound3(getFUN()->createCompound(helper::FakePDUPtr(new helper::FakePDU(2))));

        lower->close();
        // now the buffer is lossy and it can't deliver compounds to its lower
        // layer -> the 3. PDU will introduce loss.

        CPPUNIT_ASSERT(buffer->hasCapacity());
        CPPUNIT_ASSERT(buffer->isAccepting(compound1));

        upper->sendData((compound1));	// 1. compound
        CPPUNIT_ASSERT(buffer->hasCapacity());
        CPPUNIT_ASSERT(buffer->isAccepting(compound1));

        upper->sendData((compound2));	// 2. compound
        CPPUNIT_ASSERT(buffer->hasCapacity());
        CPPUNIT_ASSERT(buffer->isAccepting(compound2));

        upper->sendData((compound3));	// 3. compound (-> 1. should get lsot)
        CPPUNIT_ASSERT(buffer->hasCapacity());
        CPPUNIT_ASSERT(buffer->isAccepting(compound3));

        // finally open the lower layer again to see what has been captured
        // in the lossy buffer.
        lower->open();
        CPPUNIT_ASSERT(lower->sent.size() == 2);
        CPPUNIT_ASSERT(lower->sent[0] == compound2);
        CPPUNIT_ASSERT(lower->sent[1] == compound3);

        delete upper;
        delete buffer;
        delete lower;
    } // bitDropFront

}
}
}
}


