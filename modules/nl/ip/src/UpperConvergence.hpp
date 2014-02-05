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

#ifndef IP_UPPERCONVERGENCE_HPP
#define IP_UPPERCONVERGENCE_HPP

#include <IP/IPHeader.hpp>

#include <WNS/logger/Logger.hpp>
#include <WNS/service/nl/Address.hpp>
#include <WNS/service/nl/Service.hpp>
#include <WNS/service/nl/DataHandler.hpp>
#include <WNS/service/dll/FlowID.hpp>
#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/ldk/Forwarding.hpp>
#include <WNS/ldk/Command.hpp>


namespace ip {

	/**
	 * @brief Convergence to upper layers that use the IPv4 Service
	 * interface.
	 *
	 * \pyco{ip.Component.UpperConvergence}
	 */
	class UpperConvergence:
		public virtual wns::service::nl::Service,
		public virtual wns::service::nl::Notification,
		public wns::ldk::CommandTypeSpecifier<>,
		public wns::ldk::HasReceptor<>,
		public wns::ldk::HasConnector<>,
		public wns::ldk::HasDeliverer<>,
		public wns::ldk::Forwarding<UpperConvergence>,
		public wns::Cloneable<UpperConvergence>
	{
		typedef wns::service::nl::Address Address;

	public:
		/**
		 * @brief The service provided by the component takes
		 * the component and a pyconfig::View for initialization
		 * @param[in] _component The corresponding component
		 * @param[in] _pyco Configuration for this component.
		 */
		UpperConvergence(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& _pyco);

		virtual void
		onFUNCreated();

		/**
		 * @brief Deliver data to another peer component
		 * @param[in] _peer Destination component where data is to be delivered.
		 * @param[in] _sdu Protocal Data Unit that is to be transported.
		 * @param[in] _protocol Upper layers must indicate their protocol number when sending data.
		 */
		virtual void
		sendData(
			const wns::service::nl::Address& _source,
			const wns::service::nl::Address& _peer,
			const wns::osi::PDUPtr& _sdu,
			wns::service::nl::protocolNumber _protocol,
			wns::service::dll::FlowID _dllFlowID = 0);

		/**
		 * @brief Register DataHandler of upper layer.
		 */
		virtual void
		registerHandler(
			wns::service::nl::protocolNumber _protocol, 
			wns::service::nl::DataHandler* dh);

		/**
		 * @brief Process incoming compounds. Here the forwarding to the upper layer is done.
		 */
		virtual void
		processIncoming(const wns::ldk::CompoundPtr& compound);

		/**
		 * @brief Retrieve IP Address of this service
		 */
		virtual const wns::service::nl::Address&
		getAddress() const;

		void
		setSourceAddress(wns::service::nl::Address src);

	private:
		/**
		 * @brief Configuration for this component
		 */
		wns::pyconfig::View pyco;

		/**
		 * @brief The logger for this component.
		 */
		wns::logger::Logger log;

		/**
		 * @brief The component's IP address. It's needed for creating
		 * new packets.
		 */
		Address sourceAddress;

		/**
		 * @brief The DataHandlerContainer type.
		 */
		typedef wns::container::Registry<wns::service::nl::protocolNumber, wns::service::nl::DataHandler*> DataHandlerContainer;

		/**
		 * @brief The DataHandler registry for upper layers.
		 */
		DataHandlerContainer handlerRegistry;

		wns::ldk::CommandReaderInterface* ipHeaderReader;

		IPHeader* ipHeaderFU;
	};


} // ip
#endif // IP_UPPERCONVERGENCE_HPP


