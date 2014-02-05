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

#include <WNS/scheduler/strategy/apcstrategy/LTE_UL.hpp>
#include <WNS/scheduler/SchedulerTypes.hpp>
#include <WNS/scheduler/strategy/StrategyInterface.hpp>
#include <WNS/simulator/ISimulator.hpp>


using namespace wns::scheduler;
using namespace wns::scheduler::strategy;
using namespace wns::scheduler::strategy::apcstrategy;

STATIC_FACTORY_REGISTER_WITH_CREATOR(LTE_UL,
                    APCStrategyInterface,
                    "LTE_UL",
                    wns::PyConfigViewCreator);

LTE_UL::LTE_UL(const wns::pyconfig::View& config)
    : APCStrategy(config),
    pNull_(config.get<wns::Power>("pNull")),
    alpha_(config.get<double>("alpha")),
    sinrMargin_(config.get<wns::Ratio>("sinrMargin")),
    minimumPhyMode_(config.get<int>("minimumPhyMode"))
{
    assure(minimumPhyMode_ >= 0, "PhyMode must be >= 0");
}

LTE_UL::~LTE_UL()
{
}

void
LTE_UL::initialize(SchedulerStatePtr schedulerState,
        SchedulingMapPtr schedulingMap)
{
    APCStrategy::initialize(schedulerState,schedulingMap); // must always initialize base class too
    MESSAGE_SINGLE(NORMAL, logger, "APCStrategy::initialize(" << apcstrategyName << ")");

    assure(minimumPhyMode_ >= 0, "PhyMode must be >= 0");
    assure(minimumPhyMode_ < (phyModeMapper->getPhyModeCount()-1), "To high PhyMode, maximum is " << (phyModeMapper->getPhyModeCount()-1));
}

APCResult
LTE_UL::doStartAPC(RequestForResource& request,
                     SchedulerStatePtr schedulerState,
                     SchedulingMapPtr schedulingMap)
{
    // no power control, just nominal values
    APCResult apcResult;
    wns::Ratio pathloss     = request.cqiOnSubChannel.pathloss;
    wns::Power interference = request.cqiOnSubChannel.interference;

    if (schedulerState->defaultTxPower != wns::Power())
    { // predefined, e.g. in slave mode
        apcResult.txPower = schedulerState->defaultTxPower;
    } 
    else 
    {
        wns::Ratio scalePL;
        scalePL.set_dB(pathloss.get_dB() * alpha_);
        apcResult.txPower = pNull_ * scalePL;
    }

    MESSAGE_SINGLE(NORMAL, logger,"doStartAPC(" << request.toString() << "): "
        << "estd. PL = " << pathloss << ", estd I. = " << interference);
    
    if (schedulerState->defaultPhyModePtr != wns::service::phy::phymode::PhyModeInterfacePtr())
    { // predefined, e.g. in slave mode
        apcResult.phyModePtr = schedulerState->defaultPhyModePtr;
    }
    else
    {
        if (schedulingMap->getPhyModeUsedInResource(request.subChannel,request.timeSlot,request.spatialLayer) != 
          wns::service::phy::phymode::PhyModeInterfacePtr())
        {       
          apcResult.phyModePtr = schedulingMap->getPhyModeUsedInResource(request.subChannel,request.timeSlot,request.spatialLayer);          
        }
        else
        {
          wns::Ratio sinr = apcResult.txPower/(interference * pathloss);

          apcResult.phyModePtr = phyModeMapper->getBestPhyMode(sinr - sinrMargin_);
          apcResult.sinr = sinr;

          // Now we introduce some limiting
          if (phyModeMapper->getIndexForPhyMode(*apcResult.phyModePtr) < minimumPhyMode_)
          {
              apcResult.phyModePtr = phyModeMapper->getPhyModeForIndex(minimumPhyMode_);
              MESSAGE_SINGLE(NORMAL, logger, "doStartAPC"
                            << "Below minimum phy mode, raising to " << *(apcResult.phyModePtr));
          }
        }
    }
    apcResult.estimatedCandI = 
        ChannelQualityOnOneSubChannel(pathloss, interference, apcResult.txPower/pathloss);

    MESSAGE_SINGLE(NORMAL, logger,"doStartAPC(" << request.toString() << "): "
               << "SINR=" << apcResult.sinr << ", PhyMode=" << *(apcResult.phyModePtr));

    request.phyModePtr = apcResult.phyModePtr; // maybe needed later

    return apcResult;
}

void 
LTE_UL::postProcess(SchedulerStatePtr schedulerState,
                     SchedulingMapPtr schedulingMap)
{
}
