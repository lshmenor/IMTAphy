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

#ifndef LTEA_MAC_SCHEDULER_UPLINK_UESCHEDULERBASE_HPP
#define LTEA_MAC_SCHEDULER_UPLINK_UESCHEDULERBASE_HPP

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

#include <IMTAPHY/ltea/mac/DCI.hpp>
#include <IMTAPHY/ltea/Layer2.hpp>
#include <IMTAPHY/ltea/rlc/SegmentingQueue.hpp>
#include <IMTAPHY/ltea/mac/ModulationAndCodingSchemes.hpp>
#include <IMTAPHY/ltea/mac/harq/HARQ.hpp>
#include <IMTAPHY/ltea/mac/linkAdaptation/downlink/LinkAdaptationInterface.hpp>
#include <IMTAPHY/ltea/mac/scheduler/uplink/eNB/SchedulerBase.hpp>

#include <IMTAPHY/receivers/LteRel8Codebook.hpp>
#include <IMTAPHY/receivers/feedback/LteRel8DLFeedbackManager.hpp>
#include <IMTAPHY/Channel.hpp>
#include <IMTAPHY/Spectrum.hpp>

namespace ltea { namespace mac { namespace scheduler { namespace uplink {

    typedef std::set<imtaphy::interface::PRB> PRBSet; 
    
    class UEScheduler:
        public virtual wns::ldk::FunctionalUnit,
        public wns::ldk::CommandTypeSpecifier<DownlinkControlInformation>,
        public wns::ldk::HasReceptor<>,
        public wns::ldk::HasConnector<>,
        public wns::ldk::HasDeliverer<>,
        public wns::Observer<imtaphy::interface::IMTAphyObserver>,
        public wns::Cloneable<UEScheduler>
    {
    public:
        UEScheduler(wns::ldk::fun::FUN*, const wns::pyconfig::View&);

        // Functional Unit interfaces
        void onFUNCreated();
        void doSendData(const wns::ldk::CompoundPtr& compound);
        void doOnData(const wns::ldk::CompoundPtr& compound);

        // IMTAphy observer interface:
        void onNewTTI(unsigned int ttiNumber);
        void beforeTTIover(unsigned int ttiNumber) {};
        
        void deliverSchedulingGrant(unsigned int scheduleForTTI, ltea::mac::scheduler::uplink::SchedulingGrant& grant);
        
    protected:
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

        ltea::mac::ModulationAndCodingSchemes* mcsLookup;
        ltea::mac::harq::HARQ* harq;
        wns::ldk::CommandReaderInterface* dciReader;
        
        imtaphy::StationPhy* myStation;        
        wns::node::Interface* myServingBaseStation;
        
        imtaphy::Channel* channel;
        imtaphy::Spectrum* spectrum;
        unsigned int numTxAntennas;
        std::map<unsigned int, ltea::mac::scheduler::uplink::SchedulingGrant> receivedGrants;
        ltea::mac::scheduler::uplink::enb::SchedulerBase* myULScheduler;
        wns::Power totalTxPower;
        wns::probe::bus::ContextCollectorPtr tbSizeContextCollector;
    };
}}}}


#endif 


