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

#ifndef WNS_LDK_MULTIPLEXER_OPCODE_TEST_HPP
#define WNS_LDK_MULTIPLEXER_OPCODE_TEST_HPP

#include <WNS/ldk/multiplexer/OpcodeProvider.hpp>
#include <WNS/ldk/multiplexer/OpcodeSetter.hpp>

#include <WNS/ldk/tools/Stub.hpp>

#include <WNS/ldk/fun/Main.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <stdexcept>

namespace wns { namespace ldk { namespace multiplexer {

    class OpcodeTest : public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE( OpcodeTest );
        CPPUNIT_TEST( testOutgoing );
        CPPUNIT_TEST( testIncoming );
        CPPUNIT_TEST( testSizes );
        CPPUNIT_TEST_SUITE_END();
    public:
        void setUp();
        void tearDown();

        void testOutgoing();
        void testIncoming();
        void testSizes();
    private:
        ILayer* layer;
        fun::Main* fuNet;

        OpcodeProvider* provider;
        tools::Stub* upper;
        OpcodeSetter* setter;
        tools::Stub* lower;
    };

}
}
}


#endif // NOT defined WNS_LDK_MULTIPLEXER_OPCODE_TEST_HPP


