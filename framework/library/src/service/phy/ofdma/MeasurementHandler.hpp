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

#ifndef WNS_SERVICE_PHY_OFDMA_MEASUREMENTHANDLER_HPP
#define WNS_SERVICE_PHY_OFDMA_MEASUREMENTHANDLER_HPP

#include <WNS/service/phy/power/OFDMAMeasurement.hpp>
#include <WNS/service/Service.hpp>
#include <WNS/osi/PDU.hpp>
#include <WNS/PowerRatio.hpp>
#include <map>

namespace wns { namespace service { namespace phy { namespace ofdma {

	/**
	 * @brief OFDMA Phy Handler for Measurements of PathLoss,Fading values
	 * that depend on OFDMA subchannel.
	 * Class from which the subscriber must inherit from. Defines onMeasurementUpdate().
	 */
	class MeasurementHandler
	{
	public:
		virtual
		~MeasurementHandler()
		{}

		/** @brief this function is called in inherited classes
		 * of those classes which have subscribed to this service */
		virtual void
		onMeasurementUpdate(wns::node::Interface* source, wns::service::phy::power::OFDMAMeasurementPtr rxPowerMeasurement) = 0;
	};
} // ofdma
} // phy
} // service
} // wns
#endif // WNS_SERVICE_PHY_OFDMA_HANDLER



