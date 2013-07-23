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
#include <IMTAPHY/lsParams/LSCorrelation.hpp>
#include <IMTAPHY/tests/ChannelStub.hpp>
#include <IMTAPHY/tests/StationPhyStub.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/pyconfig/Parser.hpp>
#include <WNS/node/Registry.hpp>
#include <WNS/node/Node.hpp>
#include <WNS/pyconfig/helper/Functions.hpp>
#include <WNS/node/Interface.hpp>
#include <WNS/evaluation/statistics/moments.hpp>
#include <IMTAPHY/linkManagement/LinkManager.hpp>

#include <itpp/itbase.h>
#include <algorithm>
#include <iostream>

namespace imtaphy { namespace scm { namespace m2135 { namespace tests {
                class LSCorrelationTest :
            public CppUnit::TestFixture
                {
                    CPPUNIT_TEST_SUITE( LSCorrelationTest );
                    CPPUNIT_TEST( testSmallScenario );
                    CPPUNIT_TEST( testLargeScenario );
                    CPPUNIT_TEST( testNothing );
                    CPPUNIT_TEST_SUITE_END();

                public:

                    void setUp();
                    void tearDown();
                    void testSmallScenario();
                    void testLargeScenario();
                    void testNothing() { }
    
                private:
                    imtaphy::tests::ChannelStub* channel;
                    imtaphy::lsparams::LSCorrelation* lsTest;
                    wns::node::Registry* registry;
                    wns::node::Interface* node;
                    imtaphy::LinkManagerStub* linkManager;
                    imtaphy::Spectrum* spectrum;
                };

                CPPUNIT_TEST_SUITE_REGISTRATION( LSCorrelationTest );

                void
                LSCorrelationTest::setUp()
                {
                    channel = new imtaphy::tests::ChannelStub();
                    spectrum = new imtaphy::Spectrum(2E09, 180000.0, 100, 0);
                    channel->setSpectrum(spectrum);
                }

                void 
                LSCorrelationTest::tearDown()
                {
                    delete channel;
                    delete linkManager;
                    delete node;
                    delete registry;
                }

