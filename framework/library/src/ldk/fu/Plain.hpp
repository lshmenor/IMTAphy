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

#ifndef WNS_LDK_FU_PLAIN_HPP
#define WNS_LDK_FU_PLAIN_HPP

#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/Cloneable.hpp>

namespace wns { namespace ldk { namespace fu {

    /**
     * @brief Very simple FU. Single Input Single Output, no Decorators,
     * only configurable Command
     *
     * This Functional Unit may be used (derived from) if you don't need any
     * fancy things like special decorators or special
     * Receptor/Acceptor/Deliverer strategies.
     *
     * The FU looks like this:
     * @verbatim
     *
     * Receptor Deliverer
     *    |         |
     * -----------------
     * |               |
     * |     Plain     |
     * |               |
     * -----------------
     *        |
     *     Connector
     *
     * @verbatim
     */
        template <typename DERIVER, typename COMMAND = wns::ldk::EmptyCommand>
        class Plain :
                public virtual FunctionalUnit,
                public CommandTypeSpecifier<COMMAND>,
                public HasReceptor<>,
                public HasConnector<>,
                public HasDeliverer<>,
        public Cloneable<DERIVER>
        {
    public:
        /**
         * @brief Constructor
         */
        explicit
        Plain(fun::FUN* fuNet) :
            CommandTypeSpecifier<COMMAND>(fuNet)
            {
            }
        };

} // fu
} // ldk
} // wns

#endif // NOT defined WNS_LDK_SISOFU_HPP


