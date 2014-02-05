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

#include <WNS/scheduler/strategy/staticpriority/HARQUplinkSlaveRetransmission.hpp>

using namespace wns::scheduler::strategy::staticpriority;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    HARQUplinkSlaveRetransmission,
    SubStrategyInterface,
    "HARQUplinkSlaveRetransmission",
    wns::PyConfigViewCreator);

HARQUplinkSlaveRetransmission::HARQUplinkSlaveRetransmission(const wns::pyconfig::View& config):
    logger_(config.get("logger"))
{
    MESSAGE_SINGLE(NORMAL, logger, "HARQUplinkSlaveRetransmission(): constructed");
}

HARQUplinkSlaveRetransmission::~HARQUplinkSlaveRetransmission()
{
}

void
HARQUplinkSlaveRetransmission::initialize()
{
}

void
HARQUplinkSlaveRetransmission::checkInputMap(SchedulerStatePtr schedulerState,
					     wns::scheduler::SchedulingMapPtr schedulingMap) const
{
  int scheduledRetransmissions = 0;

  int processID = 8855994;

  int numSC = schedulerState->currentState->strategyInput->getFChannels();

  int numberOfSpatialLayers = schedulerState->currentState->strategyInput->getMaxSpatialLayers();

  wns::scheduler::UserID peerUser;

  for (int sc=0; sc < numSC; sc++)
    {
      if (!schedulingMap->subChannels[sc].subChannelIsUsable) continue;
            
      for ( SchedulingTimeSlotPtrVector::iterator iterTimeSlot =      schedulingMap->subChannels[sc].temporalResources.begin();
	    iterTimeSlot != schedulingMap->subChannels[sc].temporalResources.end(); ++iterTimeSlot)
	{
	  SchedulingTimeSlotPtr timeSlotPtr = *iterTimeSlot;
	  int timeSlotIndex = timeSlotPtr->timeSlotIndex;
                
	  if (timeSlotPtr->harq.reservedForRetransmission &&
	      timeSlotPtr->getUserID().isValid() &&
	      timeSlotPtr->getUserID() == colleagues.registry->getMyUserID() &&
	      timeSlotPtr->countScheduledCompounds()==0)
	    { 
	      scheduledRetransmissions++;
	      if (processID == 8855994)
		{
		  processID = timeSlotPtr->harq.processID;
		}
	      else
		{
		  assure(timeSlotPtr->harq.processID == processID, "More than one process scheduled " << timeSlotPtr->harq.processID << "/" << processID);
		}

	      if (peerUser.isValid())
		{
		  assure(peerUser == timeSlotPtr->getSourceUserID(), "Mismatch in UserID " << timeSlotPtr->getSourceUserID().getName() << "/" << peerUser.getName());
		}
	      else
		{
		  peerUser = timeSlotPtr->getSourceUserID();
		  assure(peerUser.isValid(), "Invalid UserID");
		}
	    }

            }
        }

  if (scheduledRetransmissions > 0)
  {
    assure(peerUser.isValid(), "Invalid UserID");

    int numberOfRetransmissions = colleagues.harq->getNumberOfRetransmissions(peerUser, processID);

    MESSAGE_BEGIN(NORMAL, logger, m, "Scheduled Retransmissions " << scheduledRetransmissions);
    m << " Available Retransmissions " << numberOfRetransmissions;
    MESSAGE_END();
    assure(scheduledRetransmissions <= numberOfRetransmissions, "More retransmissions scheduled than available");
  }
}

