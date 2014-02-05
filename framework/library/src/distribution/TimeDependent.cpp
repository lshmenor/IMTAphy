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

#include <WNS/distribution/TimeDependent.hpp>


using namespace wns::distribution;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    TimeDependent,
    Distribution,
    "wns.distribution.TimeDependent",
    wns::PyConfigViewCreator);
STATIC_FACTORY_REGISTER_WITH_CREATOR(
    TimeDependent,
    Distribution,
    "wns.distribution.TimeDependent",
    wns::distribution::RNGConfigCreator);


TimeDependent::DistributionEvent::DistributionEvent(
    TimeDependent* td,
    const wns::pyconfig::View& view) :
    target_(td),
    config_(view)
{
    assure(this->target_, "No valid target set (NULL)");
}

TimeDependent::DistributionEvent::~DistributionEvent()
{
    this->target_ = NULL;
}

void
TimeDependent::DistributionEvent::operator()()
{
    assure(this->target_, "No target set");
    this->target_->setDistribution(config_);
    this->target_->queueNextDistribution();
}

TimeDependent::TimeDependent(const wns::pyconfig::View& view) :
    Distribution(),
    distribution_(NULL),
    events_(),
    config_(view)
{
    init();
}

TimeDependent::TimeDependent(wns::rng::RNGen* rng, const wns::pyconfig::View& view) :
    Distribution(rng),
    distribution_(NULL),
    events_(),
    config_(view)
{
    init();
}

void
TimeDependent::init()
{
    for(int ii = 0; ii < config_.len("eventList"); ++ii)
    {
        wns::pyconfig::View eventView = config_.get("eventList", ii);
        DistributionEvent de (this, eventView.get("distribution"));
        wns::simulator::Time activationTime =
            eventView.get<wns::simulator::Time>("activationTime");
        assure(this->events_.find(activationTime) == this->events_.end(),
               "Event for this time is already scheduled. Not possible.");
        this->events_.insert(std::make_pair(activationTime, de));
    }

    // if the first Event is scheduled for 0.0, install distribution right
    // away
    if(this->events_.begin()->first == 0.0)
    {
        DistributionEvent ev = this->events_.begin()->second;
        this->events_.erase(this->events_.begin());
        (ev)();
    }

    else
    {
        this->queueNextDistribution();
    }
}

TimeDependent::~TimeDependent()
{
    this->removeDistribution();
}

double
TimeDependent::operator()()
{
    assure(this->distribution_, "No distribution set!");
    return (*distribution_)();
}

std::string
TimeDependent::paramString() const
{
    std::ostringstream tmp;
    tmp << "TimeDependent()";
    return tmp.str();
}

void
TimeDependent::queueNextDistribution()
{
    if(!this->events_.empty())
    {
        EventContainer::iterator itr = this->events_.begin();
        wns::simulator::getEventScheduler()->schedule(itr->second, itr->first);
        this->events_.erase(itr);
    }
}

void
TimeDependent::setDistribution(const wns::pyconfig::View& distConfig)
{
    this->removeDistribution();
    wns::distribution::RNGDistributionCreator* dc =
    wns::distribution::RNGDistributionFactory::creator(distConfig.get<std::string>("__plugin__"));
    this->distribution_ = dc->create(getRNG(), distConfig);
}

void
TimeDependent::removeDistribution()
{
    if(this->distribution_)
    {
        delete this->distribution_;
        this->distribution_ = NULL;
    }
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
