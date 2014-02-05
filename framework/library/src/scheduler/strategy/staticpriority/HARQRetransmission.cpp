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

#include <WNS/scheduler/strategy/staticpriority/HARQRetransmission.hpp>
#include <WNS/scheduler/SchedulerTypes.hpp>
#include <WNS/scheduler/strategy/dsastrategy/DSAStrategyInterface.hpp>

#include <vector>
#include <map>
#include <algorithm>
#include <iostream>

using namespace std;
using namespace wns::scheduler;
using namespace wns::scheduler::strategy;
using namespace wns::scheduler::strategy::staticpriority;

STATIC_FACTORY_REGISTER_WITH_CREATOR(HARQRetransmission,
                                     SubStrategyInterface,
                                     "HARQRetransmission",
                                     wns::PyConfigViewCreator);


HARQRetransmission::HARQRetransmission(const wns::pyconfig::View& config)
    : SubStrategy(config)
{
    blockSize=INT_MAX; // ExhaustiveRoundRobin=RoundRobin with infinite blockSize
    MESSAGE_SINGLE(NORMAL, logger, "HARQRetransmission(): constructed");
}

HARQRetransmission::~HARQRetransmission()
{
}

void
HARQRetransmission::initialize()
{
    // make state
    lastScheduledConnection = 0;
    //colleagues.harq->initialize(); // TODO!
    MESSAGE_SINGLE(NORMAL, logger, "HARQRetransmission(): initialized");
}

wns::scheduler::ConnectionID
HARQRetransmission::getValidCurrentConnection(const ConnectionSet &currentConnections, ConnectionID cid) const
{
    MESSAGE_SINGLE(NORMAL, logger, "HARQRetransmission::getValidCurrentConnection");
    // uses state var currentConnections
    wns::scheduler::ConnectionSet::iterator iter =
        currentConnections.upper_bound(cid);
    if ( iter != currentConnections.end() ) { // exists
        return *iter;
    } else { // continue with next higher cid
        return *currentConnections.begin();
    }
    //MESSAGE_SINGLE(NORMAL, logger, "getValidCurrentConnection("<<cid<<") = ");
}

wns::scheduler::ConnectionID
HARQRetransmission::getNextConnection(const ConnectionSet &currentConnections, ConnectionID cid) const
{
    MESSAGE_SINGLE(NORMAL, logger, "HARQRetransmission::getNextConnection");
    // uses state var currentConnections
    wns::scheduler::ConnectionSet::iterator iter =
        currentConnections.upper_bound(cid);
    if ( iter != currentConnections.end() ) { // found
        if ( *iter == cid ) { // go on
            iter++;
        } else {
            return *iter;
        }
    }
    if ( iter != currentConnections.end() ) { // exists
        return *iter;
    } else { // continue with next higher cid
        return *currentConnections.begin();
    }
    //MESSAGE_SINGLE(NORMAL, logger, "getNextConnection("<<cid<<") = ");
}

