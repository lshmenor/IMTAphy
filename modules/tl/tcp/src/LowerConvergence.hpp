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

#ifndef TCP_LOWERCONVERGENCE_HPP
#define TCP_LOWERCONVERGENCE_HPP

#include <WNS/service/tl/FlowID.hpp>
#include <WNS/service/nl/DataHandler.hpp>
#include <WNS/service/nl/Service.hpp>
#include <WNS/service/dll/FlowID.hpp>
#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/pyconfig/View.hpp>

namespace tcp {

	class Service;

	class LowerConvergence :
		virtual public wns::ldk::FunctionalUnit,
		public wns::ldk::CommandTypeSpecifier<>,
		public wns::ldk::HasReceptor<>,
		public wns::ldk::HasConnector<>,
		public wns::ldk::HasDeliverer<>,
		public wns::Cloneable<LowerConvergence>
	{
	public:
		typedef wns::container::Registry<wns::service::tl::FlowID, wns::service::dll::FlowID> FlowIDMapping;

		LowerConvergence(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& _config);

		virtual
		~LowerConvergence();

		// CompoundHandlerInterface
		virtual void
		doSendData(const wns::ldk::CompoundPtr& compound);

		virtual void
		doOnData(const wns::ldk::CompoundPtr& compound);

		virtual void
		onFUNCreated();

		virtual void
		setDataTransmissionService(wns::service::nl::Service* service);

		/**
		 * @brief Set the protocol number ID. This is needed to use the
		 * IP data transmission service.
		 */
		void
		setProtocolNumber(wns::service::nl::protocolNumber _protocolNumber);

		/**
		 * @brief The service of the transport layer (either UDP or TCP)
		 */
		void
		setTLService(Service* _tlService);

		void
		mapFlowID(wns::service::tl::FlowID flowID, wns::service::dll::FlowID dllFlowID);

		void
		unmapFlowID(wns::service::tl::FlowID flowID);

	private:
		/**
		 * @brief Return the data transmission service provided by the
		 * layer below.
		 */
		wns::service::nl::Service*
		getDataTransmissionService() const;

		// CompoundHandlerInterface
		virtual bool
		doIsAccepting(const wns::ldk::CompoundPtr& compound) const;

		virtual void
		doWakeup();

		/**
		 * @brief The data transmission service provided by the IP layer
		 */
		wns::service::nl::Service* dataTransmissionService;

		wns::pyconfig::View config;

		Service* tlService;

		wns::logger::Logger logger;

		struct Friends {
			FunctionalUnit* upperConvergence;
		} friends;

		/**
		 * @brief For differentiation of different transport layer
		 * protocols. The transport layer protocols register at the IP
		 * data transmission service with a protocol ID.
		 */
		wns::service::nl::protocolNumber protocolNumber;

		FlowIDMapping flowIDMapper;

        wns::ldk::CommandReaderInterface* tcpHeaderReader;
	};

} // namespace tcp

#endif // NOT defined TCP_LOWERCONVERGENCE_HPP
