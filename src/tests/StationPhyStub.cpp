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

#include <IMTAPHY/tests/StationPhyStub.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/pyconfig/Parser.hpp>
#include <iostream>
#include <IMTAPHY/tests/ChannelStub.hpp>

using namespace imtaphy::tests;

//TODO: rewrite this!!!! 

StationPhyStub::StationPhyStub(wns::node::Interface* node, const wns::pyconfig::View& pyco, wns::Position p) :
    StationPhy(node, pyco),
    position(p)
{
  
}

const wns::Position& StationPhyStub::getPosition() const
{
    return this->position;
}

    
StationPhyStub*
imtaphy::tests::createStationStub(std::string name, wns::Position pos, std::string stationType, int numElements,  double elementSpacing, double speed, wns::node::Registry* registry, imtaphy::tests::ChannelStub* channel)
{
    std::stringstream ss_stationPhy;
    if (stationType == "BS")
        ss_stationPhy   << "import openwns.node\n"
			            << "import openwns.geometry.position\n"
                        << "import imtaphy.Station\n"
                        << "import imtaphy.Antenna\n"
                        << "import imtaphy.Logger\n"
                        << "node = openwns.node.Node(\"" << name << "\")\n"
                        << "stationPhyLogger = imtaphy.Logger.Logger(\"" << name << "\"+\".stationPhy\")\n"
                        << "stationPhy = imtaphy.Station.StationPhy(node, \"" << name << "\" + \".PHY\", " 
                        << "openwns.geometry.position.Position(x = " << pos.getX() << ", y = " << pos.getY() <<", z = 10.0), stationPhyLogger,"
                        << "phyDataTransmissionName = \"tx\","
                        << "phyDataReceptionName = \"rx\","
                        << "stationType = \"" << stationType << "\"," 
                        << "antenna=imtaphy.Antenna.OmnidirectionalForTests(numElements ="<< numElements<< ", elementSpacingMeters = "<< elementSpacing<<", logger = stationPhyLogger))\n";
    else if(stationType == "MS")
        ss_stationPhy   << "import openwns.node\n"
                        << "import imtaphy.Station\n"
                        << "import imtaphy.Antenna\n"
                        << "import imtaphy.Logger\n"
                        << "node = openwns.node.Node(\"" << name << "\")\n"
                        << "stationPhyLogger = imtaphy.Logger.Logger(\"" << name << "\"+\".stationPhy\")\n"
                        << "stationPhy = imtaphy.Station.StationPhy(node, \"" << name << "\" + \".PHY\", " 
                        << "openwns.geometry.position.Position(x = " << pos.getX() << ", y = " << pos.getY() <<", z = 10.0), stationPhyLogger,"
                        << "phyDataTransmissionName = \"tx\","
                        << "phyDataReceptionName = \"rx\","
                        << "stationType = \"" << stationType << "\"," 
                        << "speed = \"" << speed << "\"," 
                        << "antenna=imtaphy.Antenna.OmnidirectionalForTests(numElements ="<< numElements<< ", elementSpacingMeters = "<< elementSpacing<<", logger = stationPhyLogger))\n";
    else
        assure(0, "Invalid stationType");
    wns::pyconfig::Parser all_stationPhy;
    all_stationPhy.loadString(ss_stationPhy.str());
    wns::pyconfig::View config(all_stationPhy, "stationPhy");
    
    wns::node::Interface* node = new wns::node::Node(registry, config.get<wns::pyconfig::View>("node"));
    imtaphy::tests::StationPhyStub* station = new imtaphy::tests::StationPhyStub(node, config, pos);
    channel->registerStationPhy(station);
    
    return station;
}

