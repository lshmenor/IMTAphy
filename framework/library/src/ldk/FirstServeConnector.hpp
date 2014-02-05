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

#ifndef WNS_LDK_FIRSTSERVECONNECTOR_HPP
#define WNS_LDK_FIRSTSERVECONNECTOR_HPP

#include <WNS/ldk/Compound.hpp>
#include <WNS/ldk/Connector.hpp>
#include <WNS/ldk/MultiLink.hpp>

namespace wns { namespace ldk {

        class FirstServeConnector
            : public Connector,
              public MultiLink<IConnectorReceptacle>
        {
        public:

            /// Connector interface implementation
            virtual bool
            hasAcceptor(const wns::ldk::CompoundPtr& compound);

            /// Connector interface implementation
            virtual IConnectorReceptacle*
            getAcceptor(const wns::ldk::CompoundPtr& compound);

        };

    } // ldk
} // wns


#endif // NOT defined  WNS_LDK_FIRSTSERVECONNECTOR_HPP

