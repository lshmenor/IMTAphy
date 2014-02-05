/*******************************************************************************
 * This file is part of IMTAphy
 * _____________________________________________________________________________
 *
 * Copyright (C) 2011
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
 ******************************************************************************

 based on code from openWNS with the following license:

*******************************************************************************
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

#include <IMTAPHY/ltea/mac/harq/HARQ.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/Cloneable.hpp>

using namespace ltea::mac::harq;


HARQ::HARQ(const wns::pyconfig::View& config):
    logger_(config.get("logger")),
    numSenderProcesses_(config.get<int>("numSenderProcesses")),
    numReceiverProcesses_(config.get<int>("numReceiverProcesses")),
    numRVs_(config.get<int>("numRVs")),
    retransmissionLimit_(config.get<int>("retransmissionLimit")),
    feedbackDecodingDelay(config.get<double>("feedbackDecodingDelay")),
    harqEntityPrototype_(new HARQEntity(config.get("harqEntity"), numSenderProcesses_, numReceiverProcesses_, numRVs_, retransmissionLimit_, feedbackDecodingDelay, logger_))
//    numRetransmissionsProbeCC("scheduler.harq.retransmissions")
{
}
                
HARQ::~HARQ()
{
}

bool 
HARQ::knowsUser(wns::node::Interface* userID)
{
    return harqEntities_.knows(userID);
}


HARQEntity*
HARQ::findEntity(wns::node::Interface* userID)
{
    if (!harqEntities_.knows(userID))
    {
        MESSAGE_SINGLE(NORMAL, logger_, "Creating new HARQEntity for peer UserID=" << userID->getName() );
        HARQEntity* he = wns::clone(harqEntityPrototype_);
        harqEntities_.insert(userID, he);
        
        return he;
    }
    
    return harqEntities_.find(userID);
}

bool
HARQ::hasRetransmission(wns::node::Interface* peer, int processID, unsigned int spatialID)
{
    assure((spatialID == 0) || (spatialID == 1), "Spatial ID can only be 0 or 1");
    assure(processID < numSenderProcesses_, "Invalid sender process id");
    
    assure(harqEntities_.knows(peer), "no entity for that user");

    return harqEntities_.find(peer)->hasRetransmission(processID, spatialID);
}


unsigned int
HARQ::getNumberOfRetransmissions(wns::node::Interface* peer, int processID)
{
    assure(processID < numSenderProcesses_, "Invalid sender process id");
    
    assure(harqEntities_.knows(peer), "no entity for that user");

    unsigned int numRetransmissions = 0;
    for (unsigned int spatial = 0; spatial < 2; spatial++)
        if (harqEntities_.find(peer)->hasRetransmission(processID, spatial))
            numRetransmissions++;
    
    return numRetransmissions;
}


void
HARQ::storeScheduledTransportBlock(wns::node::Interface* userID, wns::ldk::CompoundPtr transportBlock, unsigned int spatialID)
{
    HARQEntity* entity = this->findEntity(userID);

    assure(entity->hasCapacity(), "The HARQ Entity for peer UserID=" << userID->getName() << " does not have anymore capacity!");
    
    entity->storeScheduledTransportBlock(transportBlock, spatialID);
}

void 
HARQ::storeScheduledTransportBlock(wns::node::Interface* userID, wns::ldk::CompoundPtr transportBlock, unsigned int processID, unsigned int spatialID)
{
    HARQEntity* entity = this->findEntity(userID);

    entity->storeScheduledTransportBlock(transportBlock, processID, spatialID);
    
}

void 
HARQ::storeScheduledTransportBlocks(wns::node::Interface* userID, std::vector< wns::ldk::CompoundPtr > transportBlocks)
{
    HARQEntity* entity = this->findEntity(userID);

    entity->storeScheduledTransportBlocks(transportBlocks);
}



bool
HARQ::decodeReceivedTransportBlock(wns::node::Interface* source, wns::ldk::CompoundPtr receivedTransportBlock, imtaphy::interface::TransmissionStatusPtr status, unsigned int spatialID)
{
    HARQEntity* entity = this->findEntity(source);

    return entity->decodeReceivedTransportBlock(receivedTransportBlock, status, spatialID);
}



std::set<wns::node::Interface*, imtaphy::detail::WnsNodeInterfacePtrCompare>
HARQ::getUsersWithRetransmissions() const
{
    std::set<wns::node::Interface*, imtaphy::detail::WnsNodeInterfacePtrCompare> users;
    
    for (HARQEntityContainer::const_iterator it = harqEntities_.begin();
         it != harqEntities_.end(); ++it)
    { // foreach user
         if (it->second->hasRetransmissions())
         {
             users.insert(it->first);
         }
    }

    return users;
}

std::list<int>
HARQ::getProcessesWithRetransmissions(wns::node::Interface* peer) const
{
    if(harqEntities_.knows(peer))
    {
        return harqEntities_.find(peer)->getProcessesWithRetransmissions();
    }
    else
    {
        std::list<int> empty;
        return empty;
    }
}

unsigned int 
HARQ::getProcessWithNextRetransmissions(wns::node::Interface* peer)
{
    if(harqEntities_.knows(peer))
    {
        return harqEntities_.find(peer)->getProcessWithNextRetransmissions();
    }
    else
    {
        return -1;
    }
}


wns::ldk::CompoundPtr
HARQ::getRetransmission(wns::node::Interface* peer, int processID, unsigned int spatialID)
{
    assure((spatialID == 0) || (spatialID == 1), "Spatial ID can only be 0 or 1");
    assure(processID < numSenderProcesses_, "Invalid sender process id");
    
    assure(harqEntities_.knows(peer), "no entity for that user");

    return harqEntities_.find(peer)->getRetransmission(processID, spatialID);
}

void
HARQ::retransmissionStarted(wns::node::Interface* peer, int processID, unsigned int spatialID)
{
    assure((spatialID == 0) || (spatialID == 1), "Spatial ID can only be 0 or 1");
    assure(processID < numSenderProcesses_, "Invalid sender process id");
    
    assure(harqEntities_.knows(peer), "no entity for that user");

    return harqEntities_.find(peer)->retransmissionStarted(processID, spatialID);
}


bool
HARQ::hasFreeSenderProcess(wns::node::Interface* peer)
{
    if(harqEntities_.knows(peer))
    {
        return harqEntities_.find(peer)->hasCapacity();
    }
    return true;
}

void
HARQ::setDCIReader(wns::ldk::CommandReaderInterface* dciReader)
{
    assure(harqEntities_.size() == 0, "You have to set the DCI reader before any HARQ entities are created");
    assure(dciReader, "Invalid DCI reader");

    harqEntityPrototype_->setDCIReader(dciReader);
    
}

unsigned int 
HARQ::getACKcount(wns::node::Interface* userID)
{
    assure(this->knowsUser(userID), "Requesting ACK count of unknown user");

    return this->findEntity(userID)->getACKcount();
}

unsigned int 
HARQ::getNACKcount(wns::node::Interface* userID)
{
    assure(this->knowsUser(userID), "Requesting NACK count of unknown user");

    return this->findEntity(userID)->getNACKcount();

}






