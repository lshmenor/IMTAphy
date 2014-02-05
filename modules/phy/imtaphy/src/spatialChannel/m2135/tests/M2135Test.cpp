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

#ifndef SPATIALCHANNEL_M2135_TEST_HPP
#define SPATIALCHANNEL_M2135_TEST_HPP

#include <WNS/CppUnit.hpp>
#include <cppunit/extensions/HelperMacros.h>
#include <IMTAPHY/spatialChannel/m2135/M2135.hpp>
#include <IMTAPHY/tests/ChannelStub.hpp>
#include <IMTAPHY/tests/StationPhyStub.hpp>
#include <IMTAPHY/pathloss/M2135Pathloss.hpp>
#include <IMTAPHY/Spectrum.hpp>
#include <IMTAPHY/linkManagement/LinkManager.hpp>
#include <IMTAPHY/spatialChannel/m2135/Delays.hpp>
#include <IMTAPHY/spatialChannel/m2135/RayAngles.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/pyconfig/Parser.hpp>
#include <WNS/node/Registry.hpp>
#include <WNS/node/Node.hpp>
#include <WNS/pyconfig/helper/Functions.hpp>
#include <WNS/node/Interface.hpp>
#include <WNS/evaluation/statistics/moments.hpp>

// By default, the array access methods operator() and operator[] perform range checking. If a supplied index is out of the range defined for an array, an assertion will abort the program. To disable range checking (for performance reasons in production releases), define the BOOST_DISABLE_ASSERTS preprocessor macro prior to including multi_array.hpp in an application.

// For opt versions disable range checking in the multi_array wrapper
// WNS_NDEBUG means: assures are disabled
#ifdef  WNS_NDEBUG
#define BOOST_DISABLE_ASSERTS
#endif
#include <boost/multi_array.hpp>

#include <itpp/itbase.h>
#include <algorithm>
#include <iostream>

namespace imtaphy { namespace scm { namespace m2135 { namespace tests {
                class M2135Test :
            public CppUnit::TestFixture
                {
                
                    CPPUNIT_TEST_SUITE( M2135Test );

                    CPPUNIT_TEST( compareFloatDouble );
                    CPPUNIT_TEST( testForTesting );
                    CPPUNIT_TEST( testArraySlicing );
                    CPPUNIT_TEST( testSTLfunctions );
                    CPPUNIT_TEST( testNothing );
                    CPPUNIT_TEST( testEvolveDouble );
                    CPPUNIT_TEST( testEvolveFloat );
                
                    CPPUNIT_TEST_SUITE_END();

                public:

                    void setUp();
                    void tearDown();
                    void testForTesting();
                    void testArraySlicing();
                    void testSTLfunctions();
                    void testNothing() { }
                    void testEvolveDouble();
                    void testEvolveFloat();
                    void compareFloatDouble();
        
                private:
                    imtaphy::tests::ChannelStub* channel;
                    imtaphy::tests::StationPhyStub *bts1, *bts2, *ms1, *ms2, *ms3;
                    wns::node::Registry* registry;
                    wns::node::Interface* node;
                    imtaphy::LinkManagerStub* linkManager;
                    lsparams::LSCorrelation* lsCorrelation;
                    lsparams::LSmap* largeScaleParams;
                    wns::pyconfig::Parser wholeConfig;

                

     
                    typedef boost::multi_array_ref<double, 1> Double1D;
                    typedef boost::multi_array_ref<double, 2> Double2D;
                    typedef boost::multi_array_ref<double, 3> Double3D;
                    typedef boost::multi_array_ref<double, 4> Double4D;
                    typedef boost::multi_array_ref<double, 5> Double5D;
        
                    typedef boost::multi_array_ref<std::complex<double>, 1> ComplexDouble1D;
                    typedef boost::multi_array_ref<std::complex<double>, 2> ComplexDouble2D;
                    typedef boost::multi_array_ref<std::complex<double>, 3> ComplexDouble3D;
                    typedef boost::multi_array_ref<std::complex<double>, 4> ComplexDouble4D;
                    typedef boost::multi_array_ref<std::complex<double>, 5> ComplexDouble5D;

                };
        
