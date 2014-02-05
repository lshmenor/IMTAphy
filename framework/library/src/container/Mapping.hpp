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

#ifndef WNS_CONTAINER_MAPPING_HPP
#define WNS_CONTAINER_MAPPING_HPP

#include <WNS/Assure.hpp>
#include <map>
#include <vector>

namespace wns { namespace container {
    /**
     * @brief use this class as a base if you have to map from a certain x value
     * (discrete xvalues allowed only) to a dedicated y value and you have to
     * distinguish between different mapping curves through a parameter. You
     * have to fill the MappingField in the derived class and make sure you look
     * up with valid key values.
     */
    template <typename PARAMETER, typename RESULTTYPE>
    class Mapping
    {
    public:
        typedef std::vector<RESULTTYPE> MappingFunction;
        typedef std::map<PARAMETER, MappingFunction> MappingField;

        Mapping(unsigned int _offset = 0) :
            mf(),
            offset(_offset)
        {
        };

        RESULTTYPE map(const int xvalue, PARAMETER Parameter) const
        {
            typename MappingField::const_iterator itr = mf.find(Parameter);
            assure( itr != mf.end(), " Mapping Parameter not found!" );

            assure( xvalue + offset < (*itr).second.size(), " Mapping xvalue out of Range!");
            return (*itr).second.at(xvalue + offset);
        }

    protected:
        /**
         * @todo pab: make this more efficient, e.g. static or so.
         */
        MappingField mf;
        unsigned int offset;
    };
} // container
} // wns
#endif // WNS_CONTAINER_MAPPING_HPP
