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

#include <IMTAPHY/link2System/MMIBeffectiveSINR.hpp>
#include <IMTAPHY/link2System/Modulations.hpp>


namespace imtaphy { namespace l2s { namespace tests {
    
    
        class MMIBeffectiveSINRTest :
            public CppUnit::TestFixture
                {
                
                    CPPUNIT_TEST_SUITE( MMIBeffectiveSINRTest );
                    CPPUNIT_TEST( testForwardReverse );
                    CPPUNIT_TEST( testEffectiveSINR );
                    CPPUNIT_TEST_SUITE_END();

                public:

                    void setUp();
                    void tearDown();
                    void testForwardReverse();
                    void testEffectiveSINR();
                    
        
                private:
                };
    
                CPPUNIT_TEST_SUITE_REGISTRATION( MMIBeffectiveSINRTest);
                 


                void
                MMIBeffectiveSINRTest::setUp()
                {       
    
                }

                void 
                MMIBeffectiveSINRTest::tearDown()
                {
    
                }

                void 
                MMIBeffectiveSINRTest::testForwardReverse()
                {
                    MMIBeffectiveSINR mapper;

                    ModulationScheme modulation;
                    modulation = QPSK();
                    
                    
                    for (int sinr = -1000; sinr < 950; sinr++)
                    {
                        double x = pow(10.0, sinr / 100.0 / 10.0);
                        double y = mapper.forwardMapping(wns::Ratio::from_factor(x), modulation);
                      /*  std::cout << "QPSK with " << sinr / 100.0 << "dB gives " 
                                  <<  y
                                  << " MMIB gives " << mapper.reverseMapping(y, modulation) << "\n";
                      */            
                        CPPUNIT_ASSERT_DOUBLES_EQUAL(wns::Ratio::from_factor(x).get_dB(), mapper.reverseMapping(y, modulation).get_dB(), 0.11);
                    }
                    
                    modulation = QAM16();
                    
                    for (int sinr = -300; sinr < 2300; sinr++)
                    {
                        double x = pow(10.0, sinr / 100.0 / 10.0);
                        double y = mapper.forwardMapping(wns::Ratio::from_factor(x), modulation);
/*                        std::cout << "QAM16 with " << sinr / 100.0 << "dB gives " 
                                  <<  y
                                  << " MMIB gives " << mapper.reverseMapping(y, modulation) << "\n";*/
                                  
                        CPPUNIT_ASSERT_DOUBLES_EQUAL(wns::Ratio::from_factor(x).get_dB(), mapper.reverseMapping(y, modulation).get_dB(), 0.1);
                    }
                    
                    modulation = QAM64();
                    for (int sinr = -100; sinr < 2300; sinr++)
                    {
                        double x = pow(10.0, sinr / 100.0 / 10.0);
                        double y = mapper.forwardMapping(wns::Ratio::from_factor(x), modulation);
//                         std::cout << "QAM64 with " << sinr / 100.0 << "dB gives " 
//                                   <<  y
//                                   << " MMIB gives " << mapper.reverseMapping(y, modulation) << "\n";

                        CPPUNIT_ASSERT_DOUBLES_EQUAL(wns::Ratio::from_factor(x).get_dB(), mapper.reverseMapping(y, modulation).get_dB(), 0.1);
                    }
                }


                void
                MMIBeffectiveSINRTest::testEffectiveSINR()
                {
                    // TODO: make this more extensive...
                    
                    MMIBeffectiveSINR mapper;
                    
                    std::vector<wns::Ratio> sinrsHigh(3);
                    sinrsHigh[0] = wns::Ratio::from_dB(30.0);
                    sinrsHigh[1] = wns::Ratio::from_dB(31.0);
                    sinrsHigh[2] = wns::Ratio::from_dB(32.0);
                    
                    std::vector<wns::Ratio> sinrsLow(3);
                    sinrsLow[0] = wns::Ratio::from_dB(-30.0);
                    sinrsLow[1] = wns::Ratio::from_dB(-31.0);
                    sinrsLow[2] = wns::Ratio::from_dB(-32.0);
                    
                    CPPUNIT_ASSERT_DOUBLES_EQUAL(mapper.getEffectiveSINR(sinrsLow, imtaphy::l2s::QAM64()).get_dB(), -15, 0.1);
                    CPPUNIT_ASSERT_DOUBLES_EQUAL(mapper.getEffectiveSINR(sinrsLow, imtaphy::l2s::QAM16()).get_dB(), -15, 0.1);
                    CPPUNIT_ASSERT_DOUBLES_EQUAL(mapper.getEffectiveSINR(sinrsLow, imtaphy::l2s::QPSK()).get_dB(), -15, 0.1);

                    CPPUNIT_ASSERT_DOUBLES_EQUAL(mapper.getEffectiveSINR(sinrsHigh, imtaphy::l2s::QAM64()).get_dB(), 25, 0.1);
                    CPPUNIT_ASSERT_DOUBLES_EQUAL(mapper.getEffectiveSINR(sinrsHigh, imtaphy::l2s::QAM16()).get_dB(), 25, 0.1);
                    CPPUNIT_ASSERT_DOUBLES_EQUAL(mapper.getEffectiveSINR(sinrsHigh, imtaphy::l2s::QPSK()).get_dB(), 25, 0.1);

                }
}}}

