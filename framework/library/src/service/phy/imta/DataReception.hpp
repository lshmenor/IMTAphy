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

#ifndef WNS_SERVICE_PHY_IMTA_DATARECEPTION_HPP
#define WNS_SERVICE_PHY_IMTA_DATARECEPTION_HPP

#include <WNS/osi/PDU.hpp>
#include <WNS/SmartPtr.hpp>
#include <WNS/node/Interface.hpp>
#include <WNS/service/phy/imta/ChannelStateInfo.hpp>

namespace wns { namespace service { namespace phy { namespace imta {
	
                class DataReception :
            public virtual service::Service {
                public:
                    virtual void onData(wns::osi::PDUPtr transportBlock, 
                                        wns::node::Interface* source, 
                                        wns::SmartPtr<ChannelStateInfo> csi) = 0;
                    
                };
                
            }}}}
#endif


  
