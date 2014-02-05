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

#include <WNS/scheduler/strategy/apcstrategy/UseMaxTxPower.hpp>
#include <WNS/scheduler/strategy/apcstrategy/APCStrategy.hpp>
#include <WNS/scheduler/strategy/StrategyInterface.hpp>
#include <WNS/scheduler/SchedulerTypes.hpp>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace wns::scheduler;
using namespace wns::scheduler::strategy;
using namespace wns::scheduler::strategy::apcstrategy;

STATIC_FACTORY_REGISTER_WITH_CREATOR(UseMaxTxPower,
				     APCStrategyInterface,
				     "UseMaxTxPower",
				     wns::PyConfigViewCreator);

UseMaxTxPower::UseMaxTxPower(const wns::pyconfig::View& config)
	: APCStrategy(config)
{
}

UseMaxTxPower::~UseMaxTxPower()
{
}

// called before each timeSlot/frame
void
UseMaxTxPower::initialize(SchedulerStatePtr schedulerState,
			      SchedulingMapPtr schedulingMap)
{
	APCStrategy::initialize(schedulerState,schedulingMap); // must always initialize base class too
	MESSAGE_SINGLE(NORMAL, logger, "UseMaxTxPower::initialize("<<apcstrategyName<<")");
} // initialize

APCResult
UseMaxTxPower::doStartAPC(RequestForResource& request,
			      SchedulerStatePtr schedulerState,
			      SchedulingMapPtr schedulingMap)
{
	// no power control, just nominal values
	APCResult apcResult;
	assure(request.subChannel>=0,"need a valid subChannel");
        apcResult.txPower = schedulerState->powerCapabilities.maxPerSubband;
	if (apcResult.txPower==wns::Power()) // unknown power?
	{ // get the value
		wns::scheduler::PowerCapabilities powerCapabilities =
			schedulerState->strategy->getPowerCapabilities(request.user);
		apcResult.txPower = powerCapabilities.maxPerSubband;
	}
	wns::Ratio pathloss     = request.cqiOnSubChannel.pathloss;
	wns::Power interference = request.cqiOnSubChannel.interference;
	apcResult.sinr = apcResult.txPower/(interference*pathloss);
	apcResult.estimatedCandI = 
        ChannelQualityOnOneSubChannel(pathloss, interference, apcResult.txPower/pathloss);
	MESSAGE_SINGLE(NORMAL, logger,"doStartAPC("<<request.toString()<<"): "
		       <<"TxP="<<apcResult.txPower<<", pl="<<pathloss<<", estd: I="<<interference<<", C="<<apcResult.estimatedCandI.carrier);
	if (schedulerState->defaultPhyModePtr != wns::service::phy::phymode::PhyModeInterfacePtr())
	{ // predefined, e.g. in slave mode
		apcResult.phyModePtr = schedulerState->defaultPhyModePtr;
	}
	else
	{
		apcResult.phyModePtr = phyModeMapper->getBestPhyMode(apcResult.sinr);
	}
	MESSAGE_SINGLE(NORMAL, logger,"doStartAPC("<<request.toString()<<"): "
		       <<"SINR="<<apcResult.sinr<<", PhyMode="<<*(apcResult.phyModePtr));
	request.phyModePtr = apcResult.phyModePtr; // maybe needed later
	return apcResult;
}

void
UseMaxTxPower::postProcess(SchedulerStatePtr schedulerState,
                           SchedulingMapPtr schedulingMap)
{
    wns::Power totalPowerLimit = schedulerState->powerCapabilities.maxOverall;
    // loop over all timeSlots. Treat each individually.
    for (int timeSlotIndex=0; timeSlotIndex<schedulingMap->getNumberOfTimeSlots(); ++timeSlotIndex)
    {
        wns::Power totalUsedTxPowerOnAllSubChannels = schedulingMap->getUsedPower(timeSlotIndex);
        if (totalUsedTxPowerOnAllSubChannels == wns::Power()) { continue; } // means empty schedulingMap
        wns::Ratio reductionRatio = totalUsedTxPowerOnAllSubChannels/totalPowerLimit;
        // make this message VERBOSE only in the future:
        MESSAGE_SINGLE(NORMAL, logger,"postProcess(): usedPower[timeSlot="<<timeSlotIndex<<"]="<<totalUsedTxPowerOnAllSubChannels
                       <<", reductionRatio="<<reductionRatio);
        if (reductionRatio.get_factor() <= 1.0) { continue; } // no need to adjust
        // keep this message in the future:
        MESSAGE_SINGLE(NORMAL, logger,"postProcess(): POWER REDUCTION: usedPower[timeSlot="<<timeSlotIndex<<"]="<<totalUsedTxPowerOnAllSubChannels
                       <<", reductionRatio="<<reductionRatio);
        /* Loop over all resources in frequency dimension and adjust power down if it is too much.
           Assuming that this power distribution can vary timeslot-by-timeslot */
        for ( SubChannelVector::iterator iterSubChannel = schedulingMap->subChannels.begin();
              iterSubChannel != schedulingMap->subChannels.end(); ++iterSubChannel)
        {
            SchedulingSubChannel& subChannel = *iterSubChannel;
            SchedulingTimeSlotPtr timeSlotPtr = subChannel.temporalResources[timeSlotIndex];
            wns::Power powerOnSubchannel = timeSlotPtr->getTxPower();
            wns::Power reducedPowerOnSubchannel = powerOnSubchannel * reductionRatio;
            MESSAGE_SINGLE(NORMAL, logger,"postProcess(): subChannel="<<subChannel.subChannelIndex<<"."<<timeSlotIndex
                           <<": reducing power from "<<powerOnSubchannel<<" by "<<reductionRatio<<" to "<<reducedPowerOnSubchannel);
            timeSlotPtr->setTxPower(reducedPowerOnSubchannel);
            /** @todo: check whether PhyModes still have enough SINR to perform well. Write warnings or even assure in debug mode, if so. */
            //for ( int spatialIndex = 0; spatialIndex < numSpatialLayers; ++spatialIndex )
            //{
            //  wns::Ratio estimatedSINR = ...
            //  wns::service::phy::phymode::PhyModeInterfacePtr phyModePtr = getPhyModeUsedInResource(subChannelIndex, timeSlotIndex, spatialIndex);
            //  wns::Ratio requiredSINRforPhyMode = phyModeMapper->getMinSINRRatio(phyModePtr);
            //  assure(estimatedSINR < requiredSINRforPhyMode,"pm="<<*phyModePtr<<": estimatedSINR="<<estimatedSINR<<" < required="<<requiredSINRforPhyMode);
            //{
        }
        /** @todo: adjusting Phymodes is not possible in most cases, since all bits are used */
    } // forall timeslots
} // postProcess
