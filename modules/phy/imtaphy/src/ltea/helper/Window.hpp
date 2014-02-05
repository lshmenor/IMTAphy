/*******************************************************************************
 * This file is part of IMTAphy
 * _____________________________________________________________________________
 *
 * Copyright (C) 2011
 * Institute of Communication Networks (LKN)
 * Department of Electrical Engineering and Information Technology (EE & IT)
 * Technische Universitaet Muenchen
 * Arcisstr. 21
 * 80333 Muenchen - Germany
 * http://www.lkn.ei.tum.de/~jan/imtaphy/index.html
 * 
 * _____________________________________________________________________________
 *
 *   IMTAphy is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   IMTAphy is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with IMTAphy.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

// based on code from openWNS with the following license:

/*******************************************************************************
* This file is part of openWNS (open Wireless Network Simulator)
* _____________________________________________________________________________
*
* Copyright (C) 2004-2007
* Chair of Communication Networks (ComNets)
* Kopernikusstr. 5, D-52074 Aachen, Germany
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

#ifndef LTEA_HELPER_WINDOW_HPP
#define LTEA_HELPER_WINDOW_HPP

#include <WNS/ldk/fu/Plain.hpp>
#include <WNS/ldk/Forwarding.hpp>

#include <WNS/events/PeriodicTimeout.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>

#include <WNS/ldk/probe/Probe.hpp>
#include <WNS/probe/bus/ContextCollector.hpp>

namespace ltea { namespace helper { 

    class Window;

    class WindowCommand :
        public wns::ldk::Command
    {
    public:
        struct {} local;
        struct {} peer;

        struct {
            Window* probingFU;
        } magic;
    };

    /**
    * @brief FunctionalUnit to probe windowing throughputs. Based on wns::ldk::probe::Window
    * but simplified to only probe every windowSize seconds for the previous windowSize seconds
    *
    */
    class Window :
        public wns::ldk::probe::Probe,
        public wns::ldk::fu::Plain<Window, WindowCommand>,
        public wns::ldk::Forwarding<Window>,
        public wns::events::PeriodicTimeout
    {
    public:
        Window(wns::ldk::fun::FUN* fuNet, const wns::pyconfig::View& config);
        virtual ~Window();

        // Processor interface
        virtual void processOutgoing(const wns::ldk::CompoundPtr& compound);
        virtual void processIncoming(const wns::ldk::CompoundPtr& compound);

    protected:
        void periodically();

    private:
        simTimeType windowSize;

        wns::probe::bus::ContextCollectorPtr bitsIncomingBus;
        wns::probe::bus::ContextCollectorPtr compoundsIncomingBus;
        wns::probe::bus::ContextCollectorPtr bitsOutgoingBus;
        wns::probe::bus::ContextCollectorPtr compoundsOutgoingBus;
        wns::probe::bus::ContextCollectorPtr bitsAggregatedBus;
        wns::probe::bus::ContextCollectorPtr compoundsAggregatedBus;

        unsigned int cumulatedBitsIncoming;
        unsigned int cumulatedPDUsIncoming;

        unsigned int cumulatedBitsOutgoing;
        unsigned int cumulatedPDUsOutgoing;

        unsigned int aggregatedThroughputInBit;
        unsigned int aggregatedThroughputInPDUs;
        
        simTimeType settlingTime;

        wns::logger::Logger logger;
    };

}}

#endif 


