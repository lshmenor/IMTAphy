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

#include <WNS/scheduler/strategy/staticpriority/PersistentVoIP.hpp>
#include <WNS/scheduler/SchedulerTypes.hpp>

#include "boost/tuple/tuple.hpp"

#include <vector>
#include <map>
#include <algorithm>
#include <iostream>

using namespace std;
using namespace wns::scheduler;
using namespace wns::scheduler::strategy;
using namespace wns::scheduler::strategy::staticpriority;

STATIC_FACTORY_REGISTER_WITH_CREATOR(PersistentVoIP,
                                     SubStrategyInterface,
                                     "PersistentVoIP",
                                     wns::PyConfigViewCreator);


PersistentVoIP::PersistentVoIP(const wns::pyconfig::View& config)
    : SubStrategy(config),
    firstScheduling_(true),
    neverUsed_(true),
    numberOfFrames_(config.get<unsigned int>("numberOfFrames")),
    currentFrame_(0),
    stateTracker_(numberOfFrames_, logger),
    futurePersSetup_(numberOfFrames_),
    resources_(NULL),
    harq_(NULL),
    voicePDUSize_(config.get<Bit>("voicePDUSize")),
    resourceGridConfig_(config.get("resourceGrid")),
    harqConfig_(config.get("harq")),
    frameOccupationFairness_("scheduler.persistentvoip.FrameOccupationFairness"),
    activeCIDs_("scheduler.persistentvoip.ActiveConnections"),
    allActiveCIDs_("scheduler.persistentvoip.AllActiveConnections"),
    queuedCIDs_("scheduler.persistentvoip.QueuedConnections"),
    timeRelocatedCIDs_("scheduler.persistentvoip.TimeRelocatedConnections"),
    freqRelocatedCIDs_("scheduler.persistentvoip.FreqRelocatedConnections"),
    timeFreqRelocatedCIDs_("scheduler.persistentvoip.TimeFreqRelocatedConnections"),
    persCIDs_("scheduler.persistentvoip.PersistentConnections"),
    numPDCCH_("scheduler.persistentvoip.NumberOfPDCCHs"),
    numPersRelocPDCCH_("scheduler.persistentvoip.NumberOfPersRelocationPDCCHs"),
    numPersSetupPDCCH_("scheduler.persistentvoip.NumberOfPersSetupPDCCHs"),
    numPersSetupTimeRelocatedPDCCH_("scheduler.persistentvoip.NumberOfPersSetupTimeRelocatedPDCCHs"),
    numDynamicPDCCH_("scheduler.persistentvoip.NumberOfDynamicPDCCHs"),
    numSID_PDCCH_("scheduler.persistentvoip.NumberOfSID_PDCCHs"),
    numOtherFrame_PDCCH_("scheduler.persistentvoip.NumberOfOtherFramePDCCHs"),
    numHARQ_PDCCH_("scheduler.persistentvoip.NumberOfHARQ_PDCCHs"),
    percFailedReactivations_("scheduler.persistentvoip.failedReactivations"),
    percFailedSetup_("scheduler.persistentvoip.failedSetup"),
    percFailedFreqRelocation_("scheduler.persistentvoip.failedFreqRelocation"),
    percFailedTimeRelocation_("scheduler.persistentvoip.failedTimeRelocation"),
    percFailedTimeFreqRelocation_("scheduler.persistentvoip.failedTimeFreqRelocation"),
    percFailedDynamic_("scheduler.persistentvoip.failedDynamic"),
    percFailedSID_("scheduler.persistentvoip.failedSID"),
    percFailedHARQ_("scheduler.persistentvoip.failedHARQ"),
    dynPDUSize_("scheduler.persistentvoip.dynPDUSize"),
    TBSize_("scheduler.persistentvoip.TBSize"),
    numReserved_("scheduler.persistentvoip.ReservedRBs")
{
}

PersistentVoIP::~PersistentVoIP()
{
    if(resources_ != NULL)
        delete resources_;
}

void
PersistentVoIP::initialize()
{
}


