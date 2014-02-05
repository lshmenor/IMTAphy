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

#include <WNS/scheduler/strategy/dsastrategy/FarFirst.hpp>
#include <WNS/scheduler/SchedulerTypes.hpp>
#include <WNS/Positionable.hpp>

using namespace wns::scheduler;
using namespace wns::scheduler::strategy;
using namespace wns::scheduler::strategy::dsastrategy;

STATIC_FACTORY_REGISTER_WITH_CREATOR(FarFirst,
                                     DSAStrategyInterface,
                                     "FarFirst",
                                     wns::PyConfigViewCreator);

FarFirst::FarFirst(const wns::pyconfig::View& config)
    : DSAStrategy(config)
{

}

FarFirst::~FarFirst()
{

}

// call this before each timeSlot/frame
void
FarFirst::initialize(SchedulerStatePtr schedulerState,
                         SchedulingMapPtr schedulingMap)
{
    DSAStrategy::initialize(schedulerState,schedulingMap); // must always initialize base class too

    int maxSubChannel = schedulerState->currentState->strategyInput->getFChannels();
    int numberOfTimeSlots = schedulerState->currentState->strategyInput->getNumberOfTimeSlots();
    int maxSpatialLayers = schedulerState->currentState->strategyInput->getMaxSpatialLayers();

    int prio = schedulerState->currentState->getCurrentPriority();
    wns::scheduler::ConnectionSet conns = colleagues.registry->getConnectionsForPriority(prio);

    std::set<wns::scheduler::UserID> userIDs;
    wns::scheduler::ConnectionSet::iterator it;

    // store users in userIDs from type std::set
    for(it = conns.begin(); it != conns.end(); it++)
        userIDs.insert(colleagues.registry->getUserForCID(*it));

    unsigned int numberOfUsers = userIDs.size();
    unsigned int numberOfResources = maxSubChannel * numberOfTimeSlots * maxSpatialLayers;

    MESSAGE_SINGLE(NORMAL, logger, "getSubChannelWithDSA: Distributing " 
        << numberOfUsers << " users on " << numberOfResources << " resources.");

    if(numberOfUsers == 0)
        return;

    // store resources in sortedResources_ from type std::set
    for(int i = 0; i < maxSubChannel; i++)
    {
        for(int j = 0; j < numberOfTimeSlots; j++)
        {
            for(int k = 0; k < maxSpatialLayers; k++)
            {
                DSAResult res;
                res.subChannel = i;
                res.timeSlot = j;
                res.spatialLayer = k;
                sortedResources_.insert(res);
            }
        } 
    }

    int usersMore = numberOfResources % numberOfUsers;
    int usersLess = numberOfUsers - usersMore;
    int resourcesMore = int(numberOfResources / numberOfUsers) + 1;
    int resourcesLess = int(numberOfResources / numberOfUsers);

    MESSAGE_SINGLE(NORMAL, logger, "getSubChannelWithDSA: " 
        << usersLess << " users get " << resourcesLess << " resources, "
        << usersMore << " users get " << resourcesMore << " resources");

    assure(resourcesLess > 0, "Not enough resources for all users");
    assure(usersMore * resourcesMore + usersLess * resourcesLess == numberOfResources,
        "Mismatched resource distribution");

	// BS Position
	wns::PositionableInterface* bsPos = schedulerState->myUserID.getNode()->getService<wns::PositionableInterface*>("mobility");

	//map<Distanz zu BS, UserID>
	std::map<double, wns::scheduler::UserID, std::greater<double> > userIdMap;
    std::set<wns::scheduler::UserID>::iterator userIdIt;
	for(userIdIt = userIDs.begin(); userIdIt != userIDs.end(); userIdIt++)
	{
		// SSs Positions
		wns::PositionableInterface* ssPos = (*userIdIt).getNode()->getService<wns::PositionableInterface*>("mobility");
		userIdMap[ssPos->getDistance(bsPos)] = *userIdIt;
	}

	std::map<double, wns::scheduler::UserID, std::greater<double> >::iterator userIdMapIt;
	userIdMapIt = userIdMap.begin();

	// Resources for first UserID
	resStart_[userIdMapIt->first] = sortedResources_.begin();
	resAmount_[userIdMapIt->first] = resourcesLess;

	MESSAGE_SINGLE(NORMAL, logger, "getSubChannelWithDSA: User "
		<< userIdMapIt->second.getNodeID() << " with distance " << userIdMapIt->first << " starts at "
		<< sortedResources_.begin()->subChannel << "."
		<< sortedResources_.begin()->timeSlot << "."
		<< sortedResources_.begin()->spatialLayer
		<< " and gets " << resourcesLess << " resources");

	userIdMapIt++;

    // Resources for other UserIDs
    int plus = 0;
    int user = 0;
    int counter = 0;

    std::set<DSAResult, FreqFirst>::iterator itr;
    for(itr = sortedResources_.begin(); itr != sortedResources_.end(); itr++)
    {
        if(counter == resourcesLess + plus)
        {
			resStart_[userIdMapIt->first] = itr;
			resAmount_[userIdMapIt->first] = resourcesLess + plus;

            MESSAGE_SINGLE(NORMAL, logger, "getSubChannelWithDSA: User " 
				<< userIdMapIt->second.getNodeID() << " with distance " << userIdMapIt->first << " starts at "
				<< itr->subChannel << "."
				<< itr->timeSlot << "."
				<< itr->spatialLayer
				<< " and gets " << resourcesLess + plus << " resources");

            counter = 0;
			userIdMapIt++;
            user++;
        }
        if(plus == 0 && user == usersLess - 1)
        {
            plus++;
            counter++;
        }
    counter++;
    }
////////////////////////////////////////////////////////////////////////////////////////
	std::map<double, std::set<DSAResult>::iterator>::iterator resStartIt;
	for(resStartIt = resStart_.begin(); resStartIt != resStart_.end(); resStartIt++)
	{
		MESSAGE_SINGLE(NORMAL, logger, "FarFirst::initialize:  "
			<< "Distanz zu BS : " << resStartIt->first
			<< "  Ressourcen-Start: " 
			<< resStartIt->second->subChannel << "."
			<< resStartIt->second->timeSlot << "."
			<< resStartIt->second->spatialLayer);
	}


	std::map<double, int>::iterator resAmountIt;
	for(resAmountIt = resAmount_.begin(); resAmountIt != resAmount_.end(); resAmountIt++)
	{
		MESSAGE_SINGLE(NORMAL, logger, "FarFirst::initialize:  "
			<< "Distanz zu BS : " << resAmountIt->first
			<< "  Ressourcen-Groesse: " << resAmountIt->second);
	}

////////////////////////////////////////////////////////////////////////////////////////
} //initialize

