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

#ifndef WNS_LDK_SINGLEDELIVERER_HPP
#define WNS_LDK_SINGLEDELIVERER_HPP

#include <WNS/ldk/Compound.hpp>
#include <WNS/ldk/Deliverer.hpp>
#include <WNS/ldk/SingleLink.hpp>

namespace wns { namespace ldk {

        /**
         * @brief Single link to antother FU
         * @ingroup hasdeliverer
         * @author Marc Schinnenburg <msg@comnets.rwth-aachen.de>
         *
         * This class
         * @li implements the Deliverer interface,
         * @li may be used as parameter to HasDeliverer.
         *
         */
        class SingleDeliverer
            : virtual public Deliverer,
              public SingleLink<IDelivererReceptacle>
        {
        public:
            SingleDeliverer();

            virtual
            ~SingleDeliverer();

            virtual IDelivererReceptacle*
            getAcceptor(const CompoundPtr& compound);
        };
    } // ldk
} // wns


#endif // NOT defined WNS_LDK_SINGLEDELIVERER_HPP

