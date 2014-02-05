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

using namespace wns::simulator;


ISimulator::~ISimulator()
{
}


wns::events::scheduler::Interface*
ISimulator::getEventScheduler() const
{
    wns::events::scheduler::Interface* es = this->doGetEventScheduler();
    assure(es != NULL, "No EventScheduler available");
    return es;
}

wns::logger::Master*
ISimulator::getMasterLogger() const
{
    wns::logger::Master* ml = this->doGetMasterLogger();
    assure(ml != NULL, "No MasterLogger available");
    return ml;
}

wns::rng::RNGen*
ISimulator::getRNG() const
{
    wns::rng::RNGen* rng = this->doGetRNG();
    assure(rng != NULL, "No Random Number Generator available");
    return rng;
}

Registry*
ISimulator::getRegistry() const
{
    Registry* ur = this->doGetRegistry();
    assure(ur != NULL, "No Registry available");
    return ur;
}

wns::probe::bus::ProbeBusRegistry*
ISimulator::getProbeBusRegistry() const
{
    wns::probe::bus::ProbeBusRegistry* pbr = this->doGetProbeBusRegistry();
    assure(pbr != NULL, "No ProbeBusRegistry available");
    return pbr;
}

ResetSignal*
ISimulator::getResetSignal() const
{
    ResetSignal* rs = this->doGetResetSignal();
    assure(rs != NULL, "No ResetSignal available");
    return rs;
}

ShutdownSignal*
ISimulator::getShutdownSignal() const
{
    ShutdownSignal* sig = this->doGetShutdownSignal();
    assure(sig != NULL, "No ShutdownSignal available");
    return sig;
}

wns::pyconfig::View
ISimulator::getConfiguration() const
{
    return this->doGetConfiguration();
}

void
ISimulator::reset()
{
    this->doReset();
}

Singleton::Singleton() :
    simulator_(NULL)
{
}

Singleton::~Singleton()
{
}

ISimulator*
Singleton::getInstance()
{
    assure(simulator_.get() != NULL, "No Simulator instance available");
    return simulator_.get();
}

void
Singleton::setInstance(ISimulator* simulator)
{
    if(simulator != NULL)
    {
        assure(simulator_.get() == NULL, "cannot set simulator, already set");
    }

    simulator_.reset(simulator);
}

void
Singleton::shutdownInstance()
{
    assure(simulator_.get() != NULL, "Nothing to shutdown, no Simulator instance available");

    // if someone is trying to access the Simulator on shutdown of the simulator
    // itself, it is already NULL and will thus cause an exception.
    ISimulator* tmp = simulator_.release();

    delete tmp;
}


wns::simulator::Singleton&
wns::simulator::getSingleton()
{
    static Singleton instance;
    return instance;
}


wns::simulator::ISimulator*
wns::simulator::getInstance()
{
    return getSingleton().getInstance();
}


wns::events::scheduler::Interface*
wns::simulator::getEventScheduler()
{
    return wns::simulator::getInstance()->getEventScheduler();
}

wns::logger::Master*
wns::simulator::getMasterLogger()
{
    return wns::simulator::getInstance()->getMasterLogger();
}

wns::rng::RNGen*
wns::simulator::getRNG()
{
    return wns::simulator::getInstance()->getRNG();
}

wns::simulator::Registry*
wns::simulator::getRegistry()
{
    return wns::simulator::getInstance()->getRegistry();
}

wns::probe::bus::ProbeBusRegistry*
wns::simulator::getProbeBusRegistry()
{
    return wns::simulator::getInstance()->getProbeBusRegistry();
}

wns::simulator::ResetSignal*
wns::simulator::getResetSignal()
{
    return wns::simulator::getInstance()->getResetSignal();
}

wns::simulator::ShutdownSignal*
wns::simulator::getShutdownSignal()
{
    return wns::simulator::getInstance()->getShutdownSignal();
}

wns::pyconfig::View
wns::simulator::getConfiguration()
{
    return wns::simulator::getInstance()->getConfiguration();
}
