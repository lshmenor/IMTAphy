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

#include <WNS/scheduler/strategy/dsastrategy/DSAStrategy.hpp>
#include <WNS/scheduler/strategy/Strategy.hpp>
#include <string>

using namespace wns::scheduler;
using namespace wns::scheduler::strategy;
using namespace wns::scheduler::strategy::dsastrategy;

DSAStrategy::DSAStrategy(const wns::pyconfig::View& config)
	: logger(config.get("logger")),
	  oneUserOnOneSubChannel(config.get<bool>("oneUserOnOneSubChannel")),
	  adjacentSubchannelsOnUplink(config.get<bool>("adjacentSubchannelsOnUplink")),
	  highestDataRatePerSubChannel(0.0),
	  phyModeMapper(NULL),
	  myUserID(NULL),
	  dsastrategyName(config.get<std::string>("nameInDSAStrategyFactory"))
{
	MESSAGE_SINGLE(NORMAL, logger,"DSAStrategy="<<dsastrategyName);
}

DSAStrategy::~DSAStrategy()
{
}

void
DSAStrategy::setColleagues(RegistryProxyInterface* _registry)
{
	colleagues.registry = _registry;
	assure(colleagues.registry!=NULL,"need colleagues.registry");
	phyModeMapper = colleagues.registry->getPhyModeMapper();
	assure(phyModeMapper!=NULL,"need phyModeMapper");
	wns::service::phy::phymode::PhyModeInterfacePtr phyModePtr =
		phyModeMapper->getHighestPhyMode();
	assure(phyModePtr!=wns::service::phy::phymode::PhyModeInterfacePtr(),
	       "undefined phymode");
	assure(phyModePtr->isValid(),"need valid phymode");
	assure(phyModePtr->dataRateIsValid(),
	       "need phymode with dataRate feature:"<<*phyModePtr);
	highestDataRatePerSubChannel = phyModePtr->getDataRate();

	// UL-slave: only use those resources (subchannels, slots)
	// where the respective userID is written into
	myUserID = colleagues.registry->getMyUserID();
	assure(myUserID.isValid(),"cannot get userID");
	MESSAGE_SINGLE(NORMAL, logger, "DSAStrategy::setColleagues("
		       <<myUserID.getName()<<"): bestPhyMode="<<*phyModePtr
		       <<", maxrate/sc="<<highestDataRatePerSubChannel/1e6<<"Mbit/s");
}

void
DSAStrategy::initialize(SchedulerStatePtr schedulerState,
			SchedulingMapPtr schedulingMap)
{
	/* This method should be called before each timeSlot/frame */
	assure(colleagues.registry!=NULL,"need colleagues.registry");
	assure(schedulerState!=SchedulerStatePtr(),
	       "schedulerState must be valid");
	MESSAGE_SINGLE(NORMAL, logger, "DSAStrategy::initialize("
		       <<dsastrategyName<<"): maxrate/sc="
		       <<highestDataRatePerSubChannel/1e6<<"Mbit/s");
}

simTimeType
DSAStrategy::getCompoundDuration(RequestForResource& request) const
{
	// phyMode may be undefined (most cases),
	// so it is free to choose (later in AMC,APC)
	wns::service::phy::phymode::PhyModeInterfacePtr phyModePtr = request.phyModePtr;
	simTimeType requestedCompoundDuration = 0;
	assure(highestDataRatePerSubChannel>0.0,"highestDataRatePerSubChannel="
	       <<highestDataRatePerSubChannel);

	if (phyModePtr==wns::service::phy::phymode::PhyModeInterfacePtr())
		{
			// empty=undefined phyMode
			requestedCompoundDuration =
				request.bits / highestDataRatePerSubChannel;
		}
	else
		{
			assure(phyModePtr->isValid(),"need valid phymode");
			assure(phyModePtr->dataRateIsValid(),
			       "need phymode with dataRate feature:"<<*phyModePtr);
			double dataRatePerSubChannel = phyModePtr->getDataRate();
			requestedCompoundDuration =
				request.bits / dataRatePerSubChannel;
		}
	MESSAGE_SINGLE(NORMAL, logger, "getCompoundDuration("
		       <<request.toString()<<"): d="
		       <<requestedCompoundDuration*1e6<<"us");
	return requestedCompoundDuration;
}

