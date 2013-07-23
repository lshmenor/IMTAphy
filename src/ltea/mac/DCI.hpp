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
 ******************************************************************************/

#ifndef LTEA_MAC_DCI_HPP
#define LTEA_MAC_DCI_HPP

#include <WNS/ldk/Command.hpp>
#include <IMTAPHY/link2System/Modulations.hpp>
#include <IMTAPHY/interface/TransmissionStatus.hpp>
#include <IMTAPHY/interface/DataTransmission.hpp>
#include <IMTAPHY/detail/LinearAlgebra.hpp>
#include <IMTAPHY/Spectrum.hpp>

namespace ltea { namespace mac { 

    // Note that even though DCI stands for DownlinkControlInformation, it seems to be used for
    // uplink scheduling grants as well (OK, they are also signalied in the downlink). So we will
    // also use it for uplink transmisison infos.
    
    typedef std::map<std::string, double> SchedulingTracingDict;
    typedef std::map<imtaphy::interface::PRB, SchedulingTracingDict> PRBSchedulingTracingDictMap;

    class DownlinkControlInformation :
        public wns::ldk::Command
    {
        public:
            struct {
                // scheduling info to be used when doing non-adaptive HARQ-retransmissions
                imtaphy::interface::PrbPowerPrecodingMap prbPowerPrecoders;
            } local;
            
            struct { // this is actual protocol information for the peer
                std::vector<unsigned int> assignedToLayers;
                imtaphy::l2s::ModulationScheme modulation;
                double codeRate;
                unsigned int blockSize;     // magic?
                
                // this could also contain the assigned PRBs but currently we don't need this

                // HARQ information
                bool NDI;           // new data indicator
                unsigned int processID;      // HARQ process number (3 bits for FDD, would be 4 bits for TDD)
                int rv;             // redundancy version indicaton
            } peer;
            
            struct { // this is information we use in the simulator which is not availalbe in reality
                // HARQ information
                boost::function<void ()> ackCallback;    // for magic HARQ ACK feedback
                boost::function<void ()> nackCallback;   // for magic HARQ NACK feedback
                unsigned int spatialID;  // 0 or 1 indicating first or second spatial multiplexing TB, used for HARQ
                unsigned int transmissionAttempts; // to count the number of transmission attempts
                unsigned int mcsIndex; // the index of the chosen Modulation + CodeRate combination
                unsigned int transmittedInTTI; // the TTI during which this was transmitted, used e.g. in uplink HARQ
                bool lastHARQAttempt; // whether this is the final HARQ retransmission attempt
                unsigned int id;
                wns::Ratio estimatedLinkAdaptationSINR;  // the eff. SINR that was assumed during Link Adaptation
                wns::Ratio estimatedLinearAvgSINR; // directly from feedback, not influenced by link adaptation; linear average
                imtaphy::Direction direction;
                
                // used at the receiver to log last decoding result
                double bler;
                wns::Ratio effSINR;
                
                // dictionary that can hold arbitrary tracing information
                PRBSchedulingTracingDictMap prbTracingDict;

            } magic;
    };
}}


#endif 


