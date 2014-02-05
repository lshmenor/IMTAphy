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

#include <WNS/ldk/CommandProxy.hpp>
#include <WNS/ldk/CommandReaderInterface.hpp>
#include <WNS/ldk/fun/FUN.hpp>
#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/ldk/Command.hpp>
#include <WNS/ldk/Compound.hpp>
#include <WNS/TypeInfo.hpp>
#include <WNS/Exception.hpp>
#include <WNS/simulator/ISimulator.hpp>

using namespace wns::ldk;

CommandProxy::CommandIDType CommandProxy::serial = CommandIDType(0);

CommandProxy::CommandProxy(const wns::pyconfig::View& config) :
	logger(config.get("logger"))
{
    wns::simulator::getResetSignal()->connect(&wns::ldk::CommandProxy::clearRegistries);
}


CommandProxy::~CommandProxy()
{
	for (size_t ii = 0; ii < this->commandTypeSpecifiers.size(); ++ii) {
		if (this->commandTypeSpecifierCloned.at(ii)) {
			delete this->commandTypeSpecifiers.at(ii);
		}
	}

	/** @todo If it has not already happened, delete the global registry of
	 * CommandTypeSpecifiers */
} // ~CommandProxy


void
CommandProxy::addFunctionalUnit(
	const std::string& commandName,
	CommandTypeSpecifierInterface* functionalUnit)
{
	assure(functionalUnit != NULL, "Not a valid CommandTypeSpecifierInterface.");
	assure(functionalUnit->getFUN() != NULL, "CommandTypeSpecifierInterface is not attached to a fun::FUN.");
	assure(functionalUnit->getFUN()->getProxy() == this, "CommandProxy mismatch.");

	// Registered FU at a global CommandIDRegistry
	if (this->getCommandIDRegistry().knows(commandName))
	{
		CommandIDType id = this->getCommandIDRegistry().find(commandName);
		functionalUnit->setPCIID(id);
		if (id < this->commandTypeSpecifierCloned.size())
		{
			if (this->commandTypeSpecifierCloned.at(id) == true)
			{
				// we are re-adding a formerly removed FU, so delete the clone
				delete this->commandTypeSpecifiers.at(id);
				this->commandTypeSpecifierCloned.at(id) = false;
				this->commandTypeSpecifiers.at(id) = functionalUnit;
			}
		}
	}
	else
	{
		this->getCommandIDRegistry().insert(commandName, serial);
		functionalUnit->setPCIID(serial);
		++serial;
	}

	// if container is too small: resize and set new elements to NULL
	if (this->commandTypeSpecifiers.size() <= functionalUnit->getPCIID()) {
		size_t oldSize = this->commandTypeSpecifiers.size();
		this->commandTypeSpecifiers.resize(functionalUnit->getPCIID()+1);
		this->commandTypeSpecifierCloned.resize(functionalUnit->getPCIID()+1);
		for(size_t ii = oldSize; ii < this->commandTypeSpecifiers.size(); ++ii) {
			this->commandTypeSpecifiers.at(ii) = NULL;
			this->commandTypeSpecifierCloned.at(ii) = false;
		}
	}

	if (this->commandTypeSpecifiers.at(functionalUnit->getPCIID()) == NULL) {
		this->commandTypeSpecifiers.at(functionalUnit->getPCIID()) = functionalUnit;
		MESSAGE_SINGLE(VERBOSE, logger, "Added FU to commandTypeSpecifiers List, Type is: " << wns::TypeInfo::create(*functionalUnit).toString());
	}

	// Keep a global list of CopyCommand Objects to enable
	// "partialCopy"
	if (! this->getCopyCommandRegistry().knows(functionalUnit->getPCIID()))
	{
		this->getCopyCommandRegistry().insert(functionalUnit->getPCIID(),
						      functionalUnit->getCopyCommandInterface());
	}
	else
	{
		// Make sure that the new CommandTypeSpecifier works on the same
		// command as the already known one.
		CopyCommandInterface* tmp = functionalUnit->getCopyCommandInterface();
		assure ( wns::TypeInfo::create(*this->getCopyCommandRegistry().find(functionalUnit->getPCIID()))
			 ==
			 wns::TypeInfo::create(*tmp), "forbidden commandName duplication ('"+commandName+"')" );
		delete tmp;
	}

	// Keep a global list of CommandReader Objects to access commands
	if (! this->getCommandReaderRegistry().knows(functionalUnit->getPCIID()))
	{
		this->getCommandReaderRegistry().insert(functionalUnit->getPCIID(),
							functionalUnit->getCommandReader(this));
	}
	else
	{
		// Make sure that the new CommandTypeSpecifier works on the same
		// command as the already registered one.
		CommandReaderInterface* tmp = functionalUnit->getCommandReader(this);
		assure ( wns::TypeInfo::create(*this->getCommandReaderRegistry().find(functionalUnit->getPCIID()))
			 ==
			 wns::TypeInfo::create(*tmp),  "forbidden commandName duplication ('"+commandName+"')" );
		delete tmp;
	}

} // addFunctionalUnit


