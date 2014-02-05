/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2011
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

#ifndef WNS_SERVICE_PHY_IMTA_DATATRANSMISSION_HPP
#define WNS_SERVICE_PHY_IMTA_DATATRANSMISSION_HPP

#include <WNS/node/Node.hpp>
#include <WNS/PowerRatio.hpp>
#include <WNS/osi/PDU.hpp>
#include <WNS/service/Service.hpp>
#include <WNS/service/phy/imta/ChannelStateInfo.hpp>
#include <vector>

namespace wns { namespace service { namespace phy { namespace imta {
	
   
 
                class DataTransmission :
            public virtual service::Service {
                public:
                    virtual void registerTransmission(wns::node::Interface* destination,
                                                      // later: 
                                                      // - precoding (vector/matrix),
                                                      // - phymode
                                                      wns::Power txPower,
                                                      wns::osi::PDUPtr transportBlock,
                                                      PRBList transmitPRBs,
                                                      unsigned int TTI) = 0; 
                };
	
            }}}}
#endif
