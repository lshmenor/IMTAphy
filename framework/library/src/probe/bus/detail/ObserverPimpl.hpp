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

#ifndef WNS_PROBE_BUS_DETAIL_OBSERVERPIMPL_HPP
#define WNS_PROBE_BUS_DETAIL_OBSERVERPIMPL_HPP

#include <WNS/probe/bus/detail/IProbeBusNotification.hpp>
#include <WNS/Observer.hpp>

namespace wns { namespace probe { namespace bus { class ProbeBus; }}}

namespace wns { namespace probe { namespace bus { namespace detail {

    class ObserverPimpl:
        public wns::Observer<IProbeBusNotification>        
    {

    public:
        ObserverPimpl(wns::probe::bus::ProbeBus*);

        virtual void
        forwardMeasurement(const wns::simulator::Time&,
                           const double&,
                           const IContext&);

        virtual void
        forwardOutput();

    private:
        wns::probe::bus::ProbeBus* pb_;
    };

}
}
}
}

#endif // WNS_PROBE_BUS_DETAIL_OBSERVERPIMPL_HPP
