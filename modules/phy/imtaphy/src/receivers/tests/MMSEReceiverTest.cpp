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
    
    
        class MMSEReceiverTest :
            public CppUnit::TestFixture
                {
                
                    CPPUNIT_TEST_SUITE( MMSEReceiverTest );
                    CPPUNIT_TEST( testType1ComputeFilter4U4M );
                    CPPUNIT_TEST( testType1ComputeFilter2U1M );
                    CPPUNIT_TEST( testType1ComputeFilter4U1M );
                    CPPUNIT_TEST( testType1ComputeFilter4U2M );
                    CPPUNIT_TEST( testType1ComputeFilter1U1M );
                    CPPUNIT_TEST_SUITE_END();

                    typedef boost::multi_array_ref<std::complex<float>, 2> MatrixType;
                    
                public:

                    void setUp();
                    void tearDown();
                    void testType1ComputeFilter4U4M();
                    void testType1ComputeFilter2U1M();
                    void testType1ComputeFilter4U1M();
                    void testType1ComputeFilter4U2M();
                    void testType1ComputeFilter1U1M();
                    
        
                private:
                    imtaphy::tests::ChannelStub* channel;
                    imtaphy::tests::StationPhyStub *servingBS, *interferingBS1, *interferingBS2, *interferingBS3, *ms;
                    wns::node::Registry* registry;
                    wns::node::Interface* node;
                    imtaphy::LinkManagerStub* linkManager;
                    double lambda;
                    wns::pyconfig::Parser all_config;
                
                };
    
                CPPUNIT_TEST_SUITE_REGISTRATION( MMSEReceiverTest);
                 


                void
                MMSEReceiverTest::setUp()
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
                                 << "receiver = imtaphy.Receiver.SimpleMMSE(logger, numInterferersInDetail = 0)\n";
                                 
                    
                    all_config.loadString(ss_config.str());
                    

                    
                }

                void 
                MMSEReceiverTest::tearDown()
                {
    
                }

                void
                MMSEReceiverTest::testType1ComputeFilter4U4M()
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

                    /************************* Start 4x4 4 streams at 15dB SINR ********************************************/
                    imtaphy::receivers::Interferer interferer1;
                    interferer1.interferersPrecodingMatrix = imtaphy::detail::ComplexFloatMatrixPtr(); // smart null pointers
                    wns::Power interferenceRxPower1 = wns::Power::from_dBm(-80);
                    
		     // this has to be the sum over all rx antennas:
                    interferenceRxPower1 *= 4.0;

                    // the InterferencePowerMap is a multimap sorted in descending order
                    interferers->insert(interferenceRxPower1, interferer1);
                    interferers->seal();
                    
                    int m = 4; // 4 streams for which I want to add power
                    int U = 4;     
                    
                    
                    imtaphy::detail::ComplexFloatMatrix precodedHs44(U, m);
                    imtaphy::detail::ComplexFloatMatrix receiveFilter44(U, m);
                    
                    precodedHs44[0][0] = std::complex<float>( 0.000201072808458, -0.000360385393674);
                    precodedHs44[0][1] = std::complex<float>( 0.000055464051643, 0.000136835125187);
                    precodedHs44[0][2] = std::complex<float>( -0.000276910946899, -0.000068846941759);
                    precodedHs44[0][3] = std::complex<float>( -0.000249282541744, 0.000062665452542);
                    precodedHs44[1][0] = std::complex<float>( 0.000157733590814, 0.000208395003782);
                    precodedHs44[1][1] = std::complex<float>( 0.000095865300369, -0.000337934663258);
                    precodedHs44[1][2] = std::complex<float>( 0.000064921204748, -0.000182697772545);
                    precodedHs44[1][3] = std::complex<float>( 0.000037315921233, -0.000169071824806);
                    precodedHs44[2][0] = std::complex<float>( -0.000312700787389, -0.000021366264510);
                    precodedHs44[2][1] = std::complex<float>( 0.000069282033891, -0.000006199038628);
                    precodedHs44[2][2] = std::complex<float>( 0.000102745211506, 0.000069844246701);
                    precodedHs44[2][3] = std::complex<float>( -0.000058567450740, 0.000011072028114);
                    precodedHs44[3][0] = std::complex<float>( -0.000255876148545, 0.000108820609223);
                    precodedHs44[3][1] = std::complex<float>( -0.000224746593745, -0.000130687758950);
                    precodedHs44[3][2] = std::complex<float>( -0.000143918614401, 0.000477844426276);
                    precodedHs44[3][3] = std::complex<float>( -0.000196034975800, 0.000012639489117);


                    // compute filter
                    testee->filter->computeFilter(receiveFilter44, precodedHs44, interferers, 0, servingBS, m);
                    
                    
                    imtaphy::detail::ComplexFloatMatrix receiveFilter44Matlab(U, m);
                    
                    receiveFilter44Matlab[0][0] = std::complex<float>( 769.224319015552055, 25.236497354571611);
                    receiveFilter44Matlab[0][1] = std::complex<float>( 1422.173713439024141, 188.666940757738018);
                    receiveFilter44Matlab[0][2] = std::complex<float>( -236.036807329745898, 88.728319590995170);
                    receiveFilter44Matlab[0][3] = std::complex<float>( -2185.306413016284750, 779.338400088445042);
                    receiveFilter44Matlab[1][0] = std::complex<float>( 988.719679816034613, -277.615039399782518);
                    receiveFilter44Matlab[1][1] = std::complex<float>( 1124.448775366252221, -2147.195175923749048);
                    receiveFilter44Matlab[1][2] = std::complex<float>( 1049.977269951248900, 475.367360920756084);
                    receiveFilter44Matlab[1][3] = std::complex<float>( -857.573502148593889, -53.962559440511768);
                    receiveFilter44Matlab[2][0] = std::complex<float>( -1967.243851554459070, -751.552814476623780);
                    receiveFilter44Matlab[2][1] = std::complex<float>( -135.188301748624809, -165.884817739830055);
                    receiveFilter44Matlab[2][2] = std::complex<float>( 1647.281308531932382, -1327.370814446295299);
                    receiveFilter44Matlab[2][3] = std::complex<float>( -2183.819722532142805, -795.786293027237321);
                    receiveFilter44Matlab[3][0] = std::complex<float>( 174.267808619545491, 806.924367211386652);
                    receiveFilter44Matlab[3][1] = std::complex<float>( 126.772199996846453, -25.600733993843022);
                    receiveFilter44Matlab[3][2] = std::complex<float>( -583.539149748187924, 1454.130786388442175);
                    receiveFilter44Matlab[3][3] = std::complex<float>( -827.800670942099259, -627.887730311511859);



                    float ratioReal, ratioImag;
                    float precision = 1e-02;
                    for (int j = 0; j < U; j++)
                        for (int k = 0; k < m; k++)
                        {
                            ratioReal = receiveFilter44Matlab[j][k].real() / receiveFilter44[j][k].real();
                            ratioImag = receiveFilter44Matlab[j][k].imag() / receiveFilter44[j][k].imag();
                            
                            CPPUNIT_ASSERT_DOUBLES_EQUAL(1, ratioReal, precision);
                            CPPUNIT_ASSERT_DOUBLES_EQUAL(1, ratioImag, precision);
                        }
                    /************************* End of 4x4 4 streams at 15dB SINR ********************************************/

                    // Other cases we want to test:
                    // - 1x1 (SISO)
                    // - 4x4 with rank 1
                    // - 4x2 rank 2
                    
                    
               }
               void
               MMSEReceiverTest::testType1ComputeFilter2U1M()
               {
                   // N7imtaphy9receivers5tests16MMSEReceiverTestE::testType1ComputeFilter2U1M
                    wns::pyconfig::View receiverConfig(all_config, "receiver");

                    servingBS = imtaphy::tests::createStationStub("ServingBS", wns::Position(100,100,10), "BS", 2, 10.0 * lambda, 0, registry, channel);
                    interferingBS1 = imtaphy::tests::createStationStub("InterferingBS1", wns::Position(150,150,10), "BS", 2, 10.0 * lambda, 0, registry, channel);
                    ms = imtaphy::tests::createStationStub("MS", wns::Position(50,20,1.5), "MS",2, 0.5 * lambda, 3.0, registry, channel);

                    // currently ignored, but wants it anyway
                    linkManager->setServingLink(new LinkStub(servingBS, ms, Link::UMa, Link::NLoS, Link::NLoS, imtaphy::Link::NotApplicable,
                                                             ms->getPosition(), wns::Ratio::from_dB(0.0), 0),
                                                ms
                    );
                    

                    imtaphy::receivers::LinearReceiver* testee = new imtaphy::receivers::LinearReceiver(ms, receiverConfig);
                    testee->channelInitialized(channel);

                    imtaphy::receivers::InterferersCollectionPtr interferers(new InterferersCollection);

                    /************************* Start 2x2 1 stream at 15dB SINR ********************************************/
                    imtaphy::receivers::Interferer interferer1;
                    interferer1.interferersPrecodingMatrix = imtaphy::detail::ComplexFloatMatrixPtr(); // smart null pointers
                    wns::Power interferenceRxPower1 = wns::Power::from_dBm(-80);

		     // this has to be the sum over all rx antennas:
                    interferenceRxPower1 *= 2.0;

                    interferers->insert(interferenceRxPower1, interferer1);
                    interferers->seal();
                    
                    int m = 1; // 1 stream for which I want to add power
                    int U = 2;     


                    imtaphy::detail::ComplexFloatMatrix precodedHs21(U, m);
                    imtaphy::detail::ComplexFloatMatrix receiveFilter21(U, m);

                    precodedHs21[0][0] = std::complex<float>( -0.000331560687685, 0.000350112491956);
                    precodedHs21[1][0] = std::complex<float>( -0.000328621028458, 0.000007560819241);

                    // compute filter
                    testee->filter->computeFilter(receiveFilter21, precodedHs21, interferers, 0, servingBS, m);


                    imtaphy::detail::ComplexFloatMatrix receiveFilter21Matlab(U, m);
                    receiveFilter21Matlab[0][0] = std::complex<float>( -945.802445143872205, 998.722898300033194);
                    receiveFilter21Matlab[1][0] = std::complex<float>( -937.416840372941806, 21.567820285312337);


                    
                    float ratioReal, ratioImag;
                    float precision = 1e-03;
                    for (int j = 0; j < U; j++)
                        for (int k = 0; k < m; k++)
                        {
                            ratioReal = receiveFilter21Matlab[j][k].real() / receiveFilter21[j][k].real();
                            ratioImag = receiveFilter21Matlab[j][k].imag() / receiveFilter21[j][k].imag();

                            CPPUNIT_ASSERT_DOUBLES_EQUAL(1, ratioReal, precision);
                            CPPUNIT_ASSERT_DOUBLES_EQUAL(1, ratioImag, precision);
                        }
                    }
                    
               void
               MMSEReceiverTest::testType1ComputeFilter4U1M()
               {
                   // N7imtaphy9receivers5tests16MMSEReceiverTestE::testType1ComputeFilter4U1M
                    wns::pyconfig::View receiverConfig(all_config, "receiver");

                    servingBS = imtaphy::tests::createStationStub("ServingBS", wns::Position(100,100,10), "BS", 4, 10.0 * lambda, 0, registry, channel);
                    interferingBS1 = imtaphy::tests::createStationStub("InterferingBS1", wns::Position(150,150,10), "BS", 4, 10.0 * lambda, 0, registry, channel);
                    ms = imtaphy::tests::createStationStub("MS", wns::Position(50,20,1.5), "MS", 4, 0.5 * lambda, 3.0, registry, channel);

                    // currently ignored, but wants it anyway
                    linkManager->setServingLink(new LinkStub(servingBS, ms, Link::UMa, Link::NLoS, Link::NLoS, imtaphy::Link::NotApplicable,
                                                             ms->getPosition(), wns::Ratio::from_dB(0.0), 0),
                                                 ms
                                                );
                    
                    
                    imtaphy::receivers::LinearReceiver* testee = new imtaphy::receivers::LinearReceiver(ms, receiverConfig);
                    testee->channelInitialized(channel);

                    imtaphy::receivers::InterferersCollectionPtr interferers(new InterferersCollection);

                    /************************* Start 2x2 1 stream at 15dB SINR ********************************************/
                    imtaphy::receivers::Interferer interferer1;
                    interferer1.interferersPrecodingMatrix = imtaphy::detail::ComplexFloatMatrixPtr(); // smart null pointers
                    wns::Power interferenceRxPower1 = wns::Power::from_dBm(-80);

		     // this has to be the sum over all rx antennas:
                    interferenceRxPower1 *= 4.0;

                    // the InterferencePowerMap is a multimap sorted in descending order
                    interferers->insert(interferenceRxPower1, interferer1);
                    interferers->seal();
                    
                    int m = 1; // 1 stream for which I want to add power
                    int U = 4;     

                    imtaphy::detail::ComplexFloatMatrix precodedHs41(U, m);
                    imtaphy::detail::ComplexFloatMatrix receiveFilter41(U, m);

                    precodedHs41[0][0] = std::complex<float>( 0.000402145616916, -0.000720770787348);
                    precodedHs41[1][0] = std::complex<float>( 0.000315467181629, 0.000416790007565);
                    precodedHs41[2][0] = std::complex<float>( -0.000625401574778, -0.000042732529019);
                    precodedHs41[3][0] = std::complex<float>( -0.000511752297090, 0.000217641218446);


                    // compute filter
                    testee->filter->computeFilter(receiveFilter41, precodedHs41, interferers, 0, servingBS, m);


                    imtaphy::detail::ComplexFloatMatrix receiveFilter41Matlab(U, m);

                    receiveFilter41Matlab[0][0] = std::complex<float>( 241.285962414681762, -432.459949306618000);
                    receiveFilter41Matlab[1][0] = std::complex<float>( 189.279204665127054, 250.072545540060673);
                    receiveFilter41Matlab[2][0] = std::complex<float>( -375.238755610557291, -25.639367823832799);
                    receiveFilter41Matlab[3][0] = std::complex<float>( -307.049586833748037, 130.583969201316904);


                    
                    float ratioReal, ratioImag;
                    float precision = 1e-03;
                    for (int j = 0; j < U; j++)
                        for (int k = 0; k < m; k++)
                        {
                            ratioReal = receiveFilter41Matlab[j][k].real() / receiveFilter41[j][k].real();
                            ratioImag = receiveFilter41Matlab[j][k].imag() / receiveFilter41[j][k].imag();

                            CPPUNIT_ASSERT_DOUBLES_EQUAL(1, ratioReal, precision);
                            CPPUNIT_ASSERT_DOUBLES_EQUAL(1, ratioImag, precision);
                        }

            }
            
            void
            MMSEReceiverTest::testType1ComputeFilter4U2M()
            {
                // N7imtaphy9receivers5tests16MMSEReceiverTestE::testType1ComputeFilter4U2M
                wns::pyconfig::View receiverConfig(all_config, "receiver");

                servingBS = imtaphy::tests::createStationStub("ServingBS", wns::Position(100,100,10), "BS", 4, 10.0 * lambda, 0, registry, channel);
                interferingBS1 = imtaphy::tests::createStationStub("InterferingBS1", wns::Position(150,150,10), "BS", 4, 10.0 * lambda, 0, registry, channel);
                ms = imtaphy::tests::createStationStub("MS", wns::Position(50,20,1.5), "MS", 4, 0.5 * lambda, 3.0, registry, channel);

                // currently ignored, but wants it anyway
                linkManager->setServingLink(new LinkStub(servingBS, ms, Link::UMa, Link::NLoS, Link::NLoS, imtaphy::Link::NotApplicable,
                                                         ms->getPosition(), wns::Ratio::from_dB(0.0), 0),
                                            ms
                                            );
                

                imtaphy::receivers::LinearReceiver* testee = new imtaphy::receivers::LinearReceiver(ms, receiverConfig);
                testee->channelInitialized(channel);

                imtaphy::receivers::InterferersCollectionPtr interferers(new InterferersCollection);

                /************************* Start 4x4 2 streams at 15dB SINR ********************************************/
                imtaphy::receivers::Interferer interferer1;
                interferer1.interferersPrecodingMatrix = imtaphy::detail::ComplexFloatMatrixPtr(); // smart null pointers
                wns::Power interferenceRxPower1 = wns::Power::from_dBm(-80);

		 // this has to be the sum over all rx antennas:
		interferenceRxPower1 *= 4.0;

                // the InterferencePowerMap is a multimap sorted in descending order
                interferers->insert(interferenceRxPower1, interferer1);
                interferers->seal();
                
                int m = 2; // 2 stream for which I want to add power
                int U = 4;     

                imtaphy::detail::ComplexFloatMatrix precodedHs42(U, m);
                imtaphy::detail::ComplexFloatMatrix receiveFilter42(U, m);

                precodedHs42[0][0] = std::complex<float>( 0.000284359892746, -0.000509661911415);
                precodedHs42[0][1] = std::complex<float>( -0.000352538751397, 0.000088622332877);
                precodedHs42[1][0] = std::complex<float>( 0.000223068983371, 0.000294715040680);
                precodedHs42[1][1] = std::complex<float>( 0.000052772681900, -0.000239103667656);
                precodedHs42[2][0] = std::complex<float>( -0.000442225694490, -0.000030216461047);
                precodedHs42[2][1] = std::complex<float>( -0.000082826883151, 0.000015658212322);
                precodedHs42[3][0] = std::complex<float>( -0.000361863519560, 0.000153895581429);
                precodedHs42[3][1] = std::complex<float>( -0.000277235321476, 0.000017874936930);


                // compute filter
                testee->filter->computeFilter(receiveFilter42, precodedHs42, interferers, 0, servingBS, m);

                imtaphy::detail::ComplexFloatMatrix receiveFilter42Matlab(U, m);

                receiveFilter42Matlab[0][0] = std::complex<float>( 385.266563748594422, -359.485109060943387);
                receiveFilter42Matlab[0][1] = std::complex<float>( -896.957743972151661, 492.493472853294861);
                receiveFilter42Matlab[1][0] = std::complex<float>( 102.927247729014198, 296.676192623227507);
                receiveFilter42Matlab[1][1] = std::complex<float>( 3.931926663330074, -697.049788624102121);
                receiveFilter42Matlab[2][0] = std::complex<float>( -653.777802242851521, 43.838816119169678);
                receiveFilter42Matlab[2][1] = std::complex<float>( -467.441818162253298, -384.201335979883879);
                receiveFilter42Matlab[3][0] = std::complex<float>( -598.835600663375885, 505.699623088908993);
                receiveFilter42Matlab[3][1] = std::complex<float>( -1450.909572480023144, -234.101394662269968);


                
                float ratioReal, ratioImag;
                float precision = 1e-03;
                for (int j = 0; j < U; j++)
                    for (int k = 0; k < m; k++)
                    {
                        ratioReal = receiveFilter42Matlab[j][k].real() / receiveFilter42[j][k].real();
                        ratioImag = receiveFilter42Matlab[j][k].imag() / receiveFilter42[j][k].imag();

                        CPPUNIT_ASSERT_DOUBLES_EQUAL(1, ratioReal, precision);
                        CPPUNIT_ASSERT_DOUBLES_EQUAL(1, ratioImag, precision);
                    }

            }
        
            void
            MMSEReceiverTest::testType1ComputeFilter1U1M()
            {
                // N7imtaphy9receivers5tests16MMSEReceiverTestE::testType1ComputeFilter1U1M
                wns::pyconfig::View receiverConfig(all_config, "receiver");

                servingBS = imtaphy::tests::createStationStub("ServingBS", wns::Position(100,100,10), "BS", 1, 10.0 * lambda, 0, registry, channel);
                interferingBS1 = imtaphy::tests::createStationStub("InterferingBS1", wns::Position(150,150,10), "BS", 1, 10.0 * lambda, 0, registry, channel);
                ms = imtaphy::tests::createStationStub("MS", wns::Position(50,20,1.5), "MS", 1, 0.5 * lambda, 3.0, registry, channel);

                // currently ignored, but wants it anyway
                linkManager->setServingLink(new LinkStub(servingBS, ms, Link::UMa, Link::NLoS, Link::NLoS, imtaphy::Link::NotApplicable,
                                                         ms->getPosition(), wns::Ratio::from_dB(0.0), 0),
                                            ms
                                            );
                

                imtaphy::receivers::LinearReceiver* testee = new imtaphy::receivers::LinearReceiver(ms, receiverConfig);
                testee->channelInitialized(channel);

                imtaphy::receivers::InterferersCollectionPtr interferers(new InterferersCollection);

                /************************* Start 1x1 1 stream at 15dB SINR ********************************************/
                imtaphy::receivers::Interferer interferer1;
                interferer1.interferersPrecodingMatrix = imtaphy::detail::ComplexFloatMatrixPtr(); // smart null pointers
                wns::Power interferenceRxPower1 = wns::Power::from_dBm(-80);

                interferers->insert(interferenceRxPower1, interferer1);
                interferers->seal();
                
                int m = 1; // 2 stream for which I want to add power
                int U = 1;     


                imtaphy::detail::ComplexFloatMatrix precodedHs11(U, m);
                imtaphy::detail::ComplexFloatMatrix receiveFilter11(U, m);

                precodedHs11[0][0] = std::complex<float>( -0.000269656628542, 0.000355836017164);


                // compute filter
                testee->filter->computeFilter(receiveFilter11, precodedHs11, interferers, 0, servingBS, m);


                imtaphy::detail::ComplexFloatMatrix receiveFilter11Matlab(U, m);
                receiveFilter11Matlab[0][0] = std::complex<float>( -1288.164699531490442, 1699.848428025812609);
                
                float ratioReal, ratioImag;
                float precision = 1e-03;
                for (int j = 0; j < U; j++)
                    for (int k = 0; k < m; k++)
                    {
                        ratioReal = receiveFilter11Matlab[j][k].real() / receiveFilter11[j][k].real();
                        ratioImag = receiveFilter11Matlab[j][k].imag() / receiveFilter11[j][k].imag();

                        CPPUNIT_ASSERT_DOUBLES_EQUAL(1, ratioReal, precision);
                        CPPUNIT_ASSERT_DOUBLES_EQUAL(1, ratioImag, precision);
                    }

        }
}}}

