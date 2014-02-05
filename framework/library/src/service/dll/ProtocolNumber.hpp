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

#ifndef WNS_SERVICE_DLL_PROTOCOLNUMBER_HPP
#define WNS_SERVICE_DLL_PROTOCOLNUMBER_HPP

#include <WNS/osi/PDU.hpp>

namespace wns { namespace service { namespace dll {
	/**
	 * @brief Values for the protocol header field.
	 */
	enum protocolNumber {
		IP = 1,
		CONSTANZE = 2,
		MIH = 3,
		TESTING = 4,
		TUNNEL = 5
	};

	protocolNumber
	protocolNumberOf(const wns::osi::PDUPtr& pdu);

} // dll
} // service
} // wns

#endif // NOT defined WNS_SERVICE_DLL_PROTOCOLNUMBER_HPP


