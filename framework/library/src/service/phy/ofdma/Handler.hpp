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

#ifndef WNS_SERVICE_PHY_OFDMA_HANDLER_HPP
#define WNS_SERVICE_PHY_OFDMA_HANDLER_HPP

#include <WNS/service/phy/power/PowerMeasurement.hpp>
#include <WNS/service/Service.hpp>
#include <WNS/osi/PDU.hpp>
#include <WNS/PowerRatio.hpp>
#include <map>

namespace wns { namespace service { namespace phy { namespace ofdma {

    /**
	 * @brief OFDMA Phy Notification Handler
	 * Class from which the subscriber must inherit from. Defines onData().
	 */
	class Handler
	{
	public:
		virtual
		~Handler()
		{}

		/** @brief: (new) interface for incoming SDUs (upstack) */
		virtual void
		onData(wns::osi::PDUPtr sdu, wns::service::phy::power::PowerMeasurementPtr rxPowerMeasurement) = 0;
		// using: class PowerMeasurement from WNS/service/phy/power/PowerMeasurement.hpp
	};

    /**
     * @brief RSS Notification Handler
     */
    class RSSHandler
    {
    public:
        virtual
        ~RSSHandler()
            {};

        virtual void
        onRSSChange(wns::Power newRSSIndicator) = 0;
    };

} // ofdma
} // phy
} // service
} // wns
#endif // WNS_SERVICE_PHY_OFDMA_HANDLER



