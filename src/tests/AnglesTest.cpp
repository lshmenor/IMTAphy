/*******************************************************************************
 * This file is part of IMTAphy
 * _____________________________________________________________________________
 *
 * Copyright (C) 2010
 * Institute of Communication Networks (LKN)
 * Department of Electrical Engineering and Information Technology (EE & IT)
 * Technische Universitaet Muenchen
 * Arcisstr. 21
 * 80333 Muenchen - Germany
 * http://www.lkn.ei.tum.de/~jan/imtaphy/index.html
 * 
 * _____________________________________________________________________________
 *
 *   IMTAphy is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   IMTAphy is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with IMTAphy.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#include <WNS/CppUnit.hpp>
#include <cppunit/extensions/HelperMacros.h>
#include <WNS/Position.hpp>
#include <IMTAPHY/tests/ChannelStub.hpp>
#include <itpp/base/math/misc.h>

namespace imtaphy { namespace tests {
        class AnglesTest :
            public CppUnit::TestFixture
        {
            CPPUNIT_TEST_SUITE( AnglesTest );
            CPPUNIT_TEST ( testAzimuths );
            CPPUNIT_TEST ( testElevations );
            CPPUNIT_TEST_SUITE_END();
        
        public:
            void setUp();
            void tearDown();
        
            void testAzimuths();
            void testElevations();
        private:
            ChannelStub* channel;
        };

        CPPUNIT_TEST_SUITE_REGISTRATION( AnglesTest );
    
        void
        AnglesTest::setUp()
        {
            // The angles computation code is in the Channel base class but is 
            // inherited by the ChannelStub which does not overload the relevant methods
            channel = new ChannelStub();
        }

        void
        AnglesTest::tearDown()
        {
            delete channel; 
        }
    
        void
        AnglesTest::testAzimuths()
        {
            // check azimuth angles (in x-y plane, origin from northern, positive x-axis
            // positive angles clockwise, azimuth angle from -Pi..Pi) from (2,2,2)
            // looking into certain directions
    
    
            CPPUNIT_ASSERT_DOUBLES_EQUAL(45.0 * itpp::pi / 180.0,
                                         channel->getAzimuth(wns::Position(2,2,2),
                                                             wns::Position(3,3,0)),
                                         0.01);
    

            CPPUNIT_ASSERT_DOUBLES_EQUAL(63.44 * itpp::pi / 180.0,
                                         channel->getAzimuth(wns::Position(2,2,2),
                                                             wns::Position(4,3,0)),
                                         0.01);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(26.56 * itpp::pi / 180.0,
                                         channel->getAzimuth(wns::Position(2,2,2),
                                                             wns::Position(2.5, 3, 0)),
                                         0.01);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0 * itpp::pi / 180.0,
                                         channel->getAzimuth(wns::Position(2,2,2),
                                                             wns::Position(2,3,0)),
                                         0.01);
    
            CPPUNIT_ASSERT_DOUBLES_EQUAL(-45.0 * itpp::pi / 180.0,
                                         channel->getAzimuth(wns::Position(2,2,2),
                                                             wns::Position(1,3,0)),
                                         0.01);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(-90.0 * itpp::pi / 180.0,
                                         channel->getAzimuth(wns::Position(2,2,2),
                                                             wns::Position(1,2,0)),
                                         0.01);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(-135.0 * itpp::pi / 180.0,
                                         channel->getAzimuth(wns::Position(2,2,2),
                                                             wns::Position(1, 1, 0)),
                                         0.01);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(180.0 * itpp::pi / 180.0,
                                         channel->getAzimuth(wns::Position(2,2,2),
                                                             wns::Position(2, 1, 0)),
                                         0.01);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(135.0 * itpp::pi / 180.0,
                                         channel->getAzimuth(wns::Position(2,2,2),
                                                             wns::Position(3, 1, 0)),
                                         0.01);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(90.0 * itpp::pi / 180.0,
                                         channel->getAzimuth(wns::Position(2,2,2),
                                                             wns::Position(3,2,0)),
                                         0.01);


        }
    
        void
        AnglesTest::testElevations()
        {
            // other station is east of me
    
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0 * itpp::pi / 180.0,
                                         channel->getElevation(wns::Position(0,0,1),
                                                               wns::Position(1,0,1)),
                                         0.01);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(45.0 * itpp::pi / 180.0,
                                         channel->getElevation(wns::Position(0,0,1),
                                                               wns::Position(1,0,0)),
                                         0.01);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(-45.0 * itpp::pi / 180.0,
                                         channel->getElevation(wns::Position(0,0,1),
                                                               wns::Position(1,0,2)),
                                         0.01);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(90.0 * itpp::pi / 180.0,
                                         channel->getElevation(wns::Position(0,0,1),
                                                               wns::Position(0,0,0)),
                                         0.01);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(-90.0 * itpp::pi / 180.0,
                                         channel->getElevation(wns::Position(0,0,1),
                                                               wns::Position(0,0,2)),
                                         0.01);

            // other station now west of me: should be the same angles


            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0 * itpp::pi / 180.0,
                                         channel->getElevation(wns::Position(2,0,1),
                                                               wns::Position(1,0,1)),
                                         0.01);
    
            CPPUNIT_ASSERT_DOUBLES_EQUAL(45.0 * itpp::pi / 180.0,
                                         channel->getElevation(wns::Position(2,0,1),
                                                               wns::Position(1,0,0)),
                                         0.01);
    
            CPPUNIT_ASSERT_DOUBLES_EQUAL(-45.0 * itpp::pi / 180.0,
                                         channel->getElevation(wns::Position(2,0,1),
                                                               wns::Position(1,0,2)),
                                         0.01);
    
            CPPUNIT_ASSERT_DOUBLES_EQUAL(90.0 * itpp::pi / 180.0,
                                         channel->getElevation(wns::Position(2,0,1),
                                                               wns::Position(2,0,0)),
                                         0.01);
    
            CPPUNIT_ASSERT_DOUBLES_EQUAL(-90.0 * itpp::pi / 180.0,
                                         channel->getElevation(wns::Position(2,0,1),
                                                               wns::Position(2,0,2)),
                                         0.01);

        }
    
    }}
