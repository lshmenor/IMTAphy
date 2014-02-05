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

#ifndef WNS_LDK_MULTIPLEXER_FRAMEDISPATCHER_HPP
#define WNS_LDK_MULTIPLEXER_FRAMEDISPATCHER_HPP

#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/ldk/Processor.hpp>

#include <WNS/ldk/RoundRobinReceptor.hpp>
#include <WNS/ldk/RoundRobinConnector.hpp>

#include <WNS/ldk/fun/FUN.hpp>

#include <WNS/ldk/multiplexer/OpcodeProvider.hpp>
#include <WNS/ldk/multiplexer/OpcodeDeliverer.hpp>

namespace wns { namespace ldk { namespace multiplexer {

    /**
     * @brief Tag compounds of multiple outgoing flows, delivering incoming compounds to the right FU.
     *
     * <PRE>
         -------    -------         -------
        | FU 1a |  | FU 2a |  ...  | FU Na |
         -------    -------         -------
            |          |               |
            v          v               v
         ----------------------------------
        |          FrameDispatcher         | <---
         ----------------------------------      |
            |          |               |         | incoming injection
            v          v               v
         -------    -------         -------
        | FU 1b |  | FU 2b |  ...  | FU Nb |
         -------    -------         -------
     * </PRE>
     *
     * FrameDispatcher is an OpcodeProvider. Incoming compounds get tagged
     * according to the FU they came from. After tagging, they get delivered to
     * the matching FU below: Compounds received from FU Ia get deliverd to FU
     * Ib. Thus, in the outgoing direction, FrameDispatcher is completely
     * transparent to the different flows. <p>
     *
     * Incoming compounds can be directly delivered to the FrameDispatcher. They
     * get delivered to the FU they have been originally received from. <p>
     *
     * Use a FrameDispatcher to realize different roles in a FUN. Sub FUNs
     * connected to a FrameDispatcher from above enrich a compound with
     * information. FUs below the FrameDispatcher realize the interface to the
     * PHY. E.g., FU 1a could implement the BCH, while FU 1b drives the PHY to
     * transmit the information provided by FU 1a; FU 2a could be part of the
     * user data plane, while FU 2b drives the PHY for user data transmission.
     */
    class FrameDispatcher :
        public CommandTypeSpecifier<OpcodeCommand>,
        public HasReceptor<RoundRobinReceptor>,
        public HasConnector<RoundRobinConnector>,
        public HasDeliverer<OpcodeDeliverer>,
        public Processor<FrameDispatcher>,
        public Cloneable<FrameDispatcher>
    {
    public:
        /**
         * @todo OpcodeSetter instances are leaking
         *
         */
        FrameDispatcher(fun::FUN* fuNet, const pyconfig::View& _config);

        // processor interface
        virtual void processOutgoing(const CompoundPtr& compound);
        virtual void processIncoming(const CompoundPtr& compound);

        virtual void calculateSizes(const CommandPool* commandPool, Bit& commandPoolSize, Bit& sduSize) const;

        int getOpcodeSize() const
        {
            return opcodeSize;
        }

        // connection setup modification
        virtual FunctionalUnit* whenConnecting();

    private:
        virtual void doDownConnect(FunctionalUnit* that, const std::string& srcPort, const std::string& dstPort);
        virtual void doUpConnect(FunctionalUnit* that, const std::string& srcPort, const std::string& dstPort);

        pyconfig::View config;

        int opcodeSize;
        int opcode;

        FunctionalUnit* pending;
        bool upConnected;
        bool downConnected;

        logger::Logger logger;
    };
}
}
}

#endif // NOT defined WNS_LDK_MULTIPLEXER_FRAMEDISPATCHER_HPP


