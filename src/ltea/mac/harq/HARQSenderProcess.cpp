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

#include <IMTAPHY/ltea/mac/harq/HARQSenderProcess.hpp>
#include <IMTAPHY/ltea/mac/DCI.hpp>
#include <boost/bind.hpp>
#include <IMTAPHY/ltea/mac/harq/HARQentity.hpp>

using namespace ltea::mac::harq;

HARQSenderProcess::HARQSenderProcess(HARQEntity* entity,
                                     int processID,
                                     int numRVs,
                                     int retransmissionLimit,
                                     double feedbackDecodingDelay,
                                     wns::logger::Logger logger):
    entity_(entity),
    processID_(processID),
    numRVs_(numRVs),
    retransmissionLimit_(retransmissionLimit),
    feedbackDecodingDelay_(feedbackDecodingDelay), // in ms, typically 3ms 
    retransmissionCounter_(0),
    logger_(logger),
    retransmission(wns::ldk::CompoundPtr()), // SmartPtr equivalent of a NULL pointer
    idle(true),
    retransmissionAvailable(false)
{
}

void
HARQSenderProcess::setEntity(HARQEntity* entity)
{
    entity_ = entity;
}

void
HARQSenderProcess::newTransmission(wns::ldk::CompoundPtr transportBlock)
{
    assure(hasCapacity(), "newTransmission even though process is still busy. Call hasCapacity first");
    assure(entity_->getDCIReader(), "Invalid DCI reader");
    assure(transportBlock->getCommandPool(), "Invalid command pool");
    assure(entity_->getDCIReader()->commandIsActivated(transportBlock->getCommandPool()), "No valid DCI");

    ltea::mac::DownlinkControlInformation* dci =
        entity_->getDCIReader()->readCommand<ltea::mac::DownlinkControlInformation>(transportBlock->getCommandPool());

    dci->peer.processID = processID_;
    dci->peer.NDI = true;
    dci->peer.rv = 0; 
    dci->magic.ackCallback = boost::bind(&HARQSenderProcess::ACK, this);
    dci->magic.nackCallback = boost::bind(&HARQSenderProcess::NACK, this);
    dci->magic.transmissionAttempts = 1;

    if (retransmissionLimit_  == 0)
        dci->magic.lastHARQAttempt = true;
    else
        dci->magic.lastHARQAttempt = false;
    
    // make a copy just in case 
    retransmission = transportBlock->copy();
    retransmissionCounter_ = 0;
    idle = false;
    retransmissionAvailable = false; // retransmission is only available after receiving and processing NACK
}

bool
HARQSenderProcess::hasCapacity()
{
    return idle;
    
}

bool
HARQSenderProcess::hasRetransmission() const
{
    return retransmissionAvailable;
}

void
HARQSenderProcess::ACK()
{
    // if there should be a delay before the feedback is available (like in a real simulation), we schedule
    // a future event. When the delay should be 0, e.g., for testing, we call the postDecodingACK immediately
    if (feedbackDecodingDelay_ > 0.0)
    {
        // after (e.g. approx 3ms) processing delay, see for example Farooq Khan, Table 12.3
        wns::simulator::getEventScheduler()->scheduleDelay(
            boost::bind(&HARQSenderProcess::postDecodingACK, this), feedbackDecodingDelay_);
    }
    else
    {
        postDecodingACK();
    }

}

void
HARQSenderProcess::postDecodingACK()
{
    // only count ACK/NACK for first transmission attempts
    if (retransmissionCounter_ == 0)
    {
        entity_->countACK();
    }

    
    MESSAGE_SINGLE(NORMAL, logger_, "HARQSender processID=" << processID_ << " received ACK");
    retransmissionCounter_ = 0;
    retransmission = wns::ldk::CompoundPtr(); // SmartPtr equivalent of a NULL pointer
    idle = true;
}

void
HARQSenderProcess::NACK()
{
    // if there should be a delay before the feedback is available (like in a real simulation), we schedule
    // a future event. When the delay should be 0, e.g., for testing, we call the postDecodingNACK immediately
    if (feedbackDecodingDelay_ > 0.0)
    {
        // after approx 3ms processing delay
        wns::simulator::getEventScheduler()->scheduleDelay(
            boost::bind(&HARQSenderProcess::postDecodingNACK, this),  feedbackDecodingDelay_);
    }
    else
    {
        postDecodingNACK();
    }
}

void
HARQSenderProcess::postDecodingNACK()
{
    MESSAGE_SINGLE(NORMAL, logger_, "HARQSender processID=" << processID_ << " received NACK for transmission attempt " << retransmissionCounter_ << " out of " << retransmissionLimit_);

    retransmissionCounter_++;
    
    // e.g., at most 3 retransmissions (configurable)
    if (retransmissionCounter_ > retransmissionLimit_) {
        MESSAGE_SINGLE(NORMAL, logger_, "HARQSender processID=" << processID_ << " retransmission limit of "
        << retransmissionLimit_ << " exceeded! Dropping transport block");
        retransmissionCounter_ = 0;
        retransmission = wns::ldk::CompoundPtr(); // SmartPtr equivalent of a NULL pointer
        idle = true;
        retransmissionAvailable = false;
    }
    else
    {
        // prepare a retransmission
        retransmissionAvailable = true;
        
        ltea::mac::DownlinkControlInformation* dci =
            entity_->getDCIReader()->readCommand<ltea::mac::DownlinkControlInformation>(retransmission->getCommandPool());

        assure(dci->peer.processID == processID_, "Process ID cannot change between retransmissions");
        dci->peer.NDI = false;  // this will be a retransmission, so no new data!

        // this is where we should distinguish  between chase combining and incremental redundancy
        dci->peer.rv = 0; 

        // only count ACK/NACK for first transmission attempts
        if (dci->magic.transmissionAttempts == 1)
        {
            entity_->countNACK();
        }


        dci->magic.transmissionAttempts += 1;
        
        if (dci->magic.transmissionAttempts == retransmissionLimit_ + 1)
            dci->magic.lastHARQAttempt = true;
        else
            dci->magic.lastHARQAttempt = false;

#ifndef WNS_NDEBUG
        if (dci->magic.ackCallback.empty())
        {
            std::cout << "Trying to retransmit resource block with empty ack callback in HARQSenderProcess::posDecodingNACK" << std::endl;
            exit(1);
        }
        
        if (dci->magic.nackCallback.empty())
        {
            std::cout << "Trying to retransmit resource block with empty nack callback in HARQSenderProcess::posDecodingNACK" << std::endl;
            exit(1);
        }
#endif
    }
}

wns::ldk::CompoundPtr
HARQSenderProcess::getRetransmission()
{
    assure(retransmissionAvailable, "No transmission stored");
    assure(retransmissionCounter_, "No retransmission prepared yet");

    return retransmission;
}

void
HARQSenderProcess::retransmissionStarted()
{
   // once we send out a retransmission, we will have to wait for the next NACK to provide the next retransmission
    retransmissionAvailable = false;
}

unsigned int
HARQSenderProcess::processID() const
{
    return processID_;
}


    

