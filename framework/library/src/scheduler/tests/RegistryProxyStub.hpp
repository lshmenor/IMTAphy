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

#ifndef WNS_SCHEDULER_TESTS_REGISTRYPROXYSTUB_HPP
#define WNS_SCHEDULER_TESTS_REGISTRYPROXYSTUB_HPP

#include <WNS/scheduler/RegistryProxyInterface.hpp>
#include <WNS/scheduler/SchedulerTypes.hpp>

#include <WNS/Interval.hpp>
#include <WNS/container/RangeMap.hpp>
#include <WNS/service/phy/phymode/PhyModeMapperInterface.hpp>
#include <map>
#include <string>

namespace wns { namespace scheduler { namespace tests {

            /** @brief this class is used for tests only */
            class RegistryProxyStub
                    : public RegistryProxyInterface {
            public:
                RegistryProxyStub();
                ~RegistryProxyStub();

                // The Interface:
                UserID getUserForCID(ConnectionID cid);
                wns::service::dll::UnicastAddress getPeerAddressForCID(wns::scheduler::ConnectionID cid);
                ConnectionVector getConnectionsForUser(const UserID user);
                ConnectionID getCIDforPDU(const wns::ldk::CompoundPtr& compound);
                void setFriends(const wns::ldk::CommandTypeSpecifierInterface* _classifier);
                void setFUN(const wns::ldk::fun::FUN* fun);
                std::string getNameForUser(const UserID user);
                wns::service::phy::phymode::PhyModeMapperInterface* getPhyModeMapper() const;
                wns::SmartPtr<const wns::service::phy::phymode::PhyModeInterface> getBestPhyMode(const wns::Ratio& sinr);
                UserID getMyUserID();
                simTimeType getOverhead(int numBursts);
                ChannelQualityOnOneSubChannel estimateTxSINRAt(const UserID user, int slot, int timeSlot);
                ChannelQualityOnOneSubChannel estimateRxSINROf(const UserID user, int slot, int timeSlot);
                Bits getQueueSizeLimitPerConnection();
                void setQueueSizeLimitPerConnection(Bits bits);
                wns::service::dll::StationType getStationType(const UserID user);
                UserSet filterReachable(UserSet users); // soon obsolete
                UserSet filterReachable(UserSet users, const int frameNr);
                wns::scheduler::ConnectionSet filterReachable(wns::scheduler::ConnectionSet connections, const int frameNr, bool useHARQ);
                wns::scheduler::PowerMap calcULResources(const wns::scheduler::UserSet&, unsigned long int) const;
                wns::scheduler::UserSet getActiveULUsers() const;
                int getTotalNumberOfUsers(const wns::scheduler::UserID user);

                virtual ChannelQualitiesOnAllSubBandsPtr getChannelQualities4UserOnUplink(UserID user, int frameNr);
                virtual ChannelQualitiesOnAllSubBandsPtr getChannelQualities4UserOnDownlink(UserID user, int frameNr);

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

                int
                getNumberOfPriorities();

                void
                setNumberOfPriorities(int num);

                virtual void
                registerCID(wns::scheduler::ConnectionID cid, wns::scheduler::UserID userID/*nextHop!*/) {};

                virtual void
                deregisterCID(wns::scheduler::ConnectionID cid, const wns::scheduler::UserID userID) {};

                virtual void
                deregisterUser(const wns::scheduler::UserID userID) {};

                wns::scheduler::ConnectionList&
                getCIDListForPriority(int priority);

                wns::scheduler::ConnectionSet
                getConnectionsForPriority(int /*priority*/);

                std::string
                compoundInfo(const wns::ldk::CompoundPtr& compound);

                const wns::service::phy::phymode::PhyModeInterfacePtr
                getPhyMode(ConnectionID /*cid*/);

                int
                getPriorityForConnection(wns::scheduler::ConnectionID /*cid*/);

                bool
                getDL() const;

                virtual bool
                getCQIAvailable() const;

                // The functions to modify the stub's state and define the return values
                void setCIDforPDU(const wns::ldk::CompoundPtr& compound, ConnectionID cid);
                void associateCIDandUser(ConnectionID cid, UserID user);

            private:
                std::map<wns::ldk::CompoundPtr, ConnectionID> compound2CIDmap;
                std::map<UserID, ConnectionVector> User2CID;
                std::map<ConnectionID, UserID> CID2User;
                wns::ldk::CommandTypeSpecifierInterface* classifier;
                wns::service::phy::phymode::PhyModeInterfacePtr phyMode;
                wns::service::phy::phymode::PhyModeMapperInterface *phyModeMapper;
                UserID myUserID;
                Bits queueSizeLimit;
                int numberOfPriorities;
            };
        } // tests
    } // scheduler
} // wns
#endif // WNS_SCHEDULER_TESTS_REGISTRYPROXYSTUB_HPP


