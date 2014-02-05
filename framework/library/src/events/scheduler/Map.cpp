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

#include <WNS/events/scheduler/Map.hpp>

using namespace wns::events::scheduler;

STATIC_FACTORY_REGISTER(
    Map,
    Interface,
    "wns.events.scheduler.Map");


Map::Map() :
    Interface(),
    Subject<INotification>(),
    simTime_(0.0),
    events_(),
    nowItr_(),
    stop_(false),
    commandQueue_()
{
    events_[0] = new DiscreteTimeContainer();
    nowItr_ = events_.begin();
}

Map::~Map()
{
    while (!events_.empty())
    {
        delete events_.begin()->second;
        events_.erase(events_.begin());
    }
}

wns::events::scheduler::IEventPtr
Map::doScheduleNow(const Callable& callable)
{
    EventPtr event (new wns::events::scheduler::Map::Event(callable));
    event->scheduler_ = this;
    event->issued_ = getTime();
    event->scheduled_ = getTime();
    nowItr_->second->push_back(event);
    event->state_ = Event::Queued;
    return event;
}

wns::events::scheduler::IEventPtr
Map::doSchedule(const Callable& callable, wns::simulator::Time at)
{
    EventPtr event (new wns::events::scheduler::Map::Event(callable));
    event->scheduler_ = this;
    event->issued_ = getTime();
    event->scheduled_ = at;
    EventContainer::iterator itr = events_.find(event->getScheduled());
    if (itr == events_.end())
    {
        DiscreteTimeContainer* dtc = new DiscreteTimeContainer();
        // we need to add a new list
        dtc->push_back(event);
        events_[at] = dtc;
    }
    else
    {
        itr->second->push_back(event);
    }
    event->state_ = Event::Queued;
    return event;
}

void
Map::doCancelMapEventCalledFromMapEvent(const EventPtr& event)
{
    // we need to notify all obsevers. this is normally done in the Interface,
    // but this is called directly from the event ...
    sendNotifies(&INotification::onCancelEvent);
    doCancelMapEvent(event);
}

void
Map::doCancelMapEvent(const EventPtr& event)
{
    if (event->isRunning())
    {
        throw IEvent::CancelException("Event is currently being executed");
    }
    else if (event->isCanceled())
    {
        throw IEvent::CancelException("Event is already canceled");
    }
    else if (event->isFinished())
    {
        throw IEvent::CancelException("Event has already been called");
    }
    else if (event->isNotSubmitted())
    {
        throw IEvent::CancelException("Should never happen");
    }

    // this removes the element from the list in the scheduler
    event->removeFromAllLists();
    event->state_ = Event::Canceled;
}

void
Map::doCancelEvent(const IEventPtr& event)
{
    EventPtr mapEvent (dynamicCast<Event>(event));
    assureNotNull(mapEvent);
    doCancelMapEvent(mapEvent);
}

bool
Map::doProcessOneEvent()
{
    commandQueue_.runCommands();

    // search next event
    while (nowItr_->second->empty())
    {
        if (events_.size() == 1)
        {
            // No more events left!
            return false;
        }
        else
        {
            delete events_.begin()->second;
            events_.erase(events_.begin());
        }
        nowItr_ = events_.begin();
    }

    assure(!nowItr_->second->empty(), "There MUST be events!");

    // get next event
    EventPtr nextEvent = nowItr_->second->front();

    wns::simulator::Time newTime = nextEvent->getScheduled();

    // run until all now events are processed
    if (simTime_ < newTime)
    {
        if(stop_)
        {
            return false;
        }
        onNewSimTime(newTime);
    }

    simTime_ = newTime;

    nextEvent->state_ = Event::Running;
    (*nextEvent)();
    nextEvent->state_ = Event::Finished;

    // remove the event
    nowItr_->second->pop_front();

    return true;
}

void
Map::doReset()
{
    while (!events_.empty())
    {
        delete events_.begin()->second;
        events_.erase(events_.begin());
    }
    events_[0] = new DiscreteTimeContainer();
    nowItr_ = events_.begin();
    simTime_ = 0.0;
    commandQueue_.reset();
}

size_t
Map::doSize() const
{
    size_t size = 0;

    EventContainer::const_iterator itrEnd = events_.end();
    for (EventContainer::const_iterator itr = events_.begin();
         itr != itrEnd;
         ++itr)
    {
        size += itr->second->size();
    }
    return size;
}

void
Map::sendProcessOneEventNotification()
{
    sendNotifies(&INotification::onProcessOneEvent);
}

void
Map::sendCancelEventNotification()
{
    sendNotifies(&INotification::onCancelEvent);
}

void
Map::sendScheduleNotification()
{
    sendNotifies(&INotification::onSchedule);
}

void
Map::sendScheduleNowNotification()
{
    sendNotifies(&INotification::onScheduleNow);
}

void
Map::sendScheduleDelayNotification()
{
    sendNotifies(&INotification::onScheduleDelay);
}

void
Map::sendAddEventNotification()
{
    sendNotifies(&INotification::onAddEvent);
}

wns::simulator::Time
Map::doGetTime() const
{
    return simTime_;
}

void
Map::doStart()
{
    while(processOneEvent());
}

void
Map::doStop()
{
    stop_ = true;
}

wns::events::scheduler::ICommandPtr
Map::doQueueCommand(const Callable& callable)
{
    return commandQueue_.queueCommand(callable);
}

void
Map::doDequeueCommand(const ICommandPtr& command)
{
    return commandQueue_.dequeueCommand(command);
}
