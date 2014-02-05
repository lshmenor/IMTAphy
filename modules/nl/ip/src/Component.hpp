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

#ifndef IP_COMPONENT_HPP
#define IP_COMPONENT_HPP

#include <IP/VirtualDNS.hpp>
#include <IP/container/DataLink.hpp>
#include <IP/trace/TraceCollector.hpp>

#include <WNS/service/nl/Service.hpp>
#include <WNS/service/dll/DataTransmission.hpp>
#include <WNS/service/dll/Handler.hpp>
#include <WNS/service/nl/Address.hpp>
#include <WNS/node/component/Component.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/Layer.hpp>
#include <WNS/logger/Logger.hpp>

namespace ip {

    class UpperConvergence;
    class LowerConvergence;

    namespace container {
        class NeighbourCache;
    }

    /**
     * @brief IPv4 Component realization.
     *
     * \pyco{ip.Component.IPv4Component}
     */
    class Component:
            virtual public wns::ldk::Layer,
            public wns::node::component::Component,
            public wns::service::nl::DNSService
    {
        typedef wns::service::nl::Address Address;
        typedef wns::service::dll::UnicastDataTransmission DLLDataTransmission;
    public:
        /**
         * @brief Components take a parent node and a pyconfig::View for
         * initialization
         * @param[in] _node The parent node.
         * @param[in] _pyco Configuration for this component.
         */
        Component(
            wns::node::Interface* _node,
            const wns::pyconfig::View& _pyco);

        /**
         * @brief Component's destructor.
         */
        virtual ~Component();

        /**
         * @brief DNS lookup
         */
        virtual wns::service::nl::Address
        lookup(wns::service::nl::FQDN name);

        /**
         * @brief Find partner components within your node as given by
         * the configuration.
         */
        virtual void
        onNodeCreated();

        /**
         * @brief Find peer components in other nodes.
         */
        virtual void
        onWorldCreated();

        virtual void
        onShutdown();

    private:
        /**
         * @brief add services
         */
        virtual void
        doStartup();

        void
        leaseAddresses();

        void
        setupARP();

        void
        setupDNS();

        void
        setupForwarding();

        void
        setupRoutingTable();

        void
        setupLowerConvergence();

        trace::TraceCollector*
        getTraceCollector();

        /**
         * @brief The logger for this component.
         */
        wns::logger::Logger log;

        /**
         * @brief The Functional Unit Network that implemets IP.
         */
        wns::ldk::fun::Main* fun;

        wns::service::dll::IRuleControl* iRuleControl;

        wns::service::nl::Address sourceAddress;

        ip::container::DataLinkContainer dlls;

        wns::service::nl::FQDN domainName;

        VirtualDNS* dns;

        /**
         * @brief UpperConvergence for this component, it implements the
         * Data transmission service of this component. Packets
         * are given to the component's routing functionality.
         */
        UpperConvergence* upperConvergence;

        /**
         * @brief Lower Convergence FU to adapt to the DLL service.
         */
        LowerConvergence* lowerConvergence;

        /** @brief store the created resolvers for proper deletion */
        std::list<resolver::ResolverInterface*> resolvers;
    };

} // ip
#endif //_IP_COMPONENT_HPP