void
CommandProxy::removeFunctionalUnit(const std::string& commandName)
{
	if (!this->getCommandIDRegistry().knows(commandName))
	{
		wns::Exception e;
		e << "Can't remove FU. FU commandName \"" << commandName << "\" unknown.";
		throw wns::Exception(e);
	}

	CommandIDType id = this->getCommandIDRegistry().find(commandName);

	if (this->commandTypeSpecifiers.at(id) == NULL)
	{
		wns::Exception e;
		e << "Can't remove FU. FU with commandName \"" << commandName
		  << "\" does not exist in this FUN.";
		throw wns::Exception(e);
	}

	if (this->commandTypeSpecifierCloned.at(id) == false)
	{
		CommandTypeSpecifierInterface* ctsi = this->commandTypeSpecifiers.at(id);
		assureType(ctsi, FunctionalUnit*);
		wns::CloneableInterface* clonedFU = dynamic_cast<FunctionalUnit*>(ctsi)->clone();
		assureType(clonedFU, FunctionalUnit*);
		this->commandTypeSpecifiers.at(id) = dynamic_cast<FunctionalUnit*>(clonedFU);

		this->commandTypeSpecifierCloned.at(id) = true;
	}
}


wns::ldk::Command*
CommandProxy::getCommand(
	const CommandPool* commandPool,
	const CommandTypeSpecifierInterface* kind) const
{
	assure(kind != NULL, "Invalid argument.");
	return this->getCommand(commandPool, kind->getPCIID());
} // getCommand


wns::ldk::Command*
CommandProxy::getCommand(
	const CommandPool* commandPool,
	CommandIDType n) const
{
	assure(commandPool != NULL, "Invalid argument.");
	assure(commandPool->knows(n),
	       "Command (Command ID: " << n << ") not activated in CommandPool. Only activated Commands may be retrieved." <<
	       "\nThe following commands are in the CommandPool:\n" << commandPool->dumpCommandTypes() <<
	       "\nThe following commands are registered at the CommandProxy:\n" << dumpCommandIDRegistry());

	Command* command = commandPool->find(n);

	return command;
} // getCommand

CommandReaderInterface*
CommandProxy::getCommandReader(const std::string& role) const
{
    assure(this->getCommandIDRegistry().knows(role), "ID for unknown role="<<role<<" requested. Available="<<dumpCommandIDRegistry());
    unsigned long int id = this->getCommandIDRegistry().find(role);
    assure(this->getCommandReaderRegistry().knows(id), "CommandReader for unknown ID="<<id<<" requested.");
    return this->getCommandReaderRegistry().find(id);
} // getCommandReader

bool
CommandProxy::commandIsActivated(
	const CommandPool* commandPool,
	const CommandTypeSpecifierInterface* kind) const
{
	assure(kind != NULL, "Invalid argument.");
	return this->commandIsActivated(commandPool, kind->getPCIID());
} // commandIsActivated


bool
CommandProxy::commandIsActivated(
	const CommandPool* commandPool,
	const CommandIDType& id) const
{
	assure(commandPool != NULL, "Invalid argument.");
	return commandPool->knows(id);
}


wns::ldk::Command*
CommandProxy::activateCommand(
	CommandPool* commandPool,
	const CommandTypeSpecifierInterface* kind)
{
	assure(commandPool != NULL, "Invalid argument.");
	assure(kind != NULL, "Invalid argument.");
	assure(!commandPool->knows(kind->getPCIID()),
	       "Command " << kind->getPCIID()  
            << " already added to command pool. Trying to activate an already activated Command.\n"
            << dumpCommandIDRegistry());

	Command* command = kind->createCommand();

	commandPool->insert(kind->getPCIID(), command);

#ifndef NDEBUG
	for(CommandPool::PathContainer::const_iterator i = commandPool->path.begin();
	    i != commandPool->path.end();
	    ++i)
		assure(*i != kind->getPCIID(), "Corrupted path: trying to add a CommandTypeSpecifierInterface twice.");
#endif
	commandPool->path.push_back(kind->getPCIID());

	return command;
} // activateCommand

Command*
CommandProxy::activateCommand(
                 CommandPool* commandPool,
                 const CommandIDType& id)
{
    assure(commandPool != NULL, "Invalid argument.");
    assure(id < commandTypeSpecifiers.size(), "Invalid ID");

    return activateCommand(commandPool, commandTypeSpecifiers.at(id));
}