wns::scheduler::UserID
HARQUplinkSlaveRetransmission::getSourceUserID(SchedulerStatePtr schedulerState,
                                               wns::scheduler::SchedulingMapPtr schedulingMap)
{
  int numSC = schedulerState->currentState->strategyInput->getFChannels();

  int numberOfSpatialLayers = schedulerState->currentState->strategyInput->getMaxSpatialLayers();

  for (int sc=0; sc < numSC; sc++)
    {
      if (!schedulingMap->subChannels[sc].subChannelIsUsable) continue;
            
      for ( SchedulingTimeSlotPtrVector::iterator iterTimeSlot =      schedulingMap->subChannels[sc].temporalResources.begin();
	    iterTimeSlot != schedulingMap->subChannels[sc].temporalResources.end(); ++iterTimeSlot)
	{
	  SchedulingTimeSlotPtr timeSlotPtr = *iterTimeSlot;
	  int timeSlotIndex = timeSlotPtr->timeSlotIndex;
                
	  if (timeSlotPtr->harq.reservedForRetransmission &&
	      timeSlotPtr->getUserID().isValid() &&
	      timeSlotPtr->getUserID() == colleagues.registry->getMyUserID() &&
	      timeSlotPtr->countScheduledCompounds()==0)
	    {
	      return timeSlotPtr->getSourceUserID();
	    }
	}
    }
  // If consistency check is good, this should never happen
  return wns::scheduler::UserID();
}

int
HARQUplinkSlaveRetransmission::getProcessID(SchedulerStatePtr schedulerState,
					    wns::scheduler::SchedulingMapPtr schedulingMap)
{
  int numSC = schedulerState->currentState->strategyInput->getFChannels();

  int numberOfSpatialLayers = schedulerState->currentState->strategyInput->getMaxSpatialLayers();

  for (int sc=0; sc < numSC; sc++)
    {
      if (!schedulingMap->subChannels[sc].subChannelIsUsable) continue;
            
      for ( SchedulingTimeSlotPtrVector::iterator iterTimeSlot =      schedulingMap->subChannels[sc].temporalResources.begin();
	    iterTimeSlot != schedulingMap->subChannels[sc].temporalResources.end(); ++iterTimeSlot)
	{
	  SchedulingTimeSlotPtr timeSlotPtr = *iterTimeSlot;
	  int timeSlotIndex = timeSlotPtr->timeSlotIndex;
                
	  if (timeSlotPtr->harq.reservedForRetransmission &&
	      timeSlotPtr->getUserID().isValid() &&
	      timeSlotPtr->getUserID() == colleagues.registry->getMyUserID() &&
	      timeSlotPtr->countScheduledCompounds()==0)
	    {
	      return timeSlotPtr->harq.processID;
	    }
	}
    }
  // If consistency check is good, this should never happen
  return -1;
}