int
DSAStrategy::getSpatialLayerForSubChannel(int subChannel,
					  int timeSlot,
					  RequestForResource& request,
					  SchedulerStatePtr schedulerState,
					  SchedulingMapPtr schedulingMap) const
{
	assure(subChannel>=0,"need a valid subChannel");
	assure(subChannel<schedulerState->currentState->strategyInput->getFChannels(),
	       "invalid subChannel="<<subChannel);
	assure(timeSlot>=0,"need a valid timeSlot");
	assure(timeSlot<schedulerState->currentState->strategyInput->getNumberOfTimeSlots(),
	       "invalid timeSlot="<<timeSlot);
	assure(channelIsUsable(subChannel, timeSlot, request, schedulerState, schedulingMap),
	       "invalid timeslot");
	int numSpatialLayers = schedulerState->currentState->strategyInput->getMaxSpatialLayers();
	int getBeam = 0;
	bool ok;

	for ( int spatialLayer = 0; spatialLayer < numSpatialLayers; ++spatialLayer )
		{
			// only for MIMO. For SISO simply spatialLayer=0
			PhysicalResourceBlock& prbDescriptor =
				schedulingMap->subChannels[subChannel].temporalResources[timeSlot]->physicalResources[spatialLayer];
			// can be different if "sort" has been applied
			assure(subChannel==prbDescriptor.getSubChannelIndex(),
			       "subChannel="<<subChannel<<" != subChannelIndex="
			       <<prbDescriptor.getSubChannelIndex());
			assure(timeSlot==prbDescriptor.getTimeSlotIndex(),
			       "timeSlot="<<timeSlot<<" !=timeSlotIndex="
			       <<prbDescriptor.getTimeSlotIndex());
			assure(spatialLayer==prbDescriptor.getSpatialLayerIndex(),
			       "spatialLayer="<<spatialLayer<<" != spatialIndex="
			       <<prbDescriptor.getSpatialLayerIndex());
			// an empty subChannel can always be used:
			if (!prbDescriptor.hasScheduledCompounds())
				{
					//check the grouping constraints: other
					//user in the first spatial layer is
					//the same group and not the same user
					if (schedulerState->currentState->sdmaGroupingIsValid()
					    && (spatialLayer != 0))
						{
							PhysicalResourceBlock& firstPrb = schedulingMap->subChannels[subChannel].temporalResources[timeSlot]->physicalResources[0];
							UserID otherUser = firstPrb.getUserID();
							GroupingPtr grouping = schedulerState->currentState->getGrouping();
							if ((grouping->userGroupNumber[otherUser] != grouping->userGroupNumber[request.user])
							    || (otherUser == request.user))
								{ continue; }
						}
					return spatialLayer;
				}

			// now it is sure that the subChannel is used by
			// at least one packet
			// check if another user is blocking the subChannel
			if (oneUserOnOneSubChannel)
				{
					// checking the first packet is sufficient
					UserID otherUser = prbDescriptor.getUserIDOfScheduledCompounds();
					if (otherUser != request.user)
						continue;
				}

			// check if the PhyMode is already fixed
			wns::service::phy::phymode::PhyModeInterfacePtr phyModePtr =
				prbDescriptor.getPhyMode();
			RequestForResource requestWithGivenPhyMode = request;
			requestWithGivenPhyMode.phyModePtr = phyModePtr;
			simTimeType compoundDuration = getCompoundDuration(requestWithGivenPhyMode);

			// check if there is enough space (time) left
			simTimeType remainingTimeOnthisChannel = prbDescriptor.getFreeTime();
			ok = (remainingTimeOnthisChannel - compoundDuration) >= -wns::scheduler::strategy::slotLengthRoundingTolerance;
			if (ok)
				{
					getBeam = spatialLayer;
					break;
				}
		}
	return getBeam;
}

