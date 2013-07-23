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


// TODO: clean up the whole HARQ/HARQEntity interface, feels too complicated


#include <IMTAPHY/ltea/mac/harq/HARQentity.hpp>
#include <IMTAPHY/ltea/mac/DCI.hpp>

using namespace ltea::mac::harq;

HARQEntity::HARQEntity(const wns::pyconfig::View& config,
                       int numSenderProcesses,
                       int numReceiverProcesses,
                       int numRVs,
                       int retransmissionLimit,
                       double feedbackDecodingDelay,
                       wns::logger::Logger logger):
    numSenderProcesses_(numSenderProcesses),   // 2 vectors of processes for first and second spatially multiplexed transport block
    numReceiverProcesses_(numReceiverProcesses),
    senderProcesses_(2),
    receiverProcesses_(2),
    numRVs_(numRVs),
    retransmissionLimit_(retransmissionLimit),
    feedbackDecodingDelay_(feedbackDecodingDelay),
    logger_(logger),
    dciReader(NULL),
    nacks(0),
    acks(0),
    nextProcessToRetransmit(0)
{
    decoder_ = STATIC_FACTORY_NEW_INSTANCE(ltea::l2s::harq::DecoderInterface, wns::PyConfigViewCreator, config.get("decoder"), config.get("decoder"));
    decoder_->setEntity(this);

    for (unsigned int spatial = 0; spatial < 2; spatial++)
    {
        senderProcesses_[spatial] = HARQSenderProcessVector();
        receiverProcesses_[spatial] = HARQReceiverProcessVector();
    
        for (unsigned int ii=0; ii < numSenderProcesses_; ii++)
        {
            senderProcesses_[spatial].push_back(HARQSenderProcess(this, ii, numRVs_, retransmissionLimit_, feedbackDecodingDelay_, logger_));
        }

        for (unsigned int ii=0; ii < numReceiverProcesses_; ii++)
        {
            receiverProcesses_[spatial].push_back(HARQReceiverProcess(this, ii, logger_));
        }
    }
}

HARQEntity::~HARQEntity()
{
    // do we really need to do this?
    senderProcesses_[0].clear();
    senderProcesses_[1].clear();
    receiverProcesses_[0].clear();
    receiverProcesses_[1].clear();
    
    delete decoder_;
}


HARQEntity::HARQEntity(const HARQEntity& other):
    numSenderProcesses_(other.numSenderProcesses_),
    numReceiverProcesses_(other.numReceiverProcesses_),
    senderProcesses_(other.senderProcesses_),
    receiverProcesses_(other.receiverProcesses_),
    numRVs_(other.numRVs_),
    retransmissionLimit_(other.retransmissionLimit_),
    feedbackDecodingDelay_(other.feedbackDecodingDelay_),
    logger_(other.logger_),
    dciReader(other.dciReader),
    decoder_(wns::clone(other.decoder_)),
    nacks(other.nacks),
    acks(other.acks),
    nextProcessToRetransmit(other.nextProcessToRetransmit)
{
    for (unsigned int spatial = 0; spatial < 2; spatial++)
    {
        for (unsigned int ii=0; ii < numSenderProcesses_; ii++)
        {
            senderProcesses_[spatial][ii].setEntity(this);
        }

        for (unsigned int ii=0; ii < numReceiverProcesses_; ii++)
        {
            receiverProcesses_[spatial][ii].setEntity(this);
        }
    }
}

void
HARQEntity::storeScheduledTransportBlock(wns::ldk::CompoundPtr transportBlock, unsigned int spatialID)
{
    // calling new Transmission implies that we will set the NDI flag in the transport block to indicate new data
    
    assure(hasCapacity(), "HARQ Entity is busy but you wanted to start a new transmission! Call hasCapacity() first");
    
    // find free sender process
    for (unsigned int ii=0; ii < numSenderProcesses_; ii++)
    {
        if (senderProcesses_[spatialID][ii].hasCapacity())
        {
            senderProcesses_[spatialID][ii].newTransmission(transportBlock);
            
            MESSAGE_SINGLE(NORMAL, logger_, "newTransmission for sender process " << ii );
            return; // break; // found
        }
    }
    assure(false, "HARQEntity::newTransmission: cannot find free senderProcess");
}


void
HARQEntity::storeScheduledTransportBlocks(std::vector<wns::ldk::CompoundPtr> transportBlocks)
{
    // calling new Transmission implies that we will set the NDI flag in the transport block to indicate new data
    
    assure(hasCapacity(), "HARQ Entity is busy but you wanted to start a new transmission! Call hasCapacity() first");
    assure((transportBlocks.size() == 1) || (transportBlocks.size() == 2), "Can handle one or two transport blocks");
    
    // find free sender process
    for (unsigned int ii=0; ii < numSenderProcesses_; ii++)
    {
        if (senderProcesses_[0][ii].hasCapacity() && senderProcesses_[1][ii].hasCapacity())
        {
            senderProcesses_[0][ii].newTransmission(transportBlocks[0]);
            MESSAGE_SINGLE(NORMAL, logger_, "newTransmission for sender process " << ii << " spatial stream 0");
            
            if (transportBlocks.size() > 1)
            {
                senderProcesses_[1][ii].newTransmission(transportBlocks[1]);
                MESSAGE_SINGLE(NORMAL, logger_, "newTransmission for sender process " << ii << " spatial stream 1");
            }
            return; // break; // found
        }
    }
    assure(false, "HARQEntity::newTransmission: cannot find free senderProcess");
}

