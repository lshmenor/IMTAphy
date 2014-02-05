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

#ifndef WNS_LDK_HASDELIVERER_HPP
#define WNS_LDK_HASDELIVERER_HPP

#include <WNS/ldk/HasDelivererInterface.hpp>
#include <WNS/ldk/DelivererRegistry.hpp>
#include <WNS/ldk/SinglePort.hpp>
#include <WNS/ldk/SingleDeliverer.hpp>

namespace wns { namespace ldk {

	/**
	 * @brief HasDeliverer specific type declarations.
	 *
	 */
	namespace hasdeliverer
	{
            /**
             * @brief Default strategy to be used for upper scheduling.
             *
             */
            typedef SingleDeliverer DefaultDelivererStrategy;
	}

	/**
	 * @brief Implement the HasDeliverer Interface for a given scheduling strategy.
	 * @ingroup hasdeliverer
	 *
	 */
        template <typename DELIVERERTYPE = hasdeliverer::DefaultDelivererStrategy,
                  typename PORTID = SinglePort>
        class HasDeliverer
            : public virtual HasDelivererInterface,
              public virtual DelivererRegistry
        {
        public:
            HasDeliverer()
                : DelivererRegistry(),
                  deliverer_()
            {
                addToDelivererRegistry(PORTID().name, &deliverer_);
            }

        HasDeliverer( const HasDeliverer& other)
                : DelivererRegistry(),
                  deliverer_()
            {
                addToDelivererRegistry(PORTID().name, &deliverer_);
            }

            HasDeliverer(DelivererRegistry* dr)
                : DelivererRegistry(),
                  deliverer_()
            {
                dr->addToDelivererRegistry(PORTID().name, &deliverer_);
            }

            virtual
            ~HasDeliverer()
            {}

            DELIVERERTYPE*
            getDeliverer() const
            {
                return &deliverer_;
            }

        private:
            mutable DELIVERERTYPE deliverer_;
        };

    } // ldk
} // wns


#endif // NOT defined WNS_LDK_HASDELIVERER_HPP

