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

#ifndef WNS_LDK_PROBE_PACKET_HPP
#define WNS_LDK_PROBE_PACKET_HPP

#include <WNS/ldk/fu/Plain.hpp>
#include <WNS/ldk/Forwarding.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>

#include <WNS/ldk/probe/Probe.hpp>
#include <WNS/probe/bus/ContextCollector.hpp>

#include <vector>

namespace wns { namespace ldk { namespace probe {

    class Packet;

    /**
     * @brief Command used by the Packet FunctionalUnit.
     *
     */
    class PacketCommand :
        public Command
    {
    public:
        PacketCommand()
        {
            magic.t = 0.0;
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
            simTimeType t;
            Packet* probingFU;
        }
        magic;

    };


    /**
     * @brief FunctionalUnit to probe packet delay/throughput
     *
     * The pyconfig::View should contain the following entries:
     * @li delayIncomingProbe - the name of a probe to record incoming packet delays (in s).
     * @li delayOutgoingProbe - the name of a probe to record outgoing packet delays (in s).
     * @li throughputProbe - the name of a probe to record throughput per packet (in Bit/s).
     * @li sizeProbe - the name of a probe to record packet sizes (in Bit).
     *
     * Each of the entries must contain the name of a configured probe. The
     * content of idNames determines the way, values are put into the probes:
     * @li With an empty (non-existent) definition of idNames, probes are put without
     *     any instance id.
     * @li With an idNames array with only one element, the given idName is used
     *     as instanceId.
     * @li instanceIdArrPtr are used, when the idNames array contains more than one
     *     entry.
     *
     * idNames must match a registered IDProvider.
     */
    class Packet :
        public Probe,
        public fu::Plain<Packet, PacketCommand>,
        public Forwarding<Packet>
    {
    public:
        Packet(fun::FUN* fuNet, const wns::pyconfig::View& config);
        virtual ~Packet();

        // Processor interface
        virtual void processOutgoing(const CompoundPtr& compound);
        virtual void processIncoming(const CompoundPtr& compound);

    private:
        wns::probe::bus::ContextCollectorPtr delayIncomingBus;
        wns::probe::bus::ContextCollectorPtr delayOutgoingBus;
        wns::probe::bus::ContextCollectorPtr throughputBus;
        wns::probe::bus::ContextCollectorPtr sizeIncomingBus;
        wns::probe::bus::ContextCollectorPtr sizeOutgoingBus;

        logger::Logger logger;
    };

}
}
}

#endif // NOT defined WNS_LDK_PROBE_PACKET_HPP


