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
#include <WNS/pyconfig/View.hpp>
#include <WNS/pyconfig/Parser.hpp>
#include <WNS/node/Registry.hpp>
#include <WNS/node/Node.hpp>
#include <WNS/node/Interface.hpp>

#include <cppunit/extensions/HelperMacros.h>

#include <IMTAPHY/spatialChannel/m2135/M2135.hpp>
#include <IMTAPHY/tests/ChannelStub.hpp>
#include <IMTAPHY/tests/StationPhyStub.hpp>
#include <IMTAPHY/spatialChannel/m2135/Delays.hpp>
#include <IMTAPHY/spatialChannel/m2135/FixPar.hpp>
#include <IMTAPHY/linkManagement/LinkManager.hpp>

#include <itpp/itbase.h>

namespace imtaphy { namespace scm { namespace m2135 { namespace tests {
                class DelaysTest :
            public CppUnit::TestFixture
                {
                
                    CPPUNIT_TEST_SUITE( DelaysTest );
                    CPPUNIT_TEST( testDelays );
                    CPPUNIT_TEST_SUITE_END();

                public:

                    void setUp();
                    void tearDown();
                    void testDelays();
        
                private:
                    wns::node::Registry* registry;
                    wns::node::Interface* node;
                    imtaphy::tests::ChannelStub* channel;
                    imtaphy::tests::StationPhyStub *bts1, *bts2, *ms1, *ms2, *ms3;
                    itpp::mat *sigmas;
                    itpp:: mat delaysForAllLinks;
                    imtaphy::scm::m2135::Delays* d;
                    imtaphy::LinkManagerStub* linkManager;
                };
        
                CPPUNIT_TEST_SUITE_REGISTRATION( DelaysTest );

                void 
                DelaysTest::setUp()
                {
                    imtaphy::Spectrum* spectrum = new imtaphy::Spectrum(2E09, 180000.0, 100, 0);
    
                    channel = new imtaphy::tests::ChannelStub();
                    channel->setSpectrum(spectrum);
    
                    double lambda = spectrum->getSystemCenterFrequencyWavelenghtMeters(imtaphy::Downlink);
    
                    registry = new wns::node::Registry();
    
                    linkManager = new imtaphy::LinkManagerStub();
        
                    bts1 = createStationStub("BS1", wns::Position(100,100,10), "BS", 1, 10.0 * lambda, 0, registry, channel);
                    bts2 = createStationStub("BS2",wns::Position(150,150,10), "BS", 1, 10.0 * lambda, 0, registry, channel);
        
                    ms1 = createStationStub("MS1", wns::Position(50,20,1.5), "MS",1, 0.5 * lambda, 3.0, registry, channel);
                    ms2 = createStationStub("MS2", wns::Position(150,50,1.5), "MS",1, 0.5 * lambda, 3.0, registry, channel);
                    ms3 = createStationStub("MS3", wns::Position(50,150,1.5),"MS",1, 0.5 * lambda, 3.0, registry, channel);
        
                    imtaphy::StationList bsList = channel->getAllBaseStations();
                    imtaphy::StationList msList = channel->getAllMobileStations();
        
                    int i = 0;
                    for (imtaphy::StationList::const_iterator msIter=msList.begin(); msIter!=msList.end() ; msIter++)
                        for (imtaphy::StationList::const_iterator bsIter=bsList.begin(); bsIter!=bsList.end() ; bsIter++)
                        {
                            Link::Propagation propagation;
                         
                            if (i == 0 || i==1 || i==2 || i==4)
                                propagation = Link::LoS;
                            else 
                                propagation = Link::NLoS;
                         
                            // create link with no wrap-around, 0dB widebandLoss
                            LinkStub* link = new LinkStub(*bsIter, *msIter, Link::UMa, propagation, propagation, Link::InVehicle, 
                                                          (*msIter)->getPosition(),
                                                          wns::Ratio::from_dB(0.0), i);
                                
                            i++;
                            linkManager->addLink(link, true);

                        }
                

                    // Only testing with 20 as maxClusters as used in Matlab as test
                    d = new imtaphy::scm::m2135::Delays(sigmas, linkManager->getSCMLinks(), 20);
        
                }