                CPPUNIT_TEST_SUITE_REGISTRATION( M2135Test );


                void
                M2135Test::setUp()
                {       
                    // create float and double versions of M2135 but make sure they use same random numbers
                    itpp::RNG_reset(8041979);
                    wns::simulator::getRNG()->seed(8041979);
 

                    imtaphy::Spectrum* spectrum = new imtaphy::Spectrum(2E09, 180000.0, 100, 0);
    
                    channel = new imtaphy::tests::ChannelStub();
                    channel->setSpectrum(spectrum);
    
                    double lambda = spectrum->getSystemCenterFrequencyWavelenghtMeters(imtaphy::Downlink);
    
                    registry = new wns::node::Registry();
    
                    linkManager = new imtaphy::LinkManagerStub();
        
                    bts1 = createStationStub("BS1", wns::Position(100,100,10), "BS", 2, 10.0 * lambda, 0, registry, channel);
                    bts2 = createStationStub("BS2",wns::Position(150,150,10), "BS", 2, 10.0 * lambda, 0, registry, channel);
    
                    ms1 = createStationStub("MS1", wns::Position(50,20,1.5), "MS",2, 0.5 * lambda, 3.0, registry, channel);
                    ms2 = createStationStub("MS2", wns::Position(150,50,1.5), "MS",2, 0.5 * lambda, 3.0, registry, channel);
                    ms3 = createStationStub("MS3", wns::Position(50,150,1.5),"MS",2, 0.5 * lambda, 3.0, registry, channel);
    
                    imtaphy::StationList bsList = channel->getAllBaseStations();
                    imtaphy::StationList msList = channel->getAllMobileStations();   
    
                    int i = 0;
                    for (imtaphy::StationList::const_iterator msIter=msList.begin(); msIter!=msList.end() ; msIter++)
                        for (imtaphy::StationList::const_iterator bsIter=bsList.begin(); bsIter!=bsList.end() ; bsIter++)
                        {
                            Link::Propagation propagation;
            
                            if (i==1 || i==2 || i==3)
                                propagation = Link::NLoS;
                            else //if( i==0 || i==4 || i==5)
                                propagation = Link::LoS;
            
                            // create link with no wrap-around, 0dB widebandLoss
                            LinkStub* link = new LinkStub(*bsIter, *msIter, Link::UMa, propagation, propagation, Link::InVehicle,
                                                          (*msIter)->getPosition(), wns::Ratio::from_dB(0.0), i);
                                          
                            i++;
                            linkManager->addLink(link, true);
                        }



                    std::stringstream ss;
                    ss << "import openwns\n"
                       << "import imtaphy.Pathloss\n"
                       << "m2135Pathloss = imtaphy.Pathloss.M2135Pathloss()\n";
    
    
                    wholeConfig.loadString(ss.str());
                    wns::pyconfig::View pathlossConfig(wholeConfig, "m2135Pathloss");    
        
                    imtaphy::pathloss::M2135Pathloss* pathloss = new imtaphy::pathloss::M2135Pathloss(channel, pathlossConfig);
                    channel->setPathlossModel(pathloss);
    
                    ss << "import openwns\n"
                       << "import imtaphy.Logger\n"
                       << "import imtaphy.SCM\n"
                       << "m2135 = imtaphy.SCM.M2135(logger = imtaphy.Logger.Logger(\"SCM.M2135\"))\n";
        
                    wholeConfig.loadString(ss.str());
                    imtaphy::lsparams::RandomMatrix* rnGen = new imtaphy::lsparams::RandomMatrix();
                    lsCorrelation = new lsparams::LSCorrelation(linkManager->getAllLinks(), linkManager, rnGen);
                    largeScaleParams = lsCorrelation->generateLSCorrelation();

                }

