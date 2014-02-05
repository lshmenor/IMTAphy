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

#ifndef WNS_LDK_ROUNDROBINRECEPTOR_HPP
#define WNS_LDK_ROUNDROBINRECEPTOR_HPP

#include <WNS/ldk/Link.hpp>
#include <WNS/ldk/Receptor.hpp>
#include <WNS/ldk/RoundRobinLink.hpp>

#include <WNS/RoundRobin.hpp>

namespace wns { namespace ldk {

        /**
         * @brief Receptor scheduling strategy implementation.
         * @ingroup hasreceptor
         *
         * Receptor is one of the 5 aspects of a FU (see @ref ldkaspects.) <br>
         *
         * RoundRobinReceptor is a strategy to wake up FUs during compound
         * delivery in the outgoing data flow. A FU does not call the wakeup
         * method of upper FUs directly. Instead it delegates the task to its
         * Receptor strategy. Besides the strategy for FU selection, the
         * Receptor holds the set of FUs the FU is connected to from above for
         * outgoing data flows. <br>
         *
         * In combination with the Connector, Receptor implements the inter-FU flow
         * control.  Given a FU A having FU B in its connector set. FU B alwas has
         * FU in its receptor set (see wns::ldk::Connector.)<br>
         *
         */
        class RoundRobinReceptor
            : virtual public Receptor,
              public RoundRobinLink<IReceptorReceptacle>
        {
        public:
            /**
             * @brief Invoke the wakeup strategy.
             *
             * Thisimplementation wakes all FUs in the receptor set up using
             * a round robin strategy.
             */
            virtual void wakeup();
        };
    }
}


#endif // NOT defined WNS_LDK_ROUNDROBINRECEPTOR_HPP


