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

#ifndef IMTAPHY_INTERFACE_DATATRANSMISSION_HPP
#define IMTAPHY_INTERFACE_DATATRANSMISSION_HPP

#include <WNS/node/Node.hpp>
#include <WNS/PowerRatio.hpp>
#include <WNS/service/Service.hpp>
#include <WNS/ldk/Compound.hpp>
#include <IMTAPHY/interface/TransmissionStatus.hpp>
#include <vector>
#include <IMTAPHY/detail/LinearAlgebra.hpp>

namespace  imtaphy { namespace interface {
    
    typedef struct  {
        wns::Power power;
        imtaphy::detail::ComplexFloatMatrixPtr precoding;
    } PowerAndPrecoding;
    
    typedef std::map<imtaphy::interface::PRB, PowerAndPrecoding> PrbPowerPrecodingMap;
 
    class DataTransmission :
        public virtual wns::service::Service 
    {
    public:
        virtual void registerTransmission(wns::node::Interface* destination,
                                 const std::vector<wns::ldk::CompoundPtr> &transportBlocks,
                                 unsigned int numberOfLayers,
                                 const PrbPowerPrecodingMap &perPRBpowerAndPrecoding) = 0;
                                          
        virtual std::vector<wns::node::Interface*> getAssociatedNodes() = 0;
    };

}}
#endif
