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

#include <WNS/scheduler/strategy/staticpriority/RoundRobin.hpp>
#include <WNS/scheduler/SchedulerTypes.hpp>

#include <vector>
#include <map>
#include <algorithm>
#include <iostream>

using namespace std;
using namespace wns::scheduler;
using namespace wns::scheduler::strategy;
using namespace wns::scheduler::strategy::staticpriority;

STATIC_FACTORY_REGISTER_WITH_CREATOR(RoundRobin,
                                     SubStrategyInterface,
                                     "RoundRobin",
                                     wns::PyConfigViewCreator);


RoundRobin::RoundRobin(const wns::pyconfig::View& config)
    : SubStrategy(config),
      blockSize(config.get<int>("blockSize")),
      lastScheduledConnection(0)
{
    assure(blockSize>0,"invalid blockSize="<<blockSize);
    MESSAGE_SINGLE(NORMAL, logger, "RoundRobin(): constructed with blockSize="<<blockSize);
}

RoundRobin::~RoundRobin()
{
}

void
RoundRobin::initialize()
{
    // make state
    MESSAGE_SINGLE(NORMAL, logger, "RoundRobin(): initialized");
}

wns::scheduler::ConnectionID
RoundRobin::getValidCurrentConnection(const ConnectionSet &currentConnections, ConnectionID cid) const
{
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
RoundRobin::getNextConnection(const ConnectionSet &currentConnections, ConnectionID cid) const
{
    // uses state var currentConnections
    wns::scheduler::ConnectionSet::iterator iter =
        currentConnections.upper_bound(cid);
    if ( iter != currentConnections.end() ) { // exists
        return *iter;
    } else { // continue with next higher cid
        return *currentConnections.begin();
    }
    //MESSAGE_SINGLE(NORMAL, logger, "getNextConnection("<<cid<<") = ");
}

// return std::list<MapInfoEntryPtr>
wns::scheduler::MapInfoCollectionPtr
RoundRobin::doStartSubScheduling(SchedulerStatePtr schedulerState,
                                 wns::scheduler::SchedulingMapPtr schedulingMap)
{
    MapInfoCollectionPtr mapInfoCollection = MapInfoCollectionPtr(new wns::scheduler::MapInfoCollection); // result datastructure
    ConnectionSet &currentConnections = schedulerState->currentState->activeConnections;
    if ( currentConnections.empty() ) return mapInfoCollection; // nothing to do
    wns::scheduler::ConnectionID currentConnection = getValidCurrentConnection(currentConnections,lastScheduledConnection);
    MESSAGE_SINGLE(NORMAL, logger, "RoundRobin::doStartSubScheduling("<<printConnectionSet(currentConnections)<<") start with cid="<<currentConnection);

    bool spaceLeft=true;
    while(spaceLeft)
    {
        int pduCounter = 0;
        // schedule #=blockSize PDUs for this CID here...
        while( colleagues.queue->queueHasPDUs(currentConnection)
               && (pduCounter<blockSize)
               && spaceLeft)
        { // spaceLeft[currentConnection] to be more precise
            spaceLeft = scheduleCid(schedulerState,schedulingMap,currentConnection,pduCounter,blockSize,mapInfoCollection);
        } // while PDUs in queue
        if (!colleagues.queue->queueHasPDUs(currentConnection))
        { // exit because of queue empty (most probable case for low traffic)
            currentConnections.erase(currentConnection);
            if (currentConnections.size()==0) break; // all queues empty
        }

        if (spaceLeft)
        {
            currentConnection = getNextConnection(currentConnections,currentConnection);
            MESSAGE_SINGLE(NORMAL, logger, "doStartSubScheduling(): next connection="<<currentConnection);
        }
    } // while(spaceLeft)

    lastScheduledConnection = currentConnection; // this one really had pdus scheduled

    MESSAGE_SINGLE(NORMAL, logger, "doStartSubScheduling(): ready: mapInfoCollection of size="<<mapInfoCollection->size());
    return mapInfoCollection;
} // doStartSubScheduling

