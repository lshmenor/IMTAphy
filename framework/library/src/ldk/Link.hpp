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

#ifndef WNS_LDK_LINK_HPP
#define WNS_LDK_LINK_HPP

#include <WNS/ldk/Compound.hpp>

#include <stdint.h>

namespace wns { namespace ldk {
        /**
         * @brief Link between FUs.
         *
         * Link is the base class for all three different types of Links:
         * Connector, Receptor and Deliverer.
         *
         * Every Link may contain a set of FUs, although implementations may choose
         * to restrict the set size to some fixed values.
         *
         */
        template <typename RECEPTACLETYPE>
        class Link
        {
        public:
            typedef std::vector<RECEPTACLETYPE*> ExchangeContainer;

            virtual ~Link()
            {}

            /**
             * @brief Add a FU to the link set.
             */
            virtual void add(RECEPTACLETYPE* it) = 0;

            /**
             * @brief Erase all FUs.
             */
            virtual void clear() = 0;

            /**
             * @brief Return number of FUs added.
             */
            virtual unsigned long int size() const = 0;

            /**
             * @brief Return set of stored FUs.
             */
            virtual const ExchangeContainer get() const = 0;

            /**
             * @brief Replace the current set of stored FUs.
             */
            virtual void set(const ExchangeContainer&) = 0;
        };

    } // ldk
} // wns


#endif // NOT defined WNS_LDK_LINK_HPP


