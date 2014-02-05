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

#ifndef WNS_LDK_LINKHANDLERINTERFACE_HPP
#define WNS_LDK_LINKHANDLERINTERFACE_HPP

#include <WNS/ldk/Compound.hpp>
#include <WNS/ldk/IConnectorReceptacle.hpp>
#include <WNS/ldk/IReceptorReceptacle.hpp>
#include <WNS/ldk/IDelivererReceptacle.hpp>

namespace wns { namespace ldk {

        class LinkHandlerInterface
        {
        public:
            virtual bool
            isAccepting(IConnectorReceptacle* cr, const CompoundPtr& compound) = 0;

            virtual void
            sendData(IConnectorReceptacle* cr, const CompoundPtr& compound) = 0;

            virtual void
            wakeup(IReceptorReceptacle* rr) = 0;

            virtual void
            onData(IDelivererReceptacle* dr, const CompoundPtr& compound) = 0;

            virtual
            ~LinkHandlerInterface()
            {}

        protected:
            virtual bool
            doIsAccepting(IConnectorReceptacle* cr, const CompoundPtr& compound)
            {
                return cr->doIsAccepting(compound);
            }

            virtual void
            doSendData(IConnectorReceptacle* cr, const CompoundPtr& compound)
            {
                cr->doSendData(compound);
            }

            virtual void
            doWakeup(IReceptorReceptacle* rr)
            {
                rr->doWakeup();
            }

            virtual void
            doOnData(IDelivererReceptacle* dr, const CompoundPtr& compound)
            {
                dr->doOnData(compound);
            }
        };

    } // ldk
} // wns

#endif // NOT defined WNS_LDK_LINKHANDLERINTERFACE_HPP