wns::scheduler::MapInfoCollectionPtr
PersistentVoIP::doStartSubScheduling(SchedulerStatePtr schedulerState,
                                 wns::scheduler::SchedulingMapPtr schedulingMap)
{
    currentFrame_++;
    currentFrame_ %= numberOfFrames_;

    MapInfoCollectionPtr mapInfoCollection = 
        MapInfoCollectionPtr(new wns::scheduler::MapInfoCollection); // result datastructure

    ConnectionSet currentConnections = schedulerState->currentState->activeConnections;

    ConnectionSet activeConnections = colleagues.queue->filterQueuedCids(currentConnections);

    /* Unused scheduler ex. because no data present for QoS class */
    if (neverUsed_ && activeConnections.empty()) 
        return mapInfoCollection; 

    neverUsed_ = false;

    // Init the resources but only if there ever is any data queued for this priority
    if(firstScheduling_)
        onFirstScheduling(schedulerState, schedulingMap);

    assure(resources_ != NULL, "Invalid resource grid");

    persistentvoip::StateTracker::ClassifiedConnections cc;
    cc = stateTracker_.updateState(activeConnections, currentFrame_);

    /* Take care of CIDs not active anymore */
    processSilenced(cc.silencedCIDs);

    /* Done once per period, mostly probing */
    if(currentFrame_ == 0)
        onNewPeriod();

    // nothing to do?
    if (activeConnections.empty()) 
    {
        MESSAGE_SINGLE(VERBOSE, logger, "No connection has queued data");
        return mapInfoCollection; 
    }
    unsigned int nodeID = colleagues.registry->getMyUserID().getNodeID();

    activeCIDs_.put(cc.totalAppActive, boost::make_tuple("nodeID",nodeID,
        "Spot",colleagues.strategy->getSchedulerSpotType()));
    allActiveCIDs_.put(cc.totalActive, boost::make_tuple("nodeID",nodeID,
        "Spot",colleagues.strategy->getSchedulerSpotType()));
    queuedCIDs_.put(cc.totalQueued, boost::make_tuple("nodeID",nodeID,
        "Spot",colleagues.strategy->getSchedulerSpotType()));

    resources_->onNewFrame(currentFrame_, schedulingMap);

    SchedStatus status(this);

    ConnectionSet persistentCIDs;
    persistentCIDs = cc.persistentCIDs;

    // Check TB sizes for CIDs, remove persistent CIDs needing more RBs now and return them
    ConnectionSet needMore;
    unsigned int oldSize = persistentCIDs.size();    

    needMore = checkTBSizes(persistentCIDs);
    MESSAGE_SINGLE(NORMAL, logger, needMore.size() << " out of " << oldSize 
        << " persistent connections need more RBs than last frame.");
    // Clear the persistent reservations that do not fit anymore
    resources_->unscheduleCID(currentFrame_, needMore);

    // CIDs in the futurePersSetup set got their persistent allocation before but
    // the required PDCCH resources must be occupied in this frame.
    // Could be that those CIDs do not fit the allocation anymore (needMore),
    // In this case we use PDCCH resources for relocation, not setup
    ConnectionSet::iterator it;
    for(it = futurePersSetup_[currentFrame_].begin(); 
        it != futurePersSetup_[currentFrame_].end(); 
        it++)
    {
        if(needMore.find(*it) == needMore.end())
            status.timeRelocateFreqRelocate.first.insert(*it);
        else
            status.timeRelocateFreqRelocate.second.insert(*it);
    }

    // Try to find resources for reactivated CIDs
    status.reactivate = schedulePersistently(cc.reactivatedPersistentCIDs, currentFrame_);
    if(status.reactivate.second.size() > 0)
        status.timeRelocate.first = status.reactivate.second;

    // Try to find resources for new persistent CIDs
    status.setup = schedulePersistently(cc.newPersistentCIDs, currentFrame_);
    if(status.setup.second.size() > 0)
        status.timeRelocate.first.insert(status.setup.second.begin(), status.setup.second.end());

    // Try to find resources for persistent CIDs that do not fit their previous reservation
    status.freqRelocate = schedulePersistently(needMore, currentFrame_);
    if(status.freqRelocate.second.size() > 0)
        status.timeRelocate.first.insert(status.freqRelocate.second.begin(), status.freqRelocate.second.end());

    // Insert new and reactivated that got resources into persistent CID set
    // Those consume PDCCH resources to announce new persistent reservations
    persistentCIDs.insert(status.reactivate.first.begin(), status.reactivate.first.end());
    persistentCIDs.insert(status.setup.first.begin(), status.setup.first.end());
    // Those consume PDCCH resources to inform about reallocation
    persistentCIDs.insert(status.freqRelocate.first.begin(), status.freqRelocate.first.end());

    // Find new frames for CIDs not fitting this frame
    if(status.timeRelocate.first.size() > 0)
        status.timeRelocate = relocateCIDs(status.timeRelocate.first);

    MESSAGE_SINGLE(NORMAL, logger, "Persistently scheduling " << persistentCIDs.size()
        << " CIDs in frame " << currentFrame_ << ".");

    MapInfoCollectionPtr result;

    for(it = persistentCIDs.begin();
        it != persistentCIDs.end();
        it++)
    {

        MESSAGE_SINGLE(NORMAL, logger, "Scheduling data for CID " << *it);

        result = scheduleData(*it, true, schedulerState, schedulingMap);
        mapInfoCollection->join(*result);
        status.persistent.insert(*it);
    }

    assure(harq_ != NULL, "Need valid HARQ scheduler");
    if(colleagues.strategy->getSchedulerSpotType() == wns::scheduler::SchedulerSpot::ULMaster())
        status.harq.second = colleagues.harq->getPeersWithPendingRetransmissions();
    else
        status.harq.second = colleagues.harq->getUsersWithRetransmissions();
    
    result = harq_->doStartSubScheduling(schedulerState, schedulingMap);
    mapInfoCollection->join(*result);
    MapInfoCollectionList::iterator mIt;
    /* Save succesfull HARQ users in first */
    for(mIt = result->begin(); mIt != result->end(); mIt++)
    {
        status.harq.second.erase((*mIt)->user);
        status.harq.first.insert((*mIt)->user);
    }

    /* Check which resources are occupied by HARQ and mark as blocked */
    resources_->checkBlocked(currentFrame_, schedulingMap);

    MESSAGE_SINGLE(NORMAL, logger, "Dynamically scheduling " << cc.unpersistentCIDs.size()
        << " CIDs in frame " << currentFrame_ << ".");

    // TODO: Everything below here is only possible if enough PDCCH resources are available

    /* Now take care of unpersistent CIDs */

    for(it = cc.unpersistentCIDs.begin();
        it != cc.unpersistentCIDs.end();
        it++)
    {
        MESSAGE_SINGLE(NORMAL, logger, "Trying to schedule data for CID " << *it);

        /* TODO: Probe how many CIDs did not get resources */
        result = scheduleData(*it, false, schedulerState, schedulingMap);
        if(result->size() > 0)
        {
            mapInfoCollection->join(*result);
            status.unpersistent.first.insert(*it);
        }
        else
        {
            stateTracker_.unservedCID(*it);
            status.unpersistent.second.insert(*it);
        }
    }             

    activeConnections = colleagues.queue->filterQueuedCids(currentConnections);
    for(it = activeConnections.begin();
        it != activeConnections.end();
        it++)
    {
        if(timeRelocationFrames_.find(*it) == timeRelocationFrames_.end() ||
            timeRelocationFrames_[*it].find(currentFrame_) == timeRelocationFrames_[*it].end())
        { 
            /* Do not try again for CIDs that were already scheduled unpersistently in this frame */
            if(resources_->hasReservation(currentFrame_, *it, false))
                continue;

            MESSAGE_SINGLE(NORMAL, logger, "Trying to schedule data for CID " << *it 
                << " from other frame");

            result = scheduleData(*it, false, schedulerState, schedulingMap);
            if(result->size() > 0)
            {
                mapInfoCollection->join(*result);
                stateTracker_.servedInOtherFrameCID(*it);
                status.dynamic.first.insert(*it);
            }
            else
            {
                status.dynamic.second.insert(*it);
            }
        }
    }

    probe(status);

    futurePersSetup_[currentFrame_].clear();

    return mapInfoCollection;
} // doStartSubScheduling