void 
HARQEntity::storeScheduledTransportBlock(wns::ldk::CompoundPtr transportBlock, unsigned int processID, unsigned int spatialID)
{
    assure(processID < senderProcesses_[spatialID].size(), "Invalid process id");
    assure(senderProcesses_[spatialID][processID].hasCapacity(), "storeScheduledTransportBlock(tb, procesID, spatial) called even though not available");

    senderProcesses_[spatialID][processID].newTransmission(transportBlock);
}


bool
HARQEntity::hasCapacity()
{
    for (unsigned int ii=0; ii < numSenderProcesses_; ii++)
    {
        // Any of my send processes idle?
        if ((senderProcesses_[0][ii].hasCapacity()) && (senderProcesses_[1][ii].hasCapacity()))
        {
            return true;
        }
    }
    return false;
}

bool
HARQEntity::decodeReceivedTransportBlock(wns::ldk::CompoundPtr transportBlockRedundancyVersion, imtaphy::interface::TransmissionStatusPtr status, unsigned int spatialID)
{
    assure((spatialID == 0) || (spatialID == 1), "Invalid spatialID, can only be 0 or 1");
    
    ltea::mac::DownlinkControlInformation* dci =
            dciReader->readCommand<ltea::mac::DownlinkControlInformation>(transportBlockRedundancyVersion->getCommandPool());

    assure(receiverProcesses_[spatialID].size() > dci->peer.processID, "Invalid HARQ process ID");
    assure(spatialID == dci->magic.spatialID, "Mismatch between spatial IDs");
    
    // delegate to the responsible Receiver Process

    assure(receiverProcesses_.size() > spatialID, "No receiver processes for this spatial");
    assure(receiverProcesses_[spatialID].size() > dci->peer.processID, "Receiver process for this id not found");
    
    return receiverProcesses_[spatialID][dci->peer.processID].decodeReceivedTransportBlock(transportBlockRedundancyVersion, status);
}


std::list<int>
HARQEntity::getProcessesWithRetransmissions() const
{
    std::list<int> tmp;
    
    for (unsigned int i = 0; i < numSenderProcesses_; i++)
    {
        if ((senderProcesses_[0][i].hasRetransmission()) || (senderProcesses_[1][i].hasRetransmission()))
        {
            assure(senderProcesses_[0][i].processID() == i, "process numbering problem");
            tmp.push_back(i);
        }
    }

    return tmp;
}

unsigned int
HARQEntity::getProcessWithNextRetransmissions()
{
    assure(hasRetransmissions(), "May only be called if retransmissions are available");
    assure(numReceiverProcesses_ > 0, "No sender processes");
    
    // go over HARQ processes in a round robin style
    // The idea is that if a user is only scheduled infrequently, only the first process might
    // have a chance to retransmit.
    
    unsigned int processID = 0;
    
    for (unsigned int i = 0; i < numSenderProcesses_; i++)
    {
        processID = (nextProcessToRetransmit + i) % numSenderProcesses_;
        
        if ((senderProcesses_[0][processID].hasRetransmission()) || (senderProcesses_[1][processID].hasRetransmission()))
        {
            assure(senderProcesses_[0][processID].processID() == processID, "process numbering problem");
            break;
        }
    }

    nextProcessToRetransmit = (processID + 1) % numSenderProcesses_;
    
    return processID;
}

bool
HARQEntity::hasRetransmissions()
{
    for (unsigned int spatial = 0; spatial < 2; spatial++)
    {
        for (unsigned int ii=0; ii < numSenderProcesses_; ii++)
        {
            if (senderProcesses_[spatial][ii].hasRetransmission())
            {
                return true;
            }
        }
    }
    return false;
}

bool
HARQEntity::hasRetransmission(unsigned int processID, unsigned int spatialID)
{
    assure((spatialID == 0) || (spatialID == 1), "Spatial ID can only be 0 or 1");
    assure(processID < numSenderProcesses_, "Invalid sender process id");
    
    return senderProcesses_[spatialID][processID].hasRetransmission();
}


wns::ldk::CompoundPtr
HARQEntity::getRetransmission(unsigned int processID, unsigned int spatialID)
{
    assure((spatialID == 0) || (spatialID == 1), "Spatial ID can only be 0 or 1");
    assure(processID < numSenderProcesses_, "Invalid sender process id");
    
    return senderProcesses_[spatialID][processID].getRetransmission();
}

void
HARQEntity::retransmissionStarted(unsigned int processID, unsigned int spatialID)
{
    assure((spatialID == 0) || (spatialID == 1), "Spatial ID can only be 0 or 1");
    assure(processID < numSenderProcesses_, "Invalid sender process id");
    
    senderProcesses_[spatialID][processID].retransmissionStarted();
}


