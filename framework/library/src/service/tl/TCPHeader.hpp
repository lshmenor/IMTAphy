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

#ifndef WNS_SERVICE_TL_TCPHEADER_HPP
#define WNS_SERVICE_TL_TCPHEADER_HPP

#include <WNS/service/tl/FlowID.hpp>

namespace wns { namespace service { namespace tl {

    /**
     * @brief Interface for the FlowID. Shared between UDP and TCP Headers.
     */
     class IFlowIDHeaderPart
     {
     public:

         virtual ~IFlowIDHeaderPart()
             {
             }

         virtual const FlowID& getFlowID() const = 0;

         virtual FlowID& getFlowID() = 0;

     };

    /**
     * @brief Interface for the TCP Header
     */
     class ITCPHeader:
        public IFlowIDHeaderPart
     {
     public:
         virtual ~ITCPHeader()
             {
             }

         virtual const bool& getUrgentFlag() const = 0;

         virtual bool& getUrgentFlag() = 0;

         virtual const bool& getAckFlag() const = 0;

         virtual bool& getAckFlag() = 0;

         virtual const bool& getPushFlag() const = 0;

         virtual bool& getPushFlag() = 0;

         virtual const bool& getResetFlag() const = 0;

         virtual bool& getResetFlag() = 0;

         virtual const bool& getSynFlag() const = 0;

         virtual bool& getSynFlag() = 0;

         virtual const bool& getFinFlag() const = 0;

         virtual bool& getFinFlag() = 0;
     };

} // tl
} // service
} // wns

#endif // WNS_SERVICE_TL_TCPHEADER_HPP
