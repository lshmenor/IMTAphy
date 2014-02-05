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

#ifndef IMTAPHY_RECEIVERS_FEEDBACK_LTEREL8DLFEEDBACK_HPP
#define IMTAPHY_RECEIVERS_FEEDBACK_LTEREL8DLFEEDBACK_HPP


#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/node/Interface.hpp>
#include <IMTAPHY/StationPhy.hpp>
#include <IMTAPHY/receivers/LinearReceiver.hpp>

#include <IMTAPHY/detail/LookupTable.hpp>
#include <IMTAPHY/link2System/Modulations.hpp>
#include <IMTAPHY/link2System/LteCQIs.hpp>

#include <WNS/Observer.hpp>
#include <IMTAPHY/interface/IMTAphyObserver.hpp>


namespace imtaphy { namespace receivers { namespace feedback { 
    
    typedef int PMI;
    typedef int Rank;

    enum PMIs {EqualPower = 32, FirstAntenna = -1, SecondAntenna = -2};
    
    
    class LteRel8DownlinkFeedback
    {
        public:
            LteRel8DownlinkFeedback()
            {
                assure(0, "Do not use standard constructor, need to provide numPRBs");
            };
            
            virtual ~LteRel8DownlinkFeedback() {};
            
            LteRel8DownlinkFeedback(unsigned int _numPRBs) :
                numPRBs(_numPRBs),
                rank(1),
                pmi(_numPRBs, EqualPower),
                cqiTb1(_numPRBs, 0),
                cqiTb2(_numPRBs, 0),
                estimatedInTTI(_numPRBs, 0),
                magicSINRs(boost::extents[_numPRBs][4])
                {};

             // TODO: once this has stabilized, provide get/set methods
             unsigned int numPRBs;
             unsigned int rank;             
             std::vector<PMI> pmi;

             std::vector<imtaphy::l2s::CQI> cqiTb1;
             std::vector<imtaphy::l2s::CQI> cqiTb2;
             std::vector<unsigned int> estimatedInTTI;
             boost::multi_array<wns::Ratio, 2> magicSINRs;
             
    }; 
    typedef boost::shared_ptr<LteRel8DownlinkFeedback> LteRel8DownlinkFeedbackPtr;
    
 
    class LteRel10UplinkChannelStatus
    {
        public:
            LteRel10UplinkChannelStatus()
            {
                assure(0, "Do not use standard constructor, need to provide numPRBs");
            };
            
            LteRel10UplinkChannelStatus(unsigned int _numPRBs) :
                numPRBs(_numPRBs),
                rank(1),
                pmi(_numPRBs, EqualPower),
                sinrsTb1(_numPRBs, wns::Ratio::from_factor(1.0)), // default status 0dB
                sinrsTb2(_numPRBs, wns::Ratio::from_factor(1.0)),
                estimatedInTTI(_numPRBs, 0)
                {}

             // TODO: once this has stabilized, provide get/set methods
             unsigned int numPRBs;
             unsigned int rank;             
             std::vector<PMI> pmi;

             std::vector<wns::Ratio> sinrsTb1;
             std::vector<wns::Ratio> sinrsTb2;
             std::vector<unsigned int> estimatedInTTI;
             
    }; 
    
    typedef boost::shared_ptr<LteRel10UplinkChannelStatus> LteRel10UplinkChannelStatusPtr;

    
}}}


#endif 


