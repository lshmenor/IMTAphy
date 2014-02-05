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

#include <WNS/scheduler/strategy/dsastrategy/HighCwithHighI.hpp>
#include <WNS/scheduler/SchedulerTypes.hpp>
#include <WNS/Positionable.hpp>
#include <WNS/PowerRatio.hpp>
//#include <utility>

using namespace wns::scheduler;
using namespace wns::scheduler::strategy;
using namespace wns::scheduler::strategy::dsastrategy;

STATIC_FACTORY_REGISTER_WITH_CREATOR(HighCwithHighI,
                                     DSAStrategyInterface,
                                     "HighCwithHighI",
                                     wns::PyConfigViewCreator);

HighCwithHighI::HighCwithHighI(const wns::pyconfig::View& config)
    : DSAStrategy(config)
{

}

HighCwithHighI::~HighCwithHighI()
{

}

// call this before each timeSlot/frame
void
HighCwithHighI::initialize(SchedulerStatePtr schedulerState,
                         SchedulingMapPtr schedulingMap)
{
	DSAStrategy::initialize(schedulerState,schedulingMap); // must always initialize base class too

	int maxSubChannel = schedulerState->currentState->strategyInput->getFChannels();
	int numberOfTimeSlots = schedulerState->currentState->strategyInput->getNumberOfTimeSlots();
	int maxSpatialLayers = schedulerState->currentState->strategyInput->getMaxSpatialLayers();

	int prio = schedulerState->currentState->getCurrentPriority();
	wns::scheduler::ConnectionSet conns = colleagues.registry->getConnectionsForPriority(prio);

	// store users in userIDs from type std::set
	std::set<wns::scheduler::UserID> userIDs;

	wns::scheduler::ConnectionSet::iterator connsSetIt;
	for(connsSetIt = conns.begin(); connsSetIt != conns.end(); connsSetIt++)
		userIDs.insert(colleagues.registry->getUserForCID(*connsSetIt));

	unsigned int numberOfUsers = userIDs.size();
	unsigned int numberOfResources = maxSubChannel * numberOfTimeSlots * maxSpatialLayers;

	MESSAGE_SINGLE(NORMAL, logger, "getSubChannelWithDSA: Distributing " 
		<< numberOfUsers << " users on " << numberOfResources << " resources.");

	if(numberOfUsers == 0)
		return;

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

	// store carriers per each user in carrierMultimap<carrier, UserID>
	std::multimap<wns::Power, unsigned int> carrierMultimap;
	wns::Power tmpCarrier;

	std::set<wns::scheduler::UserID>::iterator userIdIt;
	for(userIdIt = userIDs.begin(); userIdIt != userIDs.end(); userIdIt++)
	{
		tmpCarrier = colleagues.registry->estimateTxSINRAt(*userIdIt).carrier;
		carrierMultimap.insert(std::pair<wns::Power, unsigned int>(tmpCarrier, (*userIdIt).getNodeID()));
	}

////////////////////////////////////////////////////////////////////////////////////////
//	std::multimap<wns::Power, unsigned int>::iterator carrierMultimapItt;
//	for(carrierMultimapItt = carrierMultimap.begin(); carrierMultimapItt != carrierMultimap.end(); carrierMultimapItt++)
//	{
//		std::cout << "DEBUGGG: User: " << carrierMultimapItt->second << " Signalstaerke: " << carrierMultimapItt->first << "\n";
//	}
////////////////////////////////////////////////////////////////////////////////////////

	// store sorted resources in sortedResourcesSet<DSAResult, FreqFirst>
	std::set<DSAResult, FreqFirst> sortedResourcesSet;
	DSAResult res;
	for(int i = 0; i < maxSubChannel; i++)
	{
		for(int j = 0; j < numberOfTimeSlots; j++)
		{
			for(int k = 0; k < maxSpatialLayers; k++)
			{
//				DSAResult res;
				res.subChannel = i;
				res.timeSlot = j;
				res.spatialLayer = k;
				sortedResourcesSet.insert(res);
			}
		} 
	}

	// store interference per each Slot in interferenceMultimap<interference, SlotNr>
	std::multimap<wns::Power, DSAResult> interferenceMultimap;
	wns::Power tmpInterference;
	DSAResult tmpDSAResult;

	std::set<DSAResult, FreqFirst>::iterator sortedResourcesSetIt;
	for(sortedResourcesSetIt = sortedResourcesSet.begin(); sortedResourcesSetIt != sortedResourcesSet.end(); sortedResourcesSetIt++)
	{
//		DSAResult tmpDSAResult = *sortedResourcesSetIt;
		tmpDSAResult = *sortedResourcesSetIt;
		tmpInterference = colleagues.registry->estimateTxSINRAt(schedulerState->myUserID, tmpDSAResult.timeSlot).interference;
		interferenceMultimap.insert(std::pair<wns::Power, DSAResult>(tmpInterference, tmpDSAResult));
	}

////////////////////////////////////////////////////////////////////////////////////////
//	std::multimap<wns::Power, DSAResult>::iterator interferenceMultimapItt;
//	for(interferenceMultimapItt = interferenceMultimap.begin(); interferenceMultimapItt != interferenceMultimap.end(); interferenceMultimapItt++)
//	{
//		std::cout << "DEBUGGG: Slot: " << interferenceMultimapItt->second.timeSlot << " Interferenz: " << interferenceMultimapItt->first << "\n";
//	}
////////////////////////////////////////////////////////////////////////////////////////

	int counter = 0;
    int user = 0;
	int resourcesPerUser = resourcesLess;

    DSAResult tmpResource;
	std::vector<DSAResult> tmpDSAResultVector;
//	tmpDSAResultVector.reserve(resourcesPerUser); // Vectorgroesse festlegen

	std::multimap<wns::Power, unsigned int>::iterator carrierMultimapIt;
	carrierMultimapIt = carrierMultimap.begin();

	std::multimap<wns::Power, DSAResult>::iterator interferenceMultimapIt;
	for(interferenceMultimapIt = interferenceMultimap.begin(); interferenceMultimapIt != interferenceMultimap.end(); interferenceMultimapIt++)
	{
		tmpResource.subChannel = interferenceMultimapIt->second.subChannel;
		tmpResource.timeSlot = interferenceMultimapIt->second.timeSlot;
		tmpResource.spatialLayer = interferenceMultimapIt->second.spatialLayer;
		tmpDSAResultVector.push_back(tmpResource);

		counter++;

		if(counter == resourcesPerUser)
		{
			usersResources_[carrierMultimapIt->second] = tmpDSAResultVector; 
			carrierMultimapIt++;
			tmpDSAResultVector.clear();
			user++;
			counter = 0;
		}

		if(user == usersLess)
		{
			resourcesPerUser = resourcesMore;
//			tmpDSAResultVector.reserve(resourcesPerUser); // Vectorgroesse festlegen
		}

	}

////////////////////////////////////////////////////////////////////////////////////////
//	std::map<unsigned int, std::vector<DSAResult> >::iterator usersResourcesMapItt;
//	for(usersResourcesMapItt = usersResources_.begin(); usersResourcesMapItt != usersResources_.end(); usersResourcesMapItt++)
//	{
//		std::cout << "DEBUGGG: User: " << usersResourcesMapItt->first
//		<< " bekommt folgende Slots: " << "\n";
//
//		std::vector<DSAResult>::iterator DSAResultVectorItt;
//		for(DSAResultVectorItt = usersResourcesMapItt->second.begin(); DSAResultVectorItt != usersResourcesMapItt->second.end(); DSAResultVectorItt++)
//		{
//			std::cout << DSAResultVectorItt->timeSlot << " DEBUGGG" << "\n";
//		} 
//	}
////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////
//	// request.user <-----> *userIdIt = User = UT8
//	std::set<wns::scheduler::UserID>::iterator userIdIt;
//	for(userIdIt = userIDs.begin(); userIdIt != userIDs.end(); userIdIt++)
//	{
//		std::cout << "DEBUGGG: " 
//		<< " UserID: " << (*userIdIt).getNodeID() 
//		<< " Carrier: " << colleagues.registry->estimateTxSINRAt(*userIdIt).carrier 
//		<< " Interferenz: " << colleagues.registry->estimateTxSINRAt(*userIdIt).interference 
//		<< " Pathloss: " << colleagues.registry->estimateTxSINRAt(*userIdIt).pathloss << "\n";
//	}
////////////////////////////////////////////////////////////////////////////////////////

} //initialize

