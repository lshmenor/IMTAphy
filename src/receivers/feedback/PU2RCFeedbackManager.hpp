/*******************************************************************************
 * This file is part of IMTAphy
 * _____________________________________________________________________________
 *
 * Copyright (C) 2012
 * Institute for Communication Networks (LKN)
 * Associate Institute for Signal Processing (MSV)
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

#ifndef IMTAPHY_RECEIVERS_FEEDBACK_PU2RCFEEDBACKMANAGER_HPP
#define IMTAPHY_RECEIVERS_FEEDBACK_PU2RCFEEDBACKMANAGER_HPP


#include <IMTAPHY/receivers/feedback/LteRel8DLFeedbackManager.hpp>

namespace imtaphy { namespace receivers { namespace feedback { 
    
    class PU2RCFeedback :
        public imtaphy::receivers::feedback::LteRel8DownlinkFeedback
    {
    public: 
        PU2RCFeedback() : 
            LteRel8DownlinkFeedback() {};
        PU2RCFeedback(unsigned int _numPRBs, unsigned int numPMIs, unsigned int numCIs) :
            LteRel8DownlinkFeedback(_numPRBs),
            columnIndicator(_numPRBs, 0),
            sinrMatrix(boost::extents[_numPRBs][numPMIs][numCIs])
            {
                pmi = std::vector<imtaphy::receivers::feedback::PMI>(numPRBs, 0);
            };
     
            std::vector<unsigned int> columnIndicator;
            boost::multi_array<wns::Ratio, 3> sinrMatrix; 
    };
    
    class PU2RCFeedbackManager :
        public imtaphy::receivers::feedback::LteRel8DownlinkFeedbackManager
    {
        
    public:
       PU2RCFeedbackManager(const wns::pyconfig::View& _config) :
        imtaphy::receivers::feedback::LteRel8DownlinkFeedbackManager(_config)
       {
            wns::pyconfig::Sequence pmiSequence = _config.getSequence("pmis");
            for (wns::pyconfig::Sequence::iterator<int> iter = pmiSequence.begin<int>(); iter != pmiSequence.end<int>(); iter++)
            {
                assure(*iter >= 0, "Invalid PMI: must not be negative");
                assure(*iter < 16, "Invalid PMI: must be smaller than 16");
                
                std::cout << "Adding PMI " << *iter << " to list of allowed PMIs\n";
                
                pmis.push_back(*iter);
            }
       };

        std::vector<unsigned int> getPMIs() {return pmis;}
        
        void registerMS(imtaphy::StationPhy* station, wns::node::Interface* node, imtaphy::receivers::LinearReceiver* receiver, imtaphy::Channel* channel_);
                        
    protected:
        void doUpdate(imtaphy::StationPhy* receivingStation, FeedbackContainer* feedbackContainer, unsigned int ttiNumber);
        void determinePU2RCFeedback(imtaphy::StationPhy* receivingStation, LteRel8DownlinkFeedbackPtr feedback, unsigned int tti);

    private:
        std::vector<unsigned int> pmis;
    };
}}}


#endif 