std::vector<int>
HARQRetransmission::getUsableSubChannelsIDs(wns::scheduler::UserID user, const wns::scheduler::SchedulingMapPtr& schedulingMap)
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
HARQRetransmission::doStartSubScheduling(SchedulerStatePtr schedulerState,
                                         wns::scheduler::SchedulingMapPtr schedulingMap)
{
    ConnectionSet &currentConnections = schedulerState->currentState->activeConnections;

    MapInfoCollectionPtr mapInfoCollection = MapInfoCollectionPtr(new wns::scheduler::MapInfoCollection);

    // harq colleague may be NULL, e.g. Uplink Master Scheduler does not have HARQ
    if(colleagues.harq == NULL)
    {
        MESSAGE_SINGLE(NORMAL, logger, "colleagues.harq==NULL is illegal. Please choose another strategy than HARQRetransmission");
        return mapInfoCollection;
    }

    assure(colleagues.harq!=NULL,"colleagues.harq==NULL is illegal. Please choose another strategy than HARQRetransmission");

    /**
     * @todo dbn/rs: Take reachability into account! Some target peers may be not reachable.
     */
    wns::scheduler::UserSet usersWithRetransmissions = colleagues.harq->getUsersWithRetransmissions();

    for (wns::scheduler::UserSet::iterator it = usersWithRetransmissions.begin();
	 it != usersWithRetransmissions.end();
	 ++it)
      {
    RequestForResource request(0,*it, 0, 0, true);
    dsastrategy::DSAResult resource;

	int processID = colleagues.harq->getProcessesWithRetransmissions(*it).front();

	int numberOfRetransmissions = colleagues.harq->getNumberOfRetransmissions(*it, processID);

	MESSAGE_SINGLE(NORMAL, logger, "doStartSubScheduling: "<<numberOfRetransmissions<<" HARQ retransmission(s) waiting for user=" << it->getName() << " ,processID=" << processID);

	wns::scheduler::SchedulingTimeSlotPtr resourceBlock = colleagues.harq->peekNextRetransmission(*it, processID);
	assure(resourceBlock != NULL, "resourceBlock == NULL although numberOfRetransmissions="<<numberOfRetransmissions);

	std::vector<int> subchannels = getUsableSubChannelsIDs(resourceBlock->getUserID(), schedulingMap);

	if (numberOfRetransmissions > subchannels.size())
	{
	  MESSAGE_BEGIN(NORMAL, logger, m, "Not enough room for ");
	  m << numberOfRetransmissions;
	  m << " retransmissions for user " << it->getName();
	  m << " (pid=" << processID << ") skipping this user";
	  MESSAGE_END();
	  continue;
	}


	while(resourceBlock != NULL)
	  {
	    subchannels = getUsableSubChannelsIDs(resourceBlock->getUserID(), schedulingMap);

	    size_t numAvailable = subchannels.size();

	    if (numAvailable > 0)
	      {
            resource = colleagues.strategy->getDSAStrategy()->getSubChannelWithDSA(request, schedulerState, schedulingMap);
            int sc = resource.subChannel;

		for ( SchedulingTimeSlotPtrVector::iterator iterTimeSlot = schedulingMap->subChannels[sc].temporalResources.begin();
		      iterTimeSlot != schedulingMap->subChannels[sc].temporalResources.end(); ++iterTimeSlot)
		  {
		    SchedulingTimeSlotPtr timeSlotPtr = *iterTimeSlot;
		    int timeSlotIndex = timeSlotPtr->timeSlotIndex;
		    
		    if ( ((!timeSlotPtr->getUserID().isValid()) ||
			  timeSlotPtr->getUserID() == resourceBlock->getUserID()) &&
			 timeSlotPtr->countScheduledCompounds()==0)
		      { // free space found. Pack it into.
			// We found room for the retransmission, now remove it from
			// the pending retransmissions
			resourceBlock = colleagues.harq->getNextRetransmission(*it, processID);
			MESSAGE_BEGIN(NORMAL, logger, m, "Retransmitting");
			m << " HARQ block ("<<resourceBlock->getUserID().getName()<<",processID=" << resourceBlock->harq.processID;
			m << ",Retry="<<resourceBlock->harq.retryCounter<<")";
			m << " inside subchannel.timeslot=" <<sc<<"."<<timeSlotIndex;
			m << " (ex "<<resourceBlock->subChannelIndex<<"."<<resourceBlock->timeSlotIndex<<")";
			MESSAGE_END();
			assure(resourceBlock->subChannelIndex==resourceBlock->physicalResources[0].getSubChannelIndex(),
			       "mismatch of subChannelIndex: "<<resourceBlock->subChannelIndex<<"!="<<resourceBlock->physicalResources[0].getSubChannelIndex());
			assure(resourceBlock->timeSlotIndex==resourceBlock->physicalResources[0].getTimeSlotIndex(),
			       "mismatch of timeSlotIndex: "<<resourceBlock->timeSlotIndex<<"!="<<resourceBlock->physicalResources[0].getTimeSlotIndex());
			assure (resourceBlock != wns::scheduler::SchedulingTimeSlotPtr(),"resourceBlock==NULL");

			if (resourceBlock->harq.ackCallback.empty())
			  {
			    std::cout << "Trying to transmit resource block with empty ack callback in ResourceScheduler::finishCollection" << std::endl;
			    exit(1);
			  }

			if (resourceBlock->harq.nackCallback.empty())
			  {
			    std::cout << "Trying to transmit resource block with empty nack callback in ResourceScheduler::finishCollection" << std::endl;
			    exit(1);
			  }

			schedulingMap->subChannels[sc].temporalResources[timeSlotIndex] = resourceBlock; // copy Smartptr over
			// at this point timeSlotPtr is no longer valid for use! Only resourceBlock
			resourceBlock->subChannelIndex = sc;
			resourceBlock->timeSlotIndex = timeSlotIndex;
			assure(resourceBlock->physicalResources.size()==resourceBlock->numSpatialLayers,
			       "mismatch in spatial domain: "<<resourceBlock->physicalResources.size()<<"!="<<resourceBlock->numSpatialLayers);
			// foreach PRB... fix subChannelIndex
			for ( PhysicalResourceBlockVector::iterator iterPRB = resourceBlock->physicalResources.begin();
			      iterPRB != resourceBlock->physicalResources.end(); ++iterPRB)
			  {
			    int spatialIndex = iterPRB->getSpatialLayerIndex();
			    MESSAGE_SINGLE(NORMAL, logger, sc<<"."<<timeSlotIndex<<".PRB["<<spatialIndex<<"]: Adjusting subChannelIndex from "<<iterPRB->getSubChannelIndex()<<" to "<<sc);
			    iterPRB->setSubChannelIndex(sc);
			    iterPRB->setTimeSlotIndex(timeSlotIndex);

                MapInfoEntryPtr mapInfoEntry; 
                mapInfoEntry = MapInfoEntryPtr(new MapInfoEntry());
                mapInfoEntry->user = *it;
                mapInfoEntry->subBand = sc;
                mapInfoEntry->timeSlot = timeSlotIndex;
                mapInfoEntry->spatialLayer = spatialIndex;
                mapInfoCollection->push_back(mapInfoEntry);
			  }

			resourceBlock = colleagues.harq->peekNextRetransmission(*it, processID);
			break;
		      }
		  }
	      }
	    else
	      {
		// Due to the BCH fix see above we drop the remaining retransmissions here.
		while(resourceBlock != NULL)
		  {
		    resourceBlock = colleagues.harq->getNextRetransmission(*it, processID);
		  }
	      }
	  } // while resource block to send
      } // for

    return mapInfoCollection;
} // doStartSubScheduling