DSAResult
HighCwithHighI::getSubChannelWithDSA(RequestForResource& request,
                                   SchedulerStatePtr schedulerState,
                                   SchedulingMapPtr schedulingMap)
{
//	DSAResult dsaResult;
	MESSAGE_SINGLE(NORMAL, logger, "getSubChannelWithDSA(" << request.toString()<<")");

	assure(usersResources_.find(request.user.getNodeID()) != usersResources_.end(), "No resources for user " + request.user.getNodeID());

	std::vector<DSAResult>::iterator dsaResultVectortIt;

	bool found = false;
	for(dsaResultVectortIt = usersResources_[request.user.getNodeID()].begin();
		dsaResultVectortIt != usersResources_[request.user.getNodeID()].end() && !found; 
		dsaResultVectortIt++)
	{
		found = channelIsUsable(dsaResultVectortIt->subChannel, dsaResultVectortIt->timeSlot, dsaResultVectortIt->spatialLayer,
								request, schedulerState, schedulingMap);
	}

	if(!found)
	{
		MESSAGE_SINGLE(NORMAL, logger, "getSubChannelWithDSA(): no free subchannel");
		DSAResult dsaResult;
		return dsaResult;
	}

	else
	{
		dsaResultVectortIt--;
		MESSAGE_SINGLE(NORMAL, logger, "getSubChannelWithDSA(): Granting resource: "
			<< dsaResultVectortIt->subChannel << "." << dsaResultVectortIt->timeSlot << "." 
			<< dsaResultVectortIt->spatialLayer << " to User: " << request.user.getNodeID());// << request.toString());
		return *dsaResultVectortIt;
	}
} // getSubChannelWithDSA

