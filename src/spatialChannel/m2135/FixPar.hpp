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

#ifndef SPATIALCHANNEL_FIXPAR_HPP
#define SPATIALCHANNEL_FIXPAR_HPP

// Here, fixpar is made scenario and PropagCondition dependent
// this obviates the need for making a separate iterpar for each condition separately


#include <itpp/itbase.h>
#include <WNS/Singleton.hpp>
#include <IMTAPHY/Link.hpp>

namespace imtaphy { namespace scm { namespace m2135 {

            struct Parameters{
                unsigned int NumClusters;
                double r_tau; // delay scaling parameter
                unsigned int Cluster_ASD;
                unsigned int Cluster_ASA;
                double LNS_ksi; // per cluster shadowing ksi
                double ASD_DS;
                double ASA_DS;
                double ASA_SF;
                double ASD_SF;
                double DS_SF;
                double ASD_ASA;
                double ASD_K;
                double ASA_K;
                double DS_K;
                double SF_K;
                double XPR_mu;
                double XPR_sigma;
                double DS_mu;
                double DS_sigma;
                double ASD_mu;
                double ASD_sigma;
                double ASA_mu;
                double ASA_sigma;
                double SF_sigma;
                double KF_mu;
                double KF_sigma;
                double CD_DS;
                double CD_ASD;
                double CD_ASA;
                double CD_SF;
                double CD_K;
                itpp::mat R_sqrt;
            };
            class FixPar {
            private:
                Parameters fixTable[5][3];
                
            public:
                
