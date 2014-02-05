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

#include <WNS/probe/bus/ProbeBus.hpp>
#include <WNS/probe/bus/ProbeBusRegistry.hpp>
#include <WNS/probe/bus/detail/SubjectPimpl.hpp>
#include <WNS/probe/bus/detail/ObserverPimpl.hpp>
#include <WNS/simulator/ISimulator.hpp>

#include <iostream>

using namespace wns::probe::bus;

ProbeBus::ProbeBus():
    subject_( new detail::SubjectPimpl() ),
    observer_( new detail::ObserverPimpl(this) )
{
}

ProbeBus::~ProbeBus()
{
    assure(subject_ != NULL, "This ProbeBus instance has no implementation of the subject detail");
    delete subject_;
    assure(observer_ != NULL, "This ProbeBus instance has no implementation of the observer detail");
    delete observer_;
}

void
ProbeBus::forwardMeasurement(const wns::simulator::Time& timestamp,
                             const double& aValue,
                             const IContext& theRegistry)
{
    if (this->accepts(timestamp, theRegistry))
    {
        this->onMeasurement(timestamp, aValue, theRegistry);

        assure(subject_ != NULL, "This ProbeBus instance has no implementation of the subject detail");
        subject_->forwardMeasurement(timestamp, aValue, theRegistry);
    }
}

void
ProbeBus::forwardOutput()
{
    this->output();

    assure(subject_ != NULL, "This ProbeBus instance has no implementation of the subject detail");
    subject_->forwardOutput();
}

void
ProbeBus::startObserving(ProbeBus* other)
{
    assure(observer_ != NULL, "This ProbeBus instance has no implementation of the observer detail");
    observer_->startObserving( other->subject_ );
}

void
ProbeBus::stopObserving(ProbeBus* other)
{
    assure(observer_ != NULL, "This ProbeBus instance has no implementation of the observer detail");
    observer_->stopObserving( other->subject_ );
}

bool
ProbeBus::hasObservers() const
{
    return subject_->hasObservers();
}
