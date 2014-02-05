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

#ifndef WNS_LDK_FLOWSEPARATOR_FLOWINFOPROVIDER_HPP
#define WNS_LDK_FLOWSEPARATOR_FLOWINFOPROVIDER_HPP

#include <WNS/ldk/ControlServiceInterface.hpp>

namespace wns { namespace ldk { namespace flowseparator {

    /**
     * @brief Interface to be used by the CreateOnValidFlow
     * Strategy
     */
    class FlowInfoProvider :
        virtual ControlServiceInterface
    {
    public:
        virtual
        ~FlowInfoProvider()
        {
        }

        /** @brief return whether a flow according to the given
         * key is known, i.e. whether a new fu instance may be
         * created
         */
        virtual bool
        isValidFlow(const ConstKeyPtr& key) const = 0;
    };


} // namespace flowseparator
} // namespace ldk
} // namespace wns

#endif // WNS_LDK_FLOWSEPARATOR_FLOWINFOPROVIDER_HPP
