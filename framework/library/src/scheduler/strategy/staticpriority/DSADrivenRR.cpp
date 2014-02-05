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

#include <WNS/scheduler/strategy/staticpriority/DSADrivenRR.hpp>
#include <WNS/scheduler/SchedulerTypes.hpp>

#include <vector>
#include <map>
#include <algorithm>
#include <iostream>

using namespace std;
using namespace wns::scheduler;
using namespace wns::scheduler::strategy;
using namespace wns::scheduler::strategy::staticpriority;

STATIC_FACTORY_REGISTER_WITH_CREATOR(DSADrivenRR,
                                     SubStrategyInterface,
                                     "DSADrivenRR",
                                     wns::PyConfigViewCreator);


DSADrivenRR::DSADrivenRR(const wns::pyconfig::View& config)
    : SubStrategy(config),
    lastServedConnection(0)
{
}

DSADrivenRR::~DSADrivenRR()
{
}

void
DSADrivenRR::initialize()
{
}

wns::scheduler::ConnectionID
DSADrivenRR::getNextConnection(const ConnectionSet &currentConnections, ConnectionID cid) const
{
    wns::scheduler::ConnectionSet::iterator iter =
        currentConnections.upper_bound(cid);

    if(iter != currentConnections.end()) 
    {
        return *iter;
    } 
    else 
    {
        return *currentConnections.begin();
    }
}

wns::scheduler::MapInfoCollectionPtr
DSADrivenRR::doStartSubScheduling(SchedulerStatePtr schedulerState,
                                 wns::scheduler::SchedulingMapPtr schedulingMap)
{
    MapInfoCollectionPtr mapInfoCollection = 
        MapInfoCollectionPtr(new wns::scheduler::MapInfoCollection); // result datastructure
    ConnectionSet &currentConnections = schedulerState->currentState->activeConnections;

    if (currentConnections.empty()) 
        return mapInfoCollection; // nothing to do

    wns::scheduler::ConnectionID currentConnection = 
        getNextConnection(currentConnections,lastServedConnection);

    MESSAGE_SINGLE(NORMAL, logger, "doStartSubScheduling("
        << printConnectionSet(currentConnections) << ") start with cid="
        << currentConnection);
    
    wns::scheduler::ConnectionID firstConnection = currentConnection;

    // Assure the connection we start with gets only served once
    // to avoid infinite loop
    bool first = true;

    while(first || currentConnection != firstConnection)
    {
        first = false;
        int pduCounter = 0;

        bool spaceLeft = true;
        while(colleagues.queue->queueHasPDUs(currentConnection)
               && spaceLeft)
        { 
            spaceLeft = scheduleCid(schedulerState, 
                                    schedulingMap,
                                    currentConnection,
                                    pduCounter,
                                    99999, // Infinite block size allowed
                                    mapInfoCollection);
            if(spaceLeft)
            {
                // This is the last connection that actually got resources in this frame
                lastServedConnection = currentConnection;
            }
            MESSAGE_SINGLE(NORMAL, logger, "doStartSubScheduling(): Test "
            << spaceLeft << "  " << (colleagues.queue->queueHasPDUs(currentConnection)) 
            << " "  <<  currentConnection);
        } 
 
        MESSAGE_SINGLE(NORMAL, logger, "doStartSubScheduling(): Scheduled "
            << pduCounter << " PDUs for connection " <<  currentConnection);

        currentConnections.erase(currentConnection);
        if(currentConnections.size() == 0)
        {
            break; // all queues empty
        }

        currentConnection = getNextConnection(currentConnections,currentConnection);

        MESSAGE_SINGLE(NORMAL, logger, "doStartSubScheduling(): next connection="
            << currentConnection);
    }

    MESSAGE_SINGLE(NORMAL, logger, "doStartSubScheduling(): "
        << "Finished scheduling. Last served connection was " << lastServedConnection);

    return mapInfoCollection;
} // doStartSubScheduling

