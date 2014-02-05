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

#ifndef WNS_PROBE_BUS_DETAIL_MEASUREMENTFUNCTOR_HPP
#define WNS_PROBE_BUS_DETAIL_MEASUREMENTFUNCTOR_HPP

#include <WNS/simulator/Time.hpp>
#include <WNS/probe/bus/Context.hpp>

namespace wns { namespace probe { namespace bus { namespace detail {

    /**
     * @brief Functor that is used send notifies using the forwardMeasurement
     * method.
     *
     * @author Daniel Bueltmann <me@daniel-bueltmann.de>
     */
    class MeasurementFunctor
    {
        typedef void (IProbeBusNotification::*fPtr)(const wns::simulator::Time&,
                                                    const double&,
                                                    const IContext&);
    public:
        MeasurementFunctor(fPtr f,
                           const wns::simulator::Time& time,
                           const double& value,
                           const IContext& reg):
            f_(f),
            time_(time),
            value_(value),
            registry_(reg)
            {}

        void
        operator()(IProbeBusNotification* observer)
        {
            (*observer.*f_)(time_, value_, registry_);
        }

    private:
        fPtr f_;
        const wns::simulator::Time& time_;
        const double& value_;
        const IContext& registry_;
    };

}
}
}
}

#endif // WNS_PROBE_BUS_DETAIL_MEASUREMENTFUNCTOR_HPP