MapInfoCollectionPtr
PersistentVoIP::scheduleData(ConnectionID cid, bool persistent,
                             const SchedulerStatePtr& schedulerState,
                             const SchedulingMapPtr& schedulingMap)
{
    MapInfoCollectionPtr mapInfoCollection = 
        MapInfoCollectionPtr(new wns::scheduler::MapInfoCollection);;

    unsigned int nodeID = colleagues.registry->getMyUserID().getNodeID();

    persistentvoip::TransmissionBlockPtr tb;
    if(persistent)
    {
        tb = resources_->getReservation(currentFrame_, cid, true);
        TBSize_.put(tb->getLength(), boost::make_tuple("nodeID",nodeID, 
                "schedUserID", colleagues.registry->getUserForCID(cid).getNodeID(),
                "Spot",colleagues.strategy->getSchedulerSpotType(),
                "Kind",0)); 
    }
    else
    {    
        Bit pduSize = colleagues.queue->getHeadOfLinePDUbits(cid);

        /* 
        There could be multiple PDUs queued. getHoLPDUBits cannot
        know the HoL PDU Size in the UL, only the total queue length.
        Limit the request size to voicePDUSize.
        */
        //pduSize = std::min(pduSize, voicePDUSize_);

        int i = 1;
        bool success = false;
        do
        {
            success = resources_->scheduleCID(currentFrame_, cid, int(pduSize / i), false);
            i = i * 2;
        }
        while(!success && i <= pduSize);
        
        if(!success)
        {
            MESSAGE_SINGLE(NORMAL, logger, "No free resources for CID " << cid
                << " (" << colleagues.registry->getUserForCID(cid) << ")"
                << " in frame " << currentFrame_ << ".");
            dynPDUSize_.put(0.0, boost::make_tuple("nodeID",nodeID, 
                "schedUserID", colleagues.registry->getUserForCID(cid).getNodeID(),
                "Spot",colleagues.strategy->getSchedulerSpotType())); 
            return mapInfoCollection;
        }
        else
        {
            dynPDUSize_.put(int(pduSize / (i / 2)), boost::make_tuple("nodeID",nodeID, 
                "schedUserID", colleagues.registry->getUserForCID(cid).getNodeID(),
                "Spot",colleagues.strategy->getSchedulerSpotType()));

            tb = resources_->getReservation(currentFrame_, cid, false);

            TBSize_.put(tb->getLength(), boost::make_tuple("nodeID",nodeID, 
                    "schedUserID", colleagues.registry->getUserForCID(cid).getNodeID(),
                    "Spot",colleagues.strategy->getSchedulerSpotType(),
                    "Kind",1)); 

        }
    }

    unsigned int start = tb->getStart();
    unsigned int length = tb->getLength();

    MESSAGE_SINGLE(NORMAL, logger, "Scheduling data from CID " << cid << " in RBs "
        << start << " to " << start + length - 1 << ".");

    UserID user = colleagues.registry->getUserForCID(cid);

    assure(colleagues.queue->queueHasPDUs(cid), "Cannot schedule CID " << cid
            << " because it has no queued PDUs");

    /* Could be we do not need all RBs because our MCS got better, so also end
    the loop if queue is empty. This should only happen for persistent reservations */
    int i;
    Bit totalBit = 0;
    for(i = 0; i < length && colleagues.queue->queueHasPDUs(cid); i++)
    {   
        MapInfoEntryPtr mapInfoEntry; 
        mapInfoEntry = MapInfoEntryPtr(new MapInfoEntry());
        mapInfoEntry->frameNr = schedulerState->currentState->strategyInput->getFrameNr();
        mapInfoEntry->subBand = start + i;
        mapInfoEntry->timeSlot = 0;
        mapInfoEntry->spatialLayer = 0;
        mapInfoEntry->user = user;
        mapInfoEntry->sourceUser = schedulerState->myUserID;
        mapInfoEntry->txPower = tb->getTxPower();
        mapInfoEntry->phyModePtr = tb->getMCS();

        /* SINR Estimation */
        mapInfoEntry->estimatedCQI.effectiveSINR = tb->getEstimatedSINR();
        mapInfoEntry->estimatedCQI.timeSlot = currentFrame_;
        if(colleagues.strategy->getSchedulerSpotType() == 
            wns::scheduler::SchedulerSpot::DLMaster())
        {
            ChannelQualityOnOneSubChannel est;
            est = colleagues.registry->estimateTxSINRAt(user, mapInfoEntry->subBand, currentFrame_);
            mapInfoEntry->estimatedCQI.pathloss = est.pathloss;
            mapInfoEntry->estimatedCQI.interference = est.interference;
            mapInfoEntry->estimatedCQI.carrier = mapInfoEntry->txPower / est.pathloss;
        }
        else
        {
            ChannelQualityOnOneSubChannel est;
            est = colleagues.registry->estimateRxSINROf(user, mapInfoEntry->subBand, currentFrame_);
            mapInfoEntry->estimatedCQI.pathloss = est.pathloss;
            mapInfoEntry->estimatedCQI.interference = 
                colleagues.registry->estimateTxSINRAt(schedulerState->myUserID, 
                    mapInfoEntry->subBand, currentFrame_).interference;
            mapInfoEntry->estimatedCQI.carrier = mapInfoEntry->txPower / est.pathloss;
        }

        int freeBits = schedulingMap->getFreeBitsOnSubChannel(mapInfoEntry);
        
        int queuedBits = colleagues.queue->getHeadOfLinePDUbits(cid);
        wns::ldk::CompoundPtr compoundPtr = colleagues.queue->getHeadOfLinePDUSegment(cid, freeBits);

        RequestForResource request(cid, user, freeBits, queuedBits, useHARQ);
        schedulingMap->addCompound(request, mapInfoEntry, compoundPtr, useHARQ);
        mapInfoEntry->compounds.push_back(compoundPtr);
        mapInfoCollection->push_back(mapInfoEntry);
        Bit currentBit = (queuedBits < freeBits?queuedBits:freeBits);        
        totalBit += currentBit;

        MESSAGE_SINGLE(VERBOSE, logger, "Scheduled " 
            << currentBit << " bit from CID " << cid << " in RB " << start + i);
    }
    assure(persistent || i == length, "All RBs must be used for dynamic scheduling.");

    MESSAGE_SINGLE(NORMAL, logger, "Scheduled " 
        << totalBit << " bit from CID " << cid << " in " << i << " RBs.");

    return mapInfoCollection;
}


