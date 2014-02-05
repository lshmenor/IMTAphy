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

#ifndef WNS_LDK_PROBE_WINDOW_HPP
#define WNS_LDK_PROBE_WINDOW_HPP

#include <WNS/ldk/fu/Plain.hpp>
#include <WNS/ldk/Forwarding.hpp>

#include <WNS/events/PeriodicTimeout.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>

#include <WNS/ldk/probe/Probe.hpp>
#include <WNS/SlidingWindow.hpp>
#include <WNS/probe/bus/ContextCollector.hpp>

namespace wns { namespace ldk { namespace probe {

    class Window;

    class WindowCommand :
        public Command
    {
    public:
        struct
        {
        }
        local;

        struct {} peer;

        struct
        {
            Window* probingFU;
        }
        magic;
    };

    /**
     * @brief FunctionalUnit to probe windowing throughputs
     *
     */
    class Window :
        public Probe,
        public fu::Plain<Window, WindowCommand>,
        public Forwarding<Window>,
        public events::PeriodicTimeout
    {
    public:
        Window(fun::FUN* fuNet, const wns::pyconfig::View& config);
        virtual ~Window();

        // Processor interface
        virtual void processOutgoing(const CompoundPtr& compound);
        virtual void processIncoming(const CompoundPtr& compound);

    protected:
        void periodically();

    private:
        simTimeType sampleInterval;

        wns::probe::bus::ContextCollectorPtr bitsIncomingBus;
        wns::probe::bus::ContextCollectorPtr compoundsIncomingBus;
        wns::probe::bus::ContextCollectorPtr bitsOutgoingBus;
        wns::probe::bus::ContextCollectorPtr compoundsOutgoingBus;
        wns::probe::bus::ContextCollectorPtr bitsAggregatedBus;
        wns::probe::bus::ContextCollectorPtr compoundsAggregatedBus;

        SlidingWindow cumulatedBitsIncoming;
        SlidingWindow cumulatedPDUsIncoming;

        SlidingWindow cumulatedBitsOutgoing;
        SlidingWindow cumulatedPDUsOutgoing;

        SlidingWindow aggregatedThroughputInBit;
        SlidingWindow aggregatedThroughputInPDUs;

        logger::Logger logger;
    };

}
}
}

#endif // NOT defined WNS_LDK_PROBE_WINDOW_HPP


