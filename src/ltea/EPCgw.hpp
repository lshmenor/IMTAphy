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

// based on code from openWNS with the following license:

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

#ifndef LTEA_EPCGW_HPP
#define LTEA_EPCGW_HPP

#include <IMTAPHY/ltea/Layer2.hpp>

#include <DLL/RANG.hpp>
#include <DLL/Layer2.hpp>
#include <DLL/StationManager.hpp>

#include <WNS/service/dll/FlowID.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/module/Base.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/node/component/Component.hpp>
#include <WNS/events/PeriodicTimeout.hpp>
#include <WNS/container/Registry.hpp>
#include <WNS/container/Pool.hpp>

namespace ltea {
    namespace pdcp{
    class PDCPeNB;
    }

    class EPCgw :
        public dll::RANG
    {
    public:
        EPCgw(wns::node::Interface*, const wns::pyconfig::View&);
        
        virtual ~EPCgw() {};

        /** @name wns::service::dll::DataTransmission service */
        //@{
        virtual void
        sendData(
             const wns::service::dll::UnicastAddress& _peer,
             const wns::osi::PDUPtr& _data,
             wns::service::dll::protocolNumber protocol,
             wns::service::dll::FlowID _epsBearer);

             
/*             
        wns::service::dll::FlowID
        onEPSbearerRequest(ltea::pdcp::PDCPeNB* _eNBpdcp,
                        wns::service::dll::UnicastAddress ueAddress);

        void
        onEPSbearerRelease(wns::service::dll::FlowID epsBearer);

        void
        registerIRuleControl(wns::service::dll::IRuleControl*);

        void
        deleteEPSbearer(wns::service::dll::FlowID epsBearer);
  
        */
        void
        setAssociatedUEs(std::vector< wns::node::Interface* > associatedUEs, ltea::pdcp::PDCPeNB* pdcp);

    private:
//        typedef wns::container::Registry<wns::service::dll::FlowID, wns::service::dll::UnicastAddress> EPSbearerTable;

        wns::pyconfig::View config;

        wns::logger::Logger logger;
        
//        wns::container::Pool<int> epsBearerPool;

/*
        wns::service::dll::IRuleControl* ruleControl;

        // saves FlowIDs to Basestation mapping
        wns::container::Registry<wns::service::dll::FlowID, ltea::pdcp::PDCPeNB*> epsBearerToeNB;

        EPSbearerTable epsBearerForUE;
        */
    };
}


#endif 