                void 
                M2135Test::tearDown()
                {

                }


                void
                M2135Test::compareFloatDouble()
                {
                    wns::pyconfig::View m2135Config(wholeConfig, "m2135");
        
                    // create float and double versions of M2135 but make sure they use same random numbers
                    itpp::RNG_reset(8041979);
                    wns::simulator::getRNG()->seed(8041979);
       
                    imtaphy::scm::m2135::M2135<float>* scmFloat = new imtaphy::scm::m2135::M2135<float>(channel, m2135Config);
                    scmFloat->onWorldCreated(linkManager, largeScaleParams, true);
    
                    // create float and double versions of M2135 but make sure they use same random numbers
                    itpp::RNG_reset(8041979);
                    wns::simulator::getRNG()->seed(8041979);
       
                    imtaphy::scm::m2135::M2135<double>* scmDouble = new imtaphy::scm::m2135::M2135<double>(channel, m2135Config);
                    scmDouble->onWorldCreated(linkManager, largeScaleParams, true);


                    CPPUNIT_ASSERT_EQUAL(scmFloat->maxMsAntennas, scmDouble->maxMsAntennas);
                    CPPUNIT_ASSERT_EQUAL(scmFloat->maxBsAntennas, scmDouble->maxBsAntennas);

                    for (int t = 0; t < 10; t++)
                    {
                        double time = double(t); // big values so errors could propagate (if they would)
                        scmFloat->evolve(time);
                        scmDouble->evolve(time);
    
                        // test if Channel Impulse Response evolves almost identical for double and float
                        for(unsigned int k=0; k < scmFloat->scmLinks.size(); k++)
                            for (unsigned int u=0; u < scmFloat->maxMsAntennas; u++)
                                for (unsigned int s=0; s < scmFloat->maxBsAntennas; s++)
                                    for (int n=0; n < scmFloat->MaxClusters; n++)
                                    {
                                        CPPUNIT_ASSERT_DOUBLES_EQUAL((*(scmFloat->H[imtaphy::Downlink]))[k][u][s][n].imag(), (*(scmDouble->H[imtaphy::Downlink]))[k][u][s][n].imag(), 1.0e-03);
                                        CPPUNIT_ASSERT_DOUBLES_EQUAL((*(scmFloat->H[imtaphy::Downlink]))[k][u][s][n].real(), (*(scmDouble->H[imtaphy::Downlink]))[k][u][s][n].real(), 1.0e-03);
                                    }
    
                        // test if Channel Transfer Function evolves almost identical for double and float
                        for(unsigned int k=0; k < scmFloat->scmLinks.size(); k++)
                            for (unsigned int u=0; u < scmFloat->maxMsAntennas; u++)
                                for (unsigned int s=0; s < scmFloat->maxBsAntennas; s++)
                                    for (int f=0; f < 100; f++)
                                    {
                                        // here 1.0e-05 accuracy fails
                                        CPPUNIT_ASSERT_DOUBLES_EQUAL((*(scmFloat->T[imtaphy::Downlink]))[k][f][u][s].imag(), (*(scmDouble->T[imtaphy::Downlink]))[k][f][u][s].imag(), 1.0e-03);
                                        CPPUNIT_ASSERT_DOUBLES_EQUAL((*(scmFloat->T[imtaphy::Downlink]))[k][f][u][s].real(), (*(scmDouble->T[imtaphy::Downlink]))[k][f][u][s].real(), 1.0e-03);
                                    }

                    }
                }


