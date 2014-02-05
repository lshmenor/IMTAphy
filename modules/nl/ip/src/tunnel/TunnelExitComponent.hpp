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

#ifndef IP_TUNNEL_TUNNELEXITCOMPONENT_HPP
#define IP_TUNNEL_TUNNELEXITCOMPONENT_HPP

#include <WNS/service/nl/DataHandler.hpp>
#include <WNS/service/dll/Handler.hpp>
#include <WNS/node/component/Component.hpp>
#include <WNS/logger/Logger.hpp>

namespace ip { namespace tunnel {

    /**
     * @brief Tunnel Exits
     *
     * \pyco{ip.Tunnel.TunnelExitComponent}
     */
	class TunnelExitComponent:
		public wns::node::component::Component,
		public wns::service::nl::DataHandler,
		public wns::service::dll::Notification

	{
	public:
		TunnelExitComponent(
			wns::node::Interface* _node,
			const wns::pyconfig::View& _pyco);

		virtual ~TunnelExitComponent();

		virtual void
		onNodeCreated();

		virtual void
		onWorldCreated();

		virtual void
		onShutdown();

		virtual void
		onData(wns::service::nl::Address _sourceIP, const wns::osi::PDUPtr& _pdu);

		virtual void
		registerHandler(wns::service::dll::protocolNumber protocol, wns::service::dll::Handler* handler);

		virtual void
		registerFlowHandler(wns::service::dll::FlowHandler*){};

		virtual void 
		registerIRuleControl(wns::service::dll::IRuleControl*){};


	private:

		virtual void
		doStartup();

		wns::service::dll::Handler* dllHandler;

		/**
		 * @brief The logger for this component.
		 */
		wns::logger::Logger log;

	};
} // tunnel
} // ip

#endif // IP_TUNNEL_TUNNELEXITCOMPONENT_HPP
