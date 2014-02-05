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

#ifndef DLL_COMPOUNDSWITCH_FILTER_HPP
#define DLL_COMPOUNDSWITCH_FILTER_HPP

#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/StaticFactory.hpp>
#include <WNS/ldk/Compound.hpp>

#include <DLL/compoundSwitch/CompoundSwitchConfigCreator.hpp>

namespace dll { namespace compoundSwitch {

    class CompoundSwitch;

    /**
     * @brief Interface of basic class Filter for all CompoundSwitch filters.
     * @author Markus Grauer <gra@comnets.rwth-aachen.de>
     *
     */
    class IFilter
    {
    public:

        virtual
        ~IFilter()
        {
        }

        virtual void
        onFUNCreated() = 0;

        virtual bool
        filter(const wns::ldk::CompoundPtr& compound) const = 0;

        virtual std::string
        getName() const = 0;

    };

    /**
     * @brief Basic class Filter for all CompoundSwitch filters.
     * @author Markus Grauer <gra@comnets.rwth-aachen.de>
     *
     */
    class Filter:
        public IFilter
    {
    public:

        Filter(CompoundSwitch* compoundSwitch, wns::pyconfig::View& config);

        virtual
        ~Filter();

        virtual void
        onFUNCreated()
            {
            }

        virtual bool
        filter(const wns::ldk::CompoundPtr& compound) const = 0;

        virtual std::string
        getName() const
            {
                return name_;
            }

        bool
        operator==( const Filter& rhs)
            {
                return getName() == rhs.getName();
            }



    protected:

        std::string name_;

        CompoundSwitch* compoundSwitch_;
        wns::logger::Logger logger_;
    };

    typedef compoundSwitch::CompoundSwitchConfigCreator<Filter> FilterCreator;
    typedef wns::StaticFactory<FilterCreator> FilterFactory;

}
}

#endif // NOT defined DLL_COMPOUNDSWITCH_FILTER_HPP
