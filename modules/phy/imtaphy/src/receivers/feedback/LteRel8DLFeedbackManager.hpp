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

#ifndef IMTAPHY_RECEIVERS_FEEDBACK_LTEREL8FEEDBACKMANAGER_HPP
#define IMTAPHY_RECEIVERS_FEEDBACK_LTEREL8FEEDBACKMANAGER_HPP


#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>

#include <WNS/node/Interface.hpp>
#include <IMTAPHY/StationPhy.hpp>
#include <IMTAPHY/receivers/LinearReceiver.hpp>
#include <IMTAPHY/receivers/LteRel8Codebook.hpp>

#include <IMTAPHY/link2System/MMIBeffectiveSINR.hpp>
#include <IMTAPHY/link2System/Modulations.hpp>
#include <IMTAPHY/receivers/feedback/LteFeedback.hpp>
#include <IMTAPHY/link2System/LteCQIs.hpp>

#include <WNS/Observer.hpp>
#include <IMTAPHY/interface/IMTAphyObserver.hpp>
#include <IMTAPHY/link2System/BlockErrorModel.hpp>

namespace imtaphy { namespace receivers { namespace feedback { 
    
    class DownlinkFeedbackManagerInterface
    {
    public:
        struct FeedbackContainer {
            FeedbackContainer(unsigned int numPRBs, unsigned int _bufferLength) :
            currentRank(1),
            bufferLength(_bufferLength)
            {
                for (unsigned int i = 0; i < bufferLength; i++)
                    ringBuffer.push_back(LteRel8DownlinkFeedbackPtr(new LteRel8DownlinkFeedback(numPRBs)));
            };
            std::vector<LteRel8DownlinkFeedbackPtr> ringBuffer;
            unsigned int currentRank;
            unsigned int bufferLength;
        };
        virtual
        ~DownlinkFeedbackManagerInterface(){}
        
        virtual void 
        registerMS( imtaphy::StationPhy* station, 
                    wns::node::Interface* node, 
                    imtaphy::receivers::LinearReceiver* receiver,
                    imtaphy::Channel* channel) = 0;
        
        virtual void 
        setReferencePerPRBTxPowerForUser(wns::node::Interface* node, wns::Power referencePower) = 0;

        virtual void 
        setReferencePerPRBTxPowerForUser(wns::node::Interface* node, imtaphy::interface::PRB prb, wns::Power referencePower) = 0;

        
        virtual LteRel8DownlinkFeedbackPtr 
        getFeedback(wns::node::Interface* node, unsigned int ttiNumber) = 0;
                        
        virtual wns::Ratio computeGeometry(wns::node::Interface* user) = 0;

        
        static DownlinkFeedbackManagerInterface*
        getFeedbackManager();
    };
    
    class LteRel8DownlinkFeedbackManager :
        public DownlinkFeedbackManagerInterface,
        public wns::Observer<imtaphy::interface::IMTAphyObserver>
    {
        
    public:
       LteRel8DownlinkFeedbackManager(const wns::pyconfig::View& _config);

        virtual wns::Ratio computeGeometry(wns::node::Interface* user);
       
        virtual void registerMS(imtaphy::StationPhy* station, 
                        wns::node::Interface* node, 
                        imtaphy::receivers::LinearReceiver* receiver,
                        imtaphy::Channel* channel);
        
        virtual void setReferencePerPRBTxPowerForUser(wns::node::Interface* node, wns::Power referencePower);
        virtual void setReferencePerPRBTxPowerForUser(wns::node::Interface* node, imtaphy::interface::PRB prb, wns::Power referencePower);

        
        
        virtual LteRel8DownlinkFeedbackPtr getFeedback(wns::node::Interface* node, unsigned int ttiNumber);
                        
        // IMTAphy observer interface:
        virtual void onNewTTI(unsigned int ttiNumber) {};
        virtual void beforeTTIover(unsigned int ttiNumber);
        
        virtual void computeIPNVariation(imtaphy::StationPhy* receivingStation, unsigned int ttiNumber);
        
    protected:
        typedef enum PrecodingModes{ ClosedLoopCodebookBased, NoPrecoding, SingleAntenna } PrecodingMode;

        virtual void doUpdate(imtaphy::StationPhy* receivingStation, FeedbackContainer* feedbackContainer, unsigned int ttiNumber);
        virtual unsigned int performRankUpdate(imtaphy::StationPhy* receivingStation);
        virtual void determinePMIsAndCQIs(imtaphy::StationPhy* receivingStation, LteRel8DownlinkFeedbackPtr feedback, unsigned int rank, unsigned int tti);
        virtual void determineNoPrecodingCQIs(imtaphy::StationPhy* receivingStation, LteRel8DownlinkFeedbackPtr feedback, unsigned int rank, PrecodingMode precodingMode, unsigned int tti);
        
        

        typedef std::pair<wns::node::Interface*, imtaphy::StationPhy*> NodeReceiverPair;
        typedef std::vector<NodeReceiverPair> NodeReceiverVector;

        wns::pyconfig::View config;
        unsigned int numPRBs;
        
        std::string precodingModeString;
        enum PrecodingModes precodingMode;
        unsigned int feedbackTotalDelay;
        
        NodeReceiverVector receivingStations;

        unsigned int prbsPerSubband;
        unsigned int cqiUpdateFrequency;
        unsigned int rankUpdateFrequency;

        bool initialized;
        imtaphy::receivers::LteRel8Codebook<float>* codebook;
        
 
        LteRel8DownlinkFeedbackPtr defaultFeedback;


        typedef std::map<wns::node::Interface*, FeedbackContainer*> NodeFeedbackMap;
        NodeFeedbackMap perNodeFeedback;
        imtaphy::l2s::EffectiveSINRModelInterface* effSINRModel;
        std::map<imtaphy::StationPhy*, imtaphy::StationPhy*> servingBSMap;
        std::map<imtaphy::StationPhy*, std::vector<wns::Power> > referencePowerMap;
        std::map<wns::node::Interface*, imtaphy::StationPhy*> node2StationLookup;
        bool enabled;
        imtaphy::l2s::BlockErrorModel* blerModel;
        wns::probe::bus::ContextCollectorPtr feedbackContextCollector;
        wns::probe::bus::ContextCollectorPtr ipnVariationContextCollector;
        imtaphy::Channel* channel;
        std::map<wns::node::Interface*, wns::Ratio> geometryCache;
        
        imtaphy::detail::ComplexFloatMatrixPtr dummyIPNPtr;
        std::map<wns::node::Interface*, std::vector<imtaphy::detail::ComplexFloatMatrixPtr> > lastIPNCovariances; 
        std::map<wns::node::Interface*, std::vector<double> > ipnDifferences;
    };
}}}


#endif 


