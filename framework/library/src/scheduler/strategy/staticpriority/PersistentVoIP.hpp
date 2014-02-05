/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2009
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 5, D-52074 Aachen, Germany
 * phone: ++49-241-80-27910,
 * fax: ++49-241-80-22242
 * email: info@openwns.org
 * www: http://www.openwns.org
 * _____________________________________________________________________________
 *
 * openWNS is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License version 2 as published by the
 * Free Software Foundation;
 *
 * openWNS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#ifndef WNS_SCHEDULER_STRATEGY_STATICPRIORITY_PERSISTENTVOIP_HPP
#define WNS_SCHEDULER_STRATEGY_STATICPRIORITY_PERSISTENTVOIP_HPP

#include <WNS/scheduler/strategy/staticpriority/SubStrategy.hpp>
#include <WNS/scheduler/strategy/staticpriority/persistentvoip/ResourceGrid.hpp>
#include <WNS/scheduler/strategy/staticpriority/persistentvoip/StateTracker.hpp>
#include <WNS/scheduler/strategy/Strategy.hpp>
#include <WNS/scheduler/strategy/apcstrategy/APCStrategyInterface.hpp>

#include <WNS/scheduler/SchedulingMap.hpp>
#include <WNS/scheduler/queue/QueueInterface.hpp>
#include <WNS/scheduler/RegistryProxyInterface.hpp>
#include <WNS/StaticFactory.hpp>
#include <WNS/probe/bus/ContextCollector.hpp>

namespace wns { namespace scheduler { namespace strategy { namespace staticpriority {


class PersistentVoIP
        : public SubStrategy
{
    public:
        typedef std::pair<ConnectionSet, ConnectionSet> ConnectionSetPair;
        typedef std::pair<UserSet, UserSet> UserSetPair;

        struct SchedStatus
        {
            SchedStatus(PersistentVoIP* parent):
                parent(parent),
                PDCCHCountValid(false){};

            /* SID and first time active or reactivated */
            ConnectionSetPair unpersistent;

            /* Active connections scheduled persistently */
            ConnectionSet persistent;

            /* First time active */
            ConnectionSetPair setup;

            /* Active again */
            ConnectionSetPair reactivate;

            /* Active but not fitting previous persistent reservation */
            ConnectionSetPair freqRelocate;

            /* Relocated in time because not fitting application frame
               but not fitting persistent reservation anymore done earlier. */
            ConnectionSetPair timeRelocateFreqRelocate;

            /* Not enough resources in application frame, relocated to most empty frame */
            ConnectionSetPair timeRelocate;

            /* Any other stuff done if PDCCH resources available */
            ConnectionSetPair dynamic;

            /* HARQ retransmissions (HARQ operates on User IDs not CIDs) */
            UserSetPair harq;

            bool PDCCHCountValid;
            unsigned int numPDCCH;
            unsigned int numDynamicPDCCH;
            unsigned int numHARQ_PDCCH;
            unsigned int numSID_PDCCH;
            unsigned int numOtherFramePDCCH;
            unsigned int numPersSetupPDCCH;        
            unsigned int numPersSetupTimeRelocatedPDCCH;        
            unsigned int numPersRelocPDCCH;

            PersistentVoIP* parent;

            void
            calculatePDCCH();
        };

        PersistentVoIP(const wns::pyconfig::View& config);

        ~PersistentVoIP();

        virtual void
        initialize();

        virtual wns::scheduler::MapInfoCollectionPtr
        doStartSubScheduling(SchedulerStatePtr schedulerState,
                             wns::scheduler::SchedulingMapPtr schedulingMap);

    private:
        friend class SchedStatus;

        void 
        onFirstScheduling(const SchedulerStatePtr& schedulerState,
                             wns::scheduler::SchedulingMapPtr schedulingMap);

        void
        onNewPeriod();

        /* First the successfull, then the ones that did not get resources */
        ConnectionSetPair
        schedulePersistently(const ConnectionSet& cids, unsigned int frame);

        void
        processSilenced(const ConnectionSet& cids);

        void
        processPersistent(const ConnectionSet& cids);

        ConnectionSet
        checkTBSizes(ConnectionSet& cids);

        MapInfoCollectionPtr
        scheduleData(ConnectionID cid, bool persistent,
                     const SchedulerStatePtr& schedulerState,
                     const SchedulingMapPtr& schedulingMap);

        PersistentVoIP::ConnectionSetPair
        relocateCIDs(const ConnectionSet& cids);

        void
        probe(SchedStatus& stat);

        void
        probeN(const ConnectionSetPair& csp, 
            const wns::probe::bus::ContextCollector& probe);

        bool firstScheduling_;
        bool neverUsed_;
        int numberOfSubchannels_;

        unsigned int numberOfFrames_;
        unsigned int currentFrame_;

        std::vector<ConnectionSet> futurePersSetup_;
        std::map<ConnectionID, std::set<unsigned int> > timeRelocationFrames_;

        Bit voicePDUSize_;

        persistentvoip::ResourceGrid* resources_;
        persistentvoip::StateTracker stateTracker_;

        wns::probe::bus::ContextCollector frameOccupationFairness_;
        wns::probe::bus::ContextCollector activeCIDs_;
        wns::probe::bus::ContextCollector allActiveCIDs_;
        wns::probe::bus::ContextCollector queuedCIDs_;

        wns::probe::bus::ContextCollector timeRelocatedCIDs_;
        wns::probe::bus::ContextCollector freqRelocatedCIDs_;
        wns::probe::bus::ContextCollector timeFreqRelocatedCIDs_;
        wns::probe::bus::ContextCollector persCIDs_;

        wns::probe::bus::ContextCollector numPDCCH_;
        wns::probe::bus::ContextCollector numDynamicPDCCH_;        
        wns::probe::bus::ContextCollector numPersSetupPDCCH_;        
        wns::probe::bus::ContextCollector numPersSetupTimeRelocatedPDCCH_;        
        wns::probe::bus::ContextCollector numPersRelocPDCCH_;
        wns::probe::bus::ContextCollector numSID_PDCCH_;        
        wns::probe::bus::ContextCollector numOtherFrame_PDCCH_;
        wns::probe::bus::ContextCollector numHARQ_PDCCH_;

        wns::probe::bus::ContextCollector percFailedReactivations_;
        wns::probe::bus::ContextCollector percFailedSetup_;
        wns::probe::bus::ContextCollector percFailedFreqRelocation_;
        wns::probe::bus::ContextCollector percFailedTimeRelocation_;
        wns::probe::bus::ContextCollector percFailedTimeFreqRelocation_;
        wns::probe::bus::ContextCollector percFailedDynamic_;
        wns::probe::bus::ContextCollector percFailedSID_;
        wns::probe::bus::ContextCollector percFailedHARQ_;

        wns::probe::bus::ContextCollector dynPDUSize_;
        wns::probe::bus::ContextCollector TBSize_;
        wns::probe::bus::ContextCollector numReserved_;


        wns::pyconfig::View resourceGridConfig_;
        wns::pyconfig::View harqConfig_;
        SubStrategyInterface* harq_;
};

}}}}

#endif
