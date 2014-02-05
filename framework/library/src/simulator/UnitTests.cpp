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

#include <WNS/simulator/UnitTests.hpp>
#include <WNS/rng/RNGen.hpp>
#include <WNS/events/scheduler/Interface.hpp>
#include <WNS/probe/bus/ProbeBusRegistry.hpp>
#include <ios>

using namespace wns::simulator;

UnitTests::UnitTests(const wns::pyconfig::View& configuration) :
    Simulator(configuration),
    initialRNGState_()
{
}

UnitTests::~UnitTests()
{
}

void
UnitTests::doReset()
{
    // Another implementation may also decide to delete and rebuild its members
    // from scratch, rather than resetting them (since reset is error prone,
    // needs to be implemented and tested thoroughly to not carry any old state
    // in itself.
    getEventScheduler()->reset();
    // seek to the beginning of the stream
    initialRNGState_.seekg (0, std::ios::beg);
    initialRNGState_ >> *getRNG();
    getProbeBusRegistry()->reset();
    (*getResetSignal())();
}

void
UnitTests::configureRNG(const wns::pyconfig::View& config)
{
    Simulator::configureRNG(config);
    initialRNGState_ << *getRNG();
}