bool
DSAStrategy::channelIsUsable(int subChannel,
			     int timeSlot,
			     RequestForResource& request,
			     SchedulerStatePtr schedulerState,
			     SchedulingMapPtr schedulingMap) const
{
	assure(subChannel>=0,"need a valid subChannel");
	assure(subChannel<schedulerState->currentState->strategyInput->getFChannels(),
	       "invalid subChannel="<<subChannel);
	assure(timeSlot>=0,"need a valid timeSlot");
	assure(timeSlot<schedulerState->currentState->strategyInput->getNumberOfTimeSlots(),
	       "invalid timeSlot="<<timeSlot);
	if(schedulerState->currentState->groupingIsValid())
		assure(oneUserOnOneSubChannel,
		       "oneUserOnOneSubChannel required for beamforming/SDMA");
	if (!schedulingMap->subChannels[subChannel].subChannelIsUsable)
		return false;

	int numberOfTimeSlots = schedulerState->currentState->strategyInput->getNumberOfTimeSlots();
	int numSpatialLayers = schedulerState->currentState->strategyInput->getMaxSpatialLayers();
	bool ok = false;

	for ( int spatialLayer = 0; spatialLayer < numSpatialLayers; ++spatialLayer )
		{
			// only for MIMO. For SISO simply spatialLayer = 0
			PhysicalResourceBlock& prbDescriptor =
				schedulingMap->subChannels[subChannel].temporalResources[timeSlot]->physicalResources[spatialLayer];
			// can be different if "sort" has been applied:
			assure(subChannel==prbDescriptor.getSubChannelIndex(),
			       "subChannel="<<subChannel<<" != subChannelIndex="<<prbDescriptor.getSubChannelIndex());
			assure(spatialLayer==prbDescriptor.getSpatialLayerIndex(),
			       "spatialLayer="<<spatialLayer<<" != spatialIndex="<<prbDescriptor.getSpatialLayerIndex());
			// an empty subChannel can always be used (with SISO)
			if ((!prbDescriptor.hasScheduledCompounds()))
				{
					if (numSpatialLayers != 1)
						{
							//SDMA: if all other spatial layers
							//and empty channel is usable
							ok = true;
							continue;
						}
					return true;
				}
			// now it is sure that the subChannel is used by
			// at least one packet
			// check if another user is blocking the subChannel
			if (oneUserOnOneSubChannel)
				{       // checking the first packet is enough
					UserID otherUser = prbDescriptor.getUserIDOfScheduledCompounds();
					if ((otherUser != request.user))
						{
							if (schedulerState->currentState->sdmaGroupingIsValid() && numSpatialLayers == 1)
								{
									/*SDMA
									  check the grouping constraints:
									  channelIsUsable = false:
									  if other user not the same group
									  channelIsUsable = true:
									  if empty PRB, own packets found, or
									  packets of a user within same group
									*/

									GroupingPtr grouping = schedulerState->currentState->getGrouping();
									if (grouping->userGroupNumber[otherUser] != grouping->userGroupNumber[request.user])
										{
											MESSAGE_SINGLE(NORMAL, logger, "channelIsUsable("<<subChannel<<"): tSlot="<<timeSlot<<", spatialLayer="<<spatialLayer<<": Different Groups of otherUser="<<otherUser.getName()<<" and request.user="<<request.user.getName());
											return false;
										}
									continue;
								}
							else
								{
									//SISO
									MESSAGE_SINGLE(NORMAL, logger, "channelIsUsable("<<subChannel<<"): tSlot="<<timeSlot<<", spatialLayer="<<spatialLayer<<": otherUser="<<otherUser.getName()<<" != request.user="<<request.user.getName());
									return false;
								}
						}
				}
			// check if the PhyMode is already fixed
			wns::service::phy::phymode::PhyModeInterfacePtr phyModePtr =
				prbDescriptor.getPhyMode();
			RequestForResource requestWithGivenPhyMode = request;
			requestWithGivenPhyMode.phyModePtr = phyModePtr;
			simTimeType compoundDuration = getCompoundDuration(requestWithGivenPhyMode);

			// check if there is enough space (time) left
			simTimeType remainingTimeOnthisChannel =
				prbDescriptor.getFreeTime();
			ok = (remainingTimeOnthisChannel - compoundDuration) >= -wns::scheduler::strategy::slotLengthRoundingTolerance;
			MESSAGE_SINGLE(NORMAL, logger, "channelIsUsable("
				       <<subChannel<<"): d="
				       <<compoundDuration*1e6<<"us <= "
				       <<remainingTimeOnthisChannel*1e6
				       <<"us remaining: ok="<<ok);
			if (ok) break;
		}
	return ok;
}