                void 
                M2135Test::testEvolveFloat()
                {
                    // create float and double versions of M2135 but make sure they use same random numbers
                    itpp::RNG_reset(8041979);
                    wns::simulator::getRNG()->seed(8041979);
    
                    wns::pyconfig::View m2135Config(wholeConfig, "m2135");
    
    
                    imtaphy::scm::m2135::M2135<float>* scm = new imtaphy::scm::m2135::M2135<float>(channel, m2135Config);
                    scm->onWorldCreated(linkManager, largeScaleParams, true);
    
                    // Generating some results by evolving and exporting to Matlab
                    int numTTIs = 2;
    
                    itpp:: it_file ff;
                    ff.open("CIRandCTFfloat.it");
    
                    itpp::vec K(1); K = scm->scmLinks.size();
                    itpp::vec U(1); U = scm->maxMsAntennas;
                    itpp::vec S(1); S = scm->maxBsAntennas;
                    itpp::vec N(1); N = scm->MaxClusters;
                    itpp::vec T(1); T = numTTIs;
                    //  itpp::vec F(1); F = 10;
                    itpp::vec F(1); F = 10;
    
                    ff << itpp::Name("K") << K;
                    ff << itpp::Name("U") << U;
                    ff << itpp::Name("S") << S;
                    ff << itpp::Name("N") << N; 
                    ff << itpp::Name("T") << T;
                    ff << itpp::Name("F") << F;
                    ff << itpp::Name("taus") << *(scm->delays->getScaledDelays());
    
                    RayAngles3DArray* aoas = scm->angles->getAoAs();
                    RayAngles3DArray* aods = scm->angles->getAoDs();
    
                    for(unsigned int k=0; k<scm->scmLinks.size(); k++)
                    {
                        itpp::mat AoAsForItpp,AoDsForItpp;
                        AoAsForItpp.set_size(scm->MaxClusters, scm->NumRays);
                        AoDsForItpp.set_size(scm->MaxClusters, scm->NumRays);
                        for(int n=0; n<scm->MaxClusters; n++)
                            for(int m=0; m<scm->NumRays; m++)
                            {
                                AoAsForItpp(n,m) = (*aoas)[k][n][m];
                                AoDsForItpp(n,m) = (*aods)[k][n][m];
                            }
        
                        std::stringstream variableName1, variableName2;
                        variableName1 << "AoAs_"<< k+1;

                        ff << itpp::Name(variableName1.str()) << AoAsForItpp;
                        variableName2 << "AoDs_"<< k+1;
                        ff << itpp::Name(variableName2.str()) << AoDsForItpp;
                    }
    
    
                    for(int t=0;t<numTTIs; t++)
                    {
                        scm->evolve(double(t)/10000.0);
        
                        // Export the results to Matlab
                        // Please note that this takes some time
        
                        for(unsigned int k=0; k < scm->scmLinks.size(); k++)
                            for(int n=0; n<scm->MaxClusters; n++)
                            {
                                itpp::cmat HForItpp;
                                HForItpp.set_size(scm->maxMsAntennas, scm->maxBsAntennas);
                                for(unsigned int u=0; u<scm->maxMsAntennas; u++)
                                    for(unsigned int s=0; s<scm->maxBsAntennas; s++)
                                        HForItpp(u,s) = (*(scm->H[imtaphy::Downlink]))[k][u][s][n];
             
                                // In ML H will look like H = zeros(U,S,N,K,T);
                                std::stringstream variableName;
                                variableName << "H_all_all_"<< n+1 << "_" << k+1 << "_" << t+1;
                                ff << itpp::Name(variableName.str()) << HForItpp;
                            }
                        itpp::vec freqs = "0 5 11 15 19 37 42 78 97 98";
                        for(unsigned int k=0; k<scm->scmLinks.size(); k++)
                            for (unsigned int f = 0; f < 10; f++)
                            {
                                itpp::cmat TForItpp;
                                TForItpp.set_size(scm->maxMsAntennas, scm->maxBsAntennas);
                                for(unsigned int u=0; u<scm->maxMsAntennas; u++)
                                    for(unsigned int s=0; s<scm->maxBsAntennas; s++)
                                        TForItpp(u,s) = (*(scm->T[imtaphy::Downlink]))[k][freqs(f)][u][s];
                
                                // In ML T will look like H = zeros(U,S,F,K,T);
                                std::stringstream variableName;
                                variableName << "T_all_all_"<< k+1 << "_" << t+1 << "_" << f+1;
                                ff << itpp::Name(variableName.str()) << TForItpp;
                            }
                    }
    
                    ff.close();
                }

