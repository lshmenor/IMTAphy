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

#include <WNS/probe/bus/ContextCollector.hpp>
#include <WNS/events/scheduler/Interface.hpp>

using namespace wns::probe::bus;

ContextCollector::ContextCollector(std::string probeBusId) :
    contextProviders_(ContextProviderCollection()),
    probeBus_(wns::simulator::getProbeBusRegistry()->getMeasurementSource(probeBusId))
{}

ContextCollector::ContextCollector(ContextProviderCollection contextProviders,
								   std::string probeBusId) :
	contextProviders_(contextProviders),
	probeBus_(wns::simulator::getProbeBusRegistry()->getMeasurementSource(probeBusId))
{}

bool
ContextCollector::hasObservers() const
{
    return probeBus_->hasObservers();
}

void
ContextCollector::put(const wns::osi::PDUPtr& compound, double value) const
{
    // early return if no one is listening
    if (!probeBus_->hasObservers())
    {
        return;
    }

	// Create vanilla Context object
	Context c;

	// Ask all ContextProviders to provide their input to the context
	contextProviders_.fillContext(c, compound);

	// determine simTime
	wns::simulator::Time t = wns::simulator::getEventScheduler()->getTime();
	probeBus_->forwardMeasurement(t, value, c);
}

void
ContextCollector::put(double value) const
{
    // early return if no one is listening
    if (!probeBus_->hasObservers())
    {
        return;
    }
    // Create vanilla Context object
    Context c;

    // Ask all ContextProviders to provide their input to the context
    contextProviders_.fillContext(c);

    // determine simTime
    wns::simulator::Time t = wns::simulator::getEventScheduler()->getTime();
    probeBus_->forwardMeasurement(t, value, c);
}

