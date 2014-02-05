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

#include <WNS/ldk/Layer.hpp>
#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/ldk/Command.hpp>
#include <WNS/TypeInfo.hpp>

#include <WNS/Exception.hpp>

#include <sstream>

using namespace std;
using namespace wns::ldk;

CommandPool::CommandPool(const CommandProxy* _proxy,
			 const fun::FUN* _origin) :
	PCI(),
	path(),
	proxy(_proxy),
	commands(),
	origin(_origin),
	receiver(NULL)
{
} // CommandPool


CommandPool::CommandPool(const CommandPool& that) :
        // RefCountable(),
	PCI(that),
	proxy(that.proxy),
	commands(),
	origin(that.origin),
	receiver(that.receiver)
{
	proxy->copy(this, &that);
} // CommandPool


CommandPool::~CommandPool()
{
	CommandContainer::const_iterator end = commands.end();
	for (CommandContainer::const_iterator ii = commands.begin();
	     ii != end;
	     ++ii)
	{
		Command* command = *ii;
		if (NULL != command)
		{
			delete command;
		}
	}
} // ~CommandPool


void
CommandPool::calculateSizes(
	Bit& commandPoolSize,
	Bit& dataSize,
	const FunctionalUnit* questioner) const
{
	proxy->calculateSizes(this, commandPoolSize, dataSize, questioner);
} // calculateSizes


void
CommandPool::setSDUPtr(osi::PDUPtr& sdu)
{
	if(sdu) {
		setPDULength(sdu->getLengthInBits());
	} else {
		setPDULength(0); // only a valid branch during testing
	}

	PCI::setSDU(sdu);
} // setSDU

std::string
CommandPool::dumpCommandTypes() const {
	std::stringstream str;
	CommandProxy::CommandIDType commandID = 0;
	for(CommandContainer::const_iterator iter = commands.begin();
	    iter != commands.end();
	    ++iter)
	{
 		str << "Command ID: " << commandID << ", Type: ";
		if(*iter == NULL)
		{
			str << "NULL";
		}
		else
		{
			str << TypeInfo::create(**iter);
		}
		str << "\n";
		++commandID;
	}
	return str.str();
}

bool
CommandPool::knowsSameCommandsAs(const CommandPool& swimmingPool) const
{
	bool isSame = true;
	CommandContainer::const_iterator iterMe = commands.begin();
	CommandContainer::const_iterator iterSwimmingPool = swimmingPool.commands.begin();
	while (isSame && (iterMe != commands.end()) && (iterSwimmingPool != swimmingPool.commands.end())) {
		isSame = ((*iterMe == NULL) && (*iterSwimmingPool == NULL) ||
			  (*iterMe != NULL) && (*iterSwimmingPool != NULL));
		++iterMe;
		++iterSwimmingPool;
	}
	return isSame;
}

void
CommandPool::insert(
	const CommandProxy::CommandIDType& id,
	Command* command)
{
	// if container is too small: resize and set new elements to NULL
	if (commands.size() <= id)
	{
		size_t oldSize = commands.size();
		commands.resize(id+1);
		for(size_t ii = oldSize; ii < commands.size(); ++ii)
		{
			commands.at(ii) = NULL;
		}
	}
	if (NULL != commands.at(id))
	{
		throw wns::Exception("You tried to insert a command which is already inserted!");
	}

	commands.at(id) = command;
}

wns::ldk::Command*
CommandPool::find(const CommandProxy::CommandIDType& id) const
{
	return commands.at(id);
}

bool
CommandPool::knows(const CommandProxy::CommandIDType& id) const
{
	if (commands.size() <= id) {
		return false;
	} else {
		return commands.at(id) != NULL;
	}
}

#ifndef NDEBUG
size_t
CommandPool::calcObjSize() const
{
	size_t sum = sizeof( *this );
	for (size_t ii = 0; ii < commands.size(); ++ii)
	{
		if (commands.at(ii)!=NULL)
		{
			sum += proxy->getCommandObjSize(ii);
		}
	}
	return sum;
}
#endif

