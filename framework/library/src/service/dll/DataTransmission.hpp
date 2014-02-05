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

#ifndef WNS_SERVICE_DLL_DATATRANSMISSION_HPP
#define WNS_SERVICE_DLL_DATATRANSMISSION_HPP

#include <WNS/service/Service.hpp>
#include <WNS/service/dll/Address.hpp>
#include <WNS/service/dll/ProtocolNumber.hpp>
#include <WNS/service/dll/FlowID.hpp>
#include <WNS/osi/PDU.hpp>

namespace wns { namespace service { namespace dll {

	/**
	 * @brief Service definition for components that offer Data Link Layer
	 * transmission services.
	 */
	template <class Address>
	class DataTransmission :
		virtual public wns::service::Service
	{
	public:
		/**
		 * @brief Deliver data to other peer component(s)
		 * @param[in] macAddress Destination component(s) where data is to be delivered.
		 * @param[in] data Protocol Data Unit that is to be transported.
		 */
		virtual void
		sendData(
			const Address& macAddress,
			const wns::osi::PDUPtr& data,
			protocolNumber protocol,
			wns::service::dll::FlowID _dllFlowID = wns::service::dll::NoFlowID
			)=0;

		virtual wns::service::dll::UnicastAddress
		getMACAddress() const = 0;
	};

	/**
	 * @brief Service definition for components that offer Data Link Layer
	 * unicast transmission services.
	 */
	typedef DataTransmission<wns::service::dll::UnicastAddress> UnicastDataTransmission;

	/**
	 * @brief Service definition for components that offer Data Link Layer
	 * broadcast transmission services.
	 */
	typedef DataTransmission<wns::service::dll::BroadcastAddress> BroadcastDataTransmission;

} // dll
} // service
} // wns

#endif // WNS_SERVICE_DLL_DATATRANSMISSION_HPP



