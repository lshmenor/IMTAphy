/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
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

#include <WNS/ldk/tools/BottleNeckDetective.hpp>

using namespace wns::ldk;
using namespace wns::ldk::tools;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	BottleNeckDetective,
	FunctionalUnit,
	"wns.ldk.tools.BottleNeckDetective",
	FUNConfigCreator);

BottleNeckDetectiveCommand::BottleNeckDetectiveCommand() :
	Command()
{
	magic.sender = NULL;
	magic.id = -1;
} // BottleNeckDetectiveCommand

BottleNeckDetectiveCommand::~BottleNeckDetectiveCommand()
{
	magic.sender = NULL;
	magic.id = -1;
} // ~BottleNeckDetectiveCommand

BottleNeckDetective::BottleNeckDetective(fun::FUN* fuNet, const wns::pyconfig::View& config) :
	Processor<BottleNeckDetective>(),
	CommandTypeSpecifier<BottleNeckDetectiveCommand>(fuNet),
	HasReceptor<>(),
	HasConnector<>(),
	HasDeliverer<>(),
	Cloneable<BottleNeckDetective>(),
	PeriodicTimeout(),
	id(0),
	compounds(),
	logger(config.get("logger"))
{
	MESSAGE_BEGIN(NORMAL, logger, m, "Setting periodic timeout to: ");

	m << config.get<simTimeType>("observationInterval") << " (observation interval), ";
	m << config.get<simTimeType>("offset") << " (offset)";

	this->startPeriodicTimeout(
		config.get<simTimeType>("observationInterval"),
		config.get<simTimeType>("offset"));

	MESSAGE_END();
} // BottleNeckDetective


BottleNeckDetective::~BottleNeckDetective()
{
	MESSAGE_BEGIN(NORMAL, logger, m, "Canceling periodic timeout: ");
	this->cancelPeriodicTimeout();
	MESSAGE_END();

	compounds.clear();
} // ~BottleNeckDetective


#ifndef WNS_NO_LOGGING
void
BottleNeckDetective::processOutgoing(const CompoundPtr& compound)
{
	BottleNeckDetectiveCommand* bndc = this->activateCommand(compound->getCommandPool());
	bndc->magic.sender = this;
	bndc->magic.id = ++this->id;
	compounds[id] = compound;
} // processOutgoing
#else
void
BottleNeckDetective::processOutgoing(const CompoundPtr&)
{
} // processOutgoing
#endif // WNS_NO_LOGGING


#ifndef WNS_NO_LOGGING
void
BottleNeckDetective::processIncoming(const CompoundPtr& compound)
{
	long long int id = this->getCommand(compound->getCommandPool())->magic.id;
	BottleNeckDetective* sender = this->getCommand(compound->getCommandPool())->magic.sender;
	// remove this compound from sender, it's no longer in the FUN
	sender->compounds.erase(id);
} // processOutgoing
#else
void
BottleNeckDetective::processIncoming(const CompoundPtr&)
{
} // processOutgoing
#endif //WNS_NO_LOGGING

void
BottleNeckDetective::periodically()
{
	MESSAGE_BEGIN(NORMAL, logger, m, "Current compound distribution in FUN:\n");
	std::map<std::string, int> countCompounds;
	// create map and show highscore:
	for(CompoundContainer::const_iterator itr = compounds.begin();
		itr != compounds.end();
		++itr)
	{
        if(itr->second->getJourney().size() > 0)
        {
		    Visit v = *(itr->second->getJourney().rbegin());
		    if(countCompounds.find(v.location) != countCompounds.end())
		    {
			    countCompounds[v.location]++;
		    }
		    else
		    {
			    countCompounds[v.location] = 0;
		    }
        }
	}

	std::multimap<int, std::string> countCompoundsSorted;
	for(std::map<std::string, int>::const_iterator itr = countCompounds.begin();
		itr != countCompounds.end();
		++itr)
	{
		countCompoundsSorted.insert(std::pair<int, std::string>(itr->second, itr->first));
	}

	for(std::multimap<int, std::string>::const_iterator itr = countCompoundsSorted.begin();
		itr != countCompoundsSorted.end();
		++itr)
	{
		m << "Location: " <<  itr->second << ", Compounds: " << itr->first << "\n";
	}
	m << "End of compound distribution list";
	MESSAGE_END();

	// start counting again
	compounds.clear();
}





