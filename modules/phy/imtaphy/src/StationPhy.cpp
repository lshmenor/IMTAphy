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

#include <IMTAPHY/StationPhy.hpp>

#include <IMTAPHY/Transmission.hpp>
#include <IMTAPHY/Channel.hpp>
#include <IMTAPHY/Link.hpp>
#include <IMTAPHY/linkManagement/LinkManager.hpp>
#include <IMTAPHY/receivers/ReceiverInterface.hpp>

#include <WNS/node/Interface.hpp>
#include <WNS/node/component/FQSN.hpp>
#include <WNS/PyConfigViewCreator.hpp>

#include <iostream>

using namespace imtaphy;


STATIC_FACTORY_REGISTER_WITH_CREATOR(
    imtaphy::StationPhy,
    wns::node::component::Interface,
    "imtaphy.StationPhy",
    wns::node::component::ConfigCreator);


StationPhy::StationPhy(wns::node::Interface* node, const wns::pyconfig::View& config) :
    wns::node::component::Component(node, config),
    rxServiceName(config.get<std::string>("phyDataReceptionName")),
    txServiceName(config.get<std::string>("phyDataTransmissionName")),
    rxService(NULL),
    logger(config.get("logger")),
    position(wns::Position(config.get("position"))),
    directionOfTravel(config.get<double>("directionOfTravel")),
    speed(config.get<double>("speed")),
    stationID(node->getNodeID()) // nodes have unique ids that are uniquely assigned by openwns.node.Node class in Python
{
    assure((directionOfTravel <= itpp::pi + 0.0001) && (directionOfTravel >= -itpp::pi - 0.0001), "Direction of travel should be between -Pi..Pi"); 
    
    // create the antenna and pass a standard PyConfig View
    wns::pyconfig::View antennaConfig = config.get("antenna");
    antenna = wns::StaticFactory<wns::PyConfigViewCreator<imtaphy::antenna::AntennaInterface> >::creator(antennaConfig.get<std::string>("nameInAntennaFactory"))->create(antennaConfig);

    // create the receiver and pass a station pointer in addition to a standard PyConfig View
    wns::pyconfig::View receiverConfig = config.get("receiver");
    std::string plugin = receiverConfig.get<std::string>("nameInReceiverFactory");
    imtaphy::receivers::ReceiverCreator* rc = imtaphy::receivers::ReceiverFactory::creator(plugin);
    receiver = rc->create(this, receiverConfig);
    
    addService(txServiceName, this);
    
    if (config.get<std::string>("stationType") == "BS")
        stationType = BASESTATION;
    else
        stationType = MOBILESTATION;
    
    
}

void
StationPhy::onNodeCreated()
{
    try {
        rxService = getService<imtaphy::interface::PhyNotify*>(rxServiceName);
    }
    catch(wns::container::Registry<std::string, wns::service::Service*, wns::container::registry::NoneOnErase, std::less<std::string> >::UnknownKeyValue)
    {
        // If no such service exists, let's hope we never have to do onData
    }
  
    channel = &TheIMTAChannel::Instance();
  
    // subscribe or tune to all available PRBs  
    channel->registerStationPhy(this);
  
    MESSAGE_SINGLE(NORMAL, logger, "Node created");
}

void    
StationPhy::onWorldCreated()
{
    // actually, only one station would have to do this. Channel.cpp will ignore all subsequenct calls
    channel->onWorldCreated();
}
    
void
StationPhy::onShutdown()
{
    receiver->onShutdown();
}

void
StationPhy::doStartup()
{

}

void imtaphy::StationPhy::channelInitialized()
{
    // This adds a few contexts to the Node's ContextProviderCollection
    // Later on (e.g. in receiver::channelInitialized) the CPC is usually
    // copied to add specific other contexts inheriting these default contexts:
    
    wns::node::Interface* node = getNode();
    
    node->getContextProviderCollection().addProvider(
                wns::probe::bus::contextprovider::Constant("scenario.x", 
                                                           getPosition().getX())
                                                    );
    node->getContextProviderCollection().addProvider(
                wns::probe::bus::contextprovider::Constant("scenario.y", 
                                                           getPosition().getY())
                                                    );
    
    if (stationType == imtaphy::MOBILESTATION)
    {
        imtaphy::Link* servingLink = imtaphy::TheIMTAChannel::Instance().getLinkManager()->getServingLinkForMobileStation(this);

        node->getContextProviderCollection().addProvider(
                wns::probe::bus::contextprovider::Constant("BSID", 
                                                           servingLink->getBS()->getNode()->getNodeID()));

    }
    else // Base Station
    {
        node->getContextProviderCollection().addProvider(
                wns::probe::bus::contextprovider::Constant("BSID", node->getNodeID()));
    }
    
    receiver->channelInitialized(channel);
    
    transmitDirection = imtaphy::Downlink;
    if (stationType == MOBILESTATION)
    {
        // transmissions originating here are uplink, otherwise downlink
        transmitDirection = imtaphy::Uplink;
    }
    
    if (rxService)
    {
        rxService->channelInitialized();
    }
}

