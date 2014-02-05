/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 16, D-52074 Aachen, Germany
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

#include <WNS/events/scheduler/CommandQueue.hpp>

using namespace wns::events::scheduler;

CommandQueue::CommandQueue() :
    queue_(),
    mutex_()
{
    pthread_mutex_init(&mutex_, 0);
}

CommandQueue::~CommandQueue()
{
}

void
CommandQueue::runCommands()
{
    pthread_mutex_lock(&mutex_);
    try
    {
        for(CommandContainer::iterator itr = queue_.begin();
            itr != queue_.end();
            ++itr)
        {
            (**itr).state_ = Command::Running;
            (**itr)();
            (**itr).state_ = Command::Finished;
        }
        queue_.clear();
    }
    catch(...)
    {
        pthread_mutex_unlock(&mutex_);
        throw;
    }
    pthread_mutex_unlock(&mutex_);
}

void
CommandQueue::reset()
{
    // clear Command queue
    pthread_mutex_lock(&mutex_);
    queue_.clear();
    pthread_mutex_unlock(&mutex_);
}

wns::events::scheduler::ICommandPtr
CommandQueue::queueCommand(const Callable& callee)
{
    pthread_mutex_lock(&mutex_);
    CommandPtr command(new Command(callee));
    try
    {
        queue_.push_back(command);
        command->state_ = Command::Queued;
    }
    catch(...)
    {
        pthread_mutex_unlock(&mutex_);
        throw;
    }
    pthread_mutex_unlock(&mutex_);
    return command;
}

void
CommandQueue::dequeueCommand(const ICommandPtr& command)
{
    pthread_mutex_lock(&mutex_);
    CommandPtr commandPtr = dynamicCast<Command>(command);
    if (command->isRunning())
    {
        throw ICommand::CancelException("Command is currently being executed");
    }
    else if (command->isCanceled())
    {
        throw ICommand::CancelException("Command is already canceled");
    }
    else if (command->isFinished())
    {
        throw ICommand::CancelException("Command has already been called");
    }
    else if (command->isNotSubmitted())
    {
        throw ICommand::CancelException("Should never happen");
    }
    try
    {
        queue_.remove(commandPtr);
        commandPtr->state_ = Command::Canceled;
    }
    catch(...)
    {
        pthread_mutex_unlock(&mutex_);
        throw;
    }
    pthread_mutex_unlock(&mutex_);
}
