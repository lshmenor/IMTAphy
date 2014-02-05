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

#ifndef WNS_LDK_DELIVERER_HPP
#define WNS_LDK_DELIVERER_HPP

#include <WNS/ldk/Compound.hpp>
#include <WNS/ldk/Link.hpp>
#include <WNS/ldk/IDelivererReceptacle.hpp>

#include <stdint.h>

namespace wns { namespace ldk {

        /**
         * @brief Deliverer scheduling strategy interface.
         * @ingroup hasdeliverer
         *
         * Deliverer is one of the 5 aspects of a FU (see @ref ldkaspects.) <br>
         *
         * Deliverer is an abstract base class describing the interface required for
         * the implementation of strategies to select a FU to deliver a given
         * compound to in the incoming data flow. A FU does not select a FU for
         * compound delivery in the incoming data flow directly. Instead it
         * delegates the choice to its Deliverer strategy. Besides the strategy for
         * FU selection, the Deliverer holds the set of FUs the FU is connected to
         * in the direction of incoming data flows.<br>
         *
         * For incoming data flows no inter-FU flow control is needed. This is why
         * there is no strategy pairing as for outgoing data flows
         * (Receptor/Connector.)
         *
         */
        class Deliverer
            : public virtual Link<IDelivererReceptacle>
        {
        public:
            virtual ~Deliverer()
            {}

            virtual IDelivererReceptacle* getAcceptor(const CompoundPtr& compound) = 0;
        };
    }
}


#endif // NOT defined WNS_LDK_DELIVERER_HPP