ConnectionSet
PersistentVoIP::checkTBSizes(ConnectionSet& cids)
{
    ConnectionSet needMore;

    ConnectionSet::iterator it;
    for(it = cids.begin();
        it != cids.end();
        it++)
    {
        assure(resources_->hasReservation(currentFrame_, *it, true), 
            "No persistent reservation for CID " << *it);

        Bit pduSize = colleagues.queue->getHeadOfLinePDUbits(*it);
        /* 
        There could be multiple PDUs queued. getHoLPDUBits cannot
        know the HoL PDU Size in the UL, only the total queue length.
        Limit the request size to voicePDUSize.
        */
        pduSize = std::min(pduSize, voicePDUSize_);

        bool fits = resources_->fitsPersistentReservation(currentFrame_, *it, pduSize);

        if(!fits)
        {
            needMore.insert(*it);

            MESSAGE_SINGLE(NORMAL, logger, "Persistent CID " << *it 
                << " does not fit its reservation anymore.");
        }
    }

    for(it = needMore.begin();
        it != needMore.end();
        it++)
    {
        assure(cids.find(*it) != cids.end(), "Cannot remove unknown CID " << *it);
        cids.erase(*it);
    }

    return needMore;
}

PersistentVoIP::ConnectionSetPair
PersistentVoIP::schedulePersistently(const ConnectionSet& cids, unsigned int frame)
{
    ConnectionSetPair result;

    ConnectionSet::iterator it;
    for(it = cids.begin();
        it != cids.end();
        it++)
    {
        bool success;
        success = resources_->scheduleCID(frame, *it, voicePDUSize_, true);
        if(success)
        {
            MESSAGE_SINGLE(NORMAL, logger, "Succesfully scheduled new CID " << *it);
            result.first.insert(*it);
        }
        else
        {
            MESSAGE_SINGLE(NORMAL, logger, "No free resources for persistent CID " 
                << *it << " (" << colleagues.registry->getUserForCID(*it) << ")"
                << " in frame " << currentFrame_);
            stateTracker_.silenceCID(*it, frame);
            result.second.insert(*it);
        }
    }
    return result;
}
void
PersistentVoIP::processSilenced(const ConnectionSet& cids)
{
    resources_->unscheduleCID(currentFrame_, cids);
    ConnectionSet::iterator it;
    for(it = cids.begin(); it != cids.end(); it++)
            timeRelocationFrames_.erase(*it);
}

