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

#ifndef IP_ROUTING_HPP
#define IP_ROUTING_HPP

#include <IP/IPHeader.hpp>
#include <IP/container/RoutingTable.hpp>
#include <IP/container/DataLink.hpp>

#include <WNS/service/nl/Address.hpp>
#include <WNS/service/dll/DataTransmission.hpp>

#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/ldk/Processor.hpp>
#include <WNS/ldk/Command.hpp>

#include <WNS/logger/Logger.hpp>

namespace ip {

	/**
	 * @brief Implementation of IPv4 routing functionality
	 *
	 * \pyco{ip.Component.Routing}
	 */
	class Routing:
		public wns::ldk::CommandTypeSpecifier<>,
		public wns::ldk::HasReceptor<>,
		public wns::ldk::HasConnector<>,
		public wns::ldk::HasDeliverer<>,
		public wns::ldk::Processor<Routing>,
		public wns::Cloneable<Routing>
	{
		typedef wns::service::nl::Address Address;
		typedef wns::service::dll::UnicastDataTransmission DLLDataTransmission;
	public:
		/**
		 * @brief Routing functionality takes the component
		 * and a pyconfig::View for initialization
		 * @param[in] _component The corresponding component
		 * @param[in] _pyco Configuration for this component.
		 */
		Routing(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& _pyco);

		/**
		 * @brief Component's destructor.
		 */
		virtual
		~Routing();

		/**
		 * @brief Find my friends. This is currently the UpperConvergence to 
		 * get the AddressingCommand that is used for routing.
		 */
		void
		onFUNCreated();

		void
		addRoute(const wns::service::nl::Address& netAddress,
			 const wns::service::nl::Address& gateway,
			 const wns::service::nl::Address& netMask,
			 std::string dllName);

		void
		printRoutingTable(const wns::service::nl::Address& highlight = Routing::defaultGateway) const;

		void
		processOutgoing(const wns::ldk::CompoundPtr&);

		void
		processIncoming(const wns::ldk::CompoundPtr&);

		void
		setDLLs(container::DataLinkContainer dlc);

	private:
		static wns::service::nl::Address defaultGateway;

		wns::ldk::CommandReaderInterface* ipHeaderReader;

		/**
		 * @brief Try to forward pdu via routing table.
		 * @return Returns true if pdu could be  forwarded, false otherwise.
		 */
		bool
		forwardViaRoutingTable(const wns::ldk::CompoundPtr& c);

		/**
		 * @brief Routing table. Packets with destination address in
		 * unknown ip ranges are sent to next hops due to information
		 * preconfigured in routing table
		 */
		container::RoutingTable rt;

		/**
		 * @brief Configuration for this component
		 */
		wns::pyconfig::View pyco;

		/**
		 * @brief The logger for this component.
		 */
		wns::logger::Logger log;

		container::DataLinkContainer dlls;

	protected:
		/**
		 * @brief Returns the configuration for this component
		 */
		wns::pyconfig::View
		getConfig();
	};

} // ip
#endif // NOT defined IP_ROUTING_HPP


