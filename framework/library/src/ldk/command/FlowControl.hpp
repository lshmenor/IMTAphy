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

#ifndef WNS_LDK_COMMAND_FLOWCONTROL_HPP
#define WNS_LDK_COMMAND_FLOWCONTROL_HPP

#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/ldk/Delayed.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>

#include <WNS/ldk/tools/Gate.hpp>



namespace wns { namespace ldk { namespace command {

    class FlowControlInterface
    {
    public:
        virtual void open() = 0;
        virtual void close() = 0;

    protected:
        virtual ~FlowControlInterface()
        {
        }
    };


    class FlowControlCommand :
        public Command
    {
    public:
        typedef enum {START, STOP} FrameType;

        struct
        {
        }
        local;
        struct
        {
            FrameType type;
        } peer;
        struct
        {
        }
        magic;
    };

    /**
     * @brief Start/stop accepting incoming/outgoing compounds.
     *
     * Control a Gate via the GateInterface.
     */
    class FlowControl :
        public CommandTypeSpecifier<FlowControlCommand>,
        public HasReceptor<>,
        public HasConnector<>,
        public HasDeliverer<>,
        public Delayed<FlowControl>,
        public Cloneable<FlowControl>
    {
    public:
        FlowControl(fun::FUN* fuNet, const wns::pyconfig::View& config);

        // FlowControl interface
        virtual void open();
        virtual void close();

        // PDUHandler interface
        virtual void processIncoming(const CompoundPtr& compound);
        virtual void processOutgoing(const CompoundPtr& compound);
        virtual bool hasCapacity() const;
        virtual const CompoundPtr hasSomethingToSend() const;
        virtual CompoundPtr getSomethingToSend();

        virtual void onFUNCreated();
        virtual void calculateSizes(const CommandPool* commandPool, Bit& commandPoolSize, Bit& dataSize) const;
        virtual CommandPool* createReply(const CommandPool* original) const;

    private:
        pyconfig::View config;

        CompoundPtr toSend;

        struct _friends
        {
            tools::GateInterface* gate;
        }
        friends;

        logger::Logger logger;

        void send(FlowControlCommand::FrameType type);
    };

}
}
}

#endif // NOT defined WNS_LDK_COMMAND_FLOWCONTROL_HPP