DSAResult
FarFirst::getSubChannelWithDSA(RequestForResource& request,
                                   SchedulerStatePtr schedulerState,
                                   SchedulingMapPtr schedulingMap)
{
    MESSAGE_SINGLE(NORMAL, logger, "getSubChannelWithDSA(" << request.toString()<<")");

	wns::PositionableInterface* bsPos = schedulerState->myUserID.getNode()->getService<wns::PositionableInterface*>("mobility");
	wns::PositionableInterface* ssPos = request.user.getNode()->getService<wns::PositionableInterface*>("mobility");
	double distance = ssPos->getDistance(bsPos);

	assure(resStart_.find(distance) != resStart_.end(), "No resources for user " + request.user.getNodeID());
	assure(resAmount_.find(distance) != resAmount_.end(), "Unknown resource amount for user " + request.user.getNodeID());

	int max = resAmount_[distance];
    std::set<DSAResult>::iterator dsaResultSetIt;
    int i = 0;

    bool found = false;
    for(dsaResultSetIt = resStart_[distance]; i < max && !found; dsaResultSetIt++)
    {
        i++;
        found = channelIsUsable(dsaResultSetIt->subChannel, 
                                dsaResultSetIt->timeSlot,
                                dsaResultSetIt->spatialLayer,
                                request, 
                                schedulerState, 
                                schedulingMap);
    }

    if(!found)
    {
        MESSAGE_SINGLE(NORMAL, logger, "getSubChannelWithDSA(): no free subchannel");
        DSAResult dsaResult;
        return dsaResult;
    }
    else
    {
        dsaResultSetIt--;

		MESSAGE_SINGLE(NORMAL, logger, "getSubChannelWithDSA(): Granting resource: "
			<< dsaResultSetIt->subChannel << "." << dsaResultSetIt->timeSlot << "." 
			<< dsaResultSetIt->spatialLayer << " to " << request.toString());
		return *dsaResultSetIt;
    }
} // getSubChannelWithDSA