                void 
                M2135Test::testEvolveDouble()
                {
                    // create float and double versions of M2135 but make sure they use same random numbers
                    itpp::RNG_reset(8041979);
                    wns::simulator::getRNG()->seed(8041979);
    
                    wns::pyconfig::View m2135Config(wholeConfig, "m2135");
    
                    imtaphy::scm::m2135::M2135<double>* scm = new imtaphy::scm::m2135::M2135<double>(channel, m2135Config);
                    scm->onWorldCreated(linkManager, largeScaleParams, true);
    
                    // Generating some results by evolving and exporting to Matlab
                    int numTTIs = 2;
        
                    itpp:: it_file ff;
                    ff.open("CIRandCTFdouble.it");
        
                    itpp::vec K(1); K = scm->scmLinks.size();
                    itpp::vec U(1); U = scm->maxMsAntennas;
                    itpp::vec S(1); S = scm->maxBsAntennas;
                    itpp::vec N(1); N = scm->MaxClusters;
                    itpp::vec T(1); T = numTTIs;
                    //      itpp::vec F(1); F = 10;
                    itpp::vec F(1); F = 10;
        
                    ff << itpp::Name("K") << K;
                    ff << itpp::Name("U") << U;
                    ff << itpp::Name("S") << S;
                    ff << itpp::Name("N") << N; 
                    ff << itpp::Name("T") << T;
                    ff << itpp::Name("F") << F;
                    ff << itpp::Name("taus") << *(scm->delays->getScaledDelays());
        
        
                    RayAngles3DArray* aoas = scm->angles->getAoAs();
                    RayAngles3DArray* aods = scm->angles->getAoDs();
    
                    for(unsigned int k=0; k<scm->scmLinks.size(); k++)
                    {
                        itpp::mat AoAsForItpp,AoDsForItpp;
                        AoAsForItpp.set_size(scm->MaxClusters, scm->NumRays);
                        AoDsForItpp.set_size(scm->MaxClusters, scm->NumRays);
                        for(int n=0; n<scm->MaxClusters; n++)
                            for(int m=0; m<scm->NumRays; m++)
                            {
                                AoAsForItpp(n,m) = (*aoas)[k][n][m];
                                AoDsForItpp(n,m) = (*aods)[k][n][m];
                            }
        
                        std::stringstream variableName1, variableName2;
                        variableName1 << "AoAs_"<< k+1;

                        ff << itpp::Name(variableName1.str()) << AoAsForItpp;
                        variableName2 << "AoDs_"<< k+1;
                        ff << itpp::Name(variableName2.str()) << AoDsForItpp;
                    }
    
    
                    for(int t=0;t<numTTIs; t++)
                    {
                        scm->evolve(double(t)/10000.0);
                
                        // Export the results to Matlab
                        // Please note that this takes some time
                
                        for(unsigned int k=0; k<scm->scmLinks.size(); k++)
                            for(int n=0; n<scm->MaxClusters; n++)
                            {
                                itpp::cmat HForItpp;
                                HForItpp.set_size(scm->maxMsAntennas, scm->maxBsAntennas);
                                for(unsigned int u=0; u<scm->maxMsAntennas; u++)
                                    for(unsigned int s=0; s<scm->maxBsAntennas; s++)
                                        HForItpp(u,s) = (*(scm->H[imtaphy::Downlink]))[k][u][s][n];
                                
                                // In ML H will look like H = zeros(U,S,N,K,T);
                                std::stringstream variableName;
                                variableName << "H_all_all_"<< n+1 << "_" << k+1 << "_" << t+1;
                                ff << itpp::Name(variableName.str()) << HForItpp;
                            }
                        itpp::vec freqs = "0 5 11 15 19 37 42 78 97 98";
                        for(unsigned int k=0; k<scm->scmLinks.size(); k++)
                            for (unsigned int f = 0; f < 10; f++)
                            {
                                itpp::cmat TForItpp;
                                TForItpp.set_size(scm->maxMsAntennas, scm->maxBsAntennas);
                                for(unsigned int u=0; u<scm->maxMsAntennas; u++)
                                    for(unsigned int s=0; s<scm->maxBsAntennas; s++)
                                        TForItpp(u,s) = (*(scm->T[imtaphy::Downlink]))[k][freqs(f)][u][s];
                                
                                // In ML T will look like H = zeros(U,S,F,K,T);
                                std::stringstream variableName;
                                variableName << "T_all_all_"<< k+1 << "_" << t+1 << "_" << f+1;
                                ff << itpp::Name(variableName.str()) << TForItpp;
                            }
                    }
        
                    ff.close();
                }

