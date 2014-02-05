/******************************************************************************
 * DLLBase (DLL Base classes to create FUN-based DLL)                         *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2006                                                         *
 * Chair of Communication Networks (ComNets)                                  *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnets.rwth-aachen.de                                          *
 * www: http://wns.comnets.rwth-aachen.de                                     *
 ******************************************************************************/

#include <DLL/StationManager.hpp>
#include <DLL/UpperConvergence.hpp>
#include <DLL/RANG.hpp>
#include <DLL/Layer2.hpp>

#include <WNS/node/Node.hpp>
#include <WNS/Functor.hpp>
#include <WNS/Assure.hpp>


using namespace dll;

StationManager::StationManager() :
    macAdrLookup(),
    layerLookup()
{
}

StationManager::~StationManager()
{
} // ~StationManager

void
StationManager::registerStation(ILayer2::StationIDType id,
                wns::service::dll::UnicastAddress adr,
                ILayer2* layer)
{
    //assure(! layerLookup.knows(id), "Station already registered.");

    assure(! macAdrLookup.knows(adr), "Station already registered.");
    macAdrLookup.insert(adr, layer);

    if (layerLookup.knows(id))
    {
        // id is already known -> station has more than one address
        assure(this->getStationByID(id) == layer, "A station with id " << id << " is already registered using a differend ILayer2*");
    }
    else
    {
        layerLookup.insert(id, layer);
    }

    if(nodeLookup.knows(layer->getNode()))
    {
        assure(this->getStationByNode(layer->getNode()) == layer, "A station with this node is already registered using a different ILayer2*");
    }
    else
    {
        nodeLookup.insert(layer->getNode(), layer);
    }
} // registerStation


ILayer2*
StationManager::getStationByID(ILayer2::StationIDType id) const
{
    return layerLookup.find(id);
} // getStationByID

ILayer2*
StationManager::getStationByNode(wns::node::Interface* node) const
{
    return nodeLookup.find(node);
}


wns::node::Interface*
StationManager::getNodeByID(ILayer2::StationIDType id) const
{
    return this->getStationByID(id)->getNode();
} // getStationByID


ILayer2*
StationManager::getStationByMAC(wns::service::dll::UnicastAddress adr) const
{
    assure(adr.getInteger() > 0,"getStationByMAC("<<adr<<"): bad address");
    return macAdrLookup.find(adr);
}

NodeList
StationManager::getNodeList() const
{
  NodeList nodeList;
  for(NodeLookup::const_iterator iter = nodeLookup.begin(); iter != nodeLookup.end(); ++iter)
  {
    nodeList.push_back(iter->first);
  }
  return nodeList;
}
