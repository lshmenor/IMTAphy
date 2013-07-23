/*******************************************************************************
 * This file is part of IMTAphy
 * _____________________________________________________________________________
 *
 * Copyright (C) 2011
 * Institute of Communication Networks (LKN)
 * Department of Electrical Engineering and Information Technology (EE & IT)
 * Technische Universitaet Muenchen
 * Arcisstr. 21
 * 80333 Muenchen - Germany
 * http://www.lkn.ei.tum.de/~jan/imtaphy/index.html
 * 
 * _____________________________________________________________________________
 *
 *   IMTAphy is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   IMTAphy is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with IMTAphy.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#ifndef LTEA_PDCP_COMMAND_HPP
#define LTEA_PDCP_COMMAND_HPP

#include <WNS/ldk/Command.hpp>
#include <WNS/service/dll/Address.hpp>
#include <WNS/service/dll/FlowID.hpp>

#include <IMTAPHY/ltea/helper/Types.hpp>

namespace ltea { namespace pdcp {

    class PDCPCommand :
       public wns::ldk::Command
    {
    public:
      PDCPCommand()
      {
        peer.destination = wns::service::dll::UnicastAddress(); // temporary UE address
        peer.bearerID = -1;
      }

      struct
      {
          wns::service::dll::FlowID bearerID;
      } epc;

      struct
      {
          ltea::rlc::RLCmode rlcMode; // UM, AM, or TM
          // QoS
      } local;


      struct
      {
          // TODO: do we really need these or should we have these??
          wns::service::dll::UnicastAddress source;
          wns::service::dll::UnicastAddress destination; // E2E address

          wns::service::dll::FlowID bearerID;
      } peer;

      struct
      {
      } magic;

    };

} // pdcp
} // ltea

#endif