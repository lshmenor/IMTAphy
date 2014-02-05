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

#ifndef WNS_SERVICE_DLL_FLOWESTABLISHMENTANDRELEASE_HPP
#define WNS_SERVICE_DLL_FLOWESTABLISHMENTANDRELEASE_HPP

#include <WNS/service/Service.hpp>
#include <WNS/service/nl/Address.hpp>
#include <WNS/service/tl/FlowID.hpp>
#include <WNS/service/qos/QoSClasses.hpp>

namespace wns { namespace service { namespace dll {

	/**
	 * @brief tl service specification.
	 * @ingroup interface
	 */
	class FlowEstablishmentAndRelease:
		virtual public wns::service::Service
	{
	public:
		/**
		 * @brief Establish a new flow.
		 *
		 * @param[in] _sourcePort Own local port.
		 *
		 * @param[in] _destPort On which port to connect peer.
		 *
		 * @param[in] _peer Own IP address.
		 *
		 * @param[in] _peer On which address to contact peer.
		 */
		virtual void
		establishFlow(wns::service::tl::FlowID flowID, wns::service::qos::QoSClass qosClass) = 0;

		/**
		 * @brief Release an existing flow.
		 *
		 * @param[in] _sourcePort Own local port.
		 *
		 * @param[in] _destPort On which port to connect peer.
		 *
		 * @param[in] _peer Own IP address.
		 *
		 * @param[in] _peer On which address to contact peer.
		 */
		virtual void
		releaseFlow(wns::service::tl::FlowID flowID)=0;
	};
}
}
}

#endif // WNS_SERVICE_DLL_FLOWESTABLISHMENTANDRELEASE_HPP
