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

#ifndef WNS_LDK_MULTIPLEXER_DISPATCHER_HPP
#define WNS_LDK_MULTIPLEXER_DISPATCHER_HPP

#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/ldk/Processor.hpp>
#include <WNS/ldk/RoundRobinReceptor.hpp>

#include <WNS/ldk/fun/FUN.hpp>

#include <WNS/ldk/multiplexer/OpcodeProvider.hpp>
#include <WNS/ldk/multiplexer/OpcodeDeliverer.hpp>

namespace wns { namespace ldk { namespace multiplexer {

    class OpcodeSetter;

    /**
     * @brief Tag compounds of multiple paths, delivering incoming compounds to the right FU.
     *
     * <PRE>
         ------    ------           ------
        | FU 1 |  | FU 2 |   ...   | FU N |
         ------    ------           ------
            |         |                |
            v         v                v
         ----------------------------------
        |             Dispatcher           |
         ----------------------------------
                          |
                          v
                       -------
                      | lower |
                       -------
     * </PRE>
     *
     * Dispatcher is an OpcodeProvider. Incoming compounds get tagged
     * according to the FU they came from. After tagging, they get delivered to
     * the lower FU. <p>
     *
     * Incoming compounds can be directly delivered to the FrameDispatcher. They
     * get delivered to the FU they have been originally received from.
     */
    class Dispatcher :
        public CommandTypeSpecifier<OpcodeCommand>,
        public HasReceptor<RoundRobinReceptor>,
        public HasConnector<>,
        public HasDeliverer<OpcodeDeliverer>,
        public Processor<Dispatcher>,
        public Cloneable<Dispatcher>
    {
    public:
        Dispatcher(fun::FUN* fuNet, const pyconfig::View& _config);

        virtual ~Dispatcher();

        // connection setup modification
        virtual FunctionalUnit* whenConnecting();

        // processor interface
        virtual void processOutgoing(const CompoundPtr& compound);
        virtual void processIncoming(const CompoundPtr& compound);

        virtual void calculateSizes(const CommandPool* commandPool, Bit& commandPoolSize, Bit& sduSize) const;

        int getOpcodeSize() const { return opcodeSize; }
    private:
        pyconfig::View config;

        std::list<OpcodeSetter*> opcodeSetters;

        int opcodeSize;
        int opcode;

        logger::Logger logger;
    };
}
}
}

#endif // NOT defined WNS_LDK_MULTIPLEXER_DISPATCHER_HPP


