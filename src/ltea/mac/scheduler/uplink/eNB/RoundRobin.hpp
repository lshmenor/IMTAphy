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

#ifndef LTEA_MAC_SCHEDULER_UPLINK_ENB_ROUNDROBIN_HPP
#define LTEA_MAC_SCHEDULER_UPLINK_ENB_ROUNDROBIN_HPP

#include <IMTAPHY/ltea/mac/scheduler/uplink/eNB/SchedulerBase.hpp>
#include <boost/random.hpp>


namespace ltea { namespace mac { namespace scheduler { namespace uplink { namespace enb {

    class RoundRobin :
        public SchedulerBase,
        public wns::Cloneable<RoundRobin>
    {
        
    public:
        RoundRobin(wns::ldk::fun::FUN*, const wns::pyconfig::View&);

    protected:
        void initScheduler();
        void doScheduling();
        
        unsigned int currentUser; 
        imtaphy::receivers::AntennaSelection<float> antennaSelection;
        bool threegppCalibration;
        
        // to allow std::random_shuffle to use openWNS' rng (boost::mt19937)
        boost::uniform_int<> uni;
        boost::variate_generator<boost::mt19937&, boost::uniform_int<> > rngForShuffle;

        typedef std::map<wns::node::Interface*, ltea::mac::scheduler::uplink::SchedulingResult> UserSchedulingResultMap;
        UserSchedulingResultMap threeGPPConstellation;
        bool threeGPPConstellationFixed;
        
    };

}}}}}
#endif 


