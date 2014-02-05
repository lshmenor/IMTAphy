/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 5, D-52074 Aachen, Germany
 * phone: ++49-241-80-27910,
 * fax: ++49-241-80-22242
 * email: info@openwns.org
 * www: http://www.openwns.org
 * _____________________________________________________________________________
 *
 * openWNS is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License version 2 as published by the
 * Free Software Foundation;
 *
 * openWNS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#include <DLL/StationManager.hpp>
#include <DLL/services/control/Association.hpp>
#include <DLL/Layer2.hpp>
#include <WNS/StaticFactory.hpp>
#include <WNS/module/Base.hpp>

using namespace dll::services::control;

STATIC_FACTORY_REGISTER_WITH_CREATOR(Association,
                     wns::ldk::ControlServiceInterface,
                     "dll.services.control.Association",
                     wns::ldk::CSRConfigCreator);

Association::Association(wns::ldk::ControlServiceRegistry* csr, const wns::pyconfig::View& config) :
    ControlService(csr),
    csr_(csr),
    serviceName_(config.get<std::string>("serviceName")),
    masterStation_(),
    slaveStations_(),
    logger_(config.get("logger"))
{
    if(!wns::simulator::getRegistry()->knows("DLLAssoc"))
    {
        std::map<int, int> assoc;
        wns::simulator::getRegistry()->insert("DLLAssoc", assoc);
    }
}

wns::service::dll::UnicastAddress
Association::getAssociation()
{
    assure(masterStation_.isValid(), serviceName_ + " Association not found.");
    return masterStation_;
}

bool
Association::hasAssociation()
{
    return (masterStation_.isValid());
}

bool
Association::hasAssociated(const wns::service::dll::UnicastAddress& slave) const
{
    for (std::list<wns::service::dll::UnicastAddress>::const_iterator it = slaveStations_.begin();
         it != slaveStations_.end(); ++it)
    {
        if (*it == slave)
        {
            return true;
        }
    }
    return false;

}

void
Association::registerAssociation(wns::service::dll::UnicastAddress slave, wns::service::dll::UnicastAddress master)
{
    // append to list
    slaveStations_.push_back(slave);

    MESSAGE_BEGIN(NORMAL, logger_, m, serviceName_ + " registered association from : ");
    m << slave;
    MESSAGE_END();

    std::map<int, int> assoc;
    assoc = wns::simulator::getRegistry()->find<std::map<int, int> >("DLLAssoc");
    assoc[slave.getInteger()] = master.getInteger();
    wns::simulator::getRegistry()->update("DLLAssoc", assoc);

    this->AssociationInfo::forEachObserver(AssociationFunctor( &AssociationObserverInterface::onAssociated,
                                   slave,
                                   master));
}

std::vector<wns::service::dll::UnicastAddress>
Association::getAssociated()
{
    std::vector<wns::service::dll::UnicastAddress> allAssociations;

    for (std::list<wns::service::dll::UnicastAddress>::const_iterator it = slaveStations_.begin(); it != slaveStations_.end(); ++it)
    {
        allAssociations.push_back(*it);
    }
    return allAssociations;
}

void
Association::associate(wns::service::dll::UnicastAddress slave, wns::service::dll::UnicastAddress master)
{
    // 1. Store the destination as the station I am associated to (my
    // master)
    this->masterStation_ = master;

    MESSAGE_BEGIN(NORMAL, logger_, m, serviceName_ + " associating ");
    m << slave << " to " << master;
    MESSAGE_END();

    // 2. Register the association at the master's association service
    // first obtain pointer to my peer service at the master's side
    dll::services::control::Association* peerService =
        this->csr_->getLayer<dll::ILayer2*>()->getStationManager()->getStationByMAC(master)
        ->getControlService<dll::services::control::Association>(serviceName_);
    // then register the association
    peerService->registerAssociation(slave, master);
}

void
Association::associate(dll::ILayer2* slave, dll::ILayer2* master)
{
    this->associate(slave->getDLLAddress(), master->getDLLAddress());
} //associate


void
Association::releaseFromMaster()
{
    MESSAGE_BEGIN(NORMAL, logger_, m, serviceName_ + " releasing association to ");
    m << masterStation_;
    MESSAGE_END();

    masterStation_ = wns::service::dll::UnicastAddress();
} //releaseFromMaster

void
Association::releaseClient(wns::service::dll::UnicastAddress slave)
{
    // safety check
    assure( std::find(slaveStations_.begin(), slaveStations_.end(), slave) != slaveStations_.end(),
        "Trying to release unknown client association" );

    std::map<int, int> assoc;
    assoc = wns::simulator::getRegistry()->find<std::map<int, int> >("DLLAssoc");
    assoc.erase(slave.getInteger());
    wns::simulator::getRegistry()->update("DLLAssoc", assoc);

    // remove from my set of slaves
    slaveStations_.remove(slave);
}
void
Association::releaseClient(dll::ILayer2* slave)
{
    this->releaseClient(slave->getDLLAddress());
} //releaseFromMaster
