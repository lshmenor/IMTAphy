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

#ifndef IP_VIRTUALDNS_HPP
#define IP_VIRTUALDNS_HPP

#include <WNS/service/nl/Address.hpp>
#include <WNS/service/dll/DataTransmission.hpp>
#include <WNS/node/component/Component.hpp>
#include <WNS/container/Registry.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/Layer.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/Singleton.hpp>

namespace ip {

    /**
     * @brief IPv4 Virtual DNS realization.
     *
     * \pyco{ip.VirtualDNS.VirtualDNS}
     */
    class VirtualDNS:
            virtual public wns::ldk::Layer,
            public wns::node::component::Component
    {
    public:
        typedef wns::service::nl::FQDN DomainName;
        typedef wns::service::nl::Address NLAddress;
        typedef wns::container::Registry<DomainName, NLAddress> DNSContainer;

        /**
         * @brief Components take a parent node and a pyconfig::View for
         * initialization
         * @param[in] _node The parent node.
         * @param[in] _pyco Configuration for this component.
         */
        VirtualDNS(
            wns::node::Interface* _node,
            const wns::pyconfig::View& _pyco);

        /**
         * @brief Component's destructor.
         */
        virtual ~VirtualDNS();

        virtual void
        doStartup();

        NLAddress
        request(DomainName name);

        void
        bind(DomainName name, NLAddress ip);

        void
        unbind(DomainName name);

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
         * @brief The logger for this component.
         */
        wns::logger::Logger log;

        std::string dnsZone;

        DNSContainer dnsLookup;
    };

    class VirtualDNSService {

    public:
        VirtualDNSService();

        void
        addSubnet(std::string subnetIdentifier,
                  VirtualDNS* server);

        VirtualDNS*
        getZoneManager(std::string subnetIdentifier);

    private:
        wns::logger::Logger log;

        wns::container::Registry<std::string, VirtualDNS*> servers;
    };

    typedef wns::SingletonHolder<VirtualDNSService> TheDNSService;

} // ip
#endif //_IP_VIRTUALDNS_HPP


