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
#include <IMTAPHY/spatialChannel/m2135/RayAngles.hpp>
#include <IMTAPHY/tests/ChannelStub.hpp>
#include <IMTAPHY/tests/StationPhyStub.hpp>
#include <IMTAPHY/linkManagement/LinkManager.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/pyconfig/Parser.hpp>
#include <WNS/node/Registry.hpp>
#include <WNS/node/Node.hpp>
#include <WNS/pyconfig/helper/Functions.hpp>
#include <WNS/node/Interface.hpp>
#include <WNS/evaluation/statistics/moments.hpp>
#include <boost/multi_array.hpp>


#include <itpp/itbase.h>
#include <algorithm>
#include <iostream>

namespace imtaphy { namespace scm { namespace m2135 { namespace tests {
                class RayAnglesTest :
            public CppUnit::TestFixture
                {
                    CPPUNIT_TEST_SUITE( RayAnglesTest );
                    CPPUNIT_TEST( testAoAandAoD );
                    CPPUNIT_TEST( testNothing );
                    CPPUNIT_TEST_SUITE_END();

                public:

                    void setUp();
                    void tearDown();
                    void testAoAandAoD();
                    void testNothing() { }
        
                private:
                    imtaphy::tests::ChannelStub* channel;
                    imtaphy::scm::m2135::RayAngles* rAngles;
                    wns::node::Registry* registry;
                    wns::node::Interface* node;
                    imtaphy::LinkManagerStub* linkManager;
                    imtaphy::Spectrum* spectrum;
                };

                CPPUNIT_TEST_SUITE_REGISTRATION( RayAnglesTest );


                void
                RayAnglesTest::setUp()
                {
                    spectrum = new imtaphy::Spectrum(2E09, 180000.0, 100, 0);
    
                    channel = new imtaphy::tests::ChannelStub();
                    channel->setSpectrum(spectrum);
    
    
                    linkManager = new imtaphy::LinkManagerStub();

                }

                void 
                RayAnglesTest::tearDown()
                {
                    delete channel;
                    delete linkManager;
                    delete node;
                    delete registry;
                }

                void 
                RayAnglesTest::testAoAandAoD()
                {
                    imtaphy::tests::StationPhyStub *bts1, *bts2, *ms1, *ms2, *ms3;
                    
                    double lambda = spectrum->getSystemCenterFrequencyWavelenghtMeters(imtaphy::Downlink);
    
                    registry = new wns::node::Registry();
        
                    bts1 = createStationStub("BS1", wns::Position(100,100,10), "BS", 1, 10.0 * lambda, 0, registry, channel);
                    bts2 = createStationStub("BS2",wns::Position(150,150,10), "BS", 1, 10.0 * lambda, 0, registry, channel);
        
                    ms1 = createStationStub("MS1", wns::Position(50,20,1.5), "MS",1, 0.5 * lambda, 3.0, registry, channel);
                    ms2 = createStationStub("MS2", wns::Position(150,50,1.5), "MS",1, 0.5 * lambda, 3.0, registry, channel);
                    ms3 = createStationStub("MS3", wns::Position(50,150,1.5),"MS",1, 0.5 * lambda, 3.0, registry, channel);
        
                    imtaphy::StationList bsList = channel->getAllBaseStations();
                    imtaphy::StationList msList = channel->getAllMobileStations();
                    //int temp=1;
                    int i = 0;
                    for (imtaphy::StationList::const_iterator msIter=msList.begin(); msIter!=msList.end() ; msIter++)
                        for (imtaphy::StationList::const_iterator bsIter=bsList.begin(); bsIter!=bsList.end() ; bsIter++)
                        {
                            Link::Propagation propagation;
                        
                            if (i==1 || i==2 || i==5)
                                propagation = Link::NLoS;
                            else 
                                propagation = Link::LoS;
                        
                            // create link with no wrap-around, 0dB widebandLoss
                            LinkStub* link = new LinkStub(*bsIter, *msIter, Link::UMa, propagation, propagation, Link::InVehicle, 
                                                          (*msIter)->getPosition(), wns::Ratio::from_dB(0.0), i);
                            i++;
                                
                            linkManager->addLink(link, true);
                        }
        
                    int MaxClusters = 24; // includes the subclusters for the 2 strongest paths
                    int NumRays = 20;
                    int numLinks = linkManager->getSCMLinks().size();
        
                    rAngles = new imtaphy::scm::m2135::RayAngles(linkManager->getSCMLinks(), MaxClusters, NumRays, channel);
        
                    std::vector<itpp::mat> rand_LoS, rand_NLoS;
#include "AoAtestGrids/rand_xAOD_LoS.txt"
#include "AoAtestGrids/rand_xAOD_NLoS.txt"
#include "AoAtestGrids/rand_yAOD_LoS.txt"
#include "AoAtestGrids/rand_yAOD_NLoS.txt"
#include "AoAtestGrids/rand_xAOA_LoS.txt"
#include "AoAtestGrids/rand_xAOA_NLoS.txt"
#include "AoAtestGrids/rand_yAOA_LoS.txt"
#include "AoAtestGrids/rand_yAOA_NLoS.txt"
                    rand_LoS.push_back(rand_xAOD_LoS); rand_NLoS.push_back(rand_xAOD_NLoS);
                    rand_LoS.push_back(rand_yAOD_LoS); rand_NLoS.push_back(rand_yAOD_NLoS);
                    rand_LoS.push_back(rand_xAOA_LoS); rand_NLoS.push_back(rand_xAOA_NLoS);
                    rand_LoS.push_back(rand_yAOA_LoS); rand_NLoS.push_back(rand_yAOA_NLoS);
                    itpp::vec tBs = "152.0443  88.9384   156.1707  120.7786  136.1890   77.1411";
                    itpp::vec tMs = "54.2328   91.5512  -12.6197   52.4126 -108.1948  -66.8183";
                    rAngles->initforTest(rand_LoS, rand_NLoS, tBs, tMs);
         
        
        
#include "AoAtestGrids/sigmas.txt"
#include "AoAtestGrids/clusterPowers.txt"
                    rAngles->generateAoAandAoDs(clusterPowers, &sigmas);
        
        
                    typedef boost::multi_array_ref<double, 3> Double3DArray;
                    RayAngles3DArray* aoas = rAngles->getAoAs();
                    RayAngles3DArray* aods = rAngles->getAoDs();
        
                    // the matlab code was modified to disable random coupling before generating the following matrices
#include "AoAtestGrids/aods_matlab.txt"
#include "AoAtestGrids/aoas_matlab.txt"
                    Double3DArray aoasM(aoas_matlab._data(), boost::extents[numLinks][MaxClusters-4][NumRays]);
                    Double3DArray aodsM(aods_matlab._data(), boost::extents[numLinks][MaxClusters-4][NumRays]);
                    for (int k=0; k<numLinks; k++)
                    {
                        for (int n=0; n<MaxClusters-4; n++)
                        {
                            for (int m=0; m<NumRays; m++)
                            {
                                if (!std::isnan((*aods)[k][n][m]))
                                    CPPUNIT_ASSERT_DOUBLES_EQUAL(aodsM[k][n][m], (180.0 / itpp::pi) * (*aods)[k][n][m], 1e-1);
                                if (!std::isnan((*aoas)[k][n][m]))
                                    CPPUNIT_ASSERT_DOUBLES_EQUAL(aoasM[k][n][m], (180.0 / itpp::pi) * (*aoas)[k][n][m], 1e-1);
                            }
                        }
                    }
                    delete rAngles;
                }


            }}}}
