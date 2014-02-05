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
#include <WNS/events/scheduler/NullCommand.hpp>

#include <WNS/events/PeriodicRealTimeout.hpp>
#include <WNS/TypeInfo.hpp>

using namespace wns::events;

PeriodicRealTimeout::PeriodicRealTimeout() :
    mutex(),
    period(0),
    delay(0),
    running(false),
    thread(),
    currentCommand(scheduler::makeNullCommand())
{
    pthread_mutex_init(&this->mutex, 0);
    pthread_cond_init(&this->dataRead, 0);
} // PeriodicRealTimeout


PeriodicRealTimeout::~PeriodicRealTimeout()
{
    this->cancelPeriodicRealTimeout();
    pthread_cond_destroy(&this->dataRead);
    pthread_mutex_destroy(&this->mutex);
} // ~PeriodicRealTimeout


void
PeriodicRealTimeout::startPeriodicTimeout(double _period, double _delay)
{
    this->cancelPeriodicRealTimeout();

    pthread_mutex_lock(&this->mutex);
    {
        this->period = _period;
        this->delay = _delay;

        pthread_create(&this->thread, 0, PeriodicRealTimeout::worker, this);
        pthread_cond_wait( &this->dataRead, &this->mutex );
    }
    pthread_mutex_unlock(&this->mutex);
} // setPeriodicRealTimeout


bool
PeriodicRealTimeout::hasPeriodicRealTimeoutSet()
{
    return this->running;	// this is a funky method anyway...
} // hasPeriodicRealTimeoutSet


void
PeriodicRealTimeout::cancelPeriodicRealTimeout()
{
    pthread_mutex_lock(&this->mutex);
    if (this->running)
    {
        if (this->currentCommand->isQueued())
        {
            wns::simulator::getEventScheduler()->dequeueCommand(this->currentCommand);
        }
        pthread_cancel(this->thread);
        pthread_join(this->thread, 0);
        this->running = false;
        this->currentCommand = scheduler::makeNullCommand();
    }
    pthread_mutex_unlock(&this->mutex);
} // cancelPeriodicRealTimeout


void*
PeriodicRealTimeout::worker(void* _arg)
{
    PeriodicRealTimeout* it = (PeriodicRealTimeout*) _arg;

    pthread_mutex_lock(&it->mutex);
    it->running = true;
    double delay;
    double period;
    delay = it->delay;
    period = it->period;
    pthread_mutex_unlock(&it->mutex);
    pthread_cond_signal(&it->dataRead);

    {
        timespec t;
        t.tv_sec = static_cast<time_t>(delay);
        t.tv_nsec = static_cast<long>((delay-t.tv_sec)*1E9);
        nanosleep(&t, NULL);
    }

    while(true)
    {

        int oldState;
        pthread_testcancel();
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &oldState);
        pthread_mutex_lock(&it->mutex);
        {
            if(it->currentCommand->isFinished() || it->currentCommand->isNotSubmitted())
            {
                it->currentCommand =
                    wns::simulator::getEventScheduler()->queueCommand(PeriodicRealTimeoutCommand(it));
            }
            else
            {
                // old event has not yet been executed, wait
                // until next iteration
            }
        }
        pthread_mutex_unlock(&it->mutex);
        pthread_setcancelstate(oldState, 0);
        pthread_testcancel();

        {
            timespec t;
            t.tv_sec = static_cast<time_t>(period);
            t.tv_nsec = static_cast<long>((period-t.tv_sec)*1E9);
            nanosleep(&t, NULL);
        }
    }

    return 0;
} // worker


