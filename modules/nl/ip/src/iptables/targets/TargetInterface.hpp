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

#ifndef IP_IPTABLES_TARGETS_TARGETINTERFACE_HPP
#define IP_IPTABLES_TARGETS_TARGETINTERFACE_HPP

#include <WNS/service/tl/TCPHeader.hpp>
#include <WNS/service/tl/UDPHeader.hpp>

namespace ip { namespace iptables { namespace targets {

    enum TargetResult {
        DROP = 1,
        ACCEPT = 2,
        CONT = 3
    };

    class TargetInterface
    {
    public:

        virtual
        ~TargetInterface() {};

        virtual TargetResult
        mangle(IPCommand*) = 0;

        virtual TargetResult
        mangle(IPCommand*, wns::service::tl::ITCPHeader*) = 0;

        virtual TargetResult
        mangle(IPCommand*, wns::service::tl::IUDPHeader*) = 0;
    };

    typedef wns::PyConfigViewCreator<TargetInterface, TargetInterface> TargetInterfaceCreator;

    typedef wns::StaticFactory<TargetInterfaceCreator> TargetInterfaceFactory;

} // targets
} // iptables
} // ip
#endif // IP_IPTABLES_TARGETS_TARGETINTERFACE_HPP
