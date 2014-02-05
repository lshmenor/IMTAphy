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

#ifndef WNS_SERVICE_NL_SERVICE_HPP
#define WNS_SERVICE_NL_SERVICE_HPP

#include <WNS/service/Service.hpp>
#include <WNS/service/nl/Address.hpp>
#include <WNS/service/dll/FlowID.hpp>
#include <WNS/osi/PDU.hpp>

namespace wns { namespace service { namespace nl {

	/**
	 * @brief Service definition for components that offer Nl service.
	 */
	class Service :
		virtual public wns::service::Service
	{
	public:

		/**
		 * @brief Deliver data to another peer component
		 *
		 * @param[in] _peer Destination component where data is to be
		 * delivered.
		 *
		 * @param[in] _data Protocol Data Unit that is to be
		 * transported.
		 *
		 * @param[in] _protocol This is the payload's protocol number.
		 * Used for demultiplexing
		 */
		virtual void
		sendData(
			const wns::service::nl::Address& _source,
			const wns::service::nl::Address& _peer,
			const wns::osi::PDUPtr& _data,
			protocolNumber _protocol,
			int dllFlowID = wns::service::dll::NoFlowID) = 0;
	};

	class DNSService :
		virtual public wns::service::Service
	{
	public:

		virtual wns::service::nl::Address
		lookup(FQDN name) = 0;
	};
} // nl
} // service
} // wns

#endif //_WNS_SERVICE_NL_SERVICE_HPP