PersistentVoIP::ConnectionSetPair
PersistentVoIP::relocateCIDs(const ConnectionSet& cids)
{
#ifndef WNS_NDEBUG
    std::map<ConnectionID, unsigned int> newFrames;
#endif

    ConnectionSetPair result;
    ConnectionSet cid;

    ConnectionSet::iterator it;
    for(it = cids.begin(); it != cids.end(); it++)
    {
        cid.clear();
        cid.insert(*it);
        ConnectionSetPair oneResult;
        unsigned int start = currentFrame_;
        unsigned int newFrame = start;
        std::set<unsigned int> frames;
        do
        {
            frames.insert(newFrame);
            newFrame++;
            newFrame %= numberOfFrames_;
            oneResult = schedulePersistently(cid, newFrame);
            assure(oneResult.first.size() + oneResult.second.size() == 1, 
                "Result size sum must be one");
        }
        while(oneResult.first.size() != 1 && newFrame != start);

        /* Succcess */
        if(oneResult.first.size() == 1)
        {
            stateTracker_.relocateCID(*it, currentFrame_, newFrame);
            result.first.insert(*it);
            futurePersSetup_[newFrame].insert(*it);
            timeRelocationFrames_[*it] = frames;
#ifndef WNS_NDEBUG
            newFrames[*it] = newFrame;
#endif
        }
        /* Failure*/
        else
        {
            result.second.insert(*it);    
        }
    }
#ifndef WNS_NDEBUG
    MESSAGE_BEGIN(NORMAL, logger, m, "Relocated in frame " << currentFrame_ << ": ");
    for(it = cids.begin();
        it != cids.end();    
        it++)
    {
        m << *it 
          << " (" << colleagues.registry->getUserForCID(*it) << ")"
          << " -> " << newFrames[*it] << " ";
    }
    MESSAGE_END();
#endif
    return result;
}

