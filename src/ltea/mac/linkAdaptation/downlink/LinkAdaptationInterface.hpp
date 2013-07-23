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

#ifndef LTEA_MAC_LINKADAPTATION_DOWNLINK_LINKADAPTATIONINTERFACE
#define LTEA_MAC_LINKADAPTATION_DOWNLINK_LINKADAPTATIONINTERFACE

#include <WNS/pyconfig/View.hpp>
#include <WNS/node/Interface.hpp>
#include <WNS/PowerRatio.hpp>
#include <WNS/logger/Logger.hpp>

#include <IMTAPHY/interface/TransmissionStatus.hpp>
#include <IMTAPHY/link2System/BlockErrorModel.hpp>
#include <IMTAPHY/receivers/feedback/LteRel8DLFeedbackManager.hpp>
#include <IMTAPHY/ltea/mac/ModulationAndCodingSchemes.hpp>
#include <IMTAPHY/ltea/mac/harq/HARQ.hpp>
#include <IMTAPHY/ltea/mac/DCI.hpp>
#include <map>


namespace ltea { namespace mac { namespace la { namespace downlink {

        typedef std::map<imtaphy::interface::PRB, wns::Ratio> PRBpowerOffsetMap;
    
        struct LinkAdaptationResult
        {
            unsigned int mcsIndex;
            double codeRate;
            imtaphy::l2s::ModulationScheme modulation;
            wns::Ratio estimatedSINR;
            PRBSchedulingTracingDictMap prbTracingDict;
        };
    
        class LinkAdaptationInterface 
        {
        public:
            LinkAdaptationInterface(const wns::pyconfig::View& pyConfigView) {}
            
            // TODO: we should change this and pass a list of estimated SINRs to the Link Adaptation instead of 
            // relying on the feedback + powerOffset
            virtual void setFeedbackManager(imtaphy::receivers::feedback::DownlinkFeedbackManagerInterface* feedbackManager) = 0;
            virtual void setHARQ(ltea::mac::harq::HARQ* harq) = 0;
            virtual void updateAssociatedUsers(std::vector<wns::node::Interface*> allUsers) = 0;
            
            virtual void updateFeedback(unsigned int tti) = 0;
            virtual void updateBLERStatistics(unsigned int tti) = 0;
             
            virtual LinkAdaptationResult performLinkAdaptation(wns::node::Interface* userID, 
                                                               unsigned int spatialTransportBlock, 
                                                               PRBpowerOffsetMap& prbMap, 
                                                               unsigned int tti,
                                                               unsigned int numLayersForTB, 
                                                               unsigned int pdcchLength,
                                                               bool provideRel10DMRS,
                                                               unsigned int numRel10CSIrsSets
                                                               ) = 0;                     
        };
        
        class LinkAdaptationBase :
            public LinkAdaptationInterface
        {
            public:
                LinkAdaptationBase(const wns::pyconfig::View& pyConfigView) :
                    LinkAdaptationInterface(pyConfigView),
                    feedbackManager(NULL),
                    harq(NULL),
                    numRel10CSIrsPorts(pyConfigView.get<int>("Rel10CSIports")),
                    numRel8AntennaPorts(pyConfigView.get<int>("Rel8AntennaPorts")),
                    mcsLookup(ltea::mac::TheMCSLookup::getInstance()),
                    logger(pyConfigView.get<wns::pyconfig::View>("logger")),
                    blerModel(imtaphy::l2s::TheLTEBlockErrorModel::getInstance())
                    {};
                    
                virtual void setFeedbackManager(imtaphy::receivers::feedback::DownlinkFeedbackManagerInterface* feedbackManager_)
                {
                    feedbackManager = feedbackManager_;
                }
                
                virtual void setHARQ(ltea::mac::harq::HARQ* harq_)
                {
                    harq = harq_;
                }
            
            protected:
                imtaphy::receivers::feedback::DownlinkFeedbackManagerInterface* feedbackManager;
                ltea::mac::harq::HARQ* harq;
                unsigned int numRel10CSIrsPorts;
                unsigned int numRel8AntennaPorts;
                ltea::mac::ModulationAndCodingSchemes* mcsLookup;
                wns::logger::Logger logger;
                imtaphy::l2s::BlockErrorModel* blerModel;
        };   
}}}}
#endif
