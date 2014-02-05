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

#include <WNS/scheduler/strategy/staticpriority/SubStrategyInterface.hpp>
#include <WNS/scheduler/strategy/StaticPriority.hpp>
#include <WNS/scheduler/strategy/Strategy.hpp>
#include <WNS/scheduler/strategy/SchedulerState.hpp>
#include <WNS/scheduler/SchedulingMap.hpp>
#include <WNS/scheduler/SchedulerTypes.hpp>
#include <WNS/pyconfig/View.hpp>

#include <vector>
#include <map>
#include <algorithm>
#include <iostream>
#include <set>

using namespace wns::scheduler;
using namespace wns::scheduler::strategy;
using namespace wns::scheduler::strategy::staticpriority;

STATIC_FACTORY_REGISTER_WITH_CREATOR(StaticPriority,
				     StrategyInterface,
				     "StaticPriority",
				     wns::PyConfigViewCreator);

StaticPriority::StaticPriority(const wns::pyconfig::View& config):
    Strategy(config),
    numberOfPriorities(0),
    subStrategies(),
    resourceUsage(0.0)
{
    MESSAGE_SINGLE(NORMAL, logger, "StaticPriority() instance created.");
}

StaticPriority::~StaticPriority()
{
  for (int priority = 0; priority<numberOfPriorities; ++priority)
    {
      if (subStrategies[priority] != NULL)
	{
	  delete subStrategies[priority];
	}
    }
}

float
StaticPriority::getResourceUsage() const
{
    return resourceUsage;
}


void
StaticPriority::onColleaguesKnown()
{
    /* Inherited by base class Strategy to do first-time initialization.
       It must be done in every derived method.
    */
    Strategy::onColleaguesKnown();
    numberOfPriorities = colleagues.registry->getNumberOfPriorities();

    // loop over all priorities and initialize subStrategies
    assure(numberOfPriorities == pyConfig.len("subStrategies"),
	   "numberOfPriorities=" << numberOfPriorities
	   << " != "<<pyConfig.len("subStrategies"));

    MESSAGE_SINGLE(NORMAL, logger,
		   "StaticPriority::onColleaguesKnown(), numberOfPriorities = "
		   << numberOfPriorities);

    for (int priority = 0; priority<numberOfPriorities; ++priority)
      {
	// for all priorities
	wns::pyconfig::View substrategyView = pyConfig.getView("subStrategies",
							       priority);
	std::string substrategyName = substrategyView.get<std::string>("__plugin__");
	MESSAGE_SINGLE(NORMAL, logger, "subStrategy["
		       << priority << "]=" << substrategyName);
	wns::scheduler::strategy::staticpriority::SubStrategyInterface* substrategy = NULL;
	if (substrategyName.compare("NONE") != 0)
	  {
	    // create the subscheduling strategy for this priority
	    wns::scheduler::strategy::staticpriority::SubStrategyCreator* subStrategyCreator;
	    subStrategyCreator = wns::scheduler::strategy::staticpriority::SubStrategyFactory::creator(substrategyName);
	    substrategy = subStrategyCreator->create(substrategyView);
	  }
	else
	  {
	    substrategy = NULL;
	  }
	subStrategies.push_back(substrategy);
      }

    SchedulerStatePtr schedulerState = getSchedulerState();

    // priority is out of [0..MaxPriority-1]
    for (int priority = 0; priority < numberOfPriorities; ++priority)
      {
	MESSAGE_SINGLE(NORMAL, logger, "initializing subStrategy["
		       << priority << "]");

	wns::scheduler::strategy::staticpriority::SubStrategyInterface* substrategy = subStrategies[priority];

	assure(substrategy != NULL, "The substrategy[" << priority
	       << "] is not accessible");
	substrategy->setColleagues(this, colleagues.queue, colleagues.registry,
				   colleagues.harq);
      }
}

