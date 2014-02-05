/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 16, D-52074 Aachen, Germany
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

#include <WNS/TestFixture.hpp>
#include <WNS/Assure.hpp>

// PLACE THIS AFTER ALL INCLUDES
#ifndef xx_ASSURE
#define xx_ASSURE WE_NEED_TO_DEFINE_THIS_HERE_TO_MAKE_THE_TEST_WORK_IN_OPT_MODE
#endif


namespace wns {

    class AssureTest :
        public wns::TestFixture
    {
        CPPUNIT_TEST_SUITE( AssureTest );
        CPPUNIT_TEST( except );
        CPPUNIT_TEST( exceptionContent );
        CPPUNIT_TEST_SUITE_END();
    public:
        void prepare()
        {
        }

        void cleanup()
        {
        }

        void
        except()
        {
            WNS_ASSERT_ASSURE_EXCEPTION(assure(false, "This is a test"));
        }

        void
        exceptionContent()
        {
            int lineNumber = 0;
            std::string fileName = __FILE__;
            try
            {
                lineNumber = __LINE__ + 1;
                assure(false, "This is a test.");
            }
            catch (const Exception& e)
            {
                std::stringstream expectedMessage;
                expectedMessage << "In 'void wns::AssureTest::exceptionContent()' ("
                                << fileName << ":" << lineNumber << "):\n"
                                << "failed assertion (false).\nReason: This is a test.\n";
                CPPUNIT_ASSERT_MESSAGE( std::string(e.what()) + "vs. expectedMessage:\n" + expectedMessage.str(),
                                        std::string(e.what()) == expectedMessage.str());
            }
        }
    };

    CPPUNIT_TEST_SUITE_REGISTRATION( AssureTest );
}
