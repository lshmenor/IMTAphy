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

#ifndef DLL_COMPOUNDSWITCH_FILTER_FILTERMACADDRESS_HPP
#define DLL_COMPOUNDSWITCH_FILTER_FILTERMACADDRESS_HPP

#include <DLL/compoundSwitch/Filter.hpp>
#include <DLL/compoundSwitch/CompoundSwitch.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/service/dll/Address.hpp>

namespace dll { namespace compoundSwitch { namespace filter {

    /**
     * @brief This filter matches to the source or destination MAC Address which
     * is given in the upperConvergence command
     *
     * @author Sebastian Max <smx@comnets.rwth-aachen.de>
     *
     * In the PyConfig the MAC Address and the type (i.e. source or destination)
     * has to be set.
     *
     */
    class FilterMACAddress :
        public Filter
    {
    public:
        FilterMACAddress(CompoundSwitch* compoundSwitch, wns::pyconfig::View& config);

        ~FilterMACAddress();

        virtual void
        onFUNCreated();

        virtual bool
        filter(const wns::ldk::CompoundPtr& compound) const;


    private:
        std::string ucCommandName_;
        wns::service::dll::UnicastAddress filterAddress_;
        bool filterTarget_;
    };
}
}
}

#endif // not defined DLL_COMPOUNDSWITCH_FILTER_FILTERCOMMAND_HPP
