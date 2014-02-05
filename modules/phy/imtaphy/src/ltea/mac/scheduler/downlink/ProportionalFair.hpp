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

#ifndef LTEA_MAC_SCHEDULER_DOWNLINK_PROPORTIONALFAIR_HPP
#define LTEA_MAC_SCHEDULER_DOWNLINK_PROPORTIONALFAIR_HPP

#include <IMTAPHY/ltea/mac/scheduler/downlink/SchedulerBase.hpp>
#include <IMTAPHY/receivers/LteRel8Codebook.hpp>
#include <IMTAPHY/detail/NodePtrCompare.hpp>

namespace ltea { namespace mac { namespace scheduler { namespace downlink {

    class ProportionalFair :
        public SchedulerBase,
        public wns::Cloneable<ProportionalFair>
    {
        
    public:
        ProportionalFair(wns::ldk::fun::FUN*, const wns::pyconfig::View&);

    protected:
        void initScheduler();
        void doScheduling();
        
        unsigned int currentUser; 
        imtaphy::receivers::LteRel8Codebook<float>* codebook;
        
        std::map<wns::node::Interface*, double, imtaphy::detail::WnsNodeInterfacePtrCompare> throughputHistory;
        double alpha;
        double historyExponent;
        
    private:
        double estimateExpectedThroughput(imtaphy::receivers::feedback::LteRel8DownlinkFeedbackPtr feedback, unsigned int prb);    
        
    };
}}}}


#endif 