                FixPar()
                {
                    fixTable[Link::InH][Link::LoS].NumClusters = 15;
                    fixTable[Link::InH][Link::LoS].r_tau = 3.600000;
                    fixTable[Link::InH][Link::LoS].Cluster_ASD = 5;
                    fixTable[Link::InH][Link::LoS].Cluster_ASA = 8;
                    fixTable[Link::InH][Link::LoS].LNS_ksi = 6.000000;
                    fixTable[Link::InH][Link::LoS].ASD_DS = 0.600000;
                    fixTable[Link::InH][Link::LoS].ASA_DS = 0.800000;
                    fixTable[Link::InH][Link::LoS].ASA_SF = -0.500000;
                    fixTable[Link::InH][Link::LoS].ASD_SF = -0.400000;
                    fixTable[Link::InH][Link::LoS].DS_SF = -0.800000;
                    fixTable[Link::InH][Link::LoS].ASD_ASA = 0.400000;
                    fixTable[Link::InH][Link::LoS].ASD_K = 0.000000;
                    fixTable[Link::InH][Link::LoS].ASA_K = 0.000000;
                    fixTable[Link::InH][Link::LoS].DS_K = -0.500000;
                    fixTable[Link::InH][Link::LoS].SF_K = 0.500000;
                    fixTable[Link::InH][Link::LoS].XPR_mu = 11.000000;
                    fixTable[Link::InH][Link::LoS].XPR_sigma = 0.000000;  //Check this
                    fixTable[Link::InH][Link::LoS].DS_mu = -7.700000;
                    fixTable[Link::InH][Link::LoS].DS_sigma = 0.180000;
                    fixTable[Link::InH][Link::LoS].ASD_mu = 1.600000;
                    fixTable[Link::InH][Link::LoS].ASD_sigma = 0.180000;
                    fixTable[Link::InH][Link::LoS].ASA_mu = 1.620000;
                    fixTable[Link::InH][Link::LoS].ASA_sigma = 0.220000;
                    fixTable[Link::InH][Link::LoS].SF_sigma = 3.000000;
                    fixTable[Link::InH][Link::LoS].KF_mu = 7.000000;
                    fixTable[Link::InH][Link::LoS].KF_sigma = 4.000000;
                    fixTable[Link::InH][Link::LoS].CD_DS = 8.000000;
                    fixTable[Link::InH][Link::LoS].CD_ASD = 7.000000;
                    fixTable[Link::InH][Link::LoS].CD_ASA = 5.000000;
                    fixTable[Link::InH][Link::LoS].CD_SF = 10.000000;
                    fixTable[Link::InH][Link::LoS].CD_K = 4.000000;
                    fixTable[Link::InH][Link::LoS].R_sqrt.set_size(5,5);
                    fixTable[Link::InH][Link::LoS].R_sqrt(0,0) = 0.621223718347157; fixTable[Link::InH][Link::LoS].R_sqrt(0,1) = 0.327023628702021; fixTable[Link::InH][Link::LoS].R_sqrt(0,2) = 0.498450068093346; fixTable[Link::InH][Link::LoS].R_sqrt(0,3) = -0.397383499536540; fixTable[Link::InH][Link::LoS].R_sqrt(0,4) = -0.317443730363866; 
                    fixTable[Link::InH][Link::LoS].R_sqrt(1,0) = 0.327023628702021; fixTable[Link::InH][Link::LoS].R_sqrt(1,1) = 0.923620836374880; fixTable[Link::InH][Link::LoS].R_sqrt(1,2) = 0.115165693298854; fixTable[Link::InH][Link::LoS].R_sqrt(1,3) = -0.148559279659243; fixTable[Link::InH][Link::LoS].R_sqrt(1,4) = 0.068169644262314; 
                    fixTable[Link::InH][Link::LoS].R_sqrt(2,0) = 0.498450068093346; fixTable[Link::InH][Link::LoS].R_sqrt(2,1) = 0.115165693298854; fixTable[Link::InH][Link::LoS].R_sqrt(2,2) = 0.832109040182439; fixTable[Link::InH][Link::LoS].R_sqrt(2,3) = -0.183583051030518; fixTable[Link::InH][Link::LoS].R_sqrt(2,4) = 0.110345826045748; 
                    fixTable[Link::InH][Link::LoS].R_sqrt(3,0) = -0.397383499536540; fixTable[Link::InH][Link::LoS].R_sqrt(3,1) = -0.148559279659243; fixTable[Link::InH][Link::LoS].R_sqrt(3,2) = -0.183583051030518; fixTable[Link::InH][Link::LoS].R_sqrt(3,3) = 0.856754854739268; fixTable[Link::InH][Link::LoS].R_sqrt(3,4) = 0.228658865951530; 
                    fixTable[Link::InH][Link::LoS].R_sqrt(4,0) = -0.317443730363866; fixTable[Link::InH][Link::LoS].R_sqrt(4,1) = 0.068169644262314; fixTable[Link::InH][Link::LoS].R_sqrt(4,2) = 0.110345826045748; fixTable[Link::InH][Link::LoS].R_sqrt(4,3) = 0.228658865951530; fixTable[Link::InH][Link::LoS].R_sqrt(4,4) = 0.911109927149224; 


                    fixTable[Link::InH][Link::NLoS].NumClusters = 19;
                    fixTable[Link::InH][Link::NLoS].r_tau = 3.000000;
                    fixTable[Link::InH][Link::NLoS].Cluster_ASD = 5;
                    fixTable[Link::InH][Link::NLoS].Cluster_ASA = 11;
                    fixTable[Link::InH][Link::NLoS].LNS_ksi = 3.000000;
                    fixTable[Link::InH][Link::NLoS].ASD_DS = 0.400000;
                    fixTable[Link::InH][Link::NLoS].ASA_DS = 0.000000;
                    fixTable[Link::InH][Link::NLoS].ASA_SF = -0.400000;
                    fixTable[Link::InH][Link::NLoS].ASD_SF = 0.000000;
                    fixTable[Link::InH][Link::NLoS].DS_SF = -0.500000;
                    fixTable[Link::InH][Link::NLoS].ASD_ASA = 0.000000;
                    fixTable[Link::InH][Link::NLoS].ASD_K = 0.000000;
                    fixTable[Link::InH][Link::NLoS].ASA_K = 0.000000;
                    fixTable[Link::InH][Link::NLoS].DS_K = 0.000000;
                    fixTable[Link::InH][Link::NLoS].SF_K = 0.000000;
                    fixTable[Link::InH][Link::NLoS].XPR_mu = 10.000000;
                    fixTable[Link::InH][Link::NLoS].XPR_sigma = 0.000000;
                    fixTable[Link::InH][Link::NLoS].DS_mu = -7.410000;
                    fixTable[Link::InH][Link::NLoS].DS_sigma = 0.140000;
                    fixTable[Link::InH][Link::NLoS].ASD_mu = 1.620000;
                    fixTable[Link::InH][Link::NLoS].ASD_sigma = 0.250000;
                    fixTable[Link::InH][Link::NLoS].ASA_mu = 1.770000;
                    fixTable[Link::InH][Link::NLoS].ASA_sigma = 0.160000;
                    fixTable[Link::InH][Link::NLoS].SF_sigma = 4.000000;
                    fixTable[Link::InH][Link::NLoS].KF_mu = 0.000000;
                    fixTable[Link::InH][Link::NLoS].KF_sigma = 0.000000;
                    fixTable[Link::InH][Link::NLoS].CD_DS = 5.000000;
                    fixTable[Link::InH][Link::NLoS].CD_ASD = 3.000000;
                    fixTable[Link::InH][Link::NLoS].CD_ASA = 3.000000;
                    fixTable[Link::InH][Link::NLoS].CD_SF = 6.000000;
                    fixTable[Link::InH][Link::NLoS].CD_K = -1000000;
                    fixTable[Link::InH][Link::NLoS].R_sqrt.set_size(5,5);
                    fixTable[Link::InH][Link::NLoS].R_sqrt(0,0) = 0.937210184455461; fixTable[Link::InH][Link::NLoS].R_sqrt(0,1) = 0.213628632168364; fixTable[Link::InH][Link::NLoS].R_sqrt(0,2) = -0.031342219949974; fixTable[Link::InH][Link::NLoS].R_sqrt(0,3) = -0.273893305722215; fixTable[Link::InH][Link::NLoS].R_sqrt(0,4) = 0.000000000000000; 
                    fixTable[Link::InH][Link::NLoS].R_sqrt(1,0) = 0.213628632168364; fixTable[Link::InH][Link::NLoS].R_sqrt(1,1) = 0.976387959392929; fixTable[Link::InH][Link::NLoS].R_sqrt(1,2) = 0.006857515511760; fixTable[Link::InH][Link::NLoS].R_sqrt(1,3) = 0.031342219949974; fixTable[Link::InH][Link::NLoS].R_sqrt(1,4) = 0.000000000000000; 
                    fixTable[Link::InH][Link::NLoS].R_sqrt(2,0) = -0.031342219949974; fixTable[Link::InH][Link::NLoS].R_sqrt(2,1) = 0.006857515511760; fixTable[Link::InH][Link::NLoS].R_sqrt(2,2) = 0.976387959392929; fixTable[Link::InH][Link::NLoS].R_sqrt(2,3) = -0.213628632168364; fixTable[Link::InH][Link::NLoS].R_sqrt(2,4) = 0.000000000000000; 
                    fixTable[Link::InH][Link::NLoS].R_sqrt(3,0) = -0.273893305722215; fixTable[Link::InH][Link::NLoS].R_sqrt(3,1) = 0.031342219949974; fixTable[Link::InH][Link::NLoS].R_sqrt(3,2) = -0.213628632168364; fixTable[Link::InH][Link::NLoS].R_sqrt(3,3) = 0.937210184455461; fixTable[Link::InH][Link::NLoS].R_sqrt(3,4) = 0.000000000000000; 
                    fixTable[Link::InH][Link::NLoS].R_sqrt(4,0) = 0.000000000000000; fixTable[Link::InH][Link::NLoS].R_sqrt(4,1) = 0.000000000000000; fixTable[Link::InH][Link::NLoS].R_sqrt(4,2) = 0.000000000000000; fixTable[Link::InH][Link::NLoS].R_sqrt(4,3) = 0.000000000000000; fixTable[Link::InH][Link::NLoS].R_sqrt(4,4) = 1.000000000000000; 





                    fixTable[Link::Link::UMi][Link::LoS].NumClusters = 12;
                    fixTable[Link::Link::UMi][Link::LoS].r_tau = 3.200000;
                    fixTable[Link::Link::UMi][Link::LoS].Cluster_ASD = 3;
                    fixTable[Link::Link::UMi][Link::LoS].Cluster_ASA = 17;
                    fixTable[Link::Link::UMi][Link::LoS].LNS_ksi = 3.000000;
                    fixTable[Link::Link::UMi][Link::LoS].ASD_DS = 0.500000;
                    fixTable[Link::Link::UMi][Link::LoS].ASA_DS = 0.800000;
                    fixTable[Link::Link::UMi][Link::LoS].ASA_SF = -0.400000;
                    fixTable[Link::Link::UMi][Link::LoS].ASD_SF = -0.500000;
                    fixTable[Link::Link::UMi][Link::LoS].DS_SF = -0.400000;
                    fixTable[Link::Link::UMi][Link::LoS].ASD_ASA = 0.400000;
                    fixTable[Link::Link::UMi][Link::LoS].ASD_K = -0.200000;
                    fixTable[Link::Link::UMi][Link::LoS].ASA_K = -0.300000;
                    fixTable[Link::Link::UMi][Link::LoS].DS_K = -0.700000;
                    fixTable[Link::Link::UMi][Link::LoS].SF_K = 0.500000;
                    fixTable[Link::Link::UMi][Link::LoS].XPR_mu = 9.000000;
                    fixTable[Link::Link::UMi][Link::LoS].XPR_sigma = 0.000000;
                    fixTable[Link::Link::UMi][Link::LoS].DS_mu = -7.190000;
                    fixTable[Link::Link::UMi][Link::LoS].DS_sigma = 0.400000;
                    fixTable[Link::Link::UMi][Link::LoS].ASD_mu = 1.200000;
                    fixTable[Link::Link::UMi][Link::LoS].ASD_sigma = 0.430000;
                    fixTable[Link::Link::UMi][Link::LoS].ASA_mu = 1.750000;
                    fixTable[Link::Link::UMi][Link::LoS].ASA_sigma = 0.190000;
                    fixTable[Link::Link::UMi][Link::LoS].SF_sigma = 3.000000;
                    fixTable[Link::Link::UMi][Link::LoS].KF_mu = 9.000000;
                    fixTable[Link::Link::UMi][Link::LoS].KF_sigma = 5.000000;
                    fixTable[Link::Link::UMi][Link::LoS].CD_DS = 7.000000;
                    fixTable[Link::Link::UMi][Link::LoS].CD_ASD = 8.000000;
                    fixTable[Link::Link::UMi][Link::LoS].CD_ASA = 8.000000;
                    fixTable[Link::Link::UMi][Link::LoS].CD_SF = 10.000000;
                    fixTable[Link::Link::UMi][Link::LoS].CD_K = 15.000000;
                    fixTable[Link::Link::UMi][Link::LoS].R_sqrt.set_size(5,5);
                    fixTable[Link::Link::UMi][Link::LoS].R_sqrt(0,0) = 0.753065949852805; fixTable[Link::Link::UMi][Link::LoS].R_sqrt(0,1) = 0.241023875447849; fixTable[Link::Link::UMi][Link::LoS].R_sqrt(0,2) = 0.454091158552085; fixTable[Link::UMi][Link::LoS].R_sqrt(0,3) = -0.097177920212919; fixTable[Link::UMi][Link::LoS].R_sqrt(0,4) = -0.398944655540474; 
                    fixTable[Link::UMi][Link::LoS].R_sqrt(1,0) = 0.241023875447849; fixTable[Link::UMi][Link::LoS].R_sqrt(1,1) = 0.929354051080551; fixTable[Link::UMi][Link::LoS].R_sqrt(1,2) = 0.137998056490968; fixTable[Link::UMi][Link::LoS].R_sqrt(1,3) = -0.242351266621617; fixTable[Link::UMi][Link::LoS].R_sqrt(1,4) = -0.020759074542993; 
                    fixTable[Link::UMi][Link::LoS].R_sqrt(2,0) = 0.454091158552085; fixTable[Link::UMi][Link::LoS].R_sqrt(2,1) = 0.137998056490968; fixTable[Link::UMi][Link::LoS].R_sqrt(2,2) = 0.861515602815865; fixTable[Link::UMi][Link::LoS].R_sqrt(2,3) = -0.175603398954279; fixTable[Link::UMi][Link::LoS].R_sqrt(2,4) = -0.041377149612582; 
                    fixTable[Link::UMi][Link::LoS].R_sqrt(3,0) = -0.097177920212919; fixTable[Link::UMi][Link::LoS].R_sqrt(3,1) = -0.242351266621617; fixTable[Link::UMi][Link::LoS].R_sqrt(3,2) = -0.175603398954279; fixTable[Link::UMi][Link::LoS].R_sqrt(3,3) = 0.915728740196338; fixTable[Link::UMi][Link::LoS].R_sqrt(3,4) = 0.249853229004786; 
                    fixTable[Link::UMi][Link::LoS].R_sqrt(4,0) = -0.398944655540474; fixTable[Link::UMi][Link::LoS].R_sqrt(4,1) = -0.020759074542993; fixTable[Link::UMi][Link::LoS].R_sqrt(4,2) = -0.041377149612582; fixTable[Link::UMi][Link::LoS].R_sqrt(4,3) = 0.249853229004786; fixTable[Link::UMi][Link::LoS].R_sqrt(4,4) = 0.881063855850205; 


                    fixTable[Link::UMi][Link::NLoS].NumClusters = 19;
                    fixTable[Link::UMi][Link::NLoS].r_tau = 3.000000;
                    fixTable[Link::UMi][Link::NLoS].Cluster_ASD = 10;
                    fixTable[Link::UMi][Link::NLoS].Cluster_ASA = 22;
                    fixTable[Link::UMi][Link::NLoS].LNS_ksi = 3.000000;
                    fixTable[Link::UMi][Link::NLoS].ASD_DS = 0.000000;
                    fixTable[Link::UMi][Link::NLoS].ASA_DS = 0.400000;
                    fixTable[Link::UMi][Link::NLoS].ASA_SF = -0.400000;
                    fixTable[Link::UMi][Link::NLoS].ASD_SF = 0.000000;
                    fixTable[Link::UMi][Link::NLoS].DS_SF = -0.700000;
                    fixTable[Link::UMi][Link::NLoS].ASD_ASA = 0.000000;
                    fixTable[Link::UMi][Link::NLoS].ASD_K = 0.000000;
                    fixTable[Link::UMi][Link::NLoS].ASA_K = 0.000000;
                    fixTable[Link::UMi][Link::NLoS].DS_K = 0.000000;
                    fixTable[Link::UMi][Link::NLoS].SF_K = 0.000000;
                    fixTable[Link::UMi][Link::NLoS].XPR_mu = 8.000000;
                    fixTable[Link::UMi][Link::NLoS].XPR_sigma = 0.000000;
                    fixTable[Link::UMi][Link::NLoS].DS_mu = -6.890000;
                    fixTable[Link::UMi][Link::NLoS].DS_sigma = 0.540000;
                    fixTable[Link::UMi][Link::NLoS].ASD_mu = 1.410000;
                    fixTable[Link::UMi][Link::NLoS].ASD_sigma = 0.170000;
                    fixTable[Link::UMi][Link::NLoS].ASA_mu = 1.840000;
                    fixTable[Link::UMi][Link::NLoS].ASA_sigma = 0.150000;
                    fixTable[Link::UMi][Link::NLoS].SF_sigma = 4.000000;
                    fixTable[Link::UMi][Link::NLoS].KF_mu = 0.000000;
                    fixTable[Link::UMi][Link::NLoS].KF_sigma = 0.000000;
                    fixTable[Link::UMi][Link::NLoS].CD_DS = 10.000000;
                    fixTable[Link::UMi][Link::NLoS].CD_ASD = 10.000000;
                    fixTable[Link::UMi][Link::NLoS].CD_ASA = 9.000000;
                    fixTable[Link::UMi][Link::NLoS].CD_SF = 13.000000;
                    fixTable[Link::UMi][Link::NLoS].CD_K = -1000000;
                    fixTable[Link::UMi][Link::NLoS].R_sqrt.set_size(5,5);
                    fixTable[Link::UMi][Link::NLoS].R_sqrt(0,0) = 0.913514893522226; fixTable[Link::UMi][Link::NLoS].R_sqrt(0,1) = -0.000000000000000; fixTable[Link::UMi][Link::NLoS].R_sqrt(0,2) = 0.178007039816570; fixTable[Link::UMi][Link::NLoS].R_sqrt(0,3) = -0.365792336017060; fixTable[Link::UMi][Link::NLoS].R_sqrt(0,4) = 0.000000000000000; 
                    fixTable[Link::UMi][Link::NLoS].R_sqrt(1,0) = -0.000000000000000; fixTable[Link::UMi][Link::NLoS].R_sqrt(1,1) = 1.000000000000000; fixTable[Link::UMi][Link::NLoS].R_sqrt(1,2) = 0.000000000000000; fixTable[Link::UMi][Link::NLoS].R_sqrt(1,3) = -0.000000000000000; fixTable[Link::UMi][Link::NLoS].R_sqrt(1,4) = 0.000000000000000; 
                    fixTable[Link::UMi][Link::NLoS].R_sqrt(2,0) = 0.178007039816570; fixTable[Link::UMi][Link::NLoS].R_sqrt(2,1) = 0.000000000000000; fixTable[Link::UMi][Link::NLoS].R_sqrt(2,2) = 0.967794909860288; fixTable[Link::UMi][Link::NLoS].R_sqrt(2,3) = -0.178007039816570; fixTable[Link::UMi][Link::NLoS].R_sqrt(2,4) = 0.000000000000000; 
                    fixTable[Link::UMi][Link::NLoS].R_sqrt(3,0) = -0.365792336017060; fixTable[Link::UMi][Link::NLoS].R_sqrt(3,1) = -0.000000000000000; fixTable[Link::UMi][Link::NLoS].R_sqrt(3,2) = -0.178007039816570; fixTable[Link::UMi][Link::NLoS].R_sqrt(3,3) = 0.913514893522227; fixTable[Link::UMi][Link::NLoS].R_sqrt(3,4) = 0.000000000000000; 
                    fixTable[Link::UMi][Link::NLoS].R_sqrt(4,0) = 0.000000000000000; fixTable[Link::UMi][Link::NLoS].R_sqrt(4,1) = 0.000000000000000; fixTable[Link::UMi][Link::NLoS].R_sqrt(4,2) = 0.000000000000000; fixTable[Link::UMi][Link::NLoS].R_sqrt(4,3) = 0.000000000000000; fixTable[Link::UMi][Link::NLoS].R_sqrt(4,4) = 1.000000000000000; 





                    fixTable[Link::SMa][Link::LoS].NumClusters = 15;
                    fixTable[Link::SMa][Link::LoS].r_tau = 2.400000;
                    fixTable[Link::SMa][Link::LoS].Cluster_ASD = 5;
                    fixTable[Link::SMa][Link::LoS].Cluster_ASA = 5;
                    fixTable[Link::SMa][Link::LoS].LNS_ksi = 3.000000;
                    fixTable[Link::SMa][Link::LoS].ASD_DS = 0.000000;
                    fixTable[Link::SMa][Link::LoS].ASA_DS = 0.800000;
                    fixTable[Link::SMa][Link::LoS].ASA_SF = -0.500000;
                    fixTable[Link::SMa][Link::LoS].ASD_SF = -0.500000;
                    fixTable[Link::SMa][Link::LoS].DS_SF = -0.600000;
                    fixTable[Link::SMa][Link::LoS].ASD_ASA = 0.000000;
                    fixTable[Link::SMa][Link::LoS].ASD_K = 0.000000;
                    fixTable[Link::SMa][Link::LoS].ASA_K = 0.000000;
                    fixTable[Link::SMa][Link::LoS].DS_K = 0.000000;
                    fixTable[Link::SMa][Link::LoS].SF_K = 0.000000;
                    fixTable[Link::SMa][Link::LoS].XPR_mu = 8.000000;
                    fixTable[Link::SMa][Link::LoS].XPR_sigma = 0.000000;
                    fixTable[Link::SMa][Link::LoS].DS_mu = -7.230000;
                    fixTable[Link::SMa][Link::LoS].DS_sigma = 0.380000;
                    fixTable[Link::SMa][Link::LoS].ASD_mu = 0.780000;
                    fixTable[Link::SMa][Link::LoS].ASD_sigma = 0.120000;
                    fixTable[Link::SMa][Link::LoS].ASA_mu = 1.480000;
                    fixTable[Link::SMa][Link::LoS].ASA_sigma = 0.200000;
                    fixTable[Link::SMa][Link::LoS].SF_sigma = 4.000000;
                    fixTable[Link::SMa][Link::LoS].KF_mu = 9.000000;
                    fixTable[Link::SMa][Link::LoS].KF_sigma = 7.000000;
                    fixTable[Link::SMa][Link::LoS].CD_DS = 6.000000;
                    fixTable[Link::SMa][Link::LoS].CD_ASD = 15.000000;
                    fixTable[Link::SMa][Link::LoS].CD_ASA = 20.000000;
                    fixTable[Link::SMa][Link::LoS].CD_SF = 40.000000;
                    fixTable[Link::SMa][Link::LoS].CD_K = 10.000000;
                    fixTable[Link::SMa][Link::LoS].R_sqrt.set_size(5,5);
                    fixTable[Link::SMa][Link::LoS].R_sqrt(0,0) = 0.854017192528817; fixTable[Link::SMa][Link::LoS].R_sqrt(0,1) = -0.040746823503881; fixTable[Link::SMa][Link::LoS].R_sqrt(0,2) = 0.423846524961968; fixTable[Link::SMa][Link::LoS].R_sqrt(0,3) = -0.298912118384658; fixTable[Link::SMa][Link::LoS].R_sqrt(0,4) = 0.000000000000000; 
                    fixTable[Link::SMa][Link::LoS].R_sqrt(1,0) = -0.040746823503881; fixTable[Link::SMa][Link::LoS].R_sqrt(1,1) = 0.958761974312788; fixTable[Link::SMa][Link::LoS].R_sqrt(1,2) = -0.023404035164252; fixTable[Link::SMa][Link::LoS].R_sqrt(1,3) = -0.280298812206218; fixTable[Link::SMa][Link::LoS].R_sqrt(1,4) = 0.000000000000000; 
                    fixTable[Link::SMa][Link::LoS].R_sqrt(2,0) = 0.423846524961968; fixTable[Link::SMa][Link::LoS].R_sqrt(2,1) = -0.023404035164252; fixTable[Link::SMa][Link::LoS].R_sqrt(2,2) = 0.879509345569223; fixTable[Link::SMa][Link::LoS].R_sqrt(2,3) = -0.215103894600008; fixTable[Link::SMa][Link::LoS].R_sqrt(2,4) = 0.000000000000000; 
                    fixTable[Link::SMa][Link::LoS].R_sqrt(3,0) = -0.298912118384658; fixTable[Link::SMa][Link::LoS].R_sqrt(3,1) = -0.280298812206218; fixTable[Link::SMa][Link::LoS].R_sqrt(3,2) = -0.215103894600008; fixTable[Link::SMa][Link::LoS].R_sqrt(3,3) = 0.886461750943879; fixTable[Link::SMa][Link::LoS].R_sqrt(3,4) = 0.000000000000000; 
                    fixTable[Link::SMa][Link::LoS].R_sqrt(4,0) = 0.000000000000000; fixTable[Link::SMa][Link::LoS].R_sqrt(4,1) = 0.000000000000000; fixTable[Link::SMa][Link::LoS].R_sqrt(4,2) = 0.000000000000000; fixTable[Link::SMa][Link::LoS].R_sqrt(4,3) = 0.000000000000000; fixTable[Link::SMa][Link::LoS].R_sqrt(4,4) = 1.000000000000000; 


                    fixTable[Link::SMa][Link::NLoS].NumClusters = 14;
                    fixTable[Link::SMa][Link::NLoS].r_tau = 1.500000;
                    fixTable[Link::SMa][Link::NLoS].Cluster_ASD = 2;
                    fixTable[Link::SMa][Link::NLoS].Cluster_ASA = 10;
                    fixTable[Link::SMa][Link::NLoS].LNS_ksi = 3.000000;
                    fixTable[Link::SMa][Link::NLoS].ASD_DS = 0.000000;
                    fixTable[Link::SMa][Link::NLoS].ASA_DS = 0.700000;
                    fixTable[Link::SMa][Link::NLoS].ASA_SF = 0.000000;
                    fixTable[Link::SMa][Link::NLoS].ASD_SF = -0.400000;
                    fixTable[Link::SMa][Link::NLoS].DS_SF = -0.400000;
                    fixTable[Link::SMa][Link::NLoS].ASD_ASA = 0.000000;
                    fixTable[Link::SMa][Link::NLoS].ASD_K = 0.000000;
                    fixTable[Link::SMa][Link::NLoS].ASA_K = 0.000000;
                    fixTable[Link::SMa][Link::NLoS].DS_K = 0.000000;
                    fixTable[Link::SMa][Link::NLoS].SF_K = 0.000000;
                    fixTable[Link::SMa][Link::NLoS].XPR_mu = 4.000000;
                    fixTable[Link::SMa][Link::NLoS].XPR_sigma = 0.000000;
                    fixTable[Link::SMa][Link::NLoS].DS_mu = -7.120000;
                    fixTable[Link::SMa][Link::NLoS].DS_sigma = 0.330000;
                    fixTable[Link::SMa][Link::NLoS].ASD_mu = 0.900000;
                    fixTable[Link::SMa][Link::NLoS].ASD_sigma = 0.360000;
                    fixTable[Link::SMa][Link::NLoS].ASA_mu = 1.650000;
                    fixTable[Link::SMa][Link::NLoS].ASA_sigma = 0.250000;
                    fixTable[Link::SMa][Link::NLoS].SF_sigma = 8.000000;
                    fixTable[Link::SMa][Link::NLoS].KF_mu = 0.000000;
                    fixTable[Link::SMa][Link::NLoS].KF_sigma = 0.000000;
                    fixTable[Link::SMa][Link::NLoS].CD_DS = 40.000000;
                    fixTable[Link::SMa][Link::NLoS].CD_ASD = 30.000000;
                    fixTable[Link::SMa][Link::NLoS].CD_ASA = 30.000000;
                    fixTable[Link::SMa][Link::NLoS].CD_SF = 50.000000;
                    fixTable[Link::SMa][Link::NLoS].CD_K = -1000000;
                    fixTable[Link::SMa][Link::NLoS].R_sqrt.set_size(5,5);
                    fixTable[Link::SMa][Link::NLoS].R_sqrt(0,0) = 0.888863320019977; fixTable[Link::SMa][Link::NLoS].R_sqrt(0,1) = -0.028738390905236; fixTable[Link::SMa][Link::NLoS].R_sqrt(0,2) = 0.394136210972922; fixTable[Link::SMa][Link::NLoS].R_sqrt(0,3) = -0.231846394000744; fixTable[Link::SMa][Link::NLoS].R_sqrt(0,4) = 0.000000000000000; 
                    fixTable[Link::SMa][Link::NLoS].R_sqrt(1,0) = -0.028738390905236; fixTable[Link::SMa][Link::NLoS].R_sqrt(1,1) = 0.976874642167261; fixTable[Link::SMa][Link::NLoS].R_sqrt(1,2) = 0.011594978528380; fixTable[Link::SMa][Link::NLoS].R_sqrt(1,3) = -0.211555181576079; fixTable[Link::SMa][Link::NLoS].R_sqrt(1,4) = 0.000000000000000; 
                    fixTable[Link::SMa][Link::NLoS].R_sqrt(2,0) = 0.394136210972922; fixTable[Link::SMa][Link::NLoS].R_sqrt(2,1) = 0.011594978528380; fixTable[Link::SMa][Link::NLoS].R_sqrt(2,2) = 0.917601710925213; fixTable[Link::SMa][Link::NLoS].R_sqrt(2,3) = 0.050292184084163; fixTable[Link::SMa][Link::NLoS].R_sqrt(2,4) = 0.000000000000000; 
                    fixTable[Link::SMa][Link::NLoS].R_sqrt(3,0) = -0.231846394000744; fixTable[Link::SMa][Link::NLoS].R_sqrt(3,1) = -0.211555181576079; fixTable[Link::SMa][Link::NLoS].R_sqrt(3,2) = 0.050292184084163; fixTable[Link::SMa][Link::NLoS].R_sqrt(3,3) = 0.948136251262026; fixTable[Link::SMa][Link::NLoS].R_sqrt(3,4) = 0.000000000000000; 
                    fixTable[Link::SMa][Link::NLoS].R_sqrt(4,0) = 0.000000000000000; fixTable[Link::SMa][Link::NLoS].R_sqrt(4,1) = 0.000000000000000; fixTable[Link::SMa][Link::NLoS].R_sqrt(4,2) = 0.000000000000000; fixTable[Link::SMa][Link::NLoS].R_sqrt(4,3) = 0.000000000000000; fixTable[Link::SMa][Link::NLoS].R_sqrt(4,4) = 1.000000000000000; 





                    fixTable[Link::UMa][Link::LoS].NumClusters = 12;
                    fixTable[Link::UMa][Link::LoS].r_tau = 2.500000;
                    fixTable[Link::UMa][Link::LoS].Cluster_ASD = 5;
                    fixTable[Link::UMa][Link::LoS].Cluster_ASA = 11;
                    fixTable[Link::UMa][Link::LoS].LNS_ksi = 3.000000;
                    fixTable[Link::UMa][Link::LoS].ASD_DS = 0.400000;
                    fixTable[Link::UMa][Link::LoS].ASA_DS = 0.800000;
                    fixTable[Link::UMa][Link::LoS].ASA_SF = -0.500000;
                    fixTable[Link::UMa][Link::LoS].ASD_SF = -0.500000;
                    fixTable[Link::UMa][Link::LoS].DS_SF = -0.400000;
                    fixTable[Link::UMa][Link::LoS].ASD_ASA = 0.000000;
                    fixTable[Link::UMa][Link::LoS].ASD_K = 0.000000;
                    fixTable[Link::UMa][Link::LoS].ASA_K = -0.200000;
                    fixTable[Link::UMa][Link::LoS].DS_K = -0.400000;
                    fixTable[Link::UMa][Link::LoS].SF_K = 0.000000;
                    fixTable[Link::UMa][Link::LoS].XPR_mu = 8.000000;
                    fixTable[Link::UMa][Link::LoS].XPR_sigma = 0.000000;
                    fixTable[Link::UMa][Link::LoS].DS_mu = -7.030000;
                    fixTable[Link::UMa][Link::LoS].DS_sigma = 0.660000;
                    fixTable[Link::UMa][Link::LoS].ASD_mu = 1.150000;
                    fixTable[Link::UMa][Link::LoS].ASD_sigma = 0.280000;
                    fixTable[Link::UMa][Link::LoS].ASA_mu = 1.810000;
                    fixTable[Link::UMa][Link::LoS].ASA_sigma = 0.200000;
                    fixTable[Link::UMa][Link::LoS].SF_sigma = 4.000000;
                    fixTable[Link::UMa][Link::LoS].KF_mu = 9.000000;
                    fixTable[Link::UMa][Link::LoS].KF_sigma = 3.500000;
                    fixTable[Link::UMa][Link::LoS].CD_DS = 30.000000;
                    fixTable[Link::UMa][Link::LoS].CD_ASD = 18.000000;
                    fixTable[Link::UMa][Link::LoS].CD_ASA = 15.000000;
                    fixTable[Link::UMa][Link::LoS].CD_SF = 37.000000;
                    fixTable[Link::UMa][Link::LoS].CD_K = 12.000000;
                    fixTable[Link::UMa][Link::LoS].R_sqrt.set_size(5,5);
                    fixTable[Link::UMa][Link::LoS].R_sqrt(0,0) = 0.806310951408682; fixTable[Link::UMa][Link::LoS].R_sqrt(0,1) = 0.245016774934418; fixTable[Link::UMa][Link::LoS].R_sqrt(0,2) = 0.479171304494613; fixTable[Link::UMa][Link::LoS].R_sqrt(0,3) = -0.120392914754037; fixTable[Link::UMa][Link::LoS].R_sqrt(0,4) = -0.213845356893992; 
                    fixTable[Link::UMa][Link::LoS].R_sqrt(1,0) = 0.245016774934418; fixTable[Link::UMa][Link::LoS].R_sqrt(1,1) = 0.924083471541761; fixTable[Link::UMa][Link::LoS].R_sqrt(1,2) = -0.108566442433108; fixTable[Link::UMa][Link::LoS].R_sqrt(1,3) = -0.271617534928914; fixTable[Link::UMa][Link::LoS].R_sqrt(1,4) = 0.021766026753229; 
                    fixTable[Link::UMa][Link::LoS].R_sqrt(2,0) = 0.479171304494613; fixTable[Link::UMa][Link::LoS].R_sqrt(2,1) = -0.108566442433108; fixTable[Link::UMa][Link::LoS].R_sqrt(2,2) = 0.825678402680475; fixTable[Link::UMa][Link::LoS].R_sqrt(2,3) = -0.271600920527001; fixTable[Link::UMa][Link::LoS].R_sqrt(2,4) = -0.055644441252066; 
                    fixTable[Link::UMa][Link::LoS].R_sqrt(3,0) = -0.120392914754037; fixTable[Link::UMa][Link::LoS].R_sqrt(3,1) = -0.271617534928914; fixTable[Link::UMa][Link::LoS].R_sqrt(3,2) = -0.271600920527001; fixTable[Link::UMa][Link::LoS].R_sqrt(3,3) = 0.915216117252957; fixTable[Link::UMa][Link::LoS].R_sqrt(3,4) = -0.018489442540902; 
                    fixTable[Link::UMa][Link::LoS].R_sqrt(4,0) = -0.213845356893992; fixTable[Link::UMa][Link::LoS].R_sqrt(4,1) = 0.021766026753229; fixTable[Link::UMa][Link::LoS].R_sqrt(4,2) = -0.055644441252066; fixTable[Link::UMa][Link::LoS].R_sqrt(4,3) = -0.018489442540902; fixTable[Link::UMa][Link::LoS].R_sqrt(4,4) = 0.974863190445988; 


                    fixTable[Link::UMa][Link::NLoS].NumClusters = 20;
                    fixTable[Link::UMa][Link::NLoS].r_tau = 2.300000;
                    fixTable[Link::UMa][Link::NLoS].Cluster_ASD = 2;
                    fixTable[Link::UMa][Link::NLoS].Cluster_ASA = 15;
                    fixTable[Link::UMa][Link::NLoS].LNS_ksi = 3.000000;
                    fixTable[Link::UMa][Link::NLoS].ASD_DS = 0.400000;
                    fixTable[Link::UMa][Link::NLoS].ASA_DS = 0.600000;
                    fixTable[Link::UMa][Link::NLoS].ASA_SF = 0.000000;
                    fixTable[Link::UMa][Link::NLoS].ASD_SF = -0.600000;
                    fixTable[Link::UMa][Link::NLoS].DS_SF = -0.400000;
                    fixTable[Link::UMa][Link::NLoS].ASD_ASA = 0.400000;
                    fixTable[Link::UMa][Link::NLoS].ASD_K = 0.000000;
                    fixTable[Link::UMa][Link::NLoS].ASA_K = 0.000000;
                    fixTable[Link::UMa][Link::NLoS].DS_K = 0.000000;
                    fixTable[Link::UMa][Link::NLoS].SF_K = 0.000000;
                    fixTable[Link::UMa][Link::NLoS].XPR_mu = 7.000000;
                    fixTable[Link::UMa][Link::NLoS].XPR_sigma = 0.000000;
                    fixTable[Link::UMa][Link::NLoS].DS_mu = -6.440000;
                    fixTable[Link::UMa][Link::NLoS].DS_sigma = 0.390000;
                    fixTable[Link::UMa][Link::NLoS].ASD_mu = 1.410000;
                    fixTable[Link::UMa][Link::NLoS].ASD_sigma = 0.280000;
                    fixTable[Link::UMa][Link::NLoS].ASA_mu = 1.870000;
                    fixTable[Link::UMa][Link::NLoS].ASA_sigma = 0.110000;
                    fixTable[Link::UMa][Link::NLoS].SF_sigma = 6.000000;
                    fixTable[Link::UMa][Link::NLoS].KF_mu = 0.000000;
                    fixTable[Link::UMa][Link::NLoS].KF_sigma = 0.000000;
                    fixTable[Link::UMa][Link::NLoS].CD_DS = 40.000000;
                    fixTable[Link::UMa][Link::NLoS].CD_ASD = 50.000000;
                    fixTable[Link::UMa][Link::NLoS].CD_ASA = 50.000000;
                    fixTable[Link::UMa][Link::NLoS].CD_SF = 50.000000;
                    fixTable[Link::UMa][Link::NLoS].CD_K = -100000000;
                    fixTable[Link::UMa][Link::NLoS].R_sqrt.set_size(5,5);
                    fixTable[Link::UMa][Link::NLoS].R_sqrt(0,0) = 0.913941405256431; fixTable[Link::UMa][Link::NLoS].R_sqrt(0,1) = 0.147728073775767; fixTable[Link::UMa][Link::NLoS].R_sqrt(0,2) = 0.318005795482914; fixTable[Link::UMa][Link::NLoS].R_sqrt(0,3) = -0.204352240055453; fixTable[Link::UMa][Link::NLoS].R_sqrt(0,4) = 0.000000000000000; 
                    fixTable[Link::UMa][Link::NLoS].R_sqrt(1,0) = 0.147728073775767; fixTable[Link::UMa][Link::NLoS].R_sqrt(1,1) = 0.913941405256431; fixTable[Link::UMa][Link::NLoS].R_sqrt(1,2) = 0.204352240055453; fixTable[Link::UMa][Link::NLoS].R_sqrt(1,3) = -0.318005795482914; fixTable[Link::UMa][Link::NLoS].R_sqrt(1,4) = 0.000000000000000; 
                    fixTable[Link::UMa][Link::NLoS].R_sqrt(2,0) = 0.318005795482914; fixTable[Link::UMa][Link::NLoS].R_sqrt(2,1) = 0.204352240055453; fixTable[Link::UMa][Link::NLoS].R_sqrt(2,2) = 0.923123353576218; fixTable[Link::UMa][Link::NLoS].R_sqrt(2,3) = 0.070397088759367; fixTable[Link::UMa][Link::NLoS].R_sqrt(2,4) = 0.000000000000000; 
                    fixTable[Link::UMa][Link::NLoS].R_sqrt(3,0) = -0.204352240055453; fixTable[Link::UMa][Link::NLoS].R_sqrt(3,1) = -0.318005795482914; fixTable[Link::UMa][Link::NLoS].R_sqrt(3,2) = 0.070397088759367; fixTable[Link::UMa][Link::NLoS].R_sqrt(3,3) = 0.923123353576218; fixTable[Link::UMa][Link::NLoS].R_sqrt(3,4) = 0.000000000000000; 
                    fixTable[Link::UMa][Link::NLoS].R_sqrt(4,0) = 0.000000000000000; fixTable[Link::UMa][Link::NLoS].R_sqrt(4,1) = 0.000000000000000; fixTable[Link::UMa][Link::NLoS].R_sqrt(4,2) = 0.000000000000000; fixTable[Link::UMa][Link::NLoS].R_sqrt(4,3) = 0.000000000000000; fixTable[Link::UMa][Link::NLoS].R_sqrt(4,4) = 1.000000000000000; 





                    fixTable[Link::RMa][Link::LoS].NumClusters = 11;
                    fixTable[Link::RMa][Link::LoS].r_tau = 3.800000;
                    fixTable[Link::RMa][Link::LoS].Cluster_ASD = 2;
                    fixTable[Link::RMa][Link::LoS].Cluster_ASA = 3;
                    fixTable[Link::RMa][Link::LoS].LNS_ksi = 3.000000;
                    fixTable[Link::RMa][Link::LoS].ASD_DS = 0.000000;
                    fixTable[Link::RMa][Link::LoS].ASA_DS = 0.000000;
                    fixTable[Link::RMa][Link::LoS].ASA_SF = 0.000000;
                    fixTable[Link::RMa][Link::LoS].ASD_SF = 0.000000;
                    fixTable[Link::RMa][Link::LoS].DS_SF = -0.500000;
                    fixTable[Link::RMa][Link::LoS].ASD_ASA = 0.000000;
                    fixTable[Link::RMa][Link::LoS].ASD_K = 0.000000;
                    fixTable[Link::RMa][Link::LoS].ASA_K = 0.000000;
                    fixTable[Link::RMa][Link::LoS].DS_K = 0.000000;
                    fixTable[Link::RMa][Link::LoS].SF_K = 0.000000;
                    fixTable[Link::RMa][Link::LoS].XPR_mu = 12.000000;
                    fixTable[Link::RMa][Link::LoS].XPR_sigma = 0.000000;
                    fixTable[Link::RMa][Link::LoS].DS_mu = -7.490000;
                    fixTable[Link::RMa][Link::LoS].DS_sigma = 0.550000;
                    fixTable[Link::RMa][Link::LoS].ASD_mu = 0.900000;
                    fixTable[Link::RMa][Link::LoS].ASD_sigma = 0.380000;
                    fixTable[Link::RMa][Link::LoS].ASA_mu = 1.520000;
                    fixTable[Link::RMa][Link::LoS].ASA_sigma = 0.240000;
                    fixTable[Link::RMa][Link::LoS].SF_sigma = 4.000000;
                    fixTable[Link::RMa][Link::LoS].KF_mu = 7.000000;
                    fixTable[Link::RMa][Link::LoS].KF_sigma = 4.000000;
                    fixTable[Link::RMa][Link::LoS].CD_DS = 50.000000;
                    fixTable[Link::RMa][Link::LoS].CD_ASD = 25.000000;
                    fixTable[Link::RMa][Link::LoS].CD_ASA = 35.000000;
                    fixTable[Link::RMa][Link::LoS].CD_SF = 37.000000;
                    fixTable[Link::RMa][Link::LoS].CD_K = 40.000000;
                    fixTable[Link::RMa][Link::LoS].R_sqrt.set_size(5,5);
                    fixTable[Link::RMa][Link::LoS].R_sqrt(0,0) = 0.965925826289068; fixTable[Link::RMa][Link::LoS].R_sqrt(0,1) = 0.000000000000000; fixTable[Link::RMa][Link::LoS].R_sqrt(0,2) = 0.000000000000000; fixTable[Link::RMa][Link::LoS].R_sqrt(0,3) = -0.258819045102521; fixTable[Link::RMa][Link::LoS].R_sqrt(0,4) = 0.000000000000000; 
                    fixTable[Link::RMa][Link::LoS].R_sqrt(1,0) = 0.000000000000000; fixTable[Link::RMa][Link::LoS].R_sqrt(1,1) = 1.000000000000000; fixTable[Link::RMa][Link::LoS].R_sqrt(1,2) = 0.000000000000000; fixTable[Link::RMa][Link::LoS].R_sqrt(1,3) = 0.000000000000000; fixTable[Link::RMa][Link::LoS].R_sqrt(1,4) = 0.000000000000000; 
                    fixTable[Link::RMa][Link::LoS].R_sqrt(2,0) = 0.000000000000000; fixTable[Link::RMa][Link::LoS].R_sqrt(2,1) = 0.000000000000000; fixTable[Link::RMa][Link::LoS].R_sqrt(2,2) = 1.000000000000000; fixTable[Link::RMa][Link::LoS].R_sqrt(2,3) = 0.000000000000000; fixTable[Link::RMa][Link::LoS].R_sqrt(2,4) = 0.000000000000000; 
                    fixTable[Link::RMa][Link::LoS].R_sqrt(3,0) = -0.258819045102521; fixTable[Link::RMa][Link::LoS].R_sqrt(3,1) = 0.000000000000000; fixTable[Link::RMa][Link::LoS].R_sqrt(3,2) = 0.000000000000000; fixTable[Link::RMa][Link::LoS].R_sqrt(3,3) = 0.965925826289068; fixTable[Link::RMa][Link::LoS].R_sqrt(3,4) = 0.000000000000000; 
                    fixTable[Link::RMa][Link::LoS].R_sqrt(4,0) = 0.000000000000000; fixTable[Link::RMa][Link::LoS].R_sqrt(4,1) = 0.000000000000000; fixTable[Link::RMa][Link::LoS].R_sqrt(4,2) = 0.000000000000000; fixTable[Link::RMa][Link::LoS].R_sqrt(4,3) = 0.000000000000000; fixTable[Link::RMa][Link::LoS].R_sqrt(4,4) = 1.000000000000000; 


                    fixTable[Link::RMa][Link::NLoS].NumClusters = 10;
                    fixTable[Link::RMa][Link::NLoS].r_tau = 1.700000;
                    fixTable[Link::RMa][Link::NLoS].Cluster_ASD = 2;
                    fixTable[Link::RMa][Link::NLoS].Cluster_ASA = 3;
                    fixTable[Link::RMa][Link::NLoS].LNS_ksi = 3.000000;
                    fixTable[Link::RMa][Link::NLoS].ASD_DS = -0.400000;
                    fixTable[Link::RMa][Link::NLoS].ASA_DS = 0.000000;
                    fixTable[Link::RMa][Link::NLoS].ASA_SF = 0.000000;
                    fixTable[Link::RMa][Link::NLoS].ASD_SF = 0.600000;
                    fixTable[Link::RMa][Link::NLoS].DS_SF = -0.500000;
                    fixTable[Link::RMa][Link::NLoS].ASD_ASA = 0.000000;
                    fixTable[Link::RMa][Link::NLoS].ASD_K = 0.000000;
                    fixTable[Link::RMa][Link::NLoS].ASA_K = 0.000000;
                    fixTable[Link::RMa][Link::NLoS].DS_K = 0.000000;
                    fixTable[Link::RMa][Link::NLoS].SF_K = 0.000000;
                    fixTable[Link::RMa][Link::NLoS].XPR_mu = 7.000000;
                    fixTable[Link::RMa][Link::NLoS].XPR_sigma = 0.000000;
                    fixTable[Link::RMa][Link::NLoS].DS_mu = -7.430000;
                    fixTable[Link::RMa][Link::NLoS].DS_sigma = 0.480000;
                    fixTable[Link::RMa][Link::NLoS].ASD_mu = 0.950000;
                    fixTable[Link::RMa][Link::NLoS].ASD_sigma = 0.450000;
                    fixTable[Link::RMa][Link::NLoS].ASA_mu = 1.520000;
                    fixTable[Link::RMa][Link::NLoS].ASA_sigma = 0.130000;
                    fixTable[Link::RMa][Link::NLoS].SF_sigma = 8.000000;
                    fixTable[Link::RMa][Link::NLoS].KF_mu = 0.000000;
                    fixTable[Link::RMa][Link::NLoS].KF_sigma = 0.000000;
                    fixTable[Link::RMa][Link::NLoS].CD_DS = 36.000000;
                    fixTable[Link::RMa][Link::NLoS].CD_ASD = 30.000000;
                    fixTable[Link::RMa][Link::NLoS].CD_ASA = 40.000000;
                    fixTable[Link::RMa][Link::NLoS].CD_SF = 120.000000;
                    fixTable[Link::RMa][Link::NLoS].CD_K = -1000000;
                    fixTable[Link::RMa][Link::NLoS].R_sqrt.set_size(5,5);
                    fixTable[Link::RMa][Link::NLoS].R_sqrt(0,0) = 0.955557150656242; fixTable[Link::RMa][Link::NLoS].R_sqrt(0,1) = -0.173466133506044; fixTable[Link::RMa][Link::NLoS].R_sqrt(0,2) = 0.000000000000000; fixTable[Link::RMa][Link::NLoS].R_sqrt(0,3) = -0.238369529001059; fixTable[Link::RMa][Link::NLoS].R_sqrt(0,4) = 0.000000000000000; 
                    fixTable[Link::RMa][Link::NLoS].R_sqrt(1,0) = -0.173466133506044; fixTable[Link::RMa][Link::NLoS].R_sqrt(1,1) = 0.938008596337461; fixTable[Link::RMa][Link::NLoS].R_sqrt(1,2) = -0.000000000000001; fixTable[Link::RMa][Link::NLoS].R_sqrt(1,3) = 0.300082278256296; fixTable[Link::RMa][Link::NLoS].R_sqrt(1,4) = 0.000000000000000; 
                    fixTable[Link::RMa][Link::NLoS].R_sqrt(2,0) = 0.000000000000000; fixTable[Link::RMa][Link::NLoS].R_sqrt(2,1) = -0.000000000000001; fixTable[Link::RMa][Link::NLoS].R_sqrt(2,2) = 0.999999999999999; fixTable[Link::RMa][Link::NLoS].R_sqrt(2,3) = 0.000000000000000; fixTable[Link::RMa][Link::NLoS].R_sqrt(2,4) = 0.000000000000000; 
                    fixTable[Link::RMa][Link::NLoS].R_sqrt(3,0) = -0.238369529001059; fixTable[Link::RMa][Link::NLoS].R_sqrt(3,1) = 0.300082278256296; fixTable[Link::RMa][Link::NLoS].R_sqrt(3,2) = 0.000000000000000; fixTable[Link::RMa][Link::NLoS].R_sqrt(3,3) = 0.923650688258459; fixTable[Link::RMa][Link::NLoS].R_sqrt(3,4) = 0.000000000000000; 
                    fixTable[Link::RMa][Link::NLoS].R_sqrt(4,0) = 0.000000000000000; fixTable[Link::RMa][Link::NLoS].R_sqrt(4,1) = 0.000000000000000; fixTable[Link::RMa][Link::NLoS].R_sqrt(4,2) = 0.000000000000000; fixTable[Link::RMa][Link::NLoS].R_sqrt(4,3) = 0.000000000000000; fixTable[Link::RMa][Link::NLoS].R_sqrt(4,4) = 1.000000000000000; 





                    fixTable[Link::UMi][Link::UMiO2I].NumClusters = 12;
                    fixTable[Link::UMi][Link::UMiO2I].r_tau = 2.200000;
                    fixTable[Link::UMi][Link::UMiO2I].Cluster_ASD = 5;
                    fixTable[Link::UMi][Link::UMiO2I].Cluster_ASA = 8;
                    fixTable[Link::UMi][Link::UMiO2I].LNS_ksi = 4.000000;
                    fixTable[Link::UMi][Link::UMiO2I].ASD_DS = 0.400000;
                    fixTable[Link::UMi][Link::UMiO2I].ASA_DS = 0.400000;
                    fixTable[Link::UMi][Link::UMiO2I].ASA_SF = 0.000000;
                    fixTable[Link::UMi][Link::UMiO2I].ASD_SF = 0.200000;
                    fixTable[Link::UMi][Link::UMiO2I].DS_SF = -0.500000;
                    fixTable[Link::UMi][Link::UMiO2I].ASD_ASA = 0.000000;
                    fixTable[Link::UMi][Link::UMiO2I].ASD_K = 0.000000;
                    fixTable[Link::UMi][Link::UMiO2I].ASA_K = 0.000000;
                    fixTable[Link::UMi][Link::UMiO2I].DS_K = 0.000000;
                    fixTable[Link::UMi][Link::UMiO2I].SF_K = 0.000000;
                    fixTable[Link::UMi][Link::UMiO2I].XPR_mu = 9.000000;
                    fixTable[Link::UMi][Link::UMiO2I].XPR_sigma = 0.000000;
                    fixTable[Link::UMi][Link::UMiO2I].DS_mu = -6.620000;
                    fixTable[Link::UMi][Link::UMiO2I].DS_sigma = 0.320000;
                    fixTable[Link::UMi][Link::UMiO2I].ASD_mu = 1.250000;
                    fixTable[Link::UMi][Link::UMiO2I].ASD_sigma = 0.420000;
                    fixTable[Link::UMi][Link::UMiO2I].ASA_mu = 1.760000;
                    fixTable[Link::UMi][Link::UMiO2I].ASA_sigma = 0.160000;
                    fixTable[Link::UMi][Link::UMiO2I].SF_sigma = 7.000000;
                    fixTable[Link::UMi][Link::UMiO2I].KF_mu = 0.000000;
                    fixTable[Link::UMi][Link::UMiO2I].KF_sigma = 0.000000;
                    fixTable[Link::UMi][Link::UMiO2I].CD_DS = 10.000000;
                    fixTable[Link::UMi][Link::UMiO2I].CD_ASD = 11.000000;
                    fixTable[Link::UMi][Link::UMiO2I].CD_ASA = 17.000000;
                    fixTable[Link::UMi][Link::UMiO2I].CD_SF = 7.000000;
                    fixTable[Link::UMi][Link::UMiO2I].CD_K = -1000000;
                    fixTable[Link::UMi][Link::UMiO2I].R_sqrt.set_size(5,5);
                    fixTable[Link::UMi][Link::UMiO2I].R_sqrt(0,0) = 0.896950683840360; fixTable[Link::UMi][Link::UMiO2I].R_sqrt(0,1) = 0.241943793527554; fixTable[Link::UMi][Link::UMiO2I].R_sqrt(0,2) = 0.223605545793171; fixTable[Link::UMi][Link::UMiO2I].R_sqrt(0,3) = -0.294861376620174; fixTable[Link::UMi][Link::UMiO2I].R_sqrt(0,4) = 0.000000000000000; 
                    fixTable[Link::UMi][Link::UMiO2I].R_sqrt(1,0) = 0.241943793527554; fixTable[Link::UMi][Link::UMiO2I].R_sqrt(1,1) = 0.959179465107684; fixTable[Link::UMi][Link::UMiO2I].R_sqrt(1,2) = -0.030708888757422; fixTable[Link::UMi][Link::UMiO2I].R_sqrt(1,3) = 0.143160464655990; fixTable[Link::UMi][Link::UMiO2I].R_sqrt(1,4) = 0.000000000000000; 
                    fixTable[Link::UMi][Link::UMiO2I].R_sqrt(2,0) = 0.223605545793171; fixTable[Link::UMi][Link::UMiO2I].R_sqrt(2,1) = -0.030708888757422; fixTable[Link::UMi][Link::UMiO2I].R_sqrt(2,2) = 0.973505191933743; fixTable[Link::UMi][Link::UMiO2I].R_sqrt(2,3) = 0.036676495468768; fixTable[Link::UMi][Link::UMiO2I].R_sqrt(2,4) = 0.000000000000000; 
                    fixTable[Link::UMi][Link::UMiO2I].R_sqrt(3,0) = -0.294861376620174; fixTable[Link::UMi][Link::UMiO2I].R_sqrt(3,1) = 0.143160464655990; fixTable[Link::UMi][Link::UMiO2I].R_sqrt(3,2) = 0.036676495468768; fixTable[Link::UMi][Link::UMiO2I].R_sqrt(3,3) = 0.944042734529146; fixTable[Link::UMi][Link::UMiO2I].R_sqrt(3,4) = 0.000000000000000; 
                    fixTable[Link::UMi][Link::UMiO2I].R_sqrt(4,0) = 0.000000000000000; fixTable[Link::UMi][Link::UMiO2I].R_sqrt(4,1) = 0.000000000000000; fixTable[Link::UMi][Link::UMiO2I].R_sqrt(4,2) = 0.000000000000000; fixTable[Link::UMi][Link::UMiO2I].R_sqrt(4,3) = 0.000000000000000; fixTable[Link::UMi][Link::UMiO2I].R_sqrt(4,4) = 1.000000000000000; 


                }
                ~FixPar()
                {
                    
                }


                const Parameters* operator() (const unsigned int scen, const unsigned int cond)
                {
                    // Assures for the temporary version (for final version, given below this):

                    assure( (scen>=0 && scen<=5), "Scenario Id not correct in FixPar, should be in the range from 0 to 5");
                    assure( (cond>=0 && cond<=2), "Condition value not correct in FixPar, should be in the range from 0 to 2" );
                    assure( (scen==1 || cond<2), "Only Link::UMi may have O-to-I condition, invalid condition");
                                
                    return &fixTable[scen][cond];
                }
            };
            typedef wns::SingletonHolder<FixPar> FixParSingleton;
            typedef enum{
                ASDeparture, ASArrival, DSpread, SFading, RiceanK
            }CorrleationDistance;

        }}}

#endif
