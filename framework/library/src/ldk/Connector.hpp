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

#ifndef WNS_LDK_CONNECTOR_HPP
#define WNS_LDK_CONNECTOR_HPP

#include <WNS/ldk/Compound.hpp>
#include <WNS/ldk/Link.hpp>
#include <WNS/ldk/IConnectorReceptacle.hpp>

#include <stdint.h>

namespace wns { namespace ldk {

        /**
         * @brief Connector scheduling strategy interface.
         * @ingroup hasconnector
         *
         * Connector is one of the 5 aspects of a FU (see @ref ldkaspects.) <br>
         *
         * Connector is an abstract base class describing the interface required for
         * the implementation of strategies to select a FU to deliver a given
         * compound to in the outgoing data flow.  A FU does not select a FU for
         * compound delivery in the outgoing data flow directly. Instead it
         * delegates the choice to its Connector strategy.  Besides the strategy for
         * FU selection, the Connector holds the set of FUs the FU is connected to
         * in the direction of outgoing data flows.<br>
         *
         * In combination with the Receptor, the Connector implements the inter-FU
         * flow control. Given a FU A having FU B in its connector set. FU B always
         * has FU A in its receptor set (see wns::ldk::Receptor.)<br>
         *
         */
        class Connector
            : public virtual Link<IConnectorReceptacle>
        {
        public:
            virtual ~Connector()
            {}

            /**
             * @brief Return true if there is a FU in the connector set willing to accept the given compound.
             *
             * Always call hasAcceptor to verify there is an accepting FU, before calling getAcceptor.
             */
            virtual bool hasAcceptor(const CompoundPtr& compound) = 0;

            /**
             * @brief Return the FU willing to accept the given compound.
             *
             * Return an accepting FU. This may only be called after a call to hasAcceptor returning true.
             * Always send a Compound using doSendData to the FU retrieved using getAcceptor.
             */
            virtual IConnectorReceptacle* getAcceptor(const CompoundPtr& compound) = 0;
        };
    }
}


#endif // NOT defined WNS_LDK_CONNECTOR_HPP