bool
DSAStrategy::channelIsUsable(int subChannel,
			     int timeSlot,
			     int spatialLayer,
			     RequestForResource& request,
			     SchedulerStatePtr schedulerState,
			     SchedulingMapPtr schedulingMap) const
{
	assure(subChannel>=0,"need a valid subChannel");
	assure(subChannel<schedulerState->currentState->strategyInput->getFChannels(),
	       "invalid subChannel="<<subChannel);
	assure(timeSlot>=0,"need a valid timeSlot");
	assure(timeSlot<schedulerState->currentState->strategyInput->getNumberOfTimeSlots(),
	       "invalid timeSlot="<<timeSlot);
	assure(spatialLayer>=0,"need a valid spatialLayer");
	int maxSpatialLayers = schedulerState->currentState->strategyInput->getMaxSpatialLayers();
	assure(spatialLayer<maxSpatialLayers,"invalid spatialLayer="<<spatialLayer);

	if(schedulerState->currentState->groupingIsValid())
		{
			assure(oneUserOnOneSubChannel,
			       "oneUserOnOneSubChannel required for beamforming/SDMA");
		}
	if (!schedulingMap->subChannels[subChannel].subChannelIsUsable)
		{
			// locked sc?
			return false;
		}

	// TODO: should we introduce bool allBeamsUsedByOneUserOnly
	PhysicalResourceBlock& prbDescriptor =
		schedulingMap->subChannels[subChannel].temporalResources[timeSlot]->physicalResources[spatialLayer];
	// can be different if "sort" has been applied
	assure(subChannel==prbDescriptor.getSubChannelIndex(),
	       "subChannel="<<subChannel<<" != subChannelIndex="<<prbDescriptor.getSubChannelIndex());
	assure(spatialLayer==prbDescriptor.getSpatialLayerIndex(),
	       "spatialLayer="<<spatialLayer<<" != spatialIndex="<<prbDescriptor.getSpatialLayerIndex());

	// check if another user is blocking the subChannel
	if (oneUserOnOneSubChannel)
	{
        //Only check userIDs for UEs that are non broadcasting
        bool bBroadcast = request.user.isBroadcast();
        bool bUserIDIsValid = schedulingMap->subChannels[subChannel].temporalResources[timeSlot]->physicalResources[spatialLayer].getUserID().isValid();
        
        if (!bBroadcast && bUserIDIsValid)
        {
          bool bStationIsUE = (colleagues.registry->getStationType(request.user) == wns::service::dll::StationTypes::UE() );
          if (bStationIsUE)
          {
            if (schedulingMap->subChannels[subChannel].temporalResources[timeSlot]->physicalResources[spatialLayer].getUserID() != request.user)
              return false;
          }
        }
      
		// checking the first packet is sufficient
		// uID of first packet or resource as a whole
		UserID otherUser = prbDescriptor.getUserID();
		const wns::scheduler::SchedulerSpotType& schedulerSpot = schedulerState->schedulerSpot;
		if (schedulerSpot==wns::scheduler::SchedulerSpot::ULSlave())
			{
				// uplink: I am slave. Is this resource for me?
				if (!otherUser.isValid())
					{
						// an empty subChannel can always be used
						MESSAGE_SINGLE(NORMAL, logger, "UL-slave: channelIsUsable("
							       <<subChannel<<"."<<timeSlot<<"."
							       <<spatialLayer<<"): myUser="
							       <<myUserID.getName()
							       <<", other=NULL => unusableInUL");
						return false;
					}
				if (otherUser != myUserID)
					{
						assure(otherUser.isValid(),
						       "!otherUser.isValid()");
						MESSAGE_SINGLE(NORMAL, logger,
							       "UL-slave: channelIsUsable("
							       <<subChannel<<"."<<timeSlot
							       <<"."<<spatialLayer<<"): myUser="
							       <<myUserID.getName()<<", other="
							       <<otherUser.getName()<<", request="
							       <<request.user.getName()<<": unusable");
						return false;
					}

				// at this point I'm sure that this
				// resource is principially usable by me
				MESSAGE_SINGLE(NORMAL, logger, "UL-slave: channelIsUsable("
					       <<subChannel<<"."<<timeSlot<<"."<<spatialLayer
					       <<"): myUser="<<myUserID.getName()
					       <<", other="<<otherUser.getName()
					       <<" is ok");
			}
		else
			{
				// master scheduler (DL|UL)
				// an empty subChannel can always be used
				if (!prbDescriptor.hasScheduledCompounds())
					{
						if (schedulerState->currentState->sdmaGroupingIsValid())
							{
								// check the grouping constraints:
								// user in any other spatial layer
								// is the same group and not the same user
								for (int spatialIndex = 0; spatialIndex < maxSpatialLayers; ++spatialIndex)
									{
										if (spatialIndex == spatialLayer)
											continue;
										PhysicalResourceBlock& neighbourPrb = schedulingMap->subChannels[subChannel].temporalResources[timeSlot]->physicalResources[spatialIndex];
										if(neighbourPrb.hasScheduledCompounds())
											{
												UserID otherUser = neighbourPrb.getUserID();
												GroupingPtr grouping = schedulerState->currentState->getGrouping();
												if ((grouping->userGroupNumber[otherUser] != grouping->userGroupNumber[request.user])
												    || (otherUser == request.user))
													{
														MESSAGE_SINGLE(NORMAL, logger, "channelIsUsable("<<subChannel<<"."<<timeSlot<<"."<<spatialLayer<<"): grouping constraints not met myUser= "<<request.user.getName()<<", other="<<otherUser.getName()<<" => unusable"<<"otherUser"<<otherUser<<"."<<grouping->userGroupNumber[otherUser] );
														return false;
													}
											}
									}
								MESSAGE_SINGLE(NORMAL, logger, "channelIsUsable("<<subChannel<<"."<<timeSlot<<"."<<spatialLayer<<"): grouping constraints met myUser: "<<request.user.getName()<<", other="<<otherUser.getName()<<" => isusable" );
							}
						MESSAGE_SINGLE(NORMAL, logger,
							       "channelIsUsable("<<subChannel
							       <<"."<<timeSlot<<"."<<spatialLayer
							       <<"): empty channel; can always be used (with SDMA: grouping constraints statisfied)");
						return true;
					}
				// We cannot add data to a block that
				// is already protected by HARQ
				if (prbDescriptor.isHARQEnabled())
					{
						return false;
					}

				// now we are sure that the subChannel
				// is used by at least one packet
				if (otherUser != request.user)
					{
						MESSAGE_SINGLE(NORMAL, logger,
							       "channelIsUsable("<<subChannel
							       <<"."<<timeSlot<<"."<<spatialLayer
							       <<"): otherUser="<<otherUser.getName()
							       <<" != request.user="<<request.user.getName());
						return false;
					}
				// at this point I'm sure that this
				// resource is principially usable by me
			}
	}
	else
		{
			// oneUserOnOneSubChannel==false
			// there will be a problem with different
			// txPower and PhyMode on this subChannel
			MESSAGE_SINGLE(NORMAL, logger, "channelIsUsable("
				       <<subChannel<<"."<<timeSlot<<"."
				       <<spatialLayer<<"): WARNING: oneUserOnOneSubChannel="
				       <<oneUserOnOneSubChannel<<" is NEW and untested");

			// an empty subChannel can always be used
			if (!prbDescriptor.hasScheduledCompounds())
				{
					MESSAGE_SINGLE(NORMAL, logger,
						       "channelIsUsable("<<subChannel
						       <<"."<<timeSlot<<"."<<spatialLayer
						       <<"): empty channel; can always be used");
					return true;
				}
		}

	// check if the PhyMode is already fixed
	wns::service::phy::phymode::PhyModeInterfacePtr phyModePtr =
		prbDescriptor.getPhyMode();
	assure(phyModePtr!=wns::service::phy::phymode::PhyModeInterfacePtr(),
	       "phyModePtr==NULL");
	RequestForResource requestWithGivenPhyMode = request;
	requestWithGivenPhyMode.phyModePtr = phyModePtr;
	simTimeType compoundDuration = getCompoundDuration(requestWithGivenPhyMode);

	// check if there is enough space (time) left
	simTimeType remainingTimeOnthisChannel = prbDescriptor.getFreeTime();
	bool ok = (remainingTimeOnthisChannel - compoundDuration) >= -wns::scheduler::strategy::slotLengthRoundingTolerance;
	MESSAGE_SINGLE(NORMAL, logger, "channelIsUsable("<<subChannel<<"."
		       <<timeSlot<<"."<<spatialLayer<<"): d="
		       <<compoundDuration*1e6<<"us <=? "
		       <<remainingTimeOnthisChannel*1e6<<"us remaining: ok="<<ok);
	return ok;
}

/*
  Local Variables:
  mode: c++
  fill-column: 80
  c-basic-offset: 8
  c-comment-only-line-offset: 0
  c-tab-always-indent: t
  indent-tabs-mode: t
  tab-width: 8
  End:
*/
