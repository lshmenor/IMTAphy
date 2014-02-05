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

#ifndef WNS_LDK_FUN_TEST_SUB_HPP
#define WNS_LDK_FUN_TEST_SUB_HPP

#include <WNS/ldk/fun/tests/FUNTest.hpp>

#include <WNS/ldk/tools/Stub.hpp>

#include <WNS/ldk/fun/Sub.hpp>
#include <WNS/ldk/fun/Main.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <stdexcept>

namespace wns { namespace ldk { namespace fun {

    class SubFUNInterfaceTest :
        public FUNTest
    {
        CPPUNIT_TEST_SUB_SUITE( SubFUNInterfaceTest, FUNTest );
        CPPUNIT_TEST_SUITE_END();

    protected:
        virtual FUN*
        newCandidate(ILayer* layer)
        {
            return new Sub(new Main(layer));
        } // newCandidate

        virtual void
        deleteCandidate(FUN* fun)
        {
            Sub* sub = dynamic_cast<Sub*>(fun);

            delete sub->getParent();
            delete sub;
        } // deleteCandidate
    };


    class SubTest : public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE( SubTest );
        CPPUNIT_TEST( testFindParent );
        /*
         * This test has been disabled due to changes in the meaning of FU
         * names. Now, the names of FUs are role names and hence have to be
         * unique throughout a FUN including SubFUNs.
         *
         * But anyway, to introduce a shadowing-like behavior the role names of
         * FUs inside a SubFUN could be prepended by a SubFUN name. Currently,
         * SubFUNs do not have a name.
         *
         * @todo ksw,msg
         * - Add support for SubFUN names
         * - Add automatic SubFUN name prepending
         */
//      CPPUNIT_TEST( testShadowParent );
        CPPUNIT_TEST( testCloneFUs );
        CPPUNIT_TEST( testCloneConnectionsExist );
        CPPUNIT_TEST( testCloneConnections );
        CPPUNIT_TEST( testCommand );
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp();
        void tearDown();

        void testFindParent();
//      void testShadowParent();
        void testCloneFUs();
        void testCloneConnectionsExist();
        void testCloneConnections();
        void testCommand();
    private:
        ILayer* layer;
        Main* mainNet;
        Sub* subNet;

        tools::Stub* mainFU;
        tools::Stub* subFU;
        tools::Stub* otherSubFU;
    };

}
}
}



#endif // NOT defined WNS_LDK_FUN_TEST_SUB_HPP


