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

#ifndef LTEA_MAC_HARQ_HARQENTITY_HPP
#define LTEA_MAC_HARQ_HARQENTITY_HPP

#include <IMTAPHY/ltea/l2s/harq/DecoderInterface.hpp>
#include <IMTAPHY/ltea/mac/harq/HARQSenderProcess.hpp>

#include <IMTAPHY/ltea/mac/harq/HARQReceiverProcess.hpp>

#include <IMTAPHY/interface/TransmissionStatus.hpp>
#include <WNS/ldk/Compound.hpp>
#include <WNS/ldk/CommandReaderInterface.hpp>
#include <WNS/Cloneable.hpp>
#include "HARQ.hpp"

namespace ltea { namespace mac { namespace harq {
    /**
    * @brief There is one HARQEntity per user (link) in BS. UTs only have one.
    *
    * See TS 36.213 5.3.2.1
    * "There is one HARQ entity at the UE for each Serving Cell which maintains a number of parallel HARQ processes.
    * Each HARQ process is associated with a HARQ process identifier. The HARQ entity directs HARQ information and
    * associated TBs received on the DL-SCH to the corresponding HARQ processes (see subclause 5.3.2.2)
    * The number of DL HARQ processes per HARQ entity is specified in [2], clause
    * 
When the physical layer is configured for downlink spatial multiplexing [2], one or two TBs are expected
    * per subframe and they are associated with the same HARQ process. Otherwise, one TB is expected per subframe
    * The UE shall:
    *   - If a downlink assignment has been indicated for this TTI:
    *       - allocate the TB(s) received from the physical layer and the associated HARQ information to
    *         the HARQ process indicated by the associated HARQ information.
    *
    */
    class HARQEntity:
    public wns::Cloneable<HARQEntity>
    {
    public:
        HARQEntity(const wns::pyconfig::View&,
                   int numSenderProcesses,
                   int numReceiverProcesses,
                   int numRVs,
                   int retransmissionLimit,
                   double feedbackDecodingDelay,
                   wns::logger::Logger logger);

        HARQEntity(const HARQEntity&);
        virtual ~HARQEntity();
        
        // Transmitter side:
        void storeScheduledTransportBlock(wns::ldk::CompoundPtr transportBlock, unsigned int spatialID);

        void storeScheduledTransportBlock(wns::ldk::CompoundPtr transportBlock, unsigned int processID, unsigned int spatialID);

        void storeScheduledTransportBlocks(std::vector<wns::ldk::CompoundPtr> transportBlocks);
        
        wns::ldk::CompoundPtr
        getRetransmission(unsigned int processID, unsigned int spatialID);

        void
        retransmissionStarted(unsigned int processID, unsigned int spatialID);

        
        bool
        hasCapacity();

        unsigned int getProcessWithNextRetransmissions();    
        
        std::list<int>
        getProcessesWithRetransmissions() const;
        
        bool
        hasRetransmissions();

        bool
        hasRetransmission(unsigned int processID, unsigned int spatialID);
        
        // Receiver side
        
        bool
        decodeReceivedTransportBlock(wns::ldk::CompoundPtr transportBlockRedundancyVersion, imtaphy::interface::TransmissionStatusPtr status, unsigned int spatialID);

        void setDCIReader(wns::ldk::CommandReaderInterface* _dciReader) {dciReader = _dciReader;}
        
        wns::ldk::CommandReaderInterface* getDCIReader() const {return dciReader;}
        ltea::l2s::harq::DecoderInterface* getDecoder() const {return decoder_;}
        
        void countACK() {acks++;}
        void countNACK() {nacks++;}
        unsigned int getACKcount() const  {return acks;}
        unsigned int getNACKcount() const {return nacks;} 

    private:
        unsigned int numSenderProcesses_;
        unsigned int numReceiverProcesses_;
        
        typedef std::vector<ltea::mac::harq::HARQSenderProcess> HARQSenderProcessVector;  // holds processes
        std::vector<HARQSenderProcessVector> senderProcesses_;           // for spatial 0 and 1

        typedef std::vector<HARQReceiverProcess> HARQReceiverProcessVector; // holds processes
        std::vector<HARQReceiverProcessVector> receiverProcesses_;          // for spatial 0 and 1


        // for Transmitter side
        int numRVs_;
        int retransmissionLimit_;
        double feedbackDecodingDelay_;

        wns::logger::Logger logger_;
        wns::ldk::CommandReaderInterface* dciReader;
        ltea::l2s::harq::DecoderInterface* decoder_;

        unsigned int nacks;
        unsigned int acks;
        
        unsigned int nextProcessToRetransmit;
    };
    
}}}

#endif 