                void 
                LSCorrelationTest::testSmallScenario()
                {
                    linkManager = new imtaphy::LinkManagerStub();
    
                    //lsTest->init();
                    imtaphy::tests::StationPhyStub *bts1, *bts2, *ms1, *ms2, *ms3;
                    
                    double lambda = spectrum->getSystemCenterFrequencyWavelenghtMeters(imtaphy::Downlink);
    
                    registry = new wns::node::Registry();
    
                    bts1 = createStationStub("BS1", wns::Position(5,10,10), "BS", 1, 10.0 * lambda, 0, registry, channel);
                    bts2 = createStationStub("BS2", wns::Position(10,5,10), "BS", 1, 10.0 * lambda, 0, registry, channel);
    
                    ms1 = createStationStub("MS1", wns::Position(20,30,1.5), "MS",1, 0.5 * lambda, 3.0, registry, channel);
                    ms2 = createStationStub("MS2", wns::Position(55,35,1.5), "MS",1, 0.5 * lambda, 3.0, registry, channel);
                    ms3 = createStationStub("MS3", wns::Position(45,30,1.5), "MS",1, 0.5 * lambda, 3.0, registry, channel);
    
                    imtaphy::StationList bsList = channel->getAllBaseStations();
                    imtaphy::StationList msList = channel->getAllMobileStations();
                    int i = 0;
                    for (imtaphy::StationList::const_iterator msIter=msList.begin(); msIter!=msList.end() ; msIter++)
                        for (imtaphy::StationList::const_iterator bsIter=bsList.begin(); bsIter!=bsList.end() ; bsIter++)
                        {
                            Link::Propagation propagation;
            
            
                            if (i==1 || i==2 || i==4) // fixed for the current test-case only
                            {
                                propagation = Link::NLoS;
                                /*          
                                            std::cout << "BS: " << (*bsIter)->getNode()->getName() << " has a NLoS link with MS: " 
                                            << (*msIter)->getNode()->getName() << std::endl;*/
                                                                                    
                            }
                            else if( i==0 || i==3 || i==5)
                            {
                                propagation = Link::LoS;
                                /*              
                                                std::cout << "BS: " << (*bsIter)->getNode()->getName() << " has a LoS link with MS: " 
                                                << (*msIter)->getNode()->getName() << std::endl;*/
                            }
                            // create link with no wrap-around, 0dB widebandLoss
                            LinkStub* link = new LinkStub(*bsIter, *msIter, Link::UMa, propagation, propagation, Link::InVehicle, 
                                                          (*msIter)->getPosition(), wns::Ratio::from_dB(0.0), i);
            
                            i++;
            
                            linkManager->addLink(link, true);
                        }
    



    
                    std::list<itpp::mat> gridList;
#include "grids/grid0.txt"
                    gridList.push_back(grid0);
#include "grids/grid1.txt"
                    gridList.push_back(grid1);
#include "grids/grid2.txt"
                    gridList.push_back(grid2);

#include "grids/grid3.txt"
                    gridList.push_back(grid3);

#include "grids/grid4.txt"
                    gridList.push_back(grid4);

#include "grids/grid5.txt"
                    gridList.push_back(grid5);

#include "grids/grid6.txt"
                    gridList.push_back(grid6);

#include "grids/grid7.txt"
                    gridList.push_back(grid7);

#include "grids/grid8.txt"
                    gridList.push_back(grid8);

#include "grids/grid9.txt"
                    gridList.push_back(grid9);

#include "grids/grid10.txt"
                    gridList.push_back(grid10);

#include "grids/grid11.txt"
                    gridList.push_back(grid11);
                    imtaphy::lsparams::RandomMatrixMock* rnGenMock = new imtaphy::lsparams::RandomMatrixMock();
                    rnGenMock->feedRandomMatrices(gridList);
                    lsTest = new imtaphy::lsparams::LSCorrelation(linkManager->getSCMLinks(), linkManager, rnGenMock);
                    gridList.clear();
#include "grids/sigmas.txt"

                    //std::cout << "Expected value of LS parameters was: \n" << sigmas << std::endl << std::endl;
                    imtaphy::lsparams::LSmap* lsParams = lsTest->generateLSCorrelation();
    
                    imtaphy::LinkVector scmlinks = linkManager->getSCMLinks();
                    unsigned int K = scmlinks.size();
                    itpp::mat* sigmas_calculated = new itpp::mat(K, 5);
                    for (unsigned int k = 0; k < K; k++)
                    {
                        assure(lsParams->find(scmlinks[k]) != lsParams->end(), "SCM link without large scale paramters");
        
                        sigmas_calculated->set(k, int(DSpread), (*lsParams)[scmlinks[k]].getDelaySpread());
                        sigmas_calculated->set(k, int(ASDeparture), (*lsParams)[scmlinks[k]].getAngularSpreadDeparture());
                        sigmas_calculated->set(k, int(ASArrival), (*lsParams)[scmlinks[k]].getAngularSpreadArrival());
                        sigmas_calculated->set(k, int(SFading), (*lsParams)[scmlinks[k]].getShadowFading());
                        sigmas_calculated->set(k, int(RiceanK), (*lsParams)[scmlinks[k]].getRicanK());
                    }
                    CPPUNIT_ASSERT_EQUAL_MESSAGE("Dimensions not correct for Sigma matrix", sigmas.rows(), (*sigmas_calculated).rows());
                    CPPUNIT_ASSERT_EQUAL_MESSAGE("Dimensions not correct for Sigma matrix", 5, (*sigmas_calculated).cols());
                    for (int k=0; k<sigmas.rows(); k++)
                        for (int i=0; i<5; i++)
                        {
                            // we also have a single precision implementation so we should not expect 
                            // precision to be better than 1e-04 or so
                            if(i==2) // more precision required for Delay Spread (Sigma_DS) 
                                CPPUNIT_ASSERT_DOUBLES_EQUAL(sigmas(k,i), (*sigmas_calculated)(k,i), 1e-4);
                            else
                                CPPUNIT_ASSERT_DOUBLES_EQUAL(sigmas(k,i), (*sigmas_calculated)(k,i), 1e-4);
                        }
                }

