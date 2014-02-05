/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
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

#include <WNS/scheduler/strategy/staticpriority/HARQUplinkRetransmission.hpp>
#include <WNS/scheduler/strategy/dsastrategy/DSAStrategyInterface.hpp>

using namespace wns::scheduler::strategy::staticpriority;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    HARQUplinkRetransmission,
    SubStrategyInterface,
    "HARQUplinkRetransmission",
    wns::PyConfigViewCreator);

HARQUplinkRetransmission::HARQUplinkRetransmission(const wns::pyconfig::View& config):
    logger_(config.get("logger"))
{
    MESSAGE_SINGLE(NORMAL, logger, "HARQUplinkRetransmission(): constructed");
}

HARQUplinkRetransmission::~HARQUplinkRetransmission()
{
}

void
HARQUplinkRetransmission::initialize()
{
}

std::vector<int>
HARQUplinkRetransmission::getUsableSubChannelsIDs(wns::scheduler::UserID user, const wns::scheduler::SchedulingMapPtr& schedulingMap)
{
    std::vector<int> result;

    for (wns::scheduler::SubChannelVector::const_iterator iterSubChannel = schedulingMap->subChannels.begin();
         iterSubChannel != schedulingMap->subChannels.end();
         ++iterSubChannel
        )
    {
        const SchedulingSubChannel& subChannel = *iterSubChannel;
        int subChannelIndex = subChannel.subChannelIndex;

        // Is it blocked?
        if (!subChannel.subChannelIsUsable) continue;

        for ( SchedulingTimeSlotPtrVector::const_iterator iterTimeSlot = subChannel.temporalResources.begin();
              iterTimeSlot != subChannel.temporalResources.end(); ++iterTimeSlot)
        {
            const SchedulingTimeSlotPtr timeSlotPtr = *iterTimeSlot;
            int timeSlotIndex = timeSlotPtr->timeSlotIndex;
            if ( ((!timeSlotPtr->getUserID().isValid()) ||
                  timeSlotPtr->getUserID() == user) &&
                 timeSlotPtr->countScheduledCompounds()==0)
            { // free space found. Pack it into.
                result.push_back(subChannel.subChannelIndex);
            }
        }
    }
    return result;
}

wns::scheduler::MapInfoCollectionPtr
HARQUplinkRetransmission::doStartSubScheduling(SchedulerStatePtr schedulerState,
                                               wns::scheduler::SchedulingMapPtr schedulingMap)
{
    MapInfoCollectionPtr mapInfoCollection = MapInfoCollectionPtr(new wns::scheduler::MapInfoCollection);

    if (colleagues.harq->getPeersWithPendingRetransmissions().size() == 0)
    {
        return mapInfoCollection;
    }

    wns::scheduler::UserSet us = colleagues.harq->getPeersWithPendingRetransmissions();

    wns::scheduler::UserSet::iterator user;

    for(user = us.begin(); user!=us.end(); ++user)
    {
        RequestForResource request(0,*user, 0, 0, true);
        dsastrategy::DSAResult resource;

        MESSAGE_SINGLE(NORMAL, logger, "HARQUplinkRetransmission(): Trying uplink retransmission for user " << user->getName());

        int processToSchedule = colleagues.harq->getPeerProcessesWithRetransmissions(*user).front();

        MESSAGE_BEGIN(NORMAL, logger, m, "HARQUplinkRetransmission(): user " << user->getName());
        m << " has " << colleagues.harq->getPeerProcessesWithRetransmissions(*user).size() << " processes with retransmissions";
        m << " choosing PID=" << processToSchedule;
        MESSAGE_END();

        std::vector<int> subchannels = getUsableSubChannelsIDs(*user, schedulingMap);

        size_t numAvailable = subchannels.size();

        int numRetransmissionsForUser = colleagues.harq->getNumberOfPeerRetransmissions(*user, processToSchedule);

        MESSAGE_SINGLE(NORMAL, logger, "HARQUplinkRetransmission(): need to schedule " << numRetransmissionsForUser << " on " << numAvailable << " available SCs");
        if(numRetransmissionsForUser > numAvailable)
        {
            MESSAGE_SINGLE(NORMAL, logger, "HARQUplinkRetransmission(): Not enough free SCs for  retransmission");
            continue;
        }

        while ( (numRetransmissionsForUser > 0) && (numAvailable > 0) )
        {
            resource = colleagues.strategy->getDSAStrategy()->getSubChannelWithDSA(request, schedulerState, schedulingMap);
            int sc = resource.subChannel;

            // Could be the scheduler has no resources for us
            if(sc == wns::scheduler::strategy::dsastrategy::DSAsubChannelNotFound)
                break;

            int ts = resource.timeSlot;
            int numberOfSpatialLayers = schedulingMap->subChannels[sc].temporalResources[ts]->numSpatialLayers;
            for ( int spatialIndex = 0; spatialIndex < numberOfSpatialLayers; ++spatialIndex )
            { // only for MIMO. For SISO simply spatialIndex=0
                PhysicalResourceBlock& prbDescriptor =
                    schedulingMap->subChannels[sc].temporalResources[ts]->physicalResources[spatialIndex];

                if (prbDescriptor.hasScheduledCompounds())
                {
                    continue;
                }
                else
                {
                    MESSAGE_SINGLE(NORMAL, logger, "HARQUplinkRetransmission(): Scheduling uplink retransmission for user " << user->getName() << " on SC " << sc);
                    wns::service::phy::phymode::PhyModeInterfacePtr pm = colleagues.registry->getPhyModeMapper()->getLowestPhyMode();
                    prbDescriptor.getNetBlockSizeInBits();
                    prbDescriptor.addCompound(0.001, 0, *user, colleagues.registry->getMyUserID(), wns::ldk::CompoundPtr(), pm, wns::Power(), wns::service::phy::ofdma::PatternPtr(), wns::scheduler::ChannelQualityOnOneSubChannel(), true);

                    prbDescriptor.grantFullResources();

                    // Set flag so slave strategy can find TimeSlotPtrs for HARQ Retransmissions
                    schedulingMap->subChannels[sc].temporalResources[ts]->harq.reservedForRetransmission = true;
                    schedulingMap->subChannels[sc].temporalResources[ts]->harq.processID = processToSchedule;
                    colleagues.harq->schedulePeerRetransmission(*user, processToSchedule);
                    MapInfoEntryPtr mapInfoEntry; 
                    mapInfoEntry = MapInfoEntryPtr(new MapInfoEntry());
                    mapInfoEntry->user = *user;
                    mapInfoEntry->subBand = sc;
                    mapInfoEntry->timeSlot = ts;
                    mapInfoEntry->spatialLayer = spatialIndex;
                    mapInfoCollection->push_back(mapInfoEntry);

                    numRetransmissionsForUser--;
                }
            }
            subchannels = getUsableSubChannelsIDs(*user, schedulingMap);
            numAvailable = subchannels.size();
        }
    }

    return mapInfoCollection;
}
