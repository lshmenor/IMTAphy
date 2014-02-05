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

#include <WNS/simulator/SignalHandler.hpp>

#include <iostream>

using namespace wns::simulator;

SignalHandler::SignalHandler() :
    map_()
{
}

SignalHandler::~SignalHandler()
{
    removeAllSignalHandlers();
}

void
SignalHandler::removeSignalHandler(int signum)
{
    // block all signals until we have removed the handler
    sigset_t allSignals;
    sigfillset(&allSignals);
    sigprocmask(SIG_BLOCK, &allSignals, NULL);
    if (!map_.knows(signum))
    {
        // should never happen, otherwise the implementation is broken
        std::cerr << "openWNS: Tried to removed signal handler for signal " << signum <<"!!!\n";
        std::cerr << "         But no handler was registered.";
    }
    else
    {
        map_.erase(signum);
        // restore default signal handler
        struct sigaction action;
        sigfillset (&action.sa_mask);
        action.sa_flags = 0;
        action.sa_handler = SIG_DFL;
        sigaction(signum, &action, NULL);
    }
    sigprocmask(SIG_UNBLOCK, &allSignals, NULL);
}

void
SignalHandler::removeAllSignalHandlers()
{
    // block all signals until we have removed all handlers
    sigset_t allSignals;
    sigfillset(&allSignals);
    sigprocmask(SIG_BLOCK, &allSignals, NULL);
    while(!map_.empty())
    {
        Map::const_iterator itr = map_.begin();
        // restore default signal handler
        struct sigaction action;
        sigfillset (&action.sa_mask);
        action.sa_flags = 0;
        action.sa_handler = SIG_DFL;
        sigaction(itr->first, &action, NULL);
        map_.erase(itr->first);
    }
    sigprocmask(SIG_UNBLOCK, &allSignals, NULL);
}

void
SignalHandler::catchSignal(int signum)
{
    std::cerr << "\nopenWNS: caught signal " << signum << "\n";
    // Get access to the global instance
    wns::simulator::SignalHandler& signalHandler = wns::simulator::GlobalSignalHandler::Instance();
    if (!signalHandler.map_.knows(signum))
    {
        // should never happen, otherwise the implementation is broken
        std::cerr << "openWNS: no signal handler defined!!!\n";
        return;
    }

    Handler* localHandler = signalHandler.map_.find(signum);

    if (localHandler->num_slots() == 0)
    {
        std::cerr << "openWNS: no signal handler to call!\n";
        return;
    }
    if (localHandler->num_slots() > 1)
    {
        std::cerr << "openWNS: more than one signal handler to call! Not calling any signal handler!\n";
        return;
    }

    // call the signal handler
    (*localHandler)();
}

