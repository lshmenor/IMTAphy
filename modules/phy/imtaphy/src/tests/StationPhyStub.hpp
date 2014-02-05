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

#ifndef STATIONPHYSTUB_HPP
#define STATIONPHYSTUB_HPP

#include <IMTAPHY/StationPhy.hpp>
#include <WNS/node/Registry.hpp>

#include <WNS/node/Interface.hpp>

namespace imtaphy { namespace tests {

    
        class ChannelStub;
    
        class StationPhyStub :
            public imtaphy::StationPhy
        {
    
        public:
            StationPhyStub(wns::node::Interface* node, const wns::pyconfig::View& pyco, wns::Position p);
            virtual const wns::Position& getPosition() const;
            void onNodeCreated() {}
            wns::Position position;
        };


        StationPhyStub*
        createStationStub(std::string name, wns::Position pos, std::string stationType, int numElements,  double elementSpacing, double speed, wns::node::Registry* registry, imtaphy::tests::ChannelStub* channel);


    }}
#endif // STATIONPHYSTUB_HPP