CommandPool*
CommandProxy::createCommandPool(const fun::FUN* origin) const
{
	return new CommandPool(this, origin);
} // createCommandPool


const CommandTypeSpecifierInterface*
CommandProxy::getNext(
	const CommandPool* commandPool,
	const CommandTypeSpecifierInterface* questioner) const
{
	if(!questioner)
	{
		if(commandPool->path.empty())
			return NULL;

		return getCommandTypeSpecifier(commandPool->path.back());
	}

	CommandPool::PathContainer::const_iterator i;
	for(i = commandPool->path.begin();
	    i != commandPool->path.end();
	    ++i)
	{
		if(*i == questioner->getPCIID())
			break;
	}
	assure(i != commandPool->path.end(), "Questioner is not in path. This could mean that you have included a FU on the receiver side but not on the sender side.");

	// delegate up one level.
	if(i == commandPool->path.begin())
		return NULL;

	--i;

	return this->getCommandTypeSpecifier(*i);
} // getNext


CommandPool*
CommandProxy::createReply(
	const CommandPool* original,
	const CommandTypeSpecifierInterface* questioner) const
{
	const CommandTypeSpecifierInterface* next = this->getNext(original, questioner);

	if(!next)
		return this->createCommandPool();

	return next->createReply(original);
} // createReplyPCI


void
CommandProxy::calculateSizes(
	const CommandPool* commandPool,
	Bit& commandPoolSize, Bit& dataSize,
	const CommandTypeSpecifierInterface* questioner) const
{
	const CommandTypeSpecifierInterface* next = this->getNext(commandPool, questioner);

	if(next)
	{
		// Look at the next command in the chain, if its sizes are already commited we can stop
		// the recursion
		Command* nextCommand = commandPool->commands.at(next->getPCIID());
		if ( nextCommand->sizeCommited() )
		{
			// End recursion
			commandPoolSize = nextCommand->getCommandPoolSize();
			dataSize        = nextCommand->getPayloadSize();
			MESSAGE_BEGIN(VERBOSE, logger,m,"End of recursion, already commited sizes ");
			m << " - commandPoolSize: " << commandPoolSize << " dataSize: " << dataSize;
			MESSAGE_END();
			return;
		}
		else
		{
			// Continue recursion
			next->calculateSizes(commandPool, commandPoolSize, dataSize);
			MESSAGE_BEGIN(VERBOSE, logger,m,"Sizes after: ");
			m << dynamic_cast<const wns::ldk::FunctionalUnit*>(next)->getName()
			  << " - commandPoolSize: " << commandPoolSize << " dataSize: " << dataSize;
			MESSAGE_END();
			return;
		}
	}

	// We have reached the beginning of the activation path, so we start
	// calculating with an empty commandPool and the original SDU size for
	// the data.
	commandPoolSize = 0;
	if(commandPool->getSDU())
		dataSize = commandPool->getSDU()->getLengthInBits();
	else
		dataSize = 0;

	MESSAGE_BEGIN(VERBOSE, logger,m,"End of recursion, adding SDU size ");
	m << " - commandPoolSize: " << commandPoolSize << " dataSize: " << dataSize;
	MESSAGE_END();
} // calculateSizes


void
CommandProxy::commitSizes(
	CommandPool* commandPool,
	const CommandTypeSpecifierInterface* commiter) const
{
	const CommandTypeSpecifierInterface* next = this->getNext(commandPool, commiter);

	if(next)
	{
		Command* nextCommand = commandPool->commands.at(next->getPCIID());

		if ( nextCommand->sizeCommited() )
		{
			// End of recursion, commit the commiters command Sizes
			Command* command = this->commit(commandPool, commiter);

			MESSAGE_BEGIN(VERBOSE, logger,m,"End of recursion - beginning of non-commited Path (");
			m << dynamic_cast<const wns::ldk::FunctionalUnit*>(next)->getName() << ")"
			  << " - commandPoolSize: " << command->getCommandPoolSize()
			  << " dataSize: " << command->getPayloadSize();
			MESSAGE_END();

			return;

		}
		else
		{
			// first continue recursion
			next->commitSizes(commandPool);
			// then commit my own command
			Command* command = this->commit(commandPool, commiter);

			MESSAGE_BEGIN(VERBOSE, logger,m,"Sizes after: ");
			m << dynamic_cast<const wns::ldk::FunctionalUnit*>(commiter)->getName()
			  << " - commandPoolSize: " << command->getCommandPoolSize()
			  << " dataSize: " << command->getPayloadSize();
			MESSAGE_END();
			return;
		}
	}

	// We have reached the beginning of the activation path, so we can
	// commit the commiter's command size
	Command* command = this->commit(commandPool, commiter);

	MESSAGE_BEGIN(VERBOSE, logger,m,"End of recursion - beginning of activation Path (");
	m << dynamic_cast<const wns::ldk::FunctionalUnit*>(commiter)->getName() << ")"
	  << " - commandPoolSize: " << command->getCommandPoolSize()
	  << " dataSize: " << command->getPayloadSize();
	MESSAGE_END();
} // calculateSizes