std::vector< wns::node::Interface* > 
StationPhy::getAssociatedNodes()
{
    std::vector<wns::node::Interface*> associations;
    
    if (stationType == BASESTATION)
    {
             
        LinkMap servedLinks = channel->getLinkManager()->getServedLinksForBaseStation(this);
    
        for (LinkMap::const_iterator iter = servedLinks.begin(); iter != servedLinks.end(); iter++)
        {
            StationPhy* servedStation = iter->first;
            associations.push_back(servedStation->getNode());
        }
    }
    else if (stationType == MOBILESTATION)
    {
        Link* servingLink = channel->getLinkManager()->getServingLinkForMobileStation(this);
        associations.push_back(servingLink->getBS()->getNode());
    }
    else
        assure(0, "Invalid station type");
    
    return associations;
}


void 
StationPhy::registerTransmission(wns::node::Interface* destination,
                                 const std::vector<wns::ldk::CompoundPtr> &transportBlocks,
                                 unsigned int numberOfLayers,
                                 const imtaphy::interface::PrbPowerPrecodingMap &perPRBpowerAndPrecoding)

{
    MESSAGE_BEGIN(NORMAL, logger, m, getName());
        m << "Register Transmission called in node ";
        m << getNode()->getName() << " with destination " << destination->getName() << " on the folowing PRBS with the respective Tx power\n";

        for (imtaphy::interface::PrbPowerPrecodingMap::const_iterator prbIter = perPRBpowerAndPrecoding.begin(); prbIter != perPRBpowerAndPrecoding.end(); prbIter++) 
        {
            m << "PRB " <<  prbIter->first;
            m << " with tx power of " << prbIter->second.power;
           // m << " and precoding matrix at " << prbIter->second.precoding->getLocation();
            m << "\n";
        }
    MESSAGE_END();
 
    
    StationPhy* remoteStation = destination->getService<StationPhy*>(txServiceName);
    
    assure(remoteStation, "Could not get remote StationPhy component");
    
    imtaphy::Link* link = channel->getLinkManager()->getLink(this, remoteStation);

#ifndef WNS_NDEBUG    
    // check that the precoding matrices have the right dimension (numTxAntennas-by-numLayers)
    for (imtaphy::interface::PrbPowerPrecodingMap::const_iterator iter = perPRBpowerAndPrecoding.begin(); iter != perPRBpowerAndPrecoding.end(); iter++) 
    {
        assure(iter->second.precoding->getColumns() == numberOfLayers, "Precoding matrix must have as many columns as transmission has layers (got " 
               << iter->second.precoding->getColumns() << " instead of " << numberOfLayers << ")");
        assure(iter->second.precoding->getRows() == this->antenna->getNumberOfElements(), "Precoding matrix must have as many rows as this station has antennas (got " 
                << iter->second.precoding->getRows() << " instead of " <<  this->antenna->getNumberOfElements() << ")");
    }
#endif
    
    
    assure(numberOfLayers <=  this->antenna->getNumberOfElements(), "Trying to transmit more layers than transmit antennas available");
    
    TransmissionPtr transmission = TransmissionPtr(new Transmission(transmitDirection,
                                                                    transportBlocks,
                                                                    link,
                                                                    this,
                                                                    remoteStation,
                                                                    perPRBpowerAndPrecoding,
                                                                    this->antenna->getNumberOfElements(),
                                                                    numberOfLayers));
    channel->registerTransmission(transmission);
}

void
StationPhy::onData(std::vector<wns::ldk::CompoundPtr> transportBlocks, wns::node::Interface* source,  imtaphy::interface::TransmissionStatusPtr status)
{
    assure(rxService, "No rx service had registered to forward onData to");
    rxService->onData(transportBlocks, source, status);
}


