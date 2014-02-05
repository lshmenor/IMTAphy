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

#ifndef SPATIALCHANNEL_LINKPAR_HPP
#define SPATIALCHANNEL_LINKPAR_HPP

#include <itpp/itbase.h>


namespace imtaphy { namespace scm { namespace m2135 {

            class LinkPar {
    
            private:
                unsigned int numLinks, numMS, numBS;
            public:

        
                LinkPar(unsigned int _numLinks, unsigned int _numMS, unsigned int _numBS): numLinks(_numLinks), numMS(_numMS), numBS(_numBS) {
            
                    ScenarioVector.set_size(numLinks);
                    PropagConditionVector.set_size(numLinks);
                    MsBsDistance.set_size(numLinks);
                    BsHeight.set_size(numLinks);
                    MsHeight.set_size(numLinks);
                    ThetaBs.set_size(numLinks);
                    ThetaMs.set_size(numLinks);
                    MsVelocity.set_size(numLinks);
                    MsDirection.set_size(numLinks);
                    StreetWidth.set_size(numLinks);
                    LayoutType.set_size(numLinks);
                    NumFloors.set_size(numLinks);
                    OtoI_OutdoorPL.set_size(numLinks);
                    Dist1.set_size(numLinks);
                    BuildingHeight.set_size(numLinks);
                    LoS02ILinks.set_size(numLinks);
                    LoS02VLinks.set_size(numLinks);
                    NLoS02ILinks.set_size(numLinks);
                    NLoS02VLinks.set_size(numLinks);
                    MsXY.set_size(2,numMS);
                    Pairing.set_size(numBS,numMS);
                    Pairing.zeros();
                }
        
                ~LinkPar() {
        
                }
                unsigned int getnumLinks(){return numLinks;}
                unsigned int getnumMS(){return numMS;}
                unsigned int getnumBS(){return numBS;}
                itpp::vec ScenarioVector;              // We are not using this one, instead we have a vector of structs for all links with their senario 
                itpp::vec PropagConditionVector;       // and LoS/NLoS condition
                itpp::vec MsBsDistance;
                itpp::vec BsHeight;
                itpp::vec MsHeight;
                itpp::vec ThetaBs;
                itpp::vec ThetaMs;
                itpp::vec MsVelocity;
                itpp::vec MsDirection;
                itpp::vec StreetWidth;
                itpp::vec LayoutType;
                itpp::vec NumFloors;
                itpp::vec OtoI_OutdoorPL;
                itpp::vec Dist1;
                itpp::vec BuildingHeight;
                itpp::vec LoS02ILinks;
                itpp::vec LoS02VLinks;
                itpp::vec NLoS02ILinks;
                itpp::vec NLoS02VLinks;
                itpp::mat MsXY;
                itpp::mat Pairing;
            };

        }}}

#endif
