/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 16, D-52074 Aachen, Germany
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

#ifndef WNS_LONGCREATOR_HPP
#define WNS_LONGCREATOR_HPP

#include <WNS/StaticFactory.hpp>

namespace wns
{
    /**
     * @brief Creator implementation to be used with StaticFactory.
     * @author Marc Schinnenburg <marc@schinnenburg.com>
     */
    template <typename T, typename KIND = T>
    class LongCreator :
        public LongCreator<KIND, KIND>
    {
    public:
        virtual KIND* create(long l)
        {
            return new T(l);
        }
    };

    template <typename KIND>
    class LongCreator<KIND, KIND>
    {
    public:
        virtual
        ~LongCreator()
        {
        }

        virtual KIND*
        create(long l) = 0;
    };
}

#endif // NOT defined WNS_LONGCREATOR_HPP
