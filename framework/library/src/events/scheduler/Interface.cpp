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

#include <WNS/events/scheduler/Interface.hpp>
#include <WNS/simulator/ISimulator.hpp>
#include <boost/bind.hpp>

using namespace wns::events::scheduler;


Interface::Interface()
{
}

Interface::~Interface()
{
}

wns::simulator::Time
Interface::getTime() const
{
    return this->doGetTime();
}

void
Interface::dequeueCommand(const ICommandPtr& command)
{
    assert(command && "Command invalid (NULL)");
    return this->doDequeueCommand(command);
}

void
Interface::cancelEvent(const IEventPtr& event)
{
    assureNotNull(event);
    this->sendCancelEventNotification();
    this->doCancelEvent(event);
}

void
Interface::reset()
{
    return this->doReset();
}

bool
Interface::processOneEvent()
{
    this->sendProcessOneEventNotification();
    return this->doProcessOneEvent();
}

void
Interface::start()
{
    return this->doStart();
}

void
Interface::stop()
{
    return this->doStop();
}

void
Interface::stopAt(const wns::simulator::Time& time)
{
    return this->doStopAt(time);
}

void
Interface::doStopAt(const wns::simulator::Time& time)
{
    schedule(boost::bind(&wns::events::scheduler::Interface::stop, this), time);
}

size_t
Interface::size() const
{
    return this->doSize();
}

