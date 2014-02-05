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

#ifndef WNS_SERVICE_NL_DATAHANDLER_HPP
#define WNS_SERVICE_NL_DATAHANDLER_HPP

#include <WNS/service/nl/Address.hpp>
#include <WNS/service/Service.hpp>
#include <WNS/osi/PDU.hpp>

namespace wns { namespace service { namespace nl {

	/**
	 * @brief Nl DataHandler interface.
	 *
	 * Nl components need a callback to deliver incoming data.  Users of
	 * @ref wns::node::component::nl::Service can register such a callback
	 * by calling @ref
	 * wns::node::component::nl::Service::registerDataHandler and passing
	 * an implementation of @ref wns::node::component::nl::DataHandler
	 */
	class DataHandler
	{
	public:
		/**
		 * @brief Destructor
		 */
		virtual
		~DataHandler()
		{}

		/**
		 * @brief Receive data from lower Nl layer.
		 *
		 * @param[in] _sourceIP Source IP address of this data. Therefor
		 * TCP/IP is called a protocol suite.
		 * @param[in] _pdu Data that is delivered to the Datahandler.
		 */
		
		virtual void
		onData(Address _sourceIP, 
		       const wns::osi::PDUPtr& _pdu) = 0;
	};

	class Notification
	{
	public:
		virtual
		~Notification ()
		 {}

		/**
		 * @brief Registration of DataHandlers of the upper layer.
		 *
		 * @param[in] _protocol Protocol Number this _handler is responsible for
		 *
		 * @param[in] _handler An implementation of the DataHandler interface.
		 * Will receive PDUs of the given _protocol.
		 */
		virtual void
		registerHandler(protocolNumber _protocol, DataHandler* _handler) = 0;

	};

} // nl
} // service
} // wns


#endif //WNS_SERVICE_NL_DATAHANDLER_HPP


