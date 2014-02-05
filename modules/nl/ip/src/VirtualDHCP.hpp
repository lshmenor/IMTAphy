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

#ifndef IP_VIRTUALDHCP_HPP
#define IP_VIRTUALDHCP_HPP

#include <WNS/service/nl/Address.hpp>
#include <WNS/service/dll/DataTransmission.hpp>
#include <WNS/node/component/Component.hpp>
#include <WNS/container/Pool.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/Layer.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/Singleton.hpp>

namespace ip {

    class DHCPAck
    {
    public:
        DHCPAck()
        {
            address = wns::service::nl::Address();
            subnetMask = wns::service::nl::Address();
        }

        DHCPAck(wns::service::nl::Address address,
                wns::service::nl::Address subnetMask)
        {
            this->address = address;
            this->subnetMask = subnetMask;
        }

        wns::service::nl::Address address;
        wns::service::nl::Address subnetMask;
    };

    /**
     * @brief IPv4 Virtual DHCP realization.
     *
     * \pyco{ip.VirtualDHCP.VirtualDHCP}
     */
    class VirtualDHCP:
            virtual public wns::ldk::Layer,
            public wns::node::component::Component
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
        VirtualDHCP(
            wns::node::Interface* _node,
            const wns::pyconfig::View& _pyco);

        /**
         * @brief Component's destructor.
         */
        virtual ~VirtualDHCP();

        virtual void
        doStartup();

        DHCPAck
        request();

        void
        release(Address);

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

        wns::container::Pool<Address> ipPool;

        Address subnetMask;
    };

    class VirtualDHCPService {

    public:
        typedef wns::service::nl::Address Address;

        VirtualDHCPService();

        void
        addSubnet(std::string subnetIdentifier,
                  VirtualDHCP* server);

        VirtualDHCP*
        getZoneManager(std::string subnetIdentifier);

    private:
        wns::logger::Logger log;

        wns::container::Registry<std::string, VirtualDHCP*> servers;
    };

    typedef wns::SingletonHolder<VirtualDHCPService> TheDHCPService;

} // ip
#endif //_IP_VIRTUALDHCP_HPP


