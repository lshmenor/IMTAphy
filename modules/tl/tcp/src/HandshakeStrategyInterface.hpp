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

#ifndef TCP_HANDSHAKESTRATEGYINTERFACE_HPP
#define TCP_HANDSHAKESTRATEGYINTERFACE_HPP

#include <TCP/Service.hpp>

#include <WNS/service/tl/FlowID.hpp>


namespace tcp {

	/**
	 * @brief Interface for implementing different strategies for connection establishment
	 */
	class HandshakeStrategyInterface
	{
	public:
		virtual ~HandshakeStrategyInterface() {}

		virtual void
		activeOpen(const wns::service::tl::FlowID&) = 0;

		virtual void
		passiveOpen(const wns::service::tl::FlowID&) = 0;

		virtual void
		closeConnection(const wns::service::tl::FlowID&) = 0;

		virtual void
		registerStrategyHandler(HandshakeStrategyHandlerInterface* _strategyHandler) = 0;

	};
} // namespace tcp

#endif // NOT defined TCP_HANDSHAKESTRATEGYINTERFACE_HPP
