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

#include <WNS/events/PeriodicTimeout.hpp>

using namespace wns::events;

PeriodicTimeout::PeriodicTimeoutFunctor::PeriodicTimeoutFunctor(PeriodicTimeout* _dest, wns::simulator::Time _period) :
    period_(_period),
    dest_(_dest)
{
}


void
PeriodicTimeout::PeriodicTimeoutFunctor::operator()()
{
    this->dest_->periodicEv_ =
        wns::simulator::getEventScheduler()->scheduleDelay(*this, this->period_);
    this->dest_->periodically();
}

simTimeType
PeriodicTimeout::PeriodicTimeoutFunctor::getPeriod() const
{
    return this->period_;
}

void
PeriodicTimeout::PeriodicTimeoutFunctor::print(std::ostream& aStreamRef) const
{
    aStreamRef << "<" << TypeInfo::create(*this) <<" instance at "
           << static_cast<const void* const>(this) << ">";

    aStreamRef << ", target: " << wns::TypeInfo::create(*this->dest_);
}


PeriodicTimeout::PeriodicTimeout() :
    period_(-1),
    periodicEv_(scheduler::IEventPtr())
{
}


PeriodicTimeout::~PeriodicTimeout()
{
    if(this->hasPeriodicTimeoutSet())
    {
        this->cancelPeriodicTimeout();
    }
}


PeriodicTimeout::PeriodicTimeout(const PeriodicTimeout& other) :
    period_(other.period_),
    periodicEv_(scheduler::IEventPtr())
{
    if(other.hasPeriodicTimeoutSet())
    {
        this->startPeriodicTimeout(other.period_,
                       other.periodicEv_->getScheduled() - wns::simulator::getEventScheduler()->getTime());

        assure(this->periodicEv_ != other.periodicEv_,
               "PeriodicTimeout(const PeriodicTimeout& other): "
               "Events of source and destination "
               "must not be the same");
    }
}


void
PeriodicTimeout::startPeriodicTimeout(wns::simulator::Time _period, wns::simulator::Time delay)
{
    assure(_period > 0,
           "The peroid must be >0, otherwise PeriodicTimeout will get stuck in an endless event loop!");

    if (this->hasPeriodicTimeoutSet())
    {
        this->cancelPeriodicTimeout();
    }

    this->period_ = _period;

    this->periodicEv_ = wns::simulator::getEventScheduler()->
        scheduleDelay(PeriodicTimeoutFunctor(this, this->period_), delay);
}


bool
PeriodicTimeout::hasPeriodicTimeoutSet() const
{
    return this->periodicEv_ != NULL;
}


void
PeriodicTimeout::cancelPeriodicTimeout()
{
    if (!hasPeriodicTimeoutSet())
    {
        return;
    }
    wns::simulator::getEventScheduler()->cancelEvent(this->periodicEv_);
    this->periodicEv_ = scheduler::IEventPtr();

    // invalidate period
    this->period_ = -1;
}

