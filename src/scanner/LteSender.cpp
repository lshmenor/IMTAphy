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

#include <IMTAPHY/scanner/LteSender.hpp>
#include <WNS/ldk/helper/FakePDU.hpp>
#include <IMTAPHY/StationPhy.hpp>
#include <WNS/simulator/Simulator.hpp>
#include <IMTAPHY/Link.hpp>
#include <IMTAPHY/linkManagement/LinkManager.hpp>
#include <IMTAPHY/scanner/LteScanner.hpp>
#include <WNS/Singleton.hpp>
#include <IMTAPHY/detail/LinearAlgebra.hpp>
#include <IMTAPHY/receivers/LteRel8Codebook.hpp>
#include <IMTAPHY/receivers/feedback/LteRel8DLFeedbackManager.hpp>

using namespace imtaphy::scanner;



STATIC_FACTORY_REGISTER_WITH_CREATOR(
    LteSender,
    wns::node::component::Interface,
    "imtaphy.LteSender",
    wns::node::component::ConfigCreator);

// initialize the static member:
unsigned int LteSender::maxNumberOfMobiles = 0;


LteSender::LteSender(wns::node::Interface* _node, const wns::pyconfig::View& _pyco) : 
    Component(_node, _pyco),
    txServiceName(_pyco.get<std::string>("phyDataTransmission")),
    rxServiceName(_pyco.get<std::string>("phyDataReception")),
    txPowerPerPRB(wns::Power::from_dBm(_pyco.get<double>("txPowerPerPRBdBm"))),
    tti(0),
    channel(&imtaphy::TheIMTAChannel::Instance()),
    bsId(_pyco.get<unsigned int>("bsId")),
    numberOfRounds(_pyco.get<unsigned int>("numberOfRounds")),
    nextMS(0)
//    numberOfLayers(_pyco.get<unsigned int>("numberOfLayers"))
    
{
    
    this->startObserving(channel);
    
    getNode()->addService(rxServiceName, this);
}
/*
  void
  LteSender::onNodeCreated()
  { 
    
  }
*/
void
LteSender::onWorldCreated()
{
    txService = getService<imtaphy::interface::DataTransmission*>(txServiceName);
    station = dynamic_cast<StationPhy*>(txService);
    assure(station, "Need to access station object");
    
    
    channel = &imtaphy::TheIMTAChannel::Instance();

    StationList allMobiles = channel->getAllMobileStations();
    assure(allMobiles.size(), "no mobiles");
    dummyMobile = *(allMobiles.begin());

}

void
LteSender::beforeTTIover(unsigned int _tti)
{
    tti = _tti;
   
    // don't do this in onWorldCreated because linkManager might not be ready yet
    if (tti == 1)
    {
        LinkMap mobilesMap = channel->getLinkManager()->getServedLinksForBaseStation(station);
        
        for (LinkMap::const_iterator iter = mobilesMap.begin(); iter != mobilesMap.end(); iter++)
            mobiles.push_back(iter->first); 
        std::cout << "BS " << station->getName() << " with ID " << bsId << " is serving " << mobiles.size() << " mobiles\n";
   
        // check against global (static) maximum number of mobiles served by a station to determine when
        // the simulation can be stopped because all cells served all mobiles
        if (mobiles.size() > maxNumberOfMobiles)
            maxNumberOfMobiles = mobiles.size();
    }

    // after the first TTI maxNumberOfMobiles contains the correct number, so program the termination of the simulation
    if (tti == 2)
    {
        std::cout << "Going to stop this run at time t=" << float(numberOfRounds * maxNumberOfMobiles + 1) / 1000.0 << "\n";
        wns::simulator::getEventScheduler()->stopAt(float(numberOfRounds * maxNumberOfMobiles + 1) / 1000.0);
        
        for (unsigned int i = 0; i < mobiles.size(); i++)
        {
            imtaphy::StationPhy* mobileStation = mobiles[i];
            
            LteScanner* scanner = mobileStation->getNode()->getService<LteScanner*>(rxServiceName);
            assure(scanner, "Could not get valid pointer to remote mobile's LteScaner");
            
            scanner->setSimulationDuration(numberOfRounds * maxNumberOfMobiles);
        }
    }
    
    // the next round starts:
    // check if tti > 1 to avoid situation where the first BS is not serving any mobile so maxNumberOfMobiles==0
    // at tti
    if (tti > 1) 
    {
        assure(maxNumberOfMobiles, "No mobile served at all");
        if ((tti % maxNumberOfMobiles) == 0)
            nextMS = 0;
    }

    // transmit on all antennas without precoding (identity)
    numberOfTxAntennas = station->getAntenna()->getNumberOfElements();



    std::vector<wns::ldk::CompoundPtr> tbs;
    wns::node::Interface* destination; 

    for (unsigned int prb = 0; prb <channel->getSpectrum()->getNumberOfPRBs(imtaphy::Downlink); prb++)
    {
        imtaphy::interface::PowerAndPrecoding powerAndPrecoding;
        powerAndPrecoding.precoding = wns::SingletonHolder<imtaphy::receivers::LteRel8Codebook<float> >::Instance().getPrecodingMatrix(numberOfTxAntennas,
                                                                                                                                    1, // single layer
                                                                                                                                    -1 // transmit on antenna 1
                                                                                                                                    );
        powerAndPrecoding.power = txPowerPerPRB;
        prbPowerPrecodingMap[prb] = powerAndPrecoding;
    }
    
    if (nextMS < mobiles.size())
    {
        
        destination = mobiles[nextMS]->getNode();
        nextMS++;

        tbs.clear();
        tbs.push_back( wns::ldk::CompoundPtr(new wns::ldk::Compound(NULL, wns::osi::PDUPtr(new wns::ldk::helper::FakePDU(42)))));
    }
    else
    {
        destination = dummyMobile->getNode();
        tbs.clear();
        tbs.push_back(wns::ldk::CompoundPtr(new wns::ldk::Compound(NULL, wns::osi::PDUPtr(new wns::ldk::helper::FakePDU(0)))));
    }
    
    txService->registerTransmission(destination,
                                    tbs,
                                    numberOfLayers, // #layers
                                    prbPowerPrecodingMap// precoding matrix pointer
                                   );

}





