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
    
        class LinearReceiverTest;
    
        class LinearReceiverForUnitTesting :
            public LinearReceiver
        {
        public:
            LinearReceiverForUnitTesting(StationPhy* station, const wns::pyconfig::View& pyConfigView) :
                LinearReceiver(station, pyConfigView)
                {};
            void channelInitialized(Channel* _channel) {channel = _channel;}
            friend class LinearReceiverTest;
        };
    
    
        class LinearReceiverTest :
            public CppUnit::TestFixture
                {
                
                    CPPUNIT_TEST_SUITE( LinearReceiverTest );
                    CPPUNIT_TEST( testComputeSINR );
                    CPPUNIT_TEST_SUITE_END();

                    typedef boost::multi_array_ref<std::complex<float>, 2> MatrixType;
                    
                public:

                    void setUp();
                    void tearDown();
                    void testComputeSINR();
                    
        
                private:
                    imtaphy::tests::ChannelStub* channel;
                    imtaphy::tests::StationPhyStub *servingBS, *interferingBS1, *interferingBS2, *interferingBS3, *ms;
                    wns::node::Registry* registry;
                    wns::node::Interface* node;
                    imtaphy::LinkManagerStub* linkManager;
                    double lambda;
                    wns::pyconfig::Parser all_config;
                
                };
    
                CPPUNIT_TEST_SUITE_REGISTRATION( LinearReceiverTest);
                 


                void
                LinearReceiverTest::setUp()
                {          
                    imtaphy::Spectrum* spectrum = new imtaphy::Spectrum(2E09, 180000.0, 100, 0);
    
                    channel = new imtaphy::tests::ChannelStub();
                    channel->setSpectrum(spectrum);
                    
                    // TODO:  add a spatial channel model stub to the channel to return channel matrices
                    
    
                    linkManager = new imtaphy::LinkManagerStub();
                    channel->setLinkManager(linkManager);
                    
                                        
                    lambda = spectrum->getSystemCenterFrequencyHz(imtaphy::Downlink);
    
                    registry = new wns::node::Registry();
    
                    std::stringstream ss_config;
                    ss_config << "import imtaphy.Receiver\n"
                                 << "import imtaphy.Logger\n"
                                 << "logger = imtaphy.Logger.Logger(\"LinearReceiverForUnitTesting\")\n"
                                 << "receiver = imtaphy.Receiver.LinearReceiver(logger, filter = imtaphy.Receiver.MMSEFilter())\n";
                                 
                    ;
                    all_config.loadString(ss_config.str());
                    
                    
//                     unsigned int linkId = 0;
//                     
//                     LinkStub* link = new LinkStub(servingBS, ms, Link::UMa, Link::NLoS, imtaphy::Link::NotApplicable, 
//                                                           ms->getPosition(), wns::Ratio::from_dB(0.0), linkId);
//                     linkManager->addLink(link, true);
//                     linkId++;
                    
                }

                void 
                LinearReceiverTest::tearDown()
                {
    
                }

                void
                LinearReceiverTest::testComputeSINR()
                {
                    // N7imtaphy9receivers5tests18LinearReceiverTestE::testComputeSINR
                    wns::pyconfig::View receiverConfig(all_config, "receiver");

                    servingBS = imtaphy::tests::createStationStub("ServingBS", wns::Position(100,100,10), "BS", 4, 10.0 * lambda, 0, registry, channel);
                    interferingBS1 = imtaphy::tests::createStationStub("InterferingBS1", wns::Position(150,150,10), "BS", 1, 10.0 * lambda, 0, registry, channel);
                    ms = imtaphy::tests::createStationStub("MS", wns::Position(50,20,1.5), "MS",1, 0.5 * lambda, 3.0, registry, channel);

                    // currently ignored, but wants it anyway
                    linkManager->setServingLink(new LinkStub(servingBS, ms, Link::UMa, Link::NLoS, Link::NLoS, imtaphy::Link::NotApplicable,
                                                             ms->getPosition(), wns::Ratio::from_dB(0.0), 0),
                                                ms
                    );
                    
                    
                    imtaphy::receivers::tests::LinearReceiverForUnitTesting* testee = new LinearReceiverForUnitTesting(ms, receiverConfig);
                    testee->channelInitialized(channel);

                    int m = 4; // 4 streams for which I want to add power
                    int U = 4;     
                    int S = 4;
                    
                    imtaphy::detail::ComplexFloatMatrix precodedHs44(U, m);
                    imtaphy::detail::ComplexFloatMatrix receiveFilter44Matlab(U, m);
                    imtaphy::detail::ComplexFloatMatrix iAndNoiseCovariance44(U, U);
                    
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


                    
                    iAndNoiseCovariance44[0][0] = std::complex<float>( 0.000000010000000, 0.000000000000000);
                    iAndNoiseCovariance44[0][1] = std::complex<float>( 0.000000000000000, 0.000000000000000);
                    iAndNoiseCovariance44[0][2] = std::complex<float>( 0.000000000000000, 0.000000000000000);
                    iAndNoiseCovariance44[0][3] = std::complex<float>( 0.000000000000000, 0.000000000000000);
                    iAndNoiseCovariance44[1][0] = std::complex<float>( 0.000000000000000, 0.000000000000000);
                    iAndNoiseCovariance44[1][1] = std::complex<float>( 0.000000010000000, 0.000000000000000);
                    iAndNoiseCovariance44[1][2] = std::complex<float>( 0.000000000000000, 0.000000000000000);
                    iAndNoiseCovariance44[1][3] = std::complex<float>( 0.000000000000000, 0.000000000000000);
                    iAndNoiseCovariance44[2][0] = std::complex<float>( 0.000000000000000, 0.000000000000000);
                    iAndNoiseCovariance44[2][1] = std::complex<float>( 0.000000000000000, 0.000000000000000);
                    iAndNoiseCovariance44[2][2] = std::complex<float>( 0.000000010000000, 0.000000000000000);
                    iAndNoiseCovariance44[2][3] = std::complex<float>( 0.000000000000000, 0.000000000000000);
                    iAndNoiseCovariance44[3][0] = std::complex<float>( 0.000000000000000, 0.000000000000000);
                    iAndNoiseCovariance44[3][1] = std::complex<float>( 0.000000000000000, 0.000000000000000);
                    iAndNoiseCovariance44[3][2] = std::complex<float>( 0.000000000000000, 0.000000000000000);
                    iAndNoiseCovariance44[3][3] = std::complex<float>( 0.000000010000000, 0.000000000000000);

                    float* SINRsMatlab = static_cast<float*>(mkl_malloc(sizeof(float) * m, 32));
                    SINRsMatlab[0] = 10.496728029789855;
                    SINRsMatlab[1] = 9.770056829109816;
                    SINRsMatlab[2] = 9.350008008959703;
                    SINRsMatlab[3] = 7.337135575272391;


                    // Set all inputs (precoded H, filter W, and iAndNoiseCovariance
                    
                    
                    imtaphy::detail::ComplexFloatMatrixPtr receiveFilter44hermitian = imtaphy::detail::matrixHermitian<float>(receiveFilter44Matlab);
                    
                    // let it compute sinrs
                    SINRComputationResultPtr sinrResult;
                    sinrResult = testee->computeSINRs(precodedHs44,
                                                 receiveFilter44Matlab,
                                                 *receiveFilter44hermitian,
                                                 iAndNoiseCovariance44,
                                                 U,
                                                 S,
                                                 m);
                    assure((sinrResult->interferenceAndNoisePower.size() == m) &&
                            (sinrResult->rxPower.size() == m) &&
                            (sinrResult->scaledNoisePower.size() == m), "wrong number of layers");
                        
        
                    imtaphy::interface::SINRVector sinrs(m);
                    for (unsigned int i = 0; i < m; i++)
                    {
                        sinrs[i] = wns::Ratio::from_factor(sinrResult->rxPower[i].get_mW() / sinrResult->interferenceAndNoisePower[i].get_mW());
                    }

                    
                    for (unsigned int j = 0; j < sinrs.size(); j++)
                        CPPUNIT_ASSERT_DOUBLES_EQUAL(SINRsMatlab[j], sinrs[j].get_dB(), 1e-04);
                    
                    // test outputs
                    
               }
}}}

