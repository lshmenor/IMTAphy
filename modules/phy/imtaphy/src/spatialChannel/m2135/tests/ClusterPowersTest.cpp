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
#include <IMTAPHY/spatialChannel/m2135/ClusterPowers.hpp>
#include <IMTAPHY/tests/ChannelStub.hpp>
#include <IMTAPHY/tests/StationPhyStub.hpp>
#include <IMTAPHY/Link.hpp>
#include <IMTAPHY/linkManagement/LinkManager.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/pyconfig/Parser.hpp>
#include <WNS/node/Registry.hpp>
#include <WNS/node/Node.hpp>
#include <WNS/pyconfig/helper/Functions.hpp>
#include <WNS/node/Interface.hpp>
#include <WNS/evaluation/statistics/moments.hpp>


#include <itpp/itbase.h>
#include <algorithm>
#include <iostream>

namespace imtaphy { namespace scm { namespace m2135 { namespace tests {
                class ClusterPowersTest :
            public CppUnit::TestFixture
                {
                
                    CPPUNIT_TEST_SUITE( ClusterPowersTest );
                    CPPUNIT_TEST( testPowers );
                    CPPUNIT_TEST( testNothing );
                    CPPUNIT_TEST_SUITE_END();

                public:

                    void setUp();
                    void tearDown();
                    void testPowers();
                    void testNothing() { }
        
                private:
                    imtaphy::tests::ChannelStub* channel;
                    imtaphy::scm::m2135::ClusterPowers* cPowers;
                    imtaphy::tests::StationPhyStub *bts1, *bts2, *ms1, *ms2, *ms3;
                    wns::node::Registry* registry;
                    wns::node::Interface* node;
                    imtaphy::LinkManagerStub* linkManager;
                };

                CPPUNIT_TEST_SUITE_REGISTRATION( ClusterPowersTest );

                void
                ClusterPowersTest::setUp()
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
                    for (imtaphy::StationList::const_iterator bsIter=bsList.begin(); bsIter!=bsList.end() ; bsIter++)
                        for (imtaphy::StationList::const_iterator msIter=msList.begin(); msIter!=msList.end() ; msIter++) 
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
        
                }

                void 
                ClusterPowersTest::tearDown()
                {
                    delete channel;
                    delete linkManager;
                    delete node;
                    delete registry;
                }

