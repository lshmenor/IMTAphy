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

#ifndef WNS_LDK_MULTIPLEXER_FRAMEDISPATCHER_TEST_HPP
#define WNS_LDK_MULTIPLEXER_FRAMEDISPATCHER_TEST_HPP

#include <WNS/ldk/multiplexer/FrameDispatcher.hpp>

#include <WNS/ldk/tools/Stub.hpp>

#include <WNS/ldk/fun/Main.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <stdexcept>

namespace wns { namespace ldk { namespace multiplexer {

    class FrameDispatcherTest : public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE( FrameDispatcherTest );
        CPPUNIT_TEST( testOutgoing );
        CPPUNIT_TEST( testIncoming );
        CPPUNIT_TEST( testDispatching );
        CPPUNIT_TEST( testSizes );
        /**
         * @todo pyconfig::Parser of FrameDispatcher not working
         */
//      CPPUNIT_TEST( testpyconfig::Parser );
        CPPUNIT_TEST_SUITE_END();
    public:
        void setUp();
        void tearDown();

        void testOutgoing();
        void testIncoming();
        void testDispatching();
        void testSizes();
        void testPyConfig();
    private:
        ILayer* layer;
        fun::Main* fuNet;

        tools::Stub* upper1;
        tools::Stub* upper2;
        FrameDispatcher* dispatcher;
        tools::Stub* lower1;
        tools::Stub* lower2;
    };

}
}
}


#endif // NOT defined WNS_LDK_MULTIPLEXER_FRAMEDISPATCHER_TEST_HPP