void
PersistentVoIP::onFirstScheduling(const SchedulerStatePtr& schedulerState,
                             wns::scheduler::SchedulingMapPtr schedulingMap)
{
    assure(firstScheduling_, "This method may only be called once.");
    firstScheduling_ = false;

    numberOfSubchannels_ = schedulerState->getCurrentState()
        ->strategyInput->getFChannels();

    MESSAGE_SINGLE(NORMAL, logger, "Creating resource grid with " << numberOfSubchannels_ 
        << " resources per frame in " << numberOfFrames_ << " frames.");

    resources_ = new persistentvoip::ResourceGrid(resourceGridConfig_, logger, 
        numberOfFrames_, numberOfSubchannels_, 
        colleagues.registry, schedulingMap->getSlotLength(),
        colleagues.strategy->getSchedulerSpotType());

    std::string substrategyName = harqConfig_.get<std::string>("__plugin__");
    MESSAGE_SINGLE(NORMAL, logger, "Creating HARQ scheduler of type " << substrategyName);
	wns::scheduler::strategy::staticpriority::SubStrategyCreator* subStrategyCreator;
	subStrategyCreator = 
        wns::scheduler::strategy::staticpriority::SubStrategyFactory::creator(substrategyName);
	harq_ = subStrategyCreator->create(harqConfig_);
    harq_->setColleagues(colleagues.strategy,
                         colleagues.queue,
                         colleagues.registry,
                         colleagues.harq);
}

