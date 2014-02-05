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

#ifndef WNS_LDK_ROUNDROBINLINK_HPP
#define WNS_LDK_ROUNDROBINLINK_HPP

#include <WNS/ldk/Link.hpp>
#include <WNS/RoundRobin.hpp>

namespace wns { namespace ldk {

        template <typename RECEPTACLETYPE>
        class RoundRobinLink
            : virtual public Link<RECEPTACLETYPE>
        {
            typedef wns::RoundRobin<RECEPTACLETYPE*> ContainerType;

        public:
            RoundRobinLink()
                : recs()
            {}

            virtual
            ~RoundRobinLink()
            {}


            // Link interface realization
            virtual void
            add(RECEPTACLETYPE* it)
            {
                recs.add(it);
            }

            virtual unsigned long int
            size() const
            {
                return recs.size();
            }

            virtual void
            clear()
            {
                recs.clear();
            }

            virtual const typename Link<RECEPTACLETYPE>::ExchangeContainer
            get() const
            {
                return recs.getAllElements();
            }

            virtual void
            set(const typename Link<RECEPTACLETYPE>::ExchangeContainer& src)
            {
                recs.clear();
                for(typename Link<RECEPTACLETYPE>::ExchangeContainer::const_iterator it = src.begin();
                    it != src.end();
                    ++it)
                {
                    recs.add(*it);
                }
            }

        protected:
            ContainerType recs;
        };
    } // ldk
} // wns

#endif // WNS_LDK_SINGLELINK_HPP