void
CommandProxy::commitSizes(CommandPool* commandPool, const CommandIDType& id) const
{
    commitSizes(commandPool, getCommandTypeSpecifier(id));
}

wns::ldk::Command*
CommandProxy::commit(CommandPool* commandPool,
		     const CommandTypeSpecifierInterface* commiter) const
{
    assure(commandPool != NULL, "No valid commandPool given.");
    assure(commiter != NULL, "No commiter set. "
                             "Are you using the CommandProxy::commitSizes"
                             "directly?. You should use the "
                             "CommandTypeSpecifier::commitSizes.");

	Command* command = commiter->getCommand(commandPool);
	Bit commandPoolSize = 0;
	Bit dataSize = 0;
	commiter->calculateSizes(commandPool, commandPoolSize, dataSize);
	command->setCommandPoolSize(commandPoolSize);
	command->setPayloadSize(dataSize);
	command->commit(); // makes the command Read-only
	return command;
}

void
CommandProxy::cleanup(CommandPool* commandPool) const
{
	delete commandPool;
} // cleanup


void
CommandProxy::copy(CommandPool* dst, const CommandPool* src) const
{
	for(CommandPool::PathContainer::const_iterator it = src->path.begin();
	    it != src->path.end();
	    ++it)
	{
		CommandIDType id = *it;
		dst->insert(id, this->getCopyCommandRegistry().find(id)->copy(src->find(id)));
		dst->path.push_back(id);
	}
} // copy


void
CommandProxy::partialCopy(
	const CommandTypeSpecifierInterface* initiator,
	CommandPool* dst, const CommandPool* src) const
{
	CommandIDType initiatorID = initiator->getPCIID();

	CommandPool::PathContainer::const_iterator it = src->path.begin();
	for(;it != src->path.end();
	    ++it)
	{
		CommandIDType id = *it;

		if(id == initiatorID)
			break;

		dst->insert(id, this->getCopyCommandRegistry().find(id)->copy(src->find(id)));
		dst->path.push_back(id);
	}
	assure(it != src->path.end(), "partial copy failed, initiator was not found in activation path");
} // partialCopy


const CommandTypeSpecifierInterface*
CommandProxy::getCommandTypeSpecifier(CommandIDType id) const
{
    if (!this->commandTypeSpecifiers.at(id))
    {
        //std::stringstream ss;
        //ss << id;
        //throw wns::Exception("Invalid CommandTypeSpecifier instance with id="+ss.str()+" requested.");
        // @todo: find a better way to cope with the lack of information here.
        MESSAGE_SINGLE(NORMAL, logger, "WARNING: Invalid CommandTypeSpecifier instance with id="<<(int)id<<" requested.");
        return NULL;
    }
    return this->commandTypeSpecifiers.at(id);
} // getCommandTypeSpecifier


CommandProxy::CommandIDRegistry&
CommandProxy::getCommandIDRegistry()
{
	static CommandIDRegistry registry;
	return registry;
}

std::string
CommandProxy::dumpCommandIDRegistry()
{
	CommandIDRegistry& registry = getCommandIDRegistry();
	std::stringstream str;
	for(CommandIDRegistry::const_iterator itr = registry.begin();
	    itr != registry.end();
	    ++itr)
	{
		str << "Command ID: " << itr->second << ", Role name: " << itr->first << "\n";
	}
	return str.str();
}

CommandProxy::CopyCommandInterfaceRegistry&
CommandProxy::getCopyCommandRegistry()
{
	static CopyCommandInterfaceRegistry registry;
	return registry;
}

CommandProxy::CommandReaderRegistry&
CommandProxy::getCommandReaderRegistry()
{
	static CommandReaderRegistry registry;
	return registry;
}

void
CommandProxy::clearRegistries()
{
	CommandProxy::getCommandIDRegistry().clear();
	CommandProxy::getCopyCommandRegistry().clear();
	CommandProxy::getCommandReaderRegistry().clear();
}

#ifndef NDEBUG
size_t
CommandProxy::getCommandObjSize(const CommandIDType& id) const
{
    const CommandTypeSpecifierInterface* commandTypeSpecifier
        = this->getCommandTypeSpecifier(id);
    if (commandTypeSpecifier != NULL) {
        return this->getCommandTypeSpecifier(id)->getCommandObjSize();
    } else {
        return 0;
    }
}
#endif


