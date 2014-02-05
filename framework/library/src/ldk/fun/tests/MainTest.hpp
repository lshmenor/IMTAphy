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

#ifndef WNS_LDK_FUN_TEST_MAIN_HPP
#define WNS_LDK_FUN_TEST_MAIN_HPP

#include <WNS/ldk/fun/tests/FUNTest.hpp>

#include <WNS/ldk/fun/Main.hpp>
#include <cppunit/extensions/HelperMacros.h>
#include <stdexcept>

namespace wns { namespace ldk { namespace fun {

    class MainFUNInterfaceTest :
        public FUNTest
    {
        CPPUNIT_TEST_SUB_SUITE( MainFUNInterfaceTest, FUNTest );
        CPPUNIT_TEST( testReconfigureFUN );
        CPPUNIT_TEST_SUITE_END();

    public:
        void testReconfigureFUN();

    protected:
        virtual wns::ldk::fun::FUN*
        newCandidate(wns::ldk::ILayer* layer)
        {
            return new Main(layer);
        } // newCandidate

        virtual void
        deleteCandidate(FUN* fun)
        {
            delete fun;
        } // deleteCandidate
    };

}
}
}

#endif // NOT defined WNS_LDK_FUN_TEST_MAIN_HPP