                void 
                M2135Test::testForTesting()
                {  
                    int nBS = 7;
                    int nMS = 20;
                    int nTx = 4;
                    int nRx = 4;
                    int nClusters = 24;
                    int nRays = 20;
    
                    int size = nBS * nMS * nTx * nRx * nClusters * nRays;
    
                    itpp::cvec randomVec = itpp::randn_c(size);
    

                    itpp::cvec vecExp = itpp::exp(randomVec);
    
                    // For boost multi_array help see:
                    // http://www.boost.org/doc/libs/1_43_0/libs/multi_array/doc/reference.html
    
        
                    // The following defines two objects called "arrayAccess_.." that allow to
                    // access the elements of the big vectors by using a conventional multi-
                    // dimensional array notation.
    
                    typedef boost::multi_array_ref<std::complex<double>, 6> array_type;
                    typedef array_type::index index;
    
                    array_type arrayAccess_randomVec(randomVec._data(), boost::extents[nBS][nMS][nTx][nRx][nClusters][nRays]);
                    array_type arrayAccess_vecExp(vecExp._data(), boost::extents[nBS][nMS][nTx][nRx][nClusters][nRays]);
   
                    for (int bsId = 0; bsId < nBS; bsId++)
                    {
                        //std::cout << "Now processing BS " << bsId << "\n";
                        for (int msId = 0; msId < nMS; msId++)
                            for (int s = 0; s < nTx; s++)
                                for (int u = 0; u < nRx; u++)
                                    for (int n = 0; n < nClusters; n++)
                                        for (int m = 0; m < nRays; m ++)
                                            CPPUNIT_ASSERT_EQUAL(exp(arrayAccess_randomVec[bsId][msId][s][u][n][m]), 
                                                                 arrayAccess_vecExp[bsId][msId][s][u][n][m]);
                    }
                }

                void
                M2135Test::testSTLfunctions()
                {
                    // To use STL algorithms (see, e.g., http://www.cplusplus.com/reference/algorithm/)
                    // we need to put a boost::multi_array_ref wrapper around the it++ 
                    // Vec so that the iterators for the algorithms are provided.
                    // If, e.g., complex<double> values are to be used (e.g sorted), also a 
                    // "<" comparison has to be provided. This can be implemented by 
                    // specifying a function that decides when "i<j":
                    // sort (myvector.begin()+4, myvector.end(), myfunction);
                    // with
                    // bool myfunction (int i,int j) { return (i<j); }
    
                    int size = 10;
    
                    itpp::vec randomVec = itpp::randn(size);
                    Double1D random(randomVec._data(), boost::extents[size]);
    
                    //    std::cout << "\nRandom vector before sorting: " << randomVec << "\n";
                    std::sort(random.begin(), random.end());
                    //    std::cout << "Random vector after sorting: " << randomVec << "\n";

                    double min;
                    double max;

                    min = *std::min_element(random.begin(), random.end());
                    max = *std::max_element(random.begin(), random.end());
    
                    //   std::cout << "Min value=" << min << " max value=" << max << "\n";
    
                }

