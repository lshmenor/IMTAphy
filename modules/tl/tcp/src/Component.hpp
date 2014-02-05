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

#ifndef TCP_COMPONENT_HPP
#define TCP_COMPONENT_HPP

#include <TCP/UpperConvergence.hpp>
#include <TCP/FlowHandler.hpp>

#include <WNS/node/component/Component.hpp>
#include <WNS/ldk/Layer.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/fun/Sub.hpp>
#include <WNS/ldk/FlowSeparator.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/service/nl/Service.hpp>
#include <WNS/service/dll/FlowEstablishmentAndRelease.hpp>
#include <WNS/service/dll/Handler.hpp>

namespace tcp {

    class Service;
    class IPDataHandler;
    class UpperConvergence;
    class LowerConvergence;
    class HandshakeStrategyInterface;

    /**
     * @brief Offers a TCP service to higher layers
     * @author Marc Schinnenburg <msg@comnets.rwth-aachen.de>
     */
    class Component :
            virtual public wns::ldk::Layer,
            public wns::node::component::Component
    {
    public:
        Component(wns::node::Interface* node,
                  const wns::pyconfig::View& config);

        virtual
        ~Component();

        virtual void
        onNodeCreated();

        virtual void
        onWorldCreated();

        virtual void
        onShutdown();

    private:
        Component(const Component&);	// disallow copy constructor
        Component& operator=(const Component&); // disallow assignment

        /**
         * @brief Setup Component
         */
        virtual void
        doStartup();

        /**
         * @brief Converting a protocol description from PyConfig
         * to a protocol ID defined in wns::service::nl::protocolNumber
         */
        wns::service::nl::protocolNumber
        stringToProtocolNumber(std::string _protNr);

        /**
         * @brief TCP's functional unit network
         */
        wns::ldk::fun::Main* fun;

        /**
         * @brief This group of functional units is the prototype of the
         * flow separator. For each key (flow id) a new object of the
         * prototype is instantiatedf.
         */
        wns::ldk::fun::Sub* subFUN;

        /**
         * @brief The service TCP is providing to upper layer
         */
        Service* tlService;

        /**
         * @brief The top level functional unit of TCP's FUN.
         */
        UpperConvergence* upperConvergence;

        /**
         * @brief The notification service provided to the IP layer. All
         * incoming data is forwarded to the dispatcher.
         */
        IPDataHandler* ipDataHandler;

        /**
         * @brief The bottem functional unit of TCP's FUN.
         */
        LowerConvergence* lowerConvergence;

        /**
         * @brief Data transimission service provided by IP layer
         */
        wns::service::nl::Service* ipDataTransmission;

        /**
         * @brief The flow separator FU in TCP's FUN
         */
        wns::ldk::FlowSeparator* flowSeparator;

        /**
         * @brief Connection control FU within the flow separator's prototype instance
         */
        HandshakeStrategyInterface* handshakeStrategy;

        wns::logger::Logger logger;

        /**
         * @brief FlowHandler
         */
        tcp::FlowHandler* tcpFlowHandler;
        wns::service::dll::FlowEstablishmentAndRelease* fear;
        wns::service::dll::Notification* dllNotification;
    };

} // namespace tcp


#endif // NOT defined TCP_COMPONENT_HPP


