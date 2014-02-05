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

#ifndef WNS_LDK_SINGLELINK_HPP
#define WNS_LDK_SINGLELINK_HPP

#include <WNS/ldk/Link.hpp>

#include <WNS/ldk/FunctionalUnit.hpp>

#include <iostream>

namespace wns { namespace ldk {

        template <typename RECEPTACLETYPE>
        class SingleLink
            : virtual public Link<RECEPTACLETYPE>
        {
        public:
            SingleLink()
                : rec(NULL)
            {}


            virtual
            ~SingleLink()
            {
                rec = NULL;
            }

            /**
             * @name Link interface
             */
            //{@
            virtual void
            add(RECEPTACLETYPE* it)
            {
                assure(
                    rec == NULL,
                    "Can't add " + it->getFU()->toString() + ", already got " + rec->getFU()->toString());

                assureNotNull(it);

                rec = it;
            }

            virtual unsigned long int
            size() const
            {
                return rec == NULL ? 0 : 1;
            }

            virtual void
            clear()
            {
                rec = NULL;
            }

            virtual const typename Link<RECEPTACLETYPE>::ExchangeContainer
            get() const
            {
                typename Link<RECEPTACLETYPE>::ExchangeContainer container;
                if(this->rec != NULL)
                {
                    container.push_back(rec);
                }
                return container;
            }

            virtual void
            set(const typename Link<RECEPTACLETYPE>::ExchangeContainer& src)
            {
                assure(src.size() < 2, "SingleLink takes one FU at most");

                rec = NULL;
                if (!src.empty())
                {
                    this->rec = src.front();
                    assureNotNull(rec);
                }
            }
            //@}

        protected:
            RECEPTACLETYPE*
            getRec() const
            {
                return rec;
            }

        private:
            RECEPTACLETYPE* rec;
        };
    } // ldk
} // wns

#endif // WNS_LDK_SINGLELINK_HPP