void
PersistentVoIP::probe(SchedStatus& status)
{
    if(!status.PDCCHCountValid)
        status.calculatePDCCH();

    unsigned int nodeID = colleagues.registry->getMyUserID().getNodeID();

    /* PDCCH */
    numHARQ_PDCCH_.put(status.numHARQ_PDCCH, boost::make_tuple("nodeID",nodeID,
        "Spot",colleagues.strategy->getSchedulerSpotType()));
    numSID_PDCCH_.put(status.numSID_PDCCH, boost::make_tuple("nodeID",nodeID,
        "Spot",colleagues.strategy->getSchedulerSpotType()));
    numOtherFrame_PDCCH_.put(status.numOtherFramePDCCH, boost::make_tuple("nodeID",nodeID,
        "Spot",colleagues.strategy->getSchedulerSpotType()));
    numDynamicPDCCH_.put(status.numDynamicPDCCH, boost::make_tuple("nodeID",nodeID,
        "Spot",colleagues.strategy->getSchedulerSpotType()));

    numPersSetupPDCCH_.put(status.numPersSetupPDCCH, boost::make_tuple("nodeID",nodeID,
        "Spot",colleagues.strategy->getSchedulerSpotType()));
    numPersSetupTimeRelocatedPDCCH_.put(status.numPersSetupTimeRelocatedPDCCH, 
        boost::make_tuple("nodeID",nodeID,
        "Spot",colleagues.strategy->getSchedulerSpotType()));
    numPersRelocPDCCH_.put(status.numPersRelocPDCCH, boost::make_tuple("nodeID",nodeID,
        "Spot",colleagues.strategy->getSchedulerSpotType()));
    
    numPDCCH_.put(status.numPDCCH, boost::make_tuple("nodeID",nodeID,
        "Spot",colleagues.strategy->getSchedulerSpotType()));

    /* Failure rates */
    probeN(status.reactivate, percFailedReactivations_);
    probeN(status.setup, percFailedSetup_);
    probeN(status.freqRelocate, percFailedFreqRelocation_);
    probeN(status.timeRelocate, percFailedTimeRelocation_);
    probeN(status.timeRelocateFreqRelocate, percFailedTimeFreqRelocation_);
    probeN(status.dynamic, percFailedDynamic_);
    probeN(status.unpersistent, percFailedSID_);
    UserSet::iterator it;
    for(it = status.harq.first.begin(); it != status.harq.first.end(); it++)
        percFailedHARQ_.put(0, boost::make_tuple("nodeID",nodeID, "schedUserID", it->getNodeID(),
        "Spot",colleagues.strategy->getSchedulerSpotType()));
    for(it = status.harq.second.begin(); it != status.harq.second.end(); it++)
        percFailedHARQ_.put(1, boost::make_tuple("nodeID",nodeID, "schedUserID", it->getNodeID(),
        "Spot",colleagues.strategy->getSchedulerSpotType()));
    
    /* Count */
    timeRelocatedCIDs_.put(status.timeRelocate.first.size(), 
        boost::make_tuple("nodeID",nodeID,
        "Spot",colleagues.strategy->getSchedulerSpotType()));
    freqRelocatedCIDs_.put(status.freqRelocate.first.size(), 
        boost::make_tuple("nodeID",nodeID,
        "Spot",colleagues.strategy->getSchedulerSpotType()));
    timeFreqRelocatedCIDs_.put(status.timeRelocateFreqRelocate.first.size(), 
        boost::make_tuple("nodeID",nodeID,
        "Spot",colleagues.strategy->getSchedulerSpotType()));
    persCIDs_.put(status.persistent.size(),
        boost::make_tuple("nodeID",nodeID,
        "Spot",colleagues.strategy->getSchedulerSpotType()));
    numReserved_.put(resources_->getFrame(currentFrame_)->getNumReserved(), 
        boost::make_tuple("nodeID",nodeID, 
        "Spot",colleagues.strategy->getSchedulerSpotType()));
}

