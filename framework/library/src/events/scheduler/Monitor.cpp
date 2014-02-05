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

#include <WNS/events/scheduler/Monitor.hpp>

using namespace wns::events::scheduler;

Monitor::Monitor(const wns::pyconfig::View& configuration) :
    Observer<INotification>(),
    logger_(configuration.get("logger")),
    processedEvents_(0),
    canceledEvents_(0),
    scheduledNowEvents_(0),
    scheduledEvents_(0),
    scheduledDelayEvents_(0)
{
}

void
Monitor::logStatistics()
{
    MESSAGE_BEGIN(QUIET, logger_, m, "\nStatistics for event scheduler:\n");
    long long int totalAddedEvents = scheduledNowEvents_ + scheduledEvents_ + scheduledDelayEvents_;
    m << "--------------------------------------------------------------------------\n"
      << "processed events:  " << processedEvents_ << "\n"
      << "added events:      " << totalAddedEvents << "\n"
      << "  - delay:         " << scheduledDelayEvents_ << "\n"
      << "  - now:           " << scheduledNowEvents_ << "\n"
      << "  - absolute time: " << scheduledEvents_ << "\n"
      << "canceled events:   " << canceledEvents_ << "\n";
    MESSAGE_END();

}

void
Monitor::doOnProcessOneEvent()
{
    MESSAGE_SINGLE(VERBOSE, logger_, "Processing next event");
    ++processedEvents_;
}

void
Monitor::doOnCancelEvent()
{
    MESSAGE_SINGLE(VERBOSE, logger_, "Canceling event");
    ++canceledEvents_;
}

void
Monitor::doOnSchedule()
{
    MESSAGE_SINGLE(VERBOSE, logger_, "Scheduling an event for an absolute point in time");
    ++scheduledEvents_;
}

void
Monitor::doOnScheduleDelay()
{
    MESSAGE_SINGLE(VERBOSE, logger_, "Scheduling an event with delay");
    ++scheduledDelayEvents_;
}

void
Monitor::doOnScheduleNow()
{
    MESSAGE_SINGLE(VERBOSE, logger_, "Scheduling an event for 'now'");
    ++scheduledNowEvents_;
}

void
Monitor::doOnAddEvent()
{
}
