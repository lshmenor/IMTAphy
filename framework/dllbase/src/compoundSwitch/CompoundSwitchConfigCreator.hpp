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

#ifndef DLL_COMPOUNDSWITCH_COMPOUNDSWITCHCONFIGCREATOR_HPP
#define DLL_COMPOUNDSWITCH_COMPOUNDSWITCHCONFIGCREATOR_HPP

#include <WNS/StaticFactory.hpp>
#include <WNS/pyconfig/View.hpp>

namespace dll { namespace compoundSwitch {

    class CompoundSwitch;

    /**
     * @brief Creator implementation to be used with StaticFactory.
     *
     * Useful for constructors with a CompoundSwitch and pyconfig::View
     * parameter.
     *
     */
    template <typename T, typename KIND = T>
    struct CompoundSwitchConfigCreator :
        public CompoundSwitchConfigCreator<KIND, KIND>
    {
        virtual KIND* create(CompoundSwitch* compoundSwitch, wns::pyconfig::View& config)
        {
            return new T(compoundSwitch, config);
        }
    };

    template <typename KIND>
    struct CompoundSwitchConfigCreator<KIND, KIND>
    {
    public:
        virtual KIND* create(CompoundSwitch*, wns::pyconfig::View&) = 0;

        virtual ~CompoundSwitchConfigCreator()
        {
        }
    };

}
}

#endif // NOT defined DLL_COMPOUNDSWITCH_COMPOUNDSWITCH_HPP

