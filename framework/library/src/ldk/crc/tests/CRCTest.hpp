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

#include <WNS/ldk/arq/SelectiveRepeat.hpp>
#include <WNS/ldk/buffer/Bounded.hpp>
#include <WNS/ldk/tools/Stub.hpp>
#include <WNS/ldk/tools/PERProviderStub.hpp>

#include <WNS/ldk/crc/CRC.hpp>

#include <WNS/ldk/fun/Main.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <stdexcept>

#ifndef WNS_LDK_CRC_CRCTEST_HPP
#define WNS_LDK_CRC_CRCTEST_HPP

namespace wns { namespace ldk { namespace crc {

    using namespace wns::ldk;

    class CRCTest :
        public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE( CRCTest );
        CPPUNIT_TEST( testNoErrors );
        CPPUNIT_TEST( testErrors );
        CPPUNIT_TEST( testMarking );
        CPPUNIT_TEST( testSize );
        CPPUNIT_TEST_SUITE_END();
    public:
        void setUp();
        void tearDown();

        void testNoErrors();
        void testErrors();
        void testMarking();
        void testSize();

    private:
        void emptyBuffers();
        ILayer* layer;
        fun::Main* fuNet;

        tools::Stub* upper;
        CRC* crc;
        tools::PERProviderStub* lower;

        static const int checkSumSize;
        void setUpCRC(const int _checkSumSize, const bool _Dropping);
        void setUpPERProvider(const double _PER);
    };

} // crc
} // ldk
} // wns


#endif  // NOT defined WNS_LDK_CRC_CRCTEST_HPP