void
PersistentVoIP::probeN(const ConnectionSetPair& csp, 
    const wns::probe::bus::ContextCollector& probe)
{
    unsigned int nodeID = colleagues.registry->getMyUserID().getNodeID();

    ConnectionSet::iterator it;
    for(it = csp.first.begin(); it != csp.first.end(); it++)
    {
        probe.put(0, 
            boost::make_tuple("nodeID",nodeID, 
                "schedUserID", colleagues.registry->getUserForCID(*it).getNodeID(),
                "Spot",colleagues.strategy->getSchedulerSpotType()));
    }
    for(it = csp.second.begin(); it != csp.second.end(); it++)
    {
        probe.put(1, 
            boost::make_tuple("nodeID",nodeID, 
                "schedUserID", colleagues.registry->getUserForCID(*it).getNodeID(),
                "Spot",colleagues.strategy->getSchedulerSpotType()));
    }
}

void
PersistentVoIP::onNewPeriod()
{
#ifndef WNS_NDEBUG
    for(int i = 0; i < numberOfFrames_; i++)
        MESSAGE_SINGLE(NORMAL, logger, *resources_->getFrame(i));    
#endif

    /* 
    Calculate Jain's fairness index J:
    J = 1: All frames have same amount of reserved RBs
    J -> 0: The lower J the more does the number of reserved RBs differ 
    */
    if(
#ifndef WNS_NDEBUG
        true)
#else
        frameOccupationFairness_.hasObservers())
#endif
    {
        double sum = 0;
        double squareSum = 0;
        for(int i = 0; i < numberOfFrames_; i++)
        {
            unsigned int nr = resources_->getFrame(i)->getNumReserved();
            sum += nr;
            squareSum += (nr * nr);
        }
        if(squareSum > 0)
        {
            double fairness = (sum * sum) / (double(numberOfFrames_) * squareSum);

            unsigned int nodeID = colleagues.registry->getMyUserID().getNodeID();

            frameOccupationFairness_.put(fairness, boost::make_tuple("nodeID",nodeID,
                "Spot",colleagues.strategy->getSchedulerSpotType()));

            MESSAGE_SINGLE(NORMAL, logger, "Fairness index: " << fairness);
            assure(fairness >= 0 && fairness <= 1, "Fairness index out of range");
        }
    }
}

void
PersistentVoIP::SchedStatus::calculatePDCCH()
{
    numHARQ_PDCCH = harq.first.size();
    numSID_PDCCH = unpersistent.first.size();
    numOtherFramePDCCH = dynamic.first.size();
    numDynamicPDCCH = numOtherFramePDCCH + numSID_PDCCH + numHARQ_PDCCH;

    numPersSetupPDCCH = setup.first.size();
    numPersRelocPDCCH = freqRelocate.first.size();

    /* timeRelocFreqReloc are counted in numPersRelocPDCCH */
    numPersSetupTimeRelocatedPDCCH = 
        parent->futurePersSetup_[parent->currentFrame_].size() - 
        (timeRelocateFreqRelocate.first.size() + timeRelocateFreqRelocate.second.size());

    numPDCCH = numDynamicPDCCH + numPersSetupPDCCH + 
        numPersRelocPDCCH + numPersSetupTimeRelocatedPDCCH;

    PDCCHCountValid = true;
}