                void 
                LSCorrelationTest::testLargeScenario()
                {
                    linkManager = new imtaphy::LinkManagerStub();
                    imtaphy::tests::StationPhyStub *bts1, *bts2, *bts3, *ms1, *ms2, *ms3, *ms4; 
                    
                    double lambda = spectrum->getSystemCenterFrequencyWavelenghtMeters(imtaphy::Downlink);
    
                    registry = new wns::node::Registry();
    
                    bts1 = createStationStub("BS1", wns::Position(100,100,10), "BS", 1, 10.0 * lambda, 0, registry, channel);
                    bts2 = createStationStub("BS2",wns::Position(150,150,10), "BS", 1, 10.0 * lambda, 0, registry, channel);
                    bts3 = createStationStub("BS3",wns::Position(175,125,10), "BS", 1, 10.0 * lambda, 0, registry, channel);
    
                    ms1 = createStationStub("MS1", wns::Position(50,20,1.5), "MS",1, 0.5 * lambda, 3.0, registry, channel);
                    ms2 = createStationStub("MS2", wns::Position(150,50,1.5), "MS",1, 0.5 * lambda, 3.0, registry, channel);
                    ms3 = createStationStub("MS3", wns::Position(50,150,1.5),"MS",1, 0.5 * lambda, 3.0, registry, channel);
                    ms4 = createStationStub("MS4", wns::Position(30,75,1.5),"MS",1, 0.5 * lambda, 3.0, registry, channel);
    
                    imtaphy::StationList bsList = channel->getAllBaseStations();
                    imtaphy::StationList msList = channel->getAllMobileStations();
                    int i=0;
                    for (imtaphy::StationList::const_iterator msIter=msList.begin(); msIter!=msList.end() ; msIter++)
                        for (imtaphy::StationList::const_iterator bsIter=bsList.begin(); bsIter!=bsList.end() ; bsIter++)
                        {
                            Link::Propagation propagation;
            
            
                            if (i==1 || i==2 || i==5 || i==7 || i==10 || i==11) // fixed for the current test-case only
                            {
                                propagation = Link::NLoS;
                                /*          
                                            std::cout << "BS: " << (*bsIter)->getNode()->getName() << " has a NLoS link with MS: " 
                                            << (*msIter)->getNode()->getName() << std::endl;*/
                                                                                    
                            }
                            else if( i==0 || i==3 || i==4 || i==6 || i==8 || i==9)
                            {
                                propagation = Link::LoS;
                                /*              
                                                std::cout << "BS: " << (*bsIter)->getNode()->getName() << " has a LoS link with MS: " 
                                                << (*msIter)->getNode()->getName() << std::endl;*/
                            }
                            //create link with no wrap-around, 0dB widebandLoss
                            LinkStub* link = new LinkStub(*bsIter, *msIter, Link::UMa, propagation, propagation, Link::InVehicle, 
                                                          (*msIter)->getPosition(),
                                                          wns::Ratio::from_dB(0.0), i);
            
                            i++;
            
                            linkManager->addLink(link, true);
                        }
    

                    std::list<itpp::mat> gridList;
#include "gridsLS/grid0.txt"
                    gridList.push_back(grid0);
#include "gridsLS/grid1.txt"
                    gridList.push_back(grid1);
#include "gridsLS/grid2.txt"
                    gridList.push_back(grid2);
#include "gridsLS/grid3.txt"
                    gridList.push_back(grid3);
#include "gridsLS/grid4.txt"
                    gridList.push_back(grid4);
#include "gridsLS/grid5.txt"
                    gridList.push_back(grid5);
#include "gridsLS/grid6.txt"
                    gridList.push_back(grid6);
#include "gridsLS/grid7.txt"
                    gridList.push_back(grid7);
#include "gridsLS/grid8.txt"
                    gridList.push_back(grid8);
#include "gridsLS/grid9.txt"
                    gridList.push_back(grid9);
#include "gridsLS/grid10.txt"
                    gridList.push_back(grid10);
#include "gridsLS/grid11.txt"
                    gridList.push_back(grid11);
#include "gridsLS/grid12.txt"
                    gridList.push_back(grid12);
#include "gridsLS/grid13.txt"
                    gridList.push_back(grid13);
#include "gridsLS/grid14.txt"
                    gridList.push_back(grid14);
#include "gridsLS/grid15.txt"
                    gridList.push_back(grid15);
#include "gridsLS/grid16.txt"
                    gridList.push_back(grid16);
                    imtaphy::lsparams::RandomMatrixMock* rnGenMock = new imtaphy::lsparams::RandomMatrixMock();
                    rnGenMock->feedRandomMatrices(gridList);
                    lsTest = new imtaphy::lsparams::LSCorrelation(linkManager->getSCMLinks(), linkManager, rnGenMock);
                    gridList.clear();
#include "gridsLS/sigmas.txt"
    
    
                    imtaphy::lsparams::LSmap* lsParams = lsTest->generateLSCorrelation();
    
                    imtaphy::LinkVector scmlinks = linkManager->getSCMLinks();
                    unsigned int K = scmlinks.size();
                    itpp::mat* sigmas_calculated = new itpp::mat(K, 5);
                    for (unsigned int k = 0; k < K; k++)
                    {
                        assure(lsParams->find(scmlinks[k]) != lsParams->end(), "SCM link without large scale paramters");
        
                        sigmas_calculated->set(k, int(DSpread), (*lsParams)[scmlinks[k]].getDelaySpread());
                        sigmas_calculated->set(k, int(ASDeparture), (*lsParams)[scmlinks[k]].getAngularSpreadDeparture());
                        sigmas_calculated->set(k, int(ASArrival), (*lsParams)[scmlinks[k]].getAngularSpreadArrival());
                        sigmas_calculated->set(k, int(SFading), (*lsParams)[scmlinks[k]].getShadowFading());
                        sigmas_calculated->set(k, int(RiceanK), (*lsParams)[scmlinks[k]].getRicanK());
                    }
                    CPPUNIT_ASSERT_EQUAL_MESSAGE("Dimensions not correct for Sigma matrix", sigmas.rows(), (*sigmas_calculated).rows());
                    CPPUNIT_ASSERT_EQUAL_MESSAGE("Dimensions not correct for Sigma matrix", 5, (*sigmas_calculated).cols());
                    for (int k=0; k<sigmas.rows(); k++)
                        for (int i=0; i<5; i++)
                        {
                            // we also have a single precision implementation so we should not expect 
                            // precision to be better than 1e-04 or so
                            if(i==2) // more precision required for Delay Spread (Sigma_DS) 
                                CPPUNIT_ASSERT_DOUBLES_EQUAL(sigmas(k,i), (*sigmas_calculated)(k,i), 1e-04);
                            else
                                CPPUNIT_ASSERT_DOUBLES_EQUAL(sigmas(k,i), (*sigmas_calculated)(k,i), 1e-04);
                        }

                }

            }}}}
