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

#include <WNS/events/scheduler/RealTime.hpp>
#include <sys/time.h>

using namespace wns::events::scheduler;

STATIC_FACTORY_REGISTER(
    RealTime,
    Interface,
    "wns.events.scheduler.RealTime");

RealTime::RealTime() :
    Map(),
    timeOfSchedulerStart_(),
    inSync_(true)
{
}

RealTime::~RealTime()
{
}

void
RealTime::onNewSimTime(const wns::simulator::Time& nextTime)
{
    timeval currentTime;

    double d_delay = nextTime;
    d_delay += RealTime::timevalToDouble(&timeOfSchedulerStart_);

    gettimeofday(&currentTime, NULL);
    d_delay -= RealTime::timevalToDouble(&currentTime);


    // wait
    if (d_delay > 0)
    {
        // we are in sync again!
        if (!inSync_)
        {
            std::cout << "RealTime scheduler is in sync again.\n";
            inSync_ = true;
        }

        // compute from double to timeval (needed for select statement)
        timespec delay;
        delay.tv_sec = static_cast<time_t>(d_delay);
        delay.tv_nsec = static_cast<long>((d_delay-delay.tv_sec)*1E9);

        // this causes the scheduler to wait
        /* from "man nanosleep"

        The current implementation of nanosleep() is
        based on the normal kernel timer mechanism,
        which has a resolution of 1/HZ s (see time(7)).
        Therefore, nanosleep() pauses always for at
        least the specified time, however it can take up
        to 10 ms longer than specified until the process
        becomes runnable again. For the same reason, the
        value returned in case of a delivered signal in
        *rem is usually rounded to the next larger
        multiple of 1/HZ s.
        */
        nanosleep(&delay, NULL);
    }
    // lagging
    else
    {
        // if lagging more than 10 ms show a warning
        if(d_delay < -0.010 && inSync_)
        {
            std::cout << "Warning: RealTime scheduler lagging more than 10 ms.\n";
            inSync_ = false;
        }
    }
}

void
RealTime::doReset()
{
    inSync_ = true;
    Map::doReset();
}

void
RealTime::doStart()
{
    gettimeofday(&timeOfSchedulerStart_, NULL);
    Map::doStart();
}


double
RealTime::timevalToDouble(const timeval* t)
{
    return static_cast<double>(t->tv_sec) + static_cast<double>(t->tv_usec)/1E6;
}
