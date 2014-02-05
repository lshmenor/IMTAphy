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

#include <WNS/scheduler/strategy/apcstrategy/FCFSMaxPhyMode.hpp>
#include <WNS/scheduler/strategy/apcstrategy/APCStrategy.hpp>
#include <WNS/scheduler/strategy/StrategyInterface.hpp>
#include <WNS/scheduler/SchedulerTypes.hpp>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace wns::scheduler;
using namespace wns::scheduler::strategy;
using namespace wns::scheduler::strategy::apcstrategy;

STATIC_FACTORY_REGISTER_WITH_CREATOR(FCFSMaxPhyMode,
                                     APCStrategyInterface,
                                     "FCFSMaxPhyMode",
                                     wns::PyConfigViewCreator);

FCFSMaxPhyMode::FCFSMaxPhyMode(const wns::pyconfig::View& config)
    : APCStrategy(config)
{
}

FCFSMaxPhyMode::~FCFSMaxPhyMode()
{
}

// called before each timeSlot/frame
void
FCFSMaxPhyMode::initialize(SchedulerStatePtr schedulerState,
                           SchedulingMapPtr schedulingMap)
{
    APCStrategy::initialize(schedulerState,schedulingMap); // must always initialize base class too
    MESSAGE_SINGLE(NORMAL, logger, "UseNominalTxPower::initialize("<<apcstrategyName<<")");
} // initialize

APCResult
FCFSMaxPhyMode::doStartAPC(RequestForResource& request,
                           SchedulerStatePtr schedulerState,
                           SchedulingMapPtr schedulingMap)
{
    APCResult apcResult;
    wns::scheduler::PowerCapabilities powerCapabilities =
        schedulerState->strategy->getPowerCapabilities(request.user);

    wns::Ratio pathloss     = request.cqiOnSubChannel.pathloss;
    wns::Power interference = request.cqiOnSubChannel.interference;

    assure(request.subChannel>=0,"need a valid subChannel");
    assure(request.timeSlot  >=0,"need a valid timeSlot");

    if (schedulerState->defaultTxPower!=wns::Power())
    { // predefined, e.g. in slave mode
        apcResult.txPower = schedulerState->defaultTxPower;
        apcResult.sinr = apcResult.txPower/(interference*pathloss);
        apcResult.estimatedCandI = 
            ChannelQualityOnOneSubChannel(pathloss, interference, apcResult.txPower/pathloss);
        apcResult.phyModePtr = schedulerState->defaultPhyModePtr;
    } else {
        wns::Power totalPower = powerCapabilities.maxOverall;
        wns::Power remainingTxPowerOnAllSubChannels = schedulingMap->getRemainingPower(totalPower,request.timeSlot);

        if (remainingTxPowerOnAllSubChannels == wns::Power())
        { // no more power left
            apcResult.txPower = wns::Power();
            return apcResult;
        }

        wns::Power maxPowerPerSubChannel = powerCapabilities.maxPerSubband;
        wns::Power maxTxPower =  (maxPowerPerSubChannel > remainingTxPowerOnAllSubChannels ? remainingTxPowerOnAllSubChannels : maxPowerPerSubChannel);
        wns::Ratio maxSINR = maxTxPower/(interference * pathloss);
        apcResult.phyModePtr = phyModeMapper->getBestPhyMode(maxSINR);

        // we always try to use the minimal txPower for certain phymode to save power
        wns::Ratio minSINR = phyModeMapper->getMinSINRRatio(apcResult.phyModePtr);
        apcResult.txPower = wns::Power::from_mW(minSINR.get_factor() * pathloss.get_factor() * interference.get_mW() ) ;
        apcResult.sinr = minSINR;
		apcResult.estimatedCandI = 
            ChannelQualityOnOneSubChannel(pathloss, interference, apcResult.txPower/pathloss);
    }
    MESSAGE_SINGLE(NORMAL, logger,"doStartAPC("<<request.toString()<<"): "
                   <<"SINR="<<apcResult.sinr<<", PhyMode="<<*(apcResult.phyModePtr)<<", txPower="<<apcResult.txPower);
    return apcResult;
}

