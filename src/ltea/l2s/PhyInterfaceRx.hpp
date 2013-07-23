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
 * http://www.lkn.ei.tuCm.de/~jan/imtaphy/index.html
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

#ifndef LTEA_MAC_SCHEDULER_HPP
#define LTEA_MAC_SCHEDULER_HPP

#include <IMTAPHY/interface/DataReception.hpp>

#include <WNS/probe/bus/ContextCollector.hpp>
#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/ldk/Forwarding.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>

#include <WNS/Observer.hpp>
#include <IMTAPHY/interface/IMTAphyObserver.hpp>
#include <IMTAPHY/Link.hpp>

#include <IMTAPHY/ltea/Layer2.hpp>
#include <IMTAPHY/ltea/mac/DCI.hpp>
#include <IMTAPHY/ltea/mac/scheduler/uplink/eNB/SchedulerBase.hpp>
#include <IMTAPHY/receivers/feedback/LteRel10UplinkChannelStatusManager.hpp>
#include <IMTAPHY/link2System/MMSE-FDE.hpp>


namespace ltea { namespace l2s { 

    class PhyInterfaceRx:
        public virtual wns::ldk::FunctionalUnit,
        public wns::ldk::CommandTypeSpecifier<>,
        public wns::ldk::HasReceptor<>,
        public wns::ldk::HasConnector<>,
        public wns::ldk::HasDeliverer<>,
        public wns::Cloneable<PhyInterfaceRx>,
        public imtaphy::interface::PhyNotify,
        public wns::Observer<imtaphy::interface::IMTAphyObserver>

    {
    public:
        PhyInterfaceRx(wns::ldk::fun::FUN*, const wns::pyconfig::View&);

        void doSendData(const wns::ldk::CompoundPtr& compound);

        void doOnData(const wns::ldk::CompoundPtr& compound);
        
        // PhyNotify Interface:
        // Currently we don't offer this service ourselves, but let the Layer2
        // forward the calls. Let's see how to do this nicer later
        void onData(std::vector<wns::ldk::CompoundPtr> transportBlocks, 
                                wns::node::Interface* source, 
                                imtaphy::interface::TransmissionStatusPtr status);
                
        void channelInitialized();

        void onShutdown();
        
        void onFUNCreated();

        // IMTAphy observer interface:
        void onNewTTI(unsigned int ttiNumber) {};
        void beforeTTIover(unsigned int ttiNumber);


    private:
        void processingDelayOver(std::vector<wns::ldk::CompoundPtr> transportBlocks,
                                 wns::node::Interface* source,
                                 imtaphy::interface::TransmissionStatusPtr status);

        //
        // compound handler interface
        //
        bool doIsAccepting(const wns::ldk::CompoundPtr& compound) const;

        void doWakeup();
        
        wns::logger::Logger logger;
        ltea::mac::harq::HARQ* harq;

        unsigned int windowSize;
        wns::probe::bus::ContextCollectorPtr channelGainContextCollector;
        wns::probe::bus::ContextCollectorPtr blockErrorContextCollector;
        wns::probe::bus::ContextCollectorPtr servingBSContextCollector;
        wns::probe::bus::ContextCollectorPtr propgationContextCollector;
        wns::probe::bus::ContextCollectorPtr shadowingContextCollector;
        wns::probe::bus::ContextCollectorPtr instantaneousSINRContextCollector;
        wns::probe::bus::ContextCollectorPtr instantaneousIoTContextCollector;
        wns::probe::bus::ContextCollectorPtr postFDEAvgSINRContextCollector;


        wns::probe::bus::ContextCollectorPtr jsonTracing;
        
        
        wns::probe::bus::ContextCollectorPtr laProbingActualSINR;
        wns::probe::bus::ContextCollectorPtr laProbingEstimatedSINR;
        wns::probe::bus::ContextCollectorPtr linearAvgEstimatedSINRContextCollector;
        
        bool dumpChannel;
        unsigned int numRxAntennas;
        unsigned int numServingTxAntennas;
        unsigned int numPRBs;
        double processingDelay;
        wns::ldk::CommandReaderInterface* dciReader;
        wns::ldk::CommandReaderInterface* rlcReader;
        imtaphy::Direction receivingInDirection;
        std::map<wns::node::Interface*, imtaphy::Link*> node2LinkMap;
        imtaphy::StationPhy* station;
        ltea::Layer2* layer;
        ltea::mac::scheduler::uplink::enb::SchedulerBase* eNBULScheduler; // only to be used in the eNB
        imtaphy::receivers::feedback::UplinkChannelStatusManagerInterface* uplinkChannelStatusManager;
        imtaphy::l2s::MMSEFrequencyDomainEqualization mmsefde;
        
        typedef std::map<wns::node::Interface*, wns::evaluation::statistics::Moments> NodeMomentsMap;
        NodeMomentsMap perUserLinSINR;

    };

}}


#endif 


