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

#include <WNS/scheduler/SchedulerTypes.hpp>
#include <WNS/scheduler/strategy/SchedulerState.hpp>
#include <WNS/scheduler/strategy/StrategyInterface.hpp>
#include <WNS/service/dll/StationTypes.hpp>
#include <WNS/service/phy/ofdma/Pattern.hpp>
#include <WNS/service/phy/phymode/PhyModeInterface.hpp>
#include <WNS/ldk/Command.hpp>
#include <WNS/ldk/Compound.hpp>
#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/ldk/Classifier.hpp>
#include <WNS/node/Node.hpp>
#include <WNS/PowerRatio.hpp>
#include <WNS/CandI.hpp>
#include <WNS/Enum.hpp>

#include <map>
#include <vector>
#include <set>
#include <list>
#include <string>
#include <sstream>

using namespace wns::scheduler;
using namespace wns::scheduler::strategy;


RequestForResource::RequestForResource(ConnectionID _cid, UserID _user, Bits _bits, Bits _queuedBits, bool useHARQ)
    : cid(_cid),
      user(_user),
      bits(_bits),
      queuedBits(_queuedBits),
      phyModePtr(), // empty means undefined, still open, freely selectable
      subChannel(wns::scheduler::subChannelNotFound),
      timeSlot(0),
      spatialLayer(0),
      cqiOnSubChannel(),
      useHARQ(useHARQ)
{
}

RequestForResource::~RequestForResource()
{
}

std::string
RequestForResource::toString() const
{
  std::stringstream s;
  s << "Req(cid="<<cid<<"," << user.getName();
  s <<","<<int(bits)<<"bits (queued ";
  s << int(queuedBits) << ")";
  if (phyModePtr!=wns::service::phy::phymode::PhyModeInterfacePtr()) {
    s << "," << *phyModePtr; }
  if (timeSlot>0) {
    s << ",slot=" << timeSlot; }
  if (subChannel!=wns::scheduler::subChannelNotFound) {
    s << ",sc=" << subChannel; }
  s <<")";
  return s.str();
}

simTimeType
RequestForResource::getDuration() const
{
  assure(phyModePtr!=wns::service::phy::phymode::PhyModeInterfacePtr(),"phyMode must be defined for getDuration()");
  double dataRate = phyModePtr->getDataRate();
  return bits / dataRate;
}

//////////////////////////////////////////////////////////////////////

RevolvingState::RevolvingState(const strategy::StrategyInput* _strategyInput)
    : strategyInput(_strategyInput),
      bursts(),
      schedulingMap(),
      activeConnections(),
      grouping(),
      currentPriority(0)
{
    assure(strategyInput!=NULL,"strategyInput==NULL");
    if (strategyInput->inputSchedulingMap != wns::scheduler::SchedulingMapPtr())
    {
        assure(schedulingMap == wns::scheduler::SchedulingMapPtr(),"schedulingMap must be empty before overwriting it");
        // initialize working datastructure with an nonempty schedulingMap
        schedulingMap = strategyInput->inputSchedulingMap;
    } else { // empty inputSchedulingMap
        schedulingMap = strategyInput->getEmptySchedulingMap();
    }
}

RevolvingState::~RevolvingState()
{
    strategyInput=NULL;
}

std::string
RevolvingState::toString() const
{
    return "(RevolvingState)";
}

GroupingPtr
RevolvingState::getNewGrouping()
{
    grouping = GroupingPtr(new Grouping()); // set member
    return grouping;
}

bool
RevolvingState::groupingIsValid() const
{
    return (grouping!=GroupingPtr());
}

bool
RevolvingState::sdmaGroupingIsValid() const
{
    return (groupingIsValid() && (strategyInput->getMaxSpatialLayers() > 1));
}
GroupingPtr
RevolvingState::getGrouping() const
{
    //assure(grouping!=GroupingPtr(),"invalid grouping");
    return grouping;
}

void
RevolvingState::setGrouping(GroupingPtr groupingPtr)
{
    assure(groupingPtr!=GroupingPtr(),"invalid grouping");
    grouping=groupingPtr; // set member
}

void
RevolvingState::setCurrentPriority(int priority)
{
    currentPriority=priority;
}

int
RevolvingState::getCurrentPriority() const
{
    return currentPriority;
}

void
RevolvingState::clearMap()
{
    bursts = MapInfoCollectionPtr();
}
