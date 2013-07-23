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

#ifndef LTEA_MAC_HARQ_HARQSENDERPROCESS_HPP
#define LTEA_MAC_HARQ_HARQSENDERPROCESS_HPP

#include <WNS/ldk/Compound.hpp>

namespace ltea { namespace mac { namespace harq {

    class HARQEntity;
    
    /**
     * @brief Sending side of the HARQ protocol.
     * There is one process per resource block in transit.
     * Receives ACK/NACK by callbacks currently.
     *
     * TS 36.213, 5.3.2.2:
     * For each subframe where a transmission takes place for the HARQ process, one or two (in case of downlink
     * spatial multiplexing) TBs and the associated HARQ information are received from the HARQ entity. For
     * each received TB and associated HARQ information, the HARQ process shall:
     *      - if the NDI, when provided, has been toggled compared to the value of the previous
     *        received transmission corresponding to this TB; or
     *      - if the HARQ process is equal to the broadcast process and if this is the first received
     *        transmission for the TB according to the system information schedule indicated by RRC; or
     *      - if this is the very first received transmission for this TB (i.e. there is no previous NDI
     *        for this TB):
     *          -  consider this transmission to be a new transmission.
     *      - else:
     *          -  consider this transmission to be a retransmission.
     * 
     */
    class HARQSenderProcess
    {
    public:
        HARQSenderProcess(HARQEntity*,
                          int processID,
                          int numRVs,
                          int retransmissionLimit,
                          double feedbackDecodingDelay,
                          const wns::logger::Logger);
        
        void
        setEntity(HARQEntity*);
        
        bool
        hasCapacity();
        
        void
        newTransmission(wns::ldk::CompoundPtr transportBlock);
        
        void
        ACK();
        
        void
        NACK();

        bool
        hasRetransmission() const;

        wns::ldk::CompoundPtr
        getRetransmission();
        
        void
        retransmissionStarted();
        
        unsigned int
        processID() const;
        
    private:
        void
        postDecodingACK();
        
        void
        postDecodingNACK();
        
        
        HARQEntity* entity_;
        unsigned int processID_;
        int numRVs_;
        unsigned int retransmissionLimit_;
        double feedbackDecodingDelay_;

        wns::simulator::Time decodingDelay_;

        unsigned int retransmissionCounter_;
        
        wns::logger::Logger logger_;
        wns::ldk::CompoundPtr retransmission;
        bool idle;
        bool retransmissionAvailable;
    };
    
    
}}} //namespace ltea::mac::harq

#endif 