                void 
                ClusterPowersTest::testPowers()
                {
                    itpp::mat tau_n = "0.000000000000000 0.000000000343272 0.000000017264571 0.000000019288310 0.000000026674816 0.000000037231997 0.000000045289138 0.000000076201582 0.000000093969024 0.000000133727734 0.000000193026029 0.000000298980441 NaN NaN NaN NaN NaN NaN NaN NaN ; 0.000000000000000 0.000000076936319 0.000000147508705 0.000000214601868 0.000000263420505 0.000000290875126 0.000000371341980 0.000000407165606 0.000000464423538 0.000000508780689 0.000000513895259 0.000000690384927 0.000000721084772 0.000000744211796 0.000000981163228 0.000001047747773 0.000001381918649 0.000001558491576 0.000002533810388 0.000003595249031 ; 0.000000000000000 0.000000072692921 0.000000243262066 0.000000425437328 0.000000439297963 0.000000466794470 0.000000559958252 0.000000797488380 0.000000879031910 0.000000893883822 0.000000923106209 0.000001274815033 0.000001687217698 0.000001869181249 0.000001934589334 0.000002002517076 0.000002315751407 0.000002384677432 0.000002657218705 0.000003180297088 ; 0.000000000000000 0.000000036799192 0.000000053221931 0.000000055954350 0.000000061265001 0.000000065941465 0.000000066929647 0.000000073573417 0.000000088631882 0.000000091286563 0.000000103699948 0.000000117849389 0.000000124375007 0.000000130483194 0.000000152193816 0.000000166340575 0.000000223306734 0.000000276679812 0.000000307554112 0.000000355249943 ; 0.000000000000000 0.000000011282931 0.000000033150108 0.000000184507131 0.000000229035462 0.000000291942887 0.000000329931736 0.000000334322700 0.000000644925581 0.000001020326627 0.000001824906973 0.000002557653927 NaN NaN NaN NaN NaN NaN NaN NaN ; 0.000000000000000 0.000000065953487 0.000000143713687 0.000000211062401 0.000000272938117 0.000000400962752 0.000000532296204 0.000000535637797 0.000000703406533 0.000000881596862 0.000000900409033 0.000001619933754 NaN NaN NaN NaN NaN NaN NaN NaN ";
                    itpp::mat sig = "13.047381561100972 34.117750537811077 0.000000024130018 6.050663431815087 13.099981062114759 ; 32.512863306074969 86.597578871324743 0.000000460662472 1.373989087284578 1.000000000000000 ; 51.469457265724515 112.511150229859440 0.000000474132218 2.045887315139527 1.000000000000000 ; 11.125835544082017 57.973456442476767 0.000000076911417 2.758080689139260 1.000000000000000 ; 19.108673930390221 73.274431395445660 0.000000187160804 0.750256569906956 6.386806652609405 ; 13.963125007578475 98.762583701895323 0.000000381118549 0.235245490630568 4.996732828966445 ";
                    itpp::mat ksi = "-5.144884951539957 -3.610380097435979 -0.112792075798780 1.231918974574412 -1.929094462894009 -0.069898281871550 2.210242407681651 2.517881594440842 0.104839661022872 3.799924892008383 -2.728907900137383 -1.092894541807784 NaN NaN NaN NaN NaN NaN NaN NaN; -2.579591194052494 2.093051530496839 1.855500259090089 4.280672018671990 -0.913304523638747 -4.205368601269817 2.685417159555680 0.938262547396789 7.881200973526603 1.294013144378520 3.002909423648561 -0.392052803056145 2.416857859642685 -2.920492667292119 -1.416468888708018 5.557735073177422 -0.834442442535630 -0.722866938570470 -1.827101289960941 0.583272746249657 ; 3.968714898307323 -4.082338151339979 2.061511556605220 -2.753238856835556 6.161381504174662 0.667378602331702 1.559158927681262 -0.175805296656394 1.582073264686634 1.591854414952712 0.514592753977270 0.509665201916961 2.929539472494383 -4.126195045725249 0.963699534664188 -3.045610219975115 2.062857202633463 4.249348576951284 -2.873307952240113 2.414042532253713 ; 2.289160324875151 -3.973287623492209 0.413401259714413 -1.877895610268665 4.524426924582452 3.223001424868312 1.207462782557955 -0.493609641632443 0.439427960920674 -7.509245818288738 1.059446902438144 4.452674444734305 -3.321013899528525 -3.578649251605719 0.618765734421609 -0.585055299788332 -1.529993906155297 2.181608186799184 4.925852015483405 1.957405563240580 ; -0.848222526764969 -7.832335495530182 -6.419152801361248 1.937051824846790 -2.129102780491428 1.537644646354515 0.188057153346209 -3.137725283163382 1.403184381236876 -1.517779678748303 -3.987085586087101 -3.428166592335177 NaN NaN NaN NaN NaN NaN NaN NaN ; -6.468014893629519 5.911904854927134 1.112811253432636 2.339127400318168 -4.181428223609784 0.219880714189357 2.356088613983290 1.924714979891760 0.884466419811376 -5.426599389807077 0.301996440105811 2.049097211552017 NaN NaN NaN NaN NaN NaN NaN NaN;";
                    itpp::mat power = "0.957045317082030 0.019475769928372 0.005714853299034 0.003987309168480 0.006870928149860 0.003444215088311 0.001667509263322 0.000720253165615 0.000807088655933 0.000128251949229 0.000132005870874 0.000006498378940 NaN NaN NaN NaN NaN NaN NaN NaN ; 0.181344060455596 0.056265529537089 0.054499435106350 0.028715953629331 0.089434929070765 0.184537220233573 0.034207981487039 0.048950120140733 0.009224634880856 0.039813614430051 0.026694181195553 0.046975098974322 0.023692875500571 0.078709879809536 0.041625916158329 0.007699505824342 0.022264488507889 0.017472927824351 0.006808886601119 0.001062760632604 ; 0.053573456212655 0.313631677936958 0.062190892770347 0.151664828964372 0.019153441793638 0.065676718746846 0.047863015784078 0.053767624580051 0.032547358773497 0.031904240327365 0.039486202790195 0.025992566453052 0.009106467975491 0.037214832764483 0.010662600006726 0.024752900947439 0.005255447002337 0.002926009261267 0.010900326865953 0.001729390043252 ; 0.047510982258108 0.153317013802448 0.049488761068307 0.082208262617951 0.018102298396070 0.023602110970047 0.037269264960750 0.052511422711273 0.037921837088397 0.231891086612813 0.029430400499362 0.012142664690480 0.069319536355781 0.070327352886975 0.022808202457565 0.027122037379233 0.022182104285129 0.006375243841959 0.002701057770552 0.003768359346803 ; 0.876709413412668 0.058207887773433 0.039193924610880 0.003522652551387 0.007789152572829 0.002736729754238 0.003305980365467 0.007010760747726 0.000910403462676 0.000535394701324 0.000071683436721 0.000006016610651 NaN NaN NaN NaN NaN NaN NaN NaN ; 0.908282433641648 0.003910271131291 0.010446008417583 0.007083883193622 0.028841747040280 0.008557723147109 0.004255422789041 0.004675148167024 0.004561567826978 0.014736325785509 0.003825317234959 0.000824151624957 NaN NaN NaN NaN NaN NaN NaN NaN ";

                    unsigned int MaxClusters = power.cols();
                    imtaphy::scm::m2135::ClusterPowers* cPowers;
                    cPowers = new imtaphy::scm::m2135::ClusterPowers(linkManager, MaxClusters);

                    cPowers->initforTest(ksi);
                    
                    cPowers->computeClusterPowers(sig, &tau_n);

                    for (int k = 0; k < cPowers->getScaledClusterPowers()->rows(); k++)
                        for (int n = 0; n < cPowers->getScaledClusterPowers()->cols(); n++)
                        {
                            if( !std::isnan(power(k,n)) )
                                CPPUNIT_ASSERT_DOUBLES_EQUAL(power(k,n), (*(cPowers->getScaledClusterPowers()))(k,n), 1e-4);
                        }
                }


        
            }}}}
