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

#ifndef LTEA_MAC_HARQ_HARQRECEIVERPROCESS_HPP
#define LTEA_MAC_HARQ_HARQRECEIVERPROCESS_HPP

#include <IMTAPHY/ltea/l2s/harq/DecoderInterface.hpp>

namespace ltea { namespace mac { namespace harq {

    /**
     * @brief Receiving side of the HARQ protocol.
     * Take resource block and try to decode. prepare ACK/NACK depending on result.
     *
     * TS 36.213, 5.3.2.2
     * [Upon reception of a HARQ transport block] The UE then shall:
     * -  if this is a new transmission:
     *      - replace the data currently in the soft buffer for this TB with the received data.
     * - else if this is a retransmission:
     *      - if the data has not yet been successfully decoded:
     *          - combine the received data with the data currently in the soft buffer for this TB.
     *      - if the TB size is different from the last valid TB size signalled for this TB:
     *          -    the UE may replace the data currently in the soft buffer for this TB with the received data.
     * -  attempt to decode the data in the soft buffer for this TB;
     * -  if the data in the soft buffer was successfully decoded for this TB:
     *      -  if the HARQ process is equal to the broadcast process:
     *          - deliver the decoded MAC PDU to upper layers
     *      -  else if this is the first successful decoding of the data in the soft buffer for this TB:
     *          - deliver the decoded MAC PDU to the disassembly and demultiplexing entity
     *      
- generate a positive acknowledgement (ACK) of the data in this TB.
     * - else:
     *      - generate a negative acknowledgement (NACK) of the data in this TB.
     *
     * - indicate the generated positive or negative acknowledgement for this TB to the physical layer.
     * 
     */
    class HARQReceiverProcess
    {
        
    public:
        HARQReceiverProcess(HARQEntity*, int processID, const wns::logger::Logger);
        
        HARQReceiverProcess(const HARQReceiverProcess&);
        
        void
        setEntity(HARQEntity*);
        
        bool
        decodeReceivedTransportBlock(wns::ldk::CompoundPtr transportBlockRedundancyVersion,
                                     imtaphy::interface::TransmissionStatusPtr status);
        
        int
        processID() const;
        
    private:
        HARQEntity* entity_;
        
        int processID_;

        wns::logger::Logger logger_;
        
        ltea::l2s::harq::SoftCombiningBuffer receptionBuffer_;
    };
    
    
}}} //namespace ltea::mac::harq

#endif 


