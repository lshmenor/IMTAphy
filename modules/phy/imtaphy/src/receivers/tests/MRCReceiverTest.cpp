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

// By default, the array access methods operator() and operator[] perform range checking. If a supplied index is out of the range defined for an array, an assertion will abort the program. To disable range checking (for performance reasons in production releases), define the BOOST_DISABLE_ASSERTS preprocessor macro prior to including multi_array.hpp in an application.

// For opt versions disable range checking in the multi_array wrapper
// WNS_NDEBUG means: assures are disabled
#ifdef  WNS_NDEBUG
#define BOOST_DISABLE_ASSERTS
#endif
#include <boost/multi_array.hpp>

#include <itpp/itbase.h>
#include <algorithm>

#include <WNS/pyconfig/Parser.hpp>

#include <IMTAPHY/receivers/Interferer.hpp>
#include <IMTAPHY/Transmission.hpp>
#include <IMTAPHY/Link.hpp>

#include <IMTAPHY/receivers/LinearReceiver.hpp>
#include <IMTAPHY/tests/StationPhyStub.hpp>

#include <IMTAPHY/Spectrum.hpp>
#include <IMTAPHY/tests/ChannelStub.hpp>
#include <IMTAPHY/linkManagement/LinkManager.hpp>

namespace imtaphy { namespace receivers { namespace tests {
    
    
        class MRCReceiverTest :
            public CppUnit::TestFixture
                {
                
                    CPPUNIT_TEST_SUITE( MRCReceiverTest );
                    CPPUNIT_TEST( testMRCComputeFilter4U1M );
                    CPPUNIT_TEST_SUITE_END();

                    typedef boost::multi_array_ref<std::complex<float>, 2> MatrixType;
                    
                public:

                    void setUp();
                    void tearDown();
                    void testMRCComputeFilter4U1M();
                    
        
                private:
                    imtaphy::tests::ChannelStub* channel;
                    imtaphy::tests::StationPhyStub *servingBS, *interferingBS1, *interferingBS2, *interferingBS3, *ms;
                    wns::node::Registry* registry;
                    wns::node::Interface* node;
                    imtaphy::LinkManagerStub* linkManager;
                    double lambda;
                    wns::pyconfig::Parser all_config;
                
                };
    
                CPPUNIT_TEST_SUITE_REGISTRATION( MRCReceiverTest);
                 


                void
                MRCReceiverTest::setUp()
                {          
                    imtaphy::Spectrum* spectrum = new imtaphy::Spectrum(2E09, 180000.0, 100, 0);
    
                    channel = new imtaphy::tests::ChannelStub();
                    channel->setSpectrum(spectrum);
                    
                    // TODO:  add a spatial channel model stub to the channel to return channel matrices
                    
    
                    linkManager = new imtaphy::LinkManagerStub();
                    channel->setLinkManager(linkManager);
                                        
                    lambda = spectrum->getSystemCenterFrequencyWavelenghtMeters(imtaphy::Downlink);
    
                    registry = new wns::node::Registry();
    

                    
       
                    std::stringstream ss_config;
                    ss_config << "import imtaphy.Receiver\n"
                                 << "import imtaphy.Logger\n"
                                 << "logger = imtaphy.Logger.Logger(\"LinearReceiverForUnitTesting\")\n"
                                 << "receiver = imtaphy.Receiver.LinearReceiver(logger, filter = imtaphy.Receiver.MRCFilter())\n";
                         
                    all_config.loadString(ss_config.str());
                   
                }

                void 
                MRCReceiverTest::tearDown()
                {
    
                }

                void
                MRCReceiverTest::testMRCComputeFilter4U1M()
                {
                    wns::pyconfig::View receiverConfig(all_config, "receiver");

                    servingBS = imtaphy::tests::createStationStub("ServingBS", wns::Position(100,100,10), "BS", 4, 10.0 * lambda, 0, registry, channel);
                    interferingBS1 = imtaphy::tests::createStationStub("InterferingBS1", wns::Position(150,150,10), "BS", 4, 10.0 * lambda, 0, registry, channel);
                    ms = imtaphy::tests::createStationStub("MS", wns::Position(50,20,1.5), "MS",4, 0.5 * lambda, 3.0, registry, channel);

                    // currently ignored, but wants it anyway
                    linkManager->setServingLink(new LinkStub(servingBS, ms, Link::UMa, Link::NLoS, Link::NLoS, imtaphy::Link::NotApplicable,
                                                             ms->getPosition(), wns::Ratio::from_dB(0.0), 0),
                                                ms
                    );
                    
                    
                    imtaphy::receivers::LinearReceiver* testee = new imtaphy::receivers::LinearReceiver(ms, receiverConfig);
                    testee->channelInitialized(channel);

                    imtaphy::receivers::InterferersCollectionPtr interferers(new InterferersCollection);
                    interferers->seal();

                    /************************* Start 4x4 1 streams  ********************************************/
                    
                    
                    int m = 1; // 4 streams for which I want to add power
                    int U = 4;     
                    
                    imtaphy::detail::ComplexFloatMatrix precodedHs41(U, m);
                    imtaphy::detail::ComplexFloatMatrix receiveFilter41(U, m);
                    
                    precodedHs41[0][0] = std::complex<float>( -0.000267988162746, 0.000504247171116);
                    precodedHs41[1][0] = std::complex<float>( 0.000302044060054, -0.000448457108928);
                    precodedHs41[2][0] = std::complex<float>( 0.000027985649893, 0.000132722474118);
                    precodedHs41[3][0] = std::complex<float>( -0.000277454321479, -0.000146759887778);

                    // compute filter
                   
                    testee->filter->computeFilter(receiveFilter41, precodedHs41, interferers, 0, servingBS,  m);
                    
                    float ratioReal, ratioImag;
                    float precision = 1e-03;
                    for (int j = 0; j < U; j++)
                        for (int k = 0; k < m; k++)
                        {
                            ratioReal = precodedHs41[j][k].real() / receiveFilter41[j][k].real();
                            ratioImag = precodedHs41[j][k].imag() / receiveFilter41[j][k].imag();
                            
                            CPPUNIT_ASSERT_DOUBLES_EQUAL(1, ratioReal, precision);
                            CPPUNIT_ASSERT_DOUBLES_EQUAL(1, ratioImag, precision);
                        }
                    
               }

}}}

