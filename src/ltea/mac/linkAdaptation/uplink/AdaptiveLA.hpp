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

#ifndef LTEA_MAC_LINKADAPTATION_UPLINK_ADAPTIVELA
#define LTEA_MAC_LINKADAPTATION_UPLINK_ADAPTIVELA

#include <IMTAPHY/ltea/mac/linkAdaptation/uplink/LinkAdaptationBase.hpp>
#include <WNS/evaluation/statistics/moments.hpp>

namespace ltea { namespace mac { namespace la { namespace uplink {
   
        typedef std::map<unsigned int, double> IntDoubleMap;
        typedef std::map<unsigned int, bool> IntBoolMap;
        typedef std::map<unsigned int, unsigned int> IntIntMap;

        typedef std::map<wns::node::Interface*, IntDoubleMap> NodeIntDoubleMap; 
        typedef std::map<wns::node::Interface*, IntBoolMap> NodeIntBoolMap; 
        typedef std::map<wns::node::Interface*, IntIntMap> NodeIntIntMap; 
        
        typedef std::map<unsigned int, wns::evaluation::statistics::Moments> PRBmomentsMap;
        
        
        class AdaptiveLinkAdaptation :
        public LinkAdaptationBase
        {
        public:
            AdaptiveLinkAdaptation(imtaphy::StationPhy* station, const wns::pyconfig::View& pyConfigView);
            
            virtual void updateChannelStatus(unsigned int tti);
            virtual void updateBLERStatistics(unsigned int tti);
            virtual void updateAssociatedUsers(std::vector<wns::node::Interface*> allUsers);
            
        protected:
            std::vector< wns::node::Interface* > allUsers;
            imtaphy::interface::PRB numPRBs;
            NodePRBsinrMap longTimeAvg;
            
            NodeIntBoolMap aboveLongTime;
            NodeIntIntMap crossings;
            double fastCrossingWeight;
            double crossingThreshold;
            double longTimeWeight; 
            
            wns::probe::bus::ContextCollectorPtr variationsContextCollector;
            std::map<wns::node::Interface*, PRBmomentsMap> moments;
        };
}}}}
#endif
