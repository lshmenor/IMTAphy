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

#ifndef LTEA_MAC_SCHEDULER_DOWNLINK_SCHEDULERBASE_HPP
#define LTEA_MAC_SCHEDULER_DOWNLINK_SCHEDULERBASE_HPP

#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/ldk/Forwarding.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>

#include <WNS/Observer.hpp>
#include <IMTAPHY/interface/IMTAphyObserver.hpp>
#include <IMTAPHY/interface/DataTransmission.hpp>

#include <IMTAPHY/Channel.hpp>

#include <IMTAPHY/ltea/mac/DCI.hpp>
#include <IMTAPHY/ltea/Layer2.hpp>
#include <IMTAPHY/ltea/rlc/SegmentingQueue.hpp>
#include <IMTAPHY/ltea/mac/ModulationAndCodingSchemes.hpp>
#include <IMTAPHY/ltea/mac/harq/HARQ.hpp>
#include <IMTAPHY/ltea/mac/linkAdaptation/downlink/LinkAdaptationInterface.hpp>
#include <IMTAPHY/ltea/mac/scheduler/ResourceManagerInterface.hpp>
#include <IMTAPHY/ltea/mac/scheduler/UsersPRBManager.hpp>

#include <IMTAPHY/receivers/LteRel8Codebook.hpp>
#include <IMTAPHY/receivers/feedback/LteRel8DLFeedbackManager.hpp>
#include <IMTAPHY/Spectrum.hpp>
#include <IMTAPHY/detail/NodePtrCompare.hpp>

namespace ltea { namespace mac { namespace scheduler { namespace downlink {
    
    typedef std::map< wns::node::Interface*, imtaphy::interface::PrbPowerPrecodingMap, imtaphy::detail::WnsNodeInterfacePtrCompare > PerUserPrbPowerPrecoderMap;
    typedef std::map< wns::node::Interface*, ltea::mac::la::downlink::PRBpowerOffsetMap, imtaphy::detail::WnsNodeInterfacePtrCompare > PerUserPowerOffsetMap;

        
    struct SchedulingResult 
    {
        unsigned int rank;
        wns::node::Interface* scheduledUser;
        ltea::mac::PRBSchedulingTracingDictMap prbAndTracingInfo;
        imtaphy::interface::PrbPowerPrecodingMap prbPowerPrecodingMap;
        ltea::mac::la::downlink::PRBpowerOffsetMap prbPowerOffsetForLA; // just for LinkAdaptation, not for power loading
    };
    
    class SchedulerBase:
        public virtual wns::ldk::FunctionalUnit,
        public wns::ldk::CommandTypeSpecifier<DownlinkControlInformation>,
        public wns::ldk::HasReceptor<>,
        public wns::ldk::HasConnector<>,
        public wns::ldk::HasDeliverer<>,
        public wns::Observer<imtaphy::interface::IMTAphyObserver>
    {
    public:
        SchedulerBase(wns::ldk::fun::FUN*, const wns::pyconfig::View&);

        // Functional Unit interfaces
        void onFUNCreated();
        void doSendData(const wns::ldk::CompoundPtr& compound);
        void doOnData(const wns::ldk::CompoundPtr& compound);

        // IMTAphy observer interface:
        void onNewTTI(unsigned int ttiNumber);
        void beforeTTIover(unsigned int ttiNumber) {};

        
    protected:

        virtual void determineActiveUsers();
        virtual void performRetransmissions();
        virtual bool performRetransmissionsFor(wns::node::Interface* user);
                                                       
        virtual void initScheduler() = 0;
        virtual void doScheduling() = 0;
        
        virtual void doLinkAdapationAndRegisterTransmissions();
        
        //
        // compound handler interface
        //
        bool doIsAccepting(const wns::ldk::CompoundPtr& compound) const;

        void doWakeup();
        
        wns::ldk::fun::FUN* fun;
        ltea::Layer2* layer;
        
        wns::logger::Logger logger;
        imtaphy::interface::DataTransmission* txService;
        
        struct Friends 
        {
            Friends()
            {
                pdcpCommandReader = NULL; 
            }

            wns::ldk::CommandReaderInterface* pdcpCommandReader;
        } friends;
        
        ltea::rlc::IQueue* queue;

        imtaphy::receivers::feedback::DownlinkFeedbackManagerInterface* feedbackManager;
        ltea::mac::ModulationAndCodingSchemes* mcsLookup;
        ltea::mac::harq::HARQ* harq;
        wns::ldk::CommandReaderInterface* dciReader;
        wns::Power txPowerdBmPerPRB;
        
        ltea::mac::la::downlink::LinkAdaptationInterface* linkAdaptation;
        ltea::mac::scheduler::ResourceManagerInterface* resourceManager;
        imtaphy::StationPhy* myStation;        
        
        std::list<SchedulingResult> scheduledUsers;
        std::vector<wns::node::Interface*> allUsers;
        unsigned int currentRetransmissionUser;
        imtaphy::Channel* channel;
        imtaphy::Spectrum* spectrum;
        unsigned int scheduleForTTI;
        unsigned int pdcchLength;
        bool provideRel10DMRS;
        bool syncHARQ;
        unsigned int numRel10CSIrsSets;
        unsigned int numTxAntennas;
        ltea::mac::scheduler::UsersPRBManager usersPRBManager;
        wns::probe::bus::ContextCollectorPtr rankContextCollector;
        wns::probe::bus::ContextCollectorPtr tbSizeContextCollector;

    };
}}}}


#endif 


