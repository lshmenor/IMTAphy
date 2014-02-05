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

#ifndef LTEA_MAC_HARQ_HARQ_HPP
#define LTEA_MAC_HARQ_HARQ_HPP

#include <IMTAPHY/interface/TransmissionStatus.hpp>
#include <WNS/node/Interface.hpp>
#include <IMTAPHY/ltea/mac/harq/HARQentity.hpp>
#include <set>
#include <WNS/ldk/CommandReaderInterface.hpp>
#include <IMTAPHY/detail/NodePtrCompare.hpp>

namespace ltea { namespace l2s { namespace harq {

    class DecoderInterface;
}}}

namespace ltea { namespace mac { namespace harq {

    /**
     * @brief HARQ is the instance collaborating with the scheduler.
     * Contains a collection of HARQEntity's inside; one for each peer.
     */
    class HARQ
    {
    public:
        HARQ(const wns::pyconfig::View&);
        
        virtual ~HARQ();
        
        /**
         * @brief Called by the scheduler after the scheduling step has finished. The spatialTransportBlockID
         * can be 0 or 1 for the case of spatial multiplexing
         */
        virtual void
        storeScheduledTransportBlock(wns::node::Interface* userID, wns::ldk::CompoundPtr transportBlock, unsigned int spatialID);

        virtual void
        storeScheduledTransportBlock(wns::node::Interface* userID, wns::ldk::CompoundPtr transportBlock, unsigned int processID, unsigned int spatialID);

        virtual void
        storeScheduledTransportBlocks(wns::node::Interface* userID, std::vector<wns::ldk::CompoundPtr> transportBlocks);

        
        /**
         * @brief Try to decode this redundancy version together with any previously received RV of the same TB.
         * This uses the configured decoder to decide whether the transport block can be decoded after receiving
         * this redundancy version. Returns true if decoding is successful and false otherwise. Also generates and
         * magically feeds back the ACK/NACK feedback.
         */
        virtual bool
        decodeReceivedTransportBlock(wns::node::Interface* source, wns::ldk::CompoundPtr transportBlockRedundancyVersion, imtaphy::interface::TransmissionStatusPtr status, unsigned int spatialID);
        
        virtual std::set<wns::node::Interface*, imtaphy::detail::WnsNodeInterfacePtrCompare>
        getUsersWithRetransmissions() const;
        
        std::list<int>
        getProcessesWithRetransmissions(wns::node::Interface* peer) const;

        unsigned int getProcessWithNextRetransmissions(wns::node::Interface* peer);    

        /**
         * @brief Returns the number of retransmissions for a certain user and process - can be 0, 1, or 2
         *
         */
        unsigned int
        getNumberOfRetransmissions(wns::node::Interface* peer, int processID);

        bool
        hasRetransmission(wns::node::Interface* peer, int processID, unsigned int spatialID);

        void setDCIReader(wns::ldk::CommandReaderInterface* dciReader);
        
        
        /**
         * @brief Checks whether the responsible entity has an entirely free HARQ process. 
         * Processes IDs that only have one of the two spatial processes available will be considered as busy
         * until all spatial transport blocks have been successfully delivered or discarded.
         * (cf. Holma, ""For each TTI, the packet scheduler must decide between sending a new
         *  transmission or a pending HARQ transmission to each scheduled user, i.e. a combination
         * is not allowed.")
         */
        
        virtual bool
        hasFreeSenderProcess(wns::node::Interface* peer);

        virtual wns::ldk::CompoundPtr 
        getRetransmission(wns::node::Interface* peer, int processID, unsigned int spatialID);

        virtual void 
        retransmissionStarted(wns::node::Interface* peer, int processID, unsigned int spatialID);

        
        unsigned int
        getNACKcount(wns::node::Interface* userID);
        
        unsigned int
        getACKcount(wns::node::Interface* userID);

        bool knowsUser(wns::node::Interface* userID);
        
    private:
        HARQEntity*
        findEntity(wns::node::Interface* userID);
        
        wns::logger::Logger logger_;
        
        typedef wns::container::Registry<wns::node::Interface*, HARQEntity*> HARQEntityContainer;
        
        /**
         * @brief Contains a collection of HARQEntity's inside; one for each peer.
         */
        HARQEntityContainer harqEntities_;
        
        /**
         * @brief Defines maximum number of Sender Processes.
         */
        int numSenderProcesses_;
        
        /**
         * @brief Defines maximum number of Receiver Processes.
         * Must be equal? numSenderProcesses==numReceiverProcesses ?
         */
        int numReceiverProcesses_;
        
        /**
         * @brief RV = redundancy version; Defines maximum here.
         */
        int numRVs_;
        
        int retransmissionLimit_;

        double feedbackDecodingDelay; // in ms
        
        HARQEntity* harqEntityPrototype_;
        //wns::probe::bus::ContextCollector numRetransmissionsProbeCC;
    };
    

    
}}} //namespace ltea::mac::harq

#endif 


