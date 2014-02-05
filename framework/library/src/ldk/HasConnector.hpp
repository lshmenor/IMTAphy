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

#ifndef WNS_LDK_HASCONNECTOR_HPP
#define WNS_LDK_HASCONNECTOR_HPP

#include <WNS/ldk/HasConnectorInterface.hpp>
#include <WNS/ldk/ConnectorRegistry.hpp>
#include <WNS/ldk/SinglePort.hpp>
#include <WNS/ldk/SingleConnector.hpp>


namespace wns { namespace ldk {

        /**
         * @brief HasConnector specific type declarations.
         *
         */
        namespace hasconnector
        {
            /**
             * @brief Default strategy to be used for lower scheduling.
             *
             */
            typedef SingleConnector DefaultConnectorStrategy;
        }

        /**
         * @brief Implements the HasConnector Interface for a given scheduling strategy.
         * @ingroup hasconnector
         *
         */
        template <typename CONNECTORTYPE = hasconnector::DefaultConnectorStrategy,
                  typename PORTID = SinglePort>
        class HasConnector
            : public virtual HasConnectorInterface,
              public virtual ConnectorRegistry
        {
        public:
            HasConnector()
                : ConnectorRegistry(),
                  lower_()
            {
                addToConnectorRegistry(PORTID().name, &lower_);
            }

        HasConnector(const HasConnector& other)
                : ConnectorRegistry(),
                  lower_()
            {
                addToConnectorRegistry(PORTID().name, &lower_);
            }

            HasConnector(ConnectorRegistry* cr)
                : ConnectorRegistry(),
                  lower_()
            {
                cr->addToConnectorRegistry(PORTID().name, &lower_);
            }

            virtual
            ~HasConnector()
            {}

            CONNECTORTYPE*
            getConnector() const
            {
                return &lower_;
            }

        private:
            mutable CONNECTORTYPE lower_;
        };

    } // ldk
} // wns


#endif // NOT defined WNS_LDK_HASCONNECTOR_HPP

