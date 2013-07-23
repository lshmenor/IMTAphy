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

#include <IMTAPHY/ltea/mac/harq/HARQReceiverProcess.hpp>
#include <WNS/logger/Logger.hpp>
#include <IMTAPHY/ltea/mac/DCI.hpp>
#include <IMTAPHY/ltea/mac/harq/HARQentity.hpp>

using namespace ltea::mac::harq;

HARQReceiverProcess::HARQReceiverProcess(HARQEntity* entity, int processID, wns::logger::Logger logger):
    entity_(entity),
    processID_(processID),
    logger_(logger)
{
}

HARQReceiverProcess::HARQReceiverProcess(const HARQReceiverProcess& other):
    entity_(other.entity_), // why was this NULL??
    processID_(other.processID_),
    logger_(other.logger_)
{
}

void
HARQReceiverProcess::setEntity(HARQEntity* entity)
{
    entity_ = entity;
}

bool
HARQReceiverProcess::decodeReceivedTransportBlock(wns::ldk::CompoundPtr transportBlockRedundancyVersion, imtaphy::interface::TransmissionStatusPtr status)
{
    ltea::mac::DownlinkControlInformation* dci =
        entity_->getDCIReader()->readCommand<ltea::mac::DownlinkControlInformation>(transportBlockRedundancyVersion->getCommandPool());
    
    
    MESSAGE_BEGIN(NORMAL, logger_, m, "HarqReceiverProcess::receive processID = " << processID_);
        m << ", RV = " << dci->peer.rv << ", TransmissionAttempt=" << dci->magic.transmissionAttempts
        << " NDI= " << dci->peer.NDI;
    MESSAGE_END();

    if (dci->peer.NDI)
    {
        receptionBuffer_.clear();
    }

    if (dci->magic.ackCallback.empty())
    {
        std::cout << "Tried to decode resource block with empty ack callback" << std::endl;
        exit(1);
    }

    if (dci->magic.nackCallback.empty())
    {
        std::cout << "Tried to decode resource block with empty nack callback" << std::endl;
        exit(1);
    }

    receptionBuffer_.push_back(std::make_pair<wns::ldk::CompoundPtr, imtaphy::interface::TransmissionStatusPtr>(transportBlockRedundancyVersion, status));
    
    
    if(entity_->getDecoder()->canDecode(receptionBuffer_))
    {
        MESSAGE_SINGLE(NORMAL, logger_, "HARQReceiver processID=" << processID_ << " sucessful decoded");
        
        receptionBuffer_.clear();

        // send magic ACK feedback to sending process        
        dci->magic.ackCallback();
        
        return true;
    }
    else
    {
            
        MESSAGE_SINGLE(NORMAL, logger_, "HARQReceiver processID=" << processID_ << " failed to decode"
        << " Transmission attempt: " << dci->magic.transmissionAttempts);

        // send magic NACK feedback to sending process        
        dci->magic.nackCallback();

        return false;
    }
}



int
HARQReceiverProcess::processID() const
{
    return processID_;
}

