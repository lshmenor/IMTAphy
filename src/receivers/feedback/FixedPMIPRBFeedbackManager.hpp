/*******************************************************************************
 * This file is part of IMTAphy
 * _____________________________________________________________________________
 *
 * Copyright (C) 2012
 * Institute for Communication Networks (LKN)
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

#ifndef IMTAPHY_RECEIVERS_FEEDBACK_FIXEDPMIPRBPFEEDBACKMANAGER_HPP
#define IMTAPHY_RECEIVERS_FEEDBACK_FIXEDPMIPRBPFEEDBACKMANAGER_HPP


#include <IMTAPHY/receivers/feedback/LteRel8DLFeedbackManager.hpp>

namespace imtaphy { namespace receivers { namespace feedback { 
    
    class FixedPMIPRBFeedbackManager :
        public imtaphy::receivers::feedback::LteRel8DownlinkFeedbackManager
    {
        
    public:
       FixedPMIPRBFeedbackManager(const wns::pyconfig::View& _config);

        void registerMS(imtaphy::StationPhy* station, 
                        wns::node::Interface* node, 
                        imtaphy::receivers::LinearReceiver* receiver,
                        imtaphy::Channel* channel);
        
        // IMTAphy observer interface:
        void beforeTTIover(unsigned int ttiNumber);
        
    private:
        void determinePMIsAndCQIs(imtaphy::StationPhy* receivingStation, LteRel8DownlinkFeedbackPtr feedback, unsigned int rank, unsigned int tti);
        unsigned int performRankUpdate(imtaphy::StationPhy* receivingStation);
        
        std::map<imtaphy::StationPhy*, std::vector<int> > pmiMask;
        std::vector<unsigned int> pmis;
        bool randomize;
        unsigned int fixedRank;
        
        boost::uniform_int<> uni;
        boost::variate_generator<boost::mt19937&, boost::uniform_int<> > rngForShuffle;
    };
}}}


#endif 


