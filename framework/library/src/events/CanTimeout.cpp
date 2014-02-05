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


#include <WNS/simulator/ISimulator.hpp>
#include <WNS/events/scheduler/Interface.hpp>

#include <WNS/events/CanTimeout.hpp>
#include <WNS/Assure.hpp>

using namespace wns::events;

CanTimeout::CanTimeout() :
    event(),
    scheduler(wns::simulator::getEventScheduler())
{
}


CanTimeout::~CanTimeout()
{
    if (this->hasTimeoutSet())
    {
        this->cancelTimeout();
    }
}


void
CanTimeout::setTimeout(double delay)
{
    assure(!this->hasTimeoutSet(), "A timer has been set already.");

    TimeoutEvent toEvent(this);
    this->event =
        this->scheduler->scheduleDelay(toEvent, delay);
}


void
CanTimeout::setNewTimeout(double delay)
{
    if (this->hasTimeoutSet())
    {
        this->cancelTimeout();
    }

    this->setTimeout(delay);
}


bool
CanTimeout::hasTimeoutSet() const
{
    // will be automatically converted bool
    return this->event;
}


void
CanTimeout::cancelTimeout()
{
    assure(this->hasTimeoutSet(), "No timer has been set.");

    this->scheduler->cancelEvent(this->event);
    this->event = scheduler::IEventPtr();
}


CanTimeout::TimeoutEvent::TimeoutEvent(CanTimeout* _target) :
    target(_target)
{
    assure(this->target, "target not valid (NULL)");
}


CanTimeout::TimeoutEvent::~TimeoutEvent()
{
    target = NULL;
}

void
CanTimeout::TimeoutEvent::operator()()
{
    assure(this->target, "target not valid (NULL)");

    this->target->event = scheduler::IEventPtr();
    this->target->onTimeout();
}

void
CanTimeout::TimeoutEvent::print(std::ostream& aStreamRef) const
{
    aStreamRef << "<" << TypeInfo::create(*this) <<" instance at "
           << static_cast<const void* const>(this) << ">";

    aStreamRef << ", target: " << wns::TypeInfo::create(*this->target);
}

/*
  Local Variables:
  mode: c++
  fill-column: 80
  c-basic-offset: 8
  c-comment-only-line-offset: 0
  c-tab-always-indent: t
  indent-tabs-mode: t
  tab-width: 8
  End:
*/