                void 
                M2135Test::testArraySlicing()
                {  
                    // Test the slicing of a 5D array [nTx][nRx][nClusters][T][K] into a 2D
                    // matrix, convert the matrix to it++ and back and check that everything
                    // is OK.
    
                    int nTx = 4;
                    int nRx = 4;
                    int nClusters = 20;
                    int T =3;
                    int K = 5;
    
                    int size = nTx * nRx * nClusters * T * K;
    
                    itpp::cvec randomVec = itpp::randn_c(size);
        
    
                    // to specify that an array should store its elements using the same memory layout as a Fortran
                    // multidimensional array would, that is, from first dimension to last
                    // array_t A( boost::extents[M][N], boost::fortran_storage_order() ); 

                    // to specify that an array should store its elements using the same layout as that used by 
                    // primitive C++ multidimensional arrays, that is, from last dimension to first. This is the default.
                    // array_t A( boost::extents[M][N], boost::c_storage_order() ); 

                    // Store in the normal C storage order:
                    ComplexDouble5D bigMatrix(randomVec._data(), boost::extents[nTx][nRx][nClusters][T][K]);    
    
                    // Now we create a View of the bigMatrix where we fix the first 3 dimensions 
                    // (to tx=1, rx=0, cluster=1) and collect all the entries of the T and K 
                    // dimension into a 2D matrix
                    ComplexDouble5D::array_view<2>::type smallMatrix = bigMatrix[boost::indices[1][0][1][ComplexDouble5D::index_range(0,T)][ ComplexDouble5D::index_range(0,K)]];

                    // We could use this view like smallMatrix[t][k] right now or perform STL
                    // operations on all its elements by getting the .begin() and .end() 
                    // iterators
    
                    // Now we want to trick multi_array into believing that the 2D slice
                    // is a 2D matrix of its own (Note: this will work only for slices that 
                    // consists of the all the "last" dimensions (i.e. a n-D slice must consist
                    // of the last n dimensions of the big array in the same order) of a high-D array):    
                    ComplexDouble2D slice( smallMatrix.origin(), boost::extents[T][K]);
    
                    // Let's check if the slice really contains the same entries for t  he first 3 
                    for (int i = 0; i < T; i++)
                        for (int j = 0; j < K; j++)
                            CPPUNIT_ASSERT_EQUAL(bigMatrix[1][0][1][i][j], slice[i][j]);
    

                    //  Mat (const Num_T *c_array, int rows, int cols, bool row_major=true, const Factory &f=DEFAULT_FACTORY)
                    //  Constructor taking a C-array as input. An element factory f can be specified.
    
                    // This constructs the itpp matrix in fortran-style ordering (row_major = false)
                    // Note: Internally, it++ will copy all the elements to a new matrix. If this is
                    // a major speed issue, we could maybe overload it.
                    itpp::cmat view2Ditpp = itpp::cmat(smallMatrix.origin(), T, K, false);
    
                    // Now do something with it in it++
                    itpp::cmat view2DitppExp = exp(view2Ditpp);
    
                    // get the multi_array_ref access to it
                    ComplexDouble2D view2DExp(view2DitppExp._data(), boost::extents[T][K]);
    
                    // Now check, that dealing with the big matrix explicitely and via the
                    // 2D it++ excursion yields the same results
                    for (int i = 0; i < T; i++)
                        for (int j = 0; j < K; j++)
                            CPPUNIT_ASSERT_EQUAL(exp(bigMatrix[1][0][1][i][j]), view2DExp[i][j]);
    
    
    
                }
        
            }}}}


#endif
