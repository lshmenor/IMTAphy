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

#include <WNS/scheduler/strategy/apcstrategy/FairSINR.hpp>
#include <WNS/scheduler/strategy/apcstrategy/APCStrategy.hpp>
#include <WNS/scheduler/strategy/StrategyInterface.hpp>
#include <WNS/scheduler/SchedulerTypes.hpp>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace wns::scheduler;
using namespace wns::scheduler::strategy;
using namespace wns::scheduler::strategy::apcstrategy;

STATIC_FACTORY_REGISTER_WITH_CREATOR(FairSINR,
                                     APCStrategyInterface,
                                     "FairSINR",
                                     wns::PyConfigViewCreator);

FairSINR::FairSINR(const wns::pyconfig::View& config)
    : APCStrategy(config),
      fair_sinr(wns::Ratio()),
      fair_sinrdl(config.get<double>("fair_sinrdl")),
      fair_sinrul(config.get<double>("fair_sinrul"))
{
}

FairSINR::~FairSINR()
{
}

// called before each timeSlot/frame
void
FairSINR::initialize(SchedulerStatePtr schedulerState,
                     SchedulingMapPtr schedulingMap)
{
    APCStrategy::initialize(schedulerState,schedulingMap); // must always initialize base class too
    MESSAGE_SINGLE(NORMAL, logger, "FairSINR::initialize("<<apcstrategyName<<")");
} // initialize


APCResult
FairSINR::doStartAPC(RequestForResource& request,
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
        if (schedulerState->schedulerSpot = wns::scheduler::SchedulerSpot::DLMaster())
        {
            fair_sinr.set_dB(fair_sinrdl);
        }
        else
        {
            fair_sinr.set_dB(fair_sinrul);
        }
        wns::Power fairTxPower = wns::Power::from_mW(fair_sinr.get_factor() * pathloss.get_factor() * interference.get_mW());
        wns::Power totalPower = powerCapabilities.maxOverall;
        wns::Power remainingTxPowerOnAllSubChannels = schedulingMap->getRemainingPower(totalPower,request.timeSlot);

        if ( remainingTxPowerOnAllSubChannels == wns::Power())
        {
            apcResult.txPower = wns::Power();
            return apcResult;
        }

        if (fairTxPower > powerCapabilities.maxPerSubband)
        {
            fairTxPower = powerCapabilities.maxPerSubband;

            if (fairTxPower > remainingTxPowerOnAllSubChannels)
            {
                apcResult.txPower = remainingTxPowerOnAllSubChannels;
            }
            else
            {
                apcResult.txPower = fairTxPower;
            }
            apcResult.sinr =  apcResult.txPower/(interference * pathloss);
        }
        else
        {
            if (fairTxPower > remainingTxPowerOnAllSubChannels)
            {
                apcResult.txPower = remainingTxPowerOnAllSubChannels;
                apcResult.sinr =  apcResult.txPower/(interference * pathloss);
            }
            else
            {
                apcResult.txPower = fairTxPower;
                apcResult.sinr = fair_sinr;
            }
        }
		apcResult.estimatedCandI = 
            ChannelQualityOnOneSubChannel(pathloss, interference, apcResult.txPower/pathloss);
        apcResult.phyModePtr = phyModeMapper->getBestPhyMode(apcResult.sinr);
    }
    MESSAGE_SINGLE(NORMAL, logger,"doStartAPC("<<request.toString()<<"): "
                   <<"SINR="<<apcResult.sinr<<", PhyMode="<<*(apcResult.phyModePtr)<<", txPower="<<apcResult.txPower);
    return apcResult;
}

