/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2009
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

#ifndef WNS_SCHEDULER_ILINKADAPTATIONPROXY_HPP
#define WNS_SCHEDULER_ILINKADAPTATIONPROXY_HPP

#include <WNS/service/phy/phymode/PhyModeInterface.hpp>
#include <WNS/service/phy/phymode/PhyModeMapperInterface.hpp>
#include <WNS/scheduler/SchedulerTypes.hpp>

namespace wns { namespace scheduler {

        class ILinkAdaptationProxy {
        public:
            virtual UserID getUserForCID(ConnectionID cid) = 0;
            /**@brief determines Address of next hop */

            /** @brief retrieve info about a user's power capabilities */
            virtual wns::scheduler::PowerCapabilities
            getPowerCapabilities(const UserID user) const = 0;

            /** @brief retrieve info about a user's power capabilities */
            virtual wns::scheduler::PowerCapabilities
            getPowerCapabilities() const = 0;

            virtual wns::service::phy::phymode::PhyModeMapperInterface*
            getPhyModeMapper() const = 0;

            /**
             * @brief For Link Adaptation, returns the best a PHYmode for a given SINR.
             */
            //virtual wns::SmartPtr<const wns::service::phy::phymode::PhyModeInterface>
            virtual wns::service::phy::phymode::PhyModeInterfacePtr
            getBestPhyMode(const wns::Ratio&) = 0;

            /**
             * @brief Returns (an estimate of) the current interference level at the
             * specified user's location. Included in this figure is everything
             * besides the carrier signal, i.e. inter+intra cell interference and
             * noise. This information will usually be signalled back by the mobile
             * terminals and is needed for the correct SINR estimation when sending
             * to a mobile terminal.
             * By specifying the own user ID as returned by getMyUserID(), the
             * current interference level at the own station should be returned
             * which is also needed for SINR estimation in the receive case.
             * This method assumes a flat channel. For frequency-selective channels better use CQI.
             */

            virtual ChannelQualityOnOneSubChannel 
            estimateTxSINRAt(const UserID user, int slot = WIDEBAND, int timeslot = ANYTIME) = 0;

            virtual ChannelQualityOnOneSubChannel 
            estimateRxSINROf(const UserID user, int slot = WIDEBAND, int timeslot = ANYTIME) = 0;

            virtual wns::Ratio
            getEffectiveUplinkSINR(const wns::scheduler::UserID sender, 
                const std::set<unsigned int>& scs,
                const int, 
                const wns::Power& txPower) = 0;

            virtual wns::Ratio
            getEffectiveDownlinkSINR(const wns::scheduler::UserID receiver, 
                const std::set<unsigned int>& scs, 
                const int,
                const wns::Power& txPower,
                const bool worstCase = false) = 0;
                
            virtual void 
            updateUserSubchannels (const wns::scheduler::UserID user, std::set<int>& channels)=0;
        };

    }} // namespace wns::scheduler
#endif // WNS_SCHEDULER_ILINKADAPTATIONPROXY_HPP

