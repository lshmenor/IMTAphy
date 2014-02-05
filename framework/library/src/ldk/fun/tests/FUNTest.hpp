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

#ifndef WNS_LDK_FUN_TEST_FUN_HPP
#define WNS_LDK_FUN_TEST_FUN_HPP

#include <WNS/ldk/fun/FUN.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <stdexcept>

/*
 * abstract test for the FUN interface.
 *
 * this is an abstract test, being not registered at the test suite directly.
 * it tests against the semantics of an abstract interface. every implementation of that
 * abstract interface has to stand these tests.
 *
 * classes deriving vom FUN (Main and Sub) use this abstract test by
 * implementing getFUN(...).
 *
 */

namespace wns { namespace ldk { namespace fun {

    class FUNTest : public CppUnit::TestFixture 
    {
        CPPUNIT_TEST_SUITE( FUNTest );
        CPPUNIT_TEST( testLayerDelegation );
        CPPUNIT_TEST( testAdd );
        CPPUNIT_TEST_EXCEPTION( testAddAlready1, wns::Exception );
#ifdef WNS_ASSURE_THROWS_EXCEPTION
        CPPUNIT_TEST_EXCEPTION( testAddAlready2, wns::Assure::Exception );
#endif // WNS_ASSURE_THROWS_EXCEPTION
        CPPUNIT_TEST( testConnect );
        CPPUNIT_TEST_EXCEPTION( testUnknownConnect1, wns::Exception );
        CPPUNIT_TEST_EXCEPTION( testUnknownConnect2, wns::Exception );
        CPPUNIT_TEST( testUpConnect );
        CPPUNIT_TEST( testDownConnect );
        CPPUNIT_TEST( testFindFriend );
        CPPUNIT_TEST_EXCEPTION( testFindFriendUnknown, wns::Exception );
        CPPUNIT_TEST( testFindFriendBad );
        CPPUNIT_TEST_SUITE_END_ABSTRACT();

    public:
        void setUp();
        void tearDown();

        void testLayerDelegation();
        void testAdd();
        void testAddAlready1();
        void testAddAlready2();
        void testConnect();
        void testUnknownConnect1();
        void testUnknownConnect2();
        void testUpConnect();
        void testDownConnect();
        void testFindFriend();
        void testFindFriendUnknown();
        void testFindFriendBad();

        wns::ldk::fun::FUN*
        getFUN();

    protected:
        virtual wns::ldk::fun::FUN* newCandidate(wns::ldk::ILayer* layer) = 0;
        virtual void deleteCandidate(wns::ldk::fun::FUN* fun) = 0;

        FunctionalUnit* fu1;
        FunctionalUnit* fu2;

    private:
        ILayer* layer;
        wns::ldk::fun::FUN* fuNet;
    };

}
}
}

#endif // NOT defined WNS_LDK_FUN_TEST_FUN_HPP


