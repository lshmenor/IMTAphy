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

#ifndef GLUE_CONVERGENCE_LOWER2OFDMAPHY_HPP
#define GLUE_CONVERGENCE_LOWER2OFDMAPHY_HPP

#include <GLUE/convergence/Lower.hpp>

#include <WNS/service/phy/ofdma/DataTransmission.hpp>
#include <WNS/service/phy/ofdma/Notification.hpp>
#include <WNS/service/phy/ofdma/Handler.hpp>
#include <WNS/service/phy/phymode/PhyModeMapperInterface.hpp>

#include <WNS/service/dll/Address.hpp>

#include <map>

namespace glue { namespace convergence {

    class Lower2OFDMAPhy:
        virtual public Lower,
        public wns::ldk::CommandTypeSpecifier<LowerCommand>,
        public wns::ldk::HasReceptor<>,
        public wns::ldk::HasConnector<>,
        public wns::ldk::HasDeliverer<>,
        public wns::Cloneable<Lower2OFDMAPhy>,
        virtual public wns::service::phy::ofdma::Handler
    {

    public:
        Lower2OFDMAPhy(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config);
        virtual ~Lower2OFDMAPhy();

        // wns::service::phy::ofdma::Handler
        virtual void 
        onData(wns::osi::PDUPtr, wns::service::phy::power::PowerMeasurementPtr);

        virtual void 
        setNotificationService(wns::service::Service* phy);

        virtual wns::service::phy::ofdma::Notification* 
        getNotificationService() const;

        virtual void 
        setDataTransmissionService(wns::service::Service* phy);

        virtual wns::service::phy::ofdma::DataTransmission* 
        getDataTransmissionService() const;

        virtual void 
        setMACAddress(const wns::service::dll::UnicastAddress& address);

    private:
        // CompoundHandlerInterface
        virtual void 
        doSendData(const wns::ldk::CompoundPtr& sdu);
        
        virtual void 
        doOnData(const wns::ldk::CompoundPtr& compound);

        virtual void 
        onFUNCreated();

        virtual bool 
        doIsAccepting(const wns::ldk::CompoundPtr& compound) const;

        virtual void 
        doWakeup();

        bool hasCommandOf(FunctionalUnit* routing, const wns::ldk::CompoundPtr& compound) const
        {
            return getFUN()->getProxy()->commandIsActivated(compound->getCommandPool(), routing);
        }

        void
        stopTransmitting(const wns::ldk::CompoundPtr& compound);

        wns::logger::Logger logger_;

        struct Friends {
            UnicastUpper* unicastRouting;
            BroadcastUpper* broadcastRouting;
        } friends_;

        std::string upperUnicastName_;
        std::string upperBroadcastName_;

        bool transmitting_;

        wns::service::phy::ofdma::DataTransmission* dataTransmission_;
        wns::service::phy::ofdma::Notification* notificationService_;
        wns::service::dll::UnicastAddress address_;

        wns::service::phy::phymode::PhyModeMapperInterface* phyModeMapper_;
    };
} // convergence
} // glue

#endif // NOT defined GLUE_CONVERGENCE_LOWER2OFDMAPHY_HPP


