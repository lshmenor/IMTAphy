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

#ifndef LTEA_MAC_SCHEDULER_UPLINK_ENB_SCHEDULERBASE_HPP
#define LTEA_MAC_SCHEDULER_UPLINK_ENB_SCHEDULERBASE_HPP

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
#include <WNS/service/Service.hpp>

#include <IMTAPHY/ltea/mac/DCI.hpp>
#include <IMTAPHY/ltea/Layer2.hpp>
#include <IMTAPHY/ltea/rlc/SegmentingQueue.hpp>
#include <IMTAPHY/ltea/mac/ModulationAndCodingSchemes.hpp>
#include <IMTAPHY/ltea/mac/harq/HARQ.hpp>
#include <IMTAPHY/ltea/mac/linkAdaptation/uplink/LinkAdaptationInterface.hpp>
#include <IMTAPHY/ltea/mac/scheduler/ResourceManagerInterface.hpp>
#include <IMTAPHY/ltea/mac/scheduler/UsersPRBManager.hpp>

#include <IMTAPHY/receivers/feedback/LteRel10UplinkChannelStatusManager.hpp>
#include <IMTAPHY/receivers/LteRel8Codebook.hpp>
#include <IMTAPHY/Channel.hpp>
#include <IMTAPHY/Spectrum.hpp>

namespace ltea { namespace mac { namespace scheduler { namespace uplink { 
    
    class UEScheduler;

    struct SchedulingGrant 
    {
        unsigned int scheduledForTTI;
        unsigned int rank;
        imtaphy::interface::PrbPowerPrecodingMap prbPowerPrecoders;
        unsigned int mcsIndex;
        double codeRate;
        // magic starts here
        wns::Ratio estimatedSINR; // from link adaptation
        wns::Ratio estimatedLinearAvgSINR; // from uplink estimation; linear average
        PRBSchedulingTracingDictMap prbTracingDict;
    };
    
    struct SchedulingResult 
    {
        unsigned int rank;
        wns::node::Interface* scheduledUser;
        imtaphy::interface::PrbPowerPrecodingMap prbPowerPrecoders;
        wns::Ratio closedLoopPowerControlDelta;
        bool useDeltaMCS;
    };
    
    struct SchedulingRequest
    {
        wns::node::Interface* requestingUser;
        UEScheduler* ueScheduler;
        wns::Power totalAvailableTxPower;
    };

    
    namespace enb {

    typedef std::map< wns::node::Interface*, imtaphy::interface::PrbPowerPrecodingMap, imtaphy::detail::WnsNodeInterfacePtrCompare > PerUserPRBandPrecoderMap;

    class UplinkGrants :
        public virtual wns::service::Service 
    {
        public:
            virtual void schedulingRequest(SchedulingRequest& request) = 0;
    };

    
    class PowerControlInterface {
        public:
            PowerControlInterface() {};
            
            virtual wns::Power getOpenLoopPerPRBPower(wns::node::Interface* user, unsigned int numPRBs, wns::Ratio deltaTF) = 0;
            virtual wns::Power getSRSPerPRBPower(wns::node::Interface* user) = 0;
            virtual wns::Ratio getOpenLoopPerPRBPowerHeadroom(wns::node::Interface* user, unsigned int numPRBs) = 0;
    };
    
    
    typedef std::map<wns::node::Interface*, SchedulingRequest> NodesSchedulingRequestMap;        

    class SchedulerBase:
        public virtual wns::ldk::FunctionalUnit,
        public wns::Observer<imtaphy::interface::IMTAphyObserver>,
        // unfortunately we need these interfaces/base classes to be a functional unit
        public wns::ldk::CommandTypeSpecifier<DownlinkControlInformation>,
        public wns::ldk::HasReceptor<>,
        public wns::ldk::HasConnector<>,
        public wns::ldk::HasDeliverer<>,
        public UplinkGrants,
        public PowerControlInterface
    {
    public:
        SchedulerBase(wns::ldk::fun::FUN*, const wns::pyconfig::View&);

        // Functional Unit interfaces
        void onFUNCreated();
        void doSendData(const wns::ldk::CompoundPtr& compound) {assure(0, "Don't dare to call this");}
        void doOnData(const wns::ldk::CompoundPtr& compound) {assure(0, "Don't dare to call this");}
        
        void registerHARQRetransmission(wns::node::Interface* user, imtaphy::interface::PRBVector& prbs, unsigned int failedDuringTTI);
        
        
        // IMTAphy observer interface:
        void onNewTTI(unsigned int ttiNumber);
        void beforeTTIover(unsigned int ttiNumber) {};

        
        void schedulingRequest(SchedulingRequest& request);

        // Power Control Interface
        wns::Power getOpenLoopPerPRBPower(wns::node::Interface* user, unsigned int numPRBs, wns::Ratio deltaTF);
        wns::Power getSRSPerPRBPower(wns::node::Interface* user);
        wns::Ratio getOpenLoopPerPRBPowerHeadroom(wns::node::Interface* user, unsigned int numPRBs);
        
    protected:
        // compound handler interface
        bool doIsAccepting(const wns::ldk::CompoundPtr& compound) const {assure(0, "Don't dare to call this"); return false;}
        void doWakeup() {assure(0, "Don't dare to call this");}

        virtual void applyPUCCHandPRACHrestrictions(ltea::mac::scheduler::UsersPRBManager* usersPRBManager);
                                               
        virtual void initScheduler() = 0;
        virtual void doScheduling() = 0;
        
        virtual void doLinkAdapationAndPowerControlAndInformUEs();
        
        virtual wns::Ratio getPathlossForPowerControl(imtaphy::Link* link);
        
        wns::ldk::fun::FUN* fun;
        ltea::Layer2* layer;
        
        wns::logger::Logger logger;
        imtaphy::interface::DataTransmission* txService;
        unsigned int numRxAntennas;        

        imtaphy::receivers::feedback::UplinkChannelStatusManagerInterface* channelStatusManager;
        ltea::mac::ModulationAndCodingSchemes* mcsLookup;
        wns::ldk::CommandReaderInterface* dciReader;
        
        ltea::mac::la::uplink::LinkAdaptationInterface* linkAdaptation;
        ltea::mac::scheduler::ResourceManagerInterface* resourceManager;
        imtaphy::StationPhy* myStation;        
        
        std::list<SchedulingResult> scheduledUsers;
        std::vector<wns::node::Interface*> allUsers;
        imtaphy::Channel* channel;
        imtaphy::Spectrum* spectrum;
        unsigned int scheduleForTTI;
        unsigned int currentTTI;
        unsigned int harqPeriodLength;
        std::vector<ltea::mac::scheduler::UsersPRBManager*> usersPRBManager;
        ltea::mac::scheduler::UsersPRBManager* currentUsersPRBManager;
        NodesSchedulingRequestMap schedulingRequests;
        std::map<wns::node::Interface*, imtaphy::Link*> node2LinkMap;
        double alpha;
        wns::Power P0;
        unsigned int pucchSize;
        unsigned int prachPeriod;
        unsigned int srsPeriod;
        double Ks;
        std::string pathlossEstimationMethod;
        std::map<imtaphy::Link*, double> gainMap;
        double weightingFactor;
        wns::pyconfig::View pyConfig;
    };
}}}}}


#endif 


