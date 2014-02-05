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

#ifndef WNS_SCHEDULER_TESTS_LINKADAPTATIONPROXY_HPP
#define WNS_SCHEDULER_TESTS_LINKADAPTATIONPROXY_HPP

#include <WNS/scheduler/ILinkAdaptationProxy.hpp>
#include <WNS/scheduler/SchedulerTypes.hpp>

namespace wns { namespace scheduler { namespace tests {

            /** @brief this class is used for tests only */
            class LinkAdaptationProxyStub
                    : public ILinkAdaptationProxy {
            public:
                LinkAdaptationProxyStub();
                ~LinkAdaptationProxyStub();

                UserID 
                getUserForCID(ConnectionID cid);

                wns::service::phy::phymode::PhyModeMapperInterface*
                getPhyModeMapper() const;

                wns::service::phy::phymode::PhyModeInterfacePtr
                getBestPhyMode(const wns::Ratio&);
                
                ChannelQualityOnOneSubChannel 
                estimateTxSINRAt(const UserID user, int slot, int timeSlot = ANYTIME);
                
                ChannelQualityOnOneSubChannel 
                estimateRxSINROf(const UserID user, int slot, int timeSlot = ANYTIME);

                wns::Ratio
                getEffectiveUplinkSINR(const wns::scheduler::UserID sender, 
                    const std::set<unsigned int>& scs, 
                    const int timeSlot,
                    const wns::Power& txPower);

                wns::Ratio
                getEffectiveDownlinkSINR(const wns::scheduler::UserID receiver, 
                    const std::set<unsigned int>& scs, 
                    const int timeSlot,
                    const wns::Power& txPower,
                    const bool worstCase = false);
                    
            
                void 
                updateUserSubchannels (const wns::scheduler::UserID user, std::set<int>& channels);


                virtual wns::scheduler::PowerCapabilities
                getPowerCapabilities(const UserID user) const;

                virtual wns::scheduler::PowerCapabilities
                getPowerCapabilities() const;

            private:
                wns::service::phy::phymode::PhyModeMapperInterface *phyModeMapper;
            };
        } // tests
    } // scheduler
} // wns
#endif // WNS_SCHEDULER_TESTS_LINKADAPTATIONPROXY_HPP


