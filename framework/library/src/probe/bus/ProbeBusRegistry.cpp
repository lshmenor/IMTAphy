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

#include <WNS/probe/bus/ProbeBusRegistry.hpp>
#include <WNS/probe/bus/PassThroughProbeBus.hpp>

using namespace wns::probe::bus;

ProbeBusRegistry::ProbeBusRegistry(const wns::pyconfig::View& pyco):
    pyco_(pyco),
    registry_(),
    logger_(pyco.get("logger"))
{
}

ProbeBusRegistry::ProbeBusRegistry(const wns::pyconfig::View& pyco, wns::logger::Master* ml):
    pyco_(pyco),
    registry_(),
    logger_(pyco.get("logger"), ml)
{
}

ProbeBusRegistry::~ProbeBusRegistry()
{
    reset();
}

void
ProbeBusRegistry::reset()
{
    // Delete all busses
    for (CreatedProbeBussesContainer::iterator it = createdProbeBusses_.begin();
         it != createdProbeBusses_.end();
         ++it)
    {
        delete *it;
    }
    registry_ = ProbeBusRegistryContainer();
    createdProbeBusses_ = CreatedProbeBussesContainer();
}

void
ProbeBusRegistry::startup()
{
    this->spawnProbeBusses(pyco_);

    if (registry_.size() > 0)
    {
        MESSAGE_BEGIN(NORMAL, logger_, m, "");
        m << registry_;
        MESSAGE_END();
    }
}

void
ProbeBusRegistry::spawnProbeBusses(const wns::pyconfig::View& config)
{
    for(int ii=0 ; ii < config.len("measurementSources.keys()"); ++ii)
    {
        wns::pyconfig::View subpyco = config.get("measurementSources.values()",ii);

        std::string probeBusID = config.get<std::string>("measurementSources.keys()",ii);

        ProbeBus* pb = registry_.find(probeBusID);

        this->spawnObservers(pb, subpyco);
    }
}

void
ProbeBusRegistry::spawnObservers(ProbeBus* subject, const wns::pyconfig::View& config)
{
    for(int ii=0 ; ii < config.len("observers"); ++ii)
    {
        wns::pyconfig::View subpyco = config.get("observers", ii);

        std::string nameInFactory = subpyco.get<std::string>("nameInFactory");

        wns::probe::bus::ProbeBusCreator* c =
            wns::probe::bus::ProbeBusFactory::creator(nameInFactory);

        wns::probe::bus::ProbeBus* pb = c->create(subpyco);

        createdProbeBusses_.push_back(pb);

        pb->startObserving(subject);

        this->spawnObservers(pb, subpyco);
    }  
}

ProbeBus*
ProbeBusRegistry::getMeasurementSource(const std::string& probeBusID)
{
    if (!registry_.knows(probeBusID))
    {
        registry_.insert(probeBusID, new PassThroughProbeBus());
    }
    return registry_.find(probeBusID);
}

void
ProbeBusRegistry::forwardOutput()
{
    for (ProbeBusRegistryContainer::const_iterator it = registry_.begin();
         it != registry_.end();
         ++it)
    {
        it->second->forwardOutput();
    }
}

std::string
ProbeBusRegistry::doToString() const
{
    return registry_.toString();
}