StrategyResult
StaticPriority::doStartScheduling(SchedulerStatePtr schedulerState,
				  SchedulingMapPtr schedulingMap)
{
    assure(numberOfPriorities > 0,"illegal numberOfPriorities="
	   << numberOfPriorities);
    int frameNr = schedulerState->currentState->strategyInput->getFrameNr();
    MESSAGE_SINGLE(NORMAL, logger, "StaticPriority::doStartScheduling(frame="
		   << frameNr << "):"
		   << " numberOfPriorities="<<numberOfPriorities);

    colleagues.queue->frameStarts();

    // prepare result datastructure
    MapInfoCollectionPtr mapInfoCollection = MapInfoCollectionPtr(new wns::scheduler::MapInfoCollection);
    StrategyResult strategyResult(schedulingMap,mapInfoCollection);
    this->resourceUsage=0.0;

    MESSAGE_SINGLE(NORMAL, logger, "doStartScheduling(): userSelection. getDL="
		   << schedulerState->isDL << ", isTX="
		   << schedulerState->isTx << ", schedulerSpot="
		   << wns::scheduler::SchedulerSpot::toString(schedulerState->schedulerSpot));

    // only if (maxSpatialLayers > 1)
    if (groupingRequired() && !colleagues.queue->isEmpty())
      {
	// set grouping into result output (needed later to set antennaPatterns)
	MESSAGE_SINGLE(NORMAL, logger, "doStartScheduling(): write grouping in strategyResult");
	strategyResult.sdmaGrouping =  schedulerState->currentState->getGrouping();
      }

    // priority is out of [0..MaxPriority-1]:
    for (int priority = 0; priority < numberOfPriorities; ++priority)
      {
	// grouping alternative: do grouping within each priority;
	// to be discussed; but seems to be less useful
	if (subStrategies[priority] == NULL)
	  continue;

	bool usesHARQ = subStrategies[priority]->usesHARQ();
	schedulerState->currentState->setCurrentPriority(priority);

	MESSAGE_SINGLE(NORMAL, logger, "doStartScheduling(): now scheduling priority="
		       << priority);

	// get all registered connections for the current priority
	ConnectionSet allConnections = colleagues.registry->getConnectionsForPriority(priority);
	MESSAGE_SINGLE(NORMAL, logger, "allConnections      = "
		       <<printConnectionSet(allConnections));

	ConnectionSet reachableConnections = colleagues.registry->filterReachable(allConnections, frameNr, usesHARQ);
	MESSAGE_SINGLE(NORMAL, logger, "reachableConnections= "
		       <<printConnectionSet(reachableConnections));

	GroupingPtr grouping = schedulerState->currentState->getGrouping();
	if (grouping != GroupingPtr())
	  {
	    //serve only connections of users considered in grouping
	    ConnectionSet intersectionConnections;
	    wns:scheduler::UserSet reachableUsers;
	    wns::scheduler::ConnectionSet::iterator it;

	    for(it = reachableConnections.begin(); it != reachableConnections.end(); it++)
	      {
		MESSAGE_SINGLE(NORMAL, logger, " 111 cid: "<< *it <<
			       " #groups :" << grouping->groups.size());
		wns::scheduler::UserID user = colleagues.registry->getUserForCID(*it);
		if(!user.isBroadcast())
		  {
		    if (grouping->userGroupNumber.find(user) != grouping->userGroupNumber.end())
		      {
			intersectionConnections.insert(*it);
		      }
		    else
		      {
			MESSAGE_SINGLE(NORMAL, logger, " ignore connections: "
				       << *it <<" of user:"<< user);
		      }
		  }
	      }
	    reachableConnections.clear();
	    reachableConnections = intersectionConnections;
	  }

	// do not filter out unqueued cids since the subStrategy
	// may want to update the state for every cid
	schedulerState->currentState->activeConnections = reachableConnections;

	// start SubScheduling in any case (even with empty user or cid list)
	// since substrategies may need to keep and track their own state over time
	MapInfoCollectionPtr resultBursts = subStrategies[priority]->doStartSubScheduling(schedulerState, schedulingMap);

	// copy ^ of std::list<MapInfoEntryPtr>
	if (resultBursts->size()>0)
	  {
	    MESSAGE_SINGLE(NORMAL, logger, "merged "
			   << resultBursts->size()
			   << " entries of resultBursts into"
			   << " mapInfoCollection (now size="
			   << mapInfoCollection->size() << ")");

	    // collects result bursts. Do not use merge! (sorts bySmartPtr)
	    mapInfoCollection->join(*resultBursts);
	  }
      }

    MESSAGE_SINGLE(NORMAL, logger, "StaticPriority: "
		   <<schedulingMap->getNumberOfCompounds()
		   <<" compounds scheduled");
    MESSAGE_SINGLE(NORMAL, logger, schedulingMap->toString());
    this->resourceUsage = schedulingMap->getResourceUsage();
    return strategyResult;
}
