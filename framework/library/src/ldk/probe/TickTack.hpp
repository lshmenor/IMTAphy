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

#ifndef WNS_LDK_PROBE_TICKTACK_HPP
#define WNS_LDK_PROBE_TICKTACK_HPP

#include <WNS/ldk/fu/Plain.hpp>
#include <WNS/probe/bus/ContextCollector.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/simulator/ISimulator.hpp>

#include <WNS/probe/bus/ContextCollector.hpp>

namespace wns { namespace ldk { namespace probe {

    class TickTackCommand :
        public Command
    {
    public:
        TickTackCommand()
        {
            magic.tickTime = wns::simulator::getEventScheduler()->getTime();
            magic.probed = false;
        }

        struct
        {
        }
        local;

        struct
        {
        }
        peer;

        struct
        {
            wns::simulator::Time tickTime;
            bool probed;
        }
        magic;

    };


    /**
    * @brief Measures the delay between two points in a FUN. This can be done
    * locally or in a peer FUN. Tick is the start point of the measurement
    */
    class Tick :
        public fu::Plain<Tick, TickTackCommand>
    {
    public:
        // FUNConfigCreator interface realisation
        Tick(fun::FUN* fuNet, const wns::pyconfig::View& config);
        ~Tick();

        virtual 
        void onFUNCreated();

        virtual void
        probeOutgoing();

        virtual void
        probeIncoming();

    private:
        virtual void
        doSendData(const CompoundPtr& compound);

        virtual void
        doOnData(const CompoundPtr& compound);

        virtual bool
        doIsAccepting(const CompoundPtr& compound) const
        {
            return getConnector()->hasAcceptor(compound);
        }

        virtual void
        doWakeup()
        {
            getReceptor()->wakeup();
        }

        void
        writeCommand(const CompoundPtr& compound);

        std::string probeName_;
        bool probeOutgoing_;
        wns::probe::bus::ContextCollectorPtr outSizeProbe_;
        wns::logger::Logger logger_;

    };

    /**
    * @brief Measures the delay between two points in a FUN. This can be done
    * locally or in a peer FUN. Tack is the end point of the measurement
    */
    class Tack :
        public fu::Plain<Tack>
    {
    public:
        // FUNConfigCreator interface realisation
        Tack(fun::FUN* fuNet, const wns::pyconfig::View& config);
        ~Tack();

        virtual void 
        onFUNCreated();

        virtual void
        probeOutgoing();

        virtual void
        probeIncoming();

    private:
        virtual void
        doSendData(const CompoundPtr& compound);

        virtual void
        doOnData(const CompoundPtr& compound);

        virtual bool
        doIsAccepting(const CompoundPtr& compound) const
        {
            return getConnector()->hasAcceptor(compound);
        }

        virtual void
        doWakeup()
        {
            getReceptor()->wakeup();
        }

        void
        probeIfNotProbed(const CompoundPtr& compound);

        std::string probeName_;
        wns::ldk::CommandReaderInterface* commandReader_;
        wns::probe::bus::ContextCollectorPtr delayProbe_;
        wns::probe::bus::ContextCollectorPtr inSizeProbe_;
        bool probeOutgoing_;

        fun::FUN* fun_;

        wns::logger::Logger logger_;
    };

} // probe
} // ldk
} // wns

#endif // NOT defined WNS_LDK_PROBE_TICKTACK_HPP


