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

#ifndef WNS_LDK_ARQ_PIGGYBACKER_TEST_HPP
#define WNS_LDK_ARQ_PIGGYBACKER_TEST_HPP


#include <WNS/ldk/arq/PiggyBacker.hpp>
#include <WNS/ldk/arq/StopAndWait.hpp>
#include <WNS/ldk/buffer/Bounded.hpp>
#include <WNS/ldk/tools/Stub.hpp>
#include <WNS/ldk/tools/Consumer.hpp>

#include <WNS/ldk/fun/Main.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <stdexcept>

namespace wns { namespace ldk { namespace arq {

    using namespace wns::ldk;

    class PiggyBackerTest :
        public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE( PiggyBackerTest );
        CPPUNIT_TEST( testOutgoingI );
        CPPUNIT_TEST( testOutgoingACK );
        CPPUNIT_TEST( testOutgoingBoth );
        CPPUNIT_TEST( testIncomingI );
        CPPUNIT_TEST( testIncomingACK );
        CPPUNIT_TEST( testIncomingBoth );
        CPPUNIT_TEST( testWakeupNone_None );
        CPPUNIT_TEST( testWakeupNone_I );
        CPPUNIT_TEST( testWakeupNone_ACK );
        CPPUNIT_TEST( testWakeupNone_Both );
        CPPUNIT_TEST( testWakeupI_None );
        CPPUNIT_TEST( testWakeupI_ACK );
        CPPUNIT_TEST( testWakeupACK_None );
        CPPUNIT_TEST( testWakeupACK_I );
        CPPUNIT_TEST( testSizeI );
        CPPUNIT_TEST( testSizeACK );
        CPPUNIT_TEST( testSizeBoth );
        CPPUNIT_TEST_SUITE_END();
    public:
        void setUp();
        void tearDown();

        void testOutgoingI();
        void testOutgoingACK();
        void testOutgoingBoth();
        void testIncomingI();
        void testIncomingACK();
        void testIncomingBoth();
        void testWakeupNone_None();
        void testWakeupNone_I();
        void testWakeupNone_ACK();
        void testWakeupNone_Both();
        void testWakeupI_None();
        void testWakeupI_ACK();
        void testWakeupACK_None();
        void testWakeupACK_I();
        void testSizeI();
        void testSizeACK();
        void testSizeBoth();
    private:
        ILayer* layer;
        fun::Main* fuNet;

        buffer::Bounded* buffer;
        tools::Stub* upper;
        PiggyBacker* piggy;
        StopAndWait* arq;
        tools::Stub* lower;
        tools::Consumer* consumer;

        CompoundPtr ackCompound;
        CompoundPtr iCompound;

        static const int bitsPerIFrame;
        static const int bitsPerRRFrame;

        static const int bitsIfPiggyBacked;
        static const int bitsIfNotPiggyBacked;
    };

}
}
}


#endif  // NOT defined WNS_LDK_ARQ_PIGGYBACKER_TEST_HPP
