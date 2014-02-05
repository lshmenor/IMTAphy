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

#ifndef WNS_LDK_BUFFER_BOUNDED_TEST_HPP
#define WNS_LDK_BUFFER_BOUNDED_TEST_HPP

#include <WNS/ldk/buffer/Bounded.hpp>
#include <WNS/ldk/tools/Stub.hpp>
#include <WNS/Exception.hpp>

#include <WNS/pyconfig/Parser.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace wns { namespace ldk { namespace buffer {

    class BoundedTest : public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE( BoundedTest );
        CPPUNIT_TEST( testFill );
        CPPUNIT_TEST( testWakeup );
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp();
        void tearDown();

        void testFill();
        void testWakeup();
    private:
        ILayer* layer;
        fun::FUN* fuNet;

        tools::Stub* upper;
        Bounded* buffer;
        tools::Stub* lower;
    };

    class BoundedBitTest : public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE( BoundedBitTest );
        CPPUNIT_TEST( testFill );
        CPPUNIT_TEST( testEmpty );
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp();
        void tearDown();

        void testFill();
        void testEmpty();
    private:
        ILayer* layer;
        fun::FUN* fuNet;

        tools::Stub* upper;
        Bounded* buffer;
        tools::Stub* lower;
    };

}
}
}


#endif // NOT defined WNS_LDK_BUFFER_BOUNDED_TEST_HPP


