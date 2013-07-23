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

#ifndef IMTAPHY_LINKCLASSIFIERINTERFACE
#define IMTAPHY_LINKCLASSIFIERINTERFACE

#include <WNS/pyconfig/View.hpp>
#include <IMTAPHY/Link.hpp>

namespace imtaphy { 
    class StationPhy;
    class Channel;

    namespace linktype {
        struct LinkTypes {
            imtaphy::Link::Scenario scenario;
            imtaphy::Link::Propagation propagation;
            imtaphy::Link::UserLocation userLocation;
            imtaphy::Link::Propagation outdoorPropagation;
        };
    }
    
    namespace linkclassify {
        class LinkClassifierInterface
        {
        public:
            LinkClassifierInterface(imtaphy::Channel* channel, wns::pyconfig::View config) {};
            
            
            /**
             * @brief To be called by after all stations have registered, 
             * use for initialization.
             */
            virtual
            void onWorldCreated() = 0;
             
            /**
             * @brief Determine type of link between 2 stations. wrappedMSposition 
             * will be used as the position of the mobile station when computing distances
             * to allow for wrap-around. If no wrap-around is used, set to real mobile position.
             *
             */
            virtual
            linktype::LinkTypes classifyLink(const imtaphy::StationPhy& baseStation,
                                             const imtaphy::StationPhy& mobileStation,
                                             const wns::Position& wrappedMSposition) = 0;
            
        };
    }}
#endif