wns::scheduler::MapInfoCollectionPtr
HARQUplinkSlaveRetransmission::doStartSubScheduling(SchedulerStatePtr schedulerState,
                                               wns::scheduler::SchedulingMapPtr schedulingMap)
{
#ifndef NDEBUG
  checkInputMap(schedulerState, schedulingMap);
#endif

    MapInfoCollectionPtr mapInfoCollection = MapInfoCollectionPtr(new wns::scheduler::MapInfoCollection);

    wns::scheduler::UserID sourceUserID = getSourceUserID(schedulerState, schedulingMap);

    if (!sourceUserID.isValid())
      {
	return mapInfoCollection;
      }

    int processID = getProcessID(schedulerState, schedulingMap);

    assure(processID >= 0, "Invalid processID in HARQUplinkSlaveRetransmission");

    int numberOfRetransmissions = colleagues.harq->getNumberOfRetransmissions(sourceUserID, processID);

    MESSAGE_SINGLE(NORMAL, logger, "doStartSubScheduling: "<<numberOfRetransmissions<<" HARQ retransmission(s) waiting");

    if (numberOfRetransmissions>0)
    {
        int numSC = schedulerState->currentState->strategyInput->getFChannels();

        int numberOfSpatialLayers = schedulerState->currentState->strategyInput->getMaxSpatialLayers();

        wns::scheduler::SchedulingTimeSlotPtr resourceBlock = colleagues.harq->peekNextRetransmission(sourceUserID, processID);

        assure(resourceBlock != NULL, "resourceBlock == NULL although numberOfRetransmissions="<<numberOfRetransmissions);

        bool found = false;
        bool giveUp = false;

        while(!giveUp && resourceBlock != NULL)
        {
            found = false;
            for (int sc=0; !found && sc < numSC; sc++)
            {
                if (!schedulingMap->subChannels[sc].subChannelIsUsable) continue;

                for ( SchedulingTimeSlotPtrVector::iterator iterTimeSlot =      schedulingMap->subChannels[sc].temporalResources.begin();
                      iterTimeSlot != schedulingMap->subChannels[sc].temporalResources.end(); ++iterTimeSlot)
                {
                    SchedulingTimeSlotPtr timeSlotPtr = *iterTimeSlot;
                    int timeSlotIndex = timeSlotPtr->timeSlotIndex;

                    if (timeSlotPtr->harq.reservedForRetransmission &&
                        timeSlotPtr->getUserID().isValid() &&
                        timeSlotPtr->getUserID() == resourceBlock->getUserID() &&
                        timeSlotPtr->countScheduledCompounds()==0)
                    { // free space found. Pack it into.
                        resourceBlock = colleagues.harq->getNextRetransmission(sourceUserID, processID);
                        MESSAGE_BEGIN(NORMAL, logger, m, "Retransmitting");
                        m << " HARQ block ("<<resourceBlock->getUserID().getName()<<",processID=" << resourceBlock->harq.processID;
                        m << ",Retry="<<resourceBlock->harq.retryCounter<<", TxPwr=" << resourceBlock->getTxPower() << ")";
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
			    std::cout << "Trying to retransmit resource block with empty ack callback in HARQUplinkSlaveRetransmission" << std::endl;
			    exit(1);
			  }

			if (resourceBlock->harq.nackCallback.empty())
			  {
			    std::cout << "Trying to retransmit resource block with empty nack callback in HARQUplinkSlaveRetransmission" << std::endl;
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
                        }
                        MESSAGE_SINGLE(NORMAL, logger, schedulingMap->subChannels[sc].temporalResources[timeSlotIndex]->toString());
                        resourceBlock = colleagues.harq->peekNextRetransmission(sourceUserID, processID);
                        found = true;
                        break;
                    }
                } // timeslots
            } // subcarriers

            if (!found)
                giveUp = true;

        } // while resource blocks
    } // if there are retransmissions
    /*
    #ifndef NDEBUG
    else
    {
        int numSC = schedulerState->currentState->strategyInput->getFChannels();

        int numberOfBeams = schedulerState->currentState->strategyInput->getMaxBeams();
    */
    //   int numberOfRetransmissions = colleagues.harq->getNumberOfRetransmissions(/* @todo peer */);

    /*    int scheduledRetransmissions = 0;
        for (int sc=0; sc < numSC; sc++)
        {
            if (!schedulingMap->subChannels[sc].subChannelIsUsable) continue;
            
            for ( SchedulingTimeSlotPtrVector::iterator iterTimeSlot =      schedulingMap->subChannels[sc].temporalResources.begin();
                  iterTimeSlot != schedulingMap->subChannels[sc].temporalResources.end(); ++iterTimeSlot)
            {
                SchedulingTimeSlotPtr timeSlotPtr = *iterTimeSlot;
                int timeSlotIndex = timeSlotPtr->timeSlotIndex;
                
                if (timeSlotPtr->harq.reservedForRetransmission &&
                    timeSlotPtr->getUserID().isValid() &&
                    timeSlotPtr->getUserID() == colleagues.registry->getMyUserID() &&
                    timeSlotPtr->countScheduledCompounds()==0)
                { // free space found. Pack it into.
                    scheduledRetransmissions++;
                }

            }
        }
        MESSAGE_BEGIN(NORMAL, logger, m, "Scheduled Retransmissions " << scheduledRetransmissions);
        m << " Available Retransmissions " << numberOfRetransmissions;
        MESSAGE_END();
        assure(scheduledRetransmissions <= numberOfRetransmissions, "More retransmissions scheduled than available");
    }
#endif
    */
    return mapInfoCollection;
}
