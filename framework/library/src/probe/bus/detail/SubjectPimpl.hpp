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

#ifndef WNS_PROBE_BUS_DETAIL_SUBJECTPIMPL_HPP
#define WNS_PROBE_BUS_DETAIL_SUBJECTPIMPL_HPP

#include <WNS/probe/bus/detail/IProbeBusNotification.hpp>
#include <WNS/probe/bus/Context.hpp>
#include <WNS/simulator/Time.hpp>
#include <WNS/Subject.hpp>

namespace wns { namespace probe { namespace bus { namespace detail {

    class SubjectPimpl:
        public wns::Subject<IProbeBusNotification>
    {
    public:
        void
        forwardMeasurement(const wns::simulator::Time& timestamp,
                           const double& aValue,
                           const IContext& theRegistry);

        void
        forwardOutput();
    };

}
}
}
}

#endif // WNS_PROBE_BUS_DETAIL_SUBJECTPIMPL_HPP
