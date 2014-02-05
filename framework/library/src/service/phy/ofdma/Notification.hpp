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

#ifndef WNS_SERVICE_PHY_OFDMA_NOTIFICATION_HPP
#define WNS_SERVICE_PHY_OFDMA_NOTIFICATION_HPP

#include <WNS/service/phy/ofdma/Handler.hpp>
#include <WNS/service/phy/ofdma/CarrierSensing.hpp>

#include <WNS/Subject.hpp>

namespace wns { namespace service { namespace phy { namespace ofdma {

    /**
	 * @brief OFDMA Phy Notification Service (upstack compounds)
	 */
    class Notification :
        // [rs] why not derive from virtual public service::Notification<Handler> ?
        virtual public service::Service
    {
    public:
        virtual void
        registerHandler(Handler* handler) = 0;

        virtual void
        enableReception() = 0;

        virtual void
        disableReception() = 0;

        virtual bool
        isReceptionEnabled() const = 0;

        virtual void
        registerRSSHandler(RSSHandler* rssHandler) = 0;

        virtual void
        updateRequest()
            {

            };
    };
} // ofdma
} // phy
} // service
} // wns
#endif // WNS_SERVICE_PHY_OFDMA_NOTIFICATION_HPP