                void 
                DelaysTest::testDelays()
                {               
                    // 6 links fixed
                    sigmas = new itpp::mat;
                    sigmas->set_size(6,5);
                    sigmas->set_row(0, "0.067376499132801   0.746422663643607   0.000000001124180   0.044090611190747   0.113771427857176");
                    sigmas->set_row(1, "0.123625898257966   0.537024895297904   0.000000000555782   0.016104478084403   0.251832569031204");
                    sigmas->set_row(2, "0.072028671340517   0.312448637807776   0.000000000046831   0.020163515927574   0.056057088571178");
                    sigmas->set_row(3, "0.530514951072463   0.620212990537093   0.000000005332643   0.000524407627073   0.010000000000000");
                    sigmas->set_row(4, "0.021861188845098   1.150342445317491   0.000000001556478   0.047993167479971   0.046242099878933");
                    sigmas->set_row(5, "0.684787917112403   0.826488346942369   0.000000013558967   0.000221632310388   0.010000000000000");
                    *sigmas = 100*(*sigmas);
                
                    delaysForAllLinks.set_size(6,20);
        
                    // LoS
                    //delaysForAllLinks.set_row(0,"0.220729796825974   0.494303190770865   0.939994149880595   0.127395601822945   0.760229036784084   0.633025155496652   0.020714162809680   0.119011084239334    0.840961206689038   0.958734367105274   0.797140624830230   0.905936501616352 nan nan nan nan nan nan nan nan");
                    //delaysForAllLinks.set_row(1,"0.509627043708110   0.040365254756060   0.526248264277099   0.215021411197999   0.045911747435199   0.315486195782261   0.770135490521603   0.095241855681149    0.823477073936384   0.016384612219561   0.955778332006108   0.745308222986484 nan nan nan nan nan nan nan nan");
                    //delaysForAllLinks.set_row(2,"0.838201948646865   0.832350153983682   0.941748364068005   0.417538064792799   0.805427967205775   0.707647958150268   0.614112510521505   0.019873640250792    0.015784215208637   0.165017289876231   0.478117611558193   0.271358383189727 nan nan nan nan nan nan nan nan");
                    //delaysForAllLinks.set_row(3,"0.607036204580462   0.729800222518653   0.806493081248432   0.703185574806528   0.761279655754715   0.661998525723366   0.470318363837607   0.199685435673502    0.720985926559908   0.295383565286349   0.239886288097073   0.084471911649723 nan nan nan nan nan nan nan nan");
    
                    // NLOS
                    //delaysForAllLinks.set_row(4,"0.023050328497231   0.960908715096095   0.474224396569689   0.894059404889856   0.806629444677160   0.229025339331137   0.480784309959967   0.523393363302178     0.319566517380490   0.610130241382928   0.515717669425659   0.849293451730644   0.963478688629917   0.884581746889610   0.777813354292836   0.489765193229016  0.806176050792288   0.799360215542382   0.027874791868804   0.476440188999087");
                    //delaysForAllLinks.set_row(5,"0.494373803849642   0.260326267293252   0.821905970848666   0.077941233759693   0.868698584692104   0.394185725799678   0.322287138699423   0.831115823850065     0.625146716800403   0.138495449478695   0.344034176226086   0.945178269699610   0.085279170468408   0.637844135551220   0.106073332781251   0.528630992267268  0.389530158002976   0.042318794135669   0.940269683549956   0.381575525962831");
        
                    // Re-arranged for PropagConditionVector = [1 1 1 0 1 0] as used in Matlab
                    delaysForAllLinks.set_row(0,"0.220729796825974   0.494303190770865   0.939994149880595   0.127395601822945   0.760229036784084   0.633025155496652   0.020714162809680   0.119011084239334    0.840961206689038   0.958734367105274   0.797140624830230   0.905936501616352 nan nan nan nan nan nan nan nan");
                    delaysForAllLinks.set_row(1,"0.509627043708110   0.040365254756060   0.526248264277099   0.215021411197999   0.045911747435199   0.315486195782261   0.770135490521603   0.095241855681149    0.823477073936384   0.016384612219561   0.955778332006108   0.745308222986484 nan nan nan nan nan nan nan nan");
                    delaysForAllLinks.set_row(2,"0.838201948646865   0.832350153983682   0.941748364068005   0.417538064792799   0.805427967205775   0.707647958150268   0.614112510521505   0.019873640250792    0.015784215208637   0.165017289876231   0.478117611558193   0.271358383189727 nan nan nan nan nan nan nan nan");
                    delaysForAllLinks.set_row(3,"0.023050328497231   0.960908715096095   0.474224396569689   0.894059404889856   0.806629444677160   0.229025339331137   0.480784309959967   0.523393363302178    0.319566517380490   0.610130241382928   0.515717669425659   0.849293451730644   0.963478688629917   0.884581746889610   0.777813354292836   0.489765193229016  0.806176050792288   0.799360215542382   0.027874791868804   0.476440188999087");
                    delaysForAllLinks.set_row(4,"0.607036204580462   0.729800222518653   0.806493081248432   0.703185574806528   0.761279655754715   0.661998525723366   0.470318363837607   0.199685435673502    0.720985926559908   0.295383565286349   0.239886288097073   0.084471911649723 nan nan nan nan nan nan nan nan");
                    delaysForAllLinks.set_row(5,"0.494373803849642   0.260326267293252   0.821905970848666   0.077941233759693   0.868698584692104   0.394185725799678   0.322287138699423   0.831115823850065     0.625146716800403   0.138495449478695   0.344034176226086   0.945178269699610   0.085279170468408   0.637844135551220   0.106073332781251   0.528630992267268  0.389530158002976   0.042318794135669   0.940269683549956   0.381575525962831");
        
                    d->initForTest(delaysForAllLinks, sigmas);
                    d->generateDelays();
        
                    itpp::mat delaysMatlab;
                    delaysMatlab = "0   0.000554793940796   0.001591974624213   0.003683615100679   0.005187611268936   0.006520083663355   0.011666287152183   0.018618246795169   0.041276369015708   0.056723785267029               0.058637156105179   0.107775035059291                 NaN                 NaN                 NaN                 NaN                 NaN                 NaN                 NaN                 NaN;\
                                        0   0.002070153262269   0.003000660832066   0.003455965868438   0.008291613412047   0.008737543923636   0.015400873287193   0.020727728593005   0.032042312211127   0.042181144309267           0.043970090347557   0.056497788571631                 NaN                 NaN                 NaN                 NaN                 NaN                 NaN                 NaN                 NaN;\
                                        0   0.000136370758785   0.000144573032642   0.000183067456676   0.000334597395509   0.000500576507719   0.000793646546842   0.000952264359364   0.001456792657326   0.002039124597164           0.004517252540074   0.004786980180246                 NaN                 NaN                 NaN                 NaN                 NaN                 NaN                 NaN                 NaN;\
                                        0   0.000327594608460   0.009171598793102   0.010478723878704   0.015471857166100   0.021793325941887   0.021862285428110   0.022903647881959   0.026255091270264   0.056036437863378           0.074843603730969   0.076655627670505   0.082988503042973   0.085258441579887   0.086371688309049   0.086943433982518   0.135355561736121   0.176214561851414   0.434530601135562   0.457839493424165;\
                                        0   0.002245007206322   0.003888256754194   0.004361084074740   0.005333835556142   0.007682463717860   0.011055152880530   0.020984649770896   0.039083963609632   0.047181918692048           0.054319214895604   0.087796108478305                 NaN                 NaN                 NaN                 NaN                 NaN                 NaN                 NaN                 NaN;\
                                        0   0.001623780365845   0.026313600966102   0.040106081910689   0.043581147836546   0.122646698663789   0.128917373847064   0.181214336258777   0.202108302829343   0.272734303239364           0.276439434093655   0.282873804116689   0.315172068996546   0.335535694825210   0.402119236651486   0.598931164344201   0.682105343110106   0.750152512234510   0.778211809896507   0.968669849745574";
    
                    delaysMatlab = 1.0e-05 * delaysMatlab;
        
                    unsigned int k;
                    imtaphy::LinkVector links = linkManager->getSCMLinks();
                    for (k=0; k < links.size() ; k++)
                    {
                        unsigned int nClusters = FixParSingleton::Instance()(links[k]->getScenario(), links[k]->getPropagation())->NumClusters;
                
                        for (unsigned int j=0; j<nClusters; j++)
                            CPPUNIT_ASSERT_DOUBLES_EQUAL( (*(d->getDelays()))(k,j), delaysMatlab(k,j), 1e-15 );
                    }
                }

                void 
                DelaysTest::tearDown()
                {
                    delete sigmas;
                    delete channel;
                    delete linkManager;
                    delete node;
                    delete registry;
                }
        
            }}}}
