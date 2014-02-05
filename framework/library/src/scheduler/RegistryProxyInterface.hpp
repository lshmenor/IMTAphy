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

#ifndef WNS_SCHEDULER_REGISTRYPROXYINTERFACE_HPP
#define WNS_SCHEDULER_REGISTRYPROXYINTERFACE_HPP

#include <WNS/service/dll/Address.hpp>
#include <WNS/service/phy/phymode/PhyModeInterface.hpp>
#include <WNS/service/phy/phymode/PhyModeMapperInterface.hpp>
#include <WNS/scheduler/SchedulingMap.hpp>
#include <WNS/scheduler/SchedulerTypes.hpp>
#include <WNS/scheduler/ILinkAdaptationProxy.hpp>
#include <WNS/ldk/Classifier.hpp>
#include <WNS/PowerRatio.hpp>
#include <string>

namespace wns { namespace scheduler {
        namespace apcstrategy {
            class APCStrategyInterface;
        }
        namespace dsastrategy {
            class DSAStrategyInterface;
        }
    }}

namespace wns { namespace scheduler {

        /**
         * @brief System specific proxy that forwards queries from generic scheduler
         * components to the system specific entities. IDs such as UserIDs,
         * ConnectionIDs etc have to be mapped to the scheduler internal types so
         * that such system specific details are hidden.
         */
        class RegistryProxyInterface :
            public ILinkAdaptationProxy
        {
        public:
            virtual ~RegistryProxyInterface() {};
            /**
             * @brief Retrieve the peer user at the other end of the connection that
             * is specified by the connection id. Both CID and UserID are scheduler
             * internal types and might differ from those used in the host
             * system. In that case, they have to be mapped accordingly.
             *
             */
            virtual UserID getUserForCID(ConnectionID cid) = 0;
            /**@brief determines Address of next hop */
            virtual wns::service::dll::UnicastAddress
            getPeerAddressForCID(wns::scheduler::ConnectionID cid) = 0;
            /**
             * @brief Retrieve a list of (internal) Connection IDs that the specified
             * user is associated with. The convention is that the strategy serves
             * the different connections sequentially in the order they are
             * contained in the vector. Thus, connection IDs should be ordered by
             * descending priority, e.g. control/management connections should come
             * first.
             * Again, these IDs might differ externally in
             * the system.
             */
            virtual ConnectionVector getConnectionsForUser(const UserID user) = 0;
            /**
             * @brief Retrieves a scheduler internal CID from the compound. This is
             * usually done by asking a friend FU, e.g., the classifier.
             */
            virtual ConnectionID getCIDforPDU(const wns::ldk::CompoundPtr& compound) = 0;
            /**
             * @brief Function that is used by the scheduler FU that instantiates
             * the different scheduling objects such as the registry. The FU can do
             * a findFriend to retrieve a pointer to the classifier. This is given
             * to the registry with this function.
             *
             * \todo could be abolished because registry knows fun and could do it
             *by itself
             */
            virtual void setFriends(const wns::ldk::CommandTypeSpecifierInterface* _classifier) = 0;
            /**
             * @brief By calling this function, the scheduler FU gives the registry
             * a pointer to the FUN it belongs to. The registry needs this, e.g., to
             * get the layer and references to entities such as station/conn managers.
             */
            virtual void setFUN(const wns::ldk::fun::FUN* fun) = 0;
            /**
             * @brief Useful for debugging/logging purposes to attach meaningful
             * names to a user internally only known by its UserID (wns::node::Node*)
             */
            virtual std::string getNameForUser(const UserID user) = 0;
            /**
             * @brief Returns the station's own UserID.
             */
            virtual UserID getMyUserID() = 0;

            /** @brief ??? */
            virtual Bits getQueueSizeLimitPerConnection() = 0;

            /**
             * @brief Returns the type of the user, e.g. base station, relay station,
             * subscriber station
             */
            virtual wns::service::dll::StationType
            getStationType(const wns::scheduler::UserID user) = 0;

            /**
             * @brief filters the given set of users and return only those that can
             * be reached within the next scheduling round.
             * Soon obsolete.
             * Use filterReachable(UserSet users, const int frameNr) for future code.
             */
            virtual UserSet filterReachable(UserSet users) = 0;

            /**
             * @brief filters the given set of users and return only those that can
             * be reached in the given frame
             */
            virtual UserSet filterReachable(UserSet users, const int frameNr) = 0;

            /**
             * @brief filters the given set of connections and return only those that can
             * be reached in the given frame
             */
            virtual wns::scheduler::ConnectionSet
            filterReachable(wns::scheduler::ConnectionSet connections, const int frameNr, bool usesHARQ) = 0;

            /** @brief get the ChannelsQualities (CQI) on all the subbands of the user.
                Eventually for a future frameNr (prediction). */
            virtual ChannelQualitiesOnAllSubBandsPtr getChannelQualities4UserOnUplink(UserID user, int frameNr) = 0;
            virtual ChannelQualitiesOnAllSubBandsPtr getChannelQualities4UserOnDownlink(UserID user, int frameNr) = 0;
	    
	    /**
	     *@todo dbn: DEPRECATED: calcULResources is only used in old PCRR scheduling strategy. To be removed
	     */
            /** @brief returns information for all active uplink users about
                their requested share of the overall resources, the maximum number of subbands
                that may be allocated to them and the ul txpower they may use. */
            virtual wns::scheduler::PowerMap
            calcULResources(const wns::scheduler::UserSet&, unsigned long int) const = 0;

            virtual wns::scheduler::UserSet
            getActiveULUsers() const = 0;

            /** @brief returns one for UTs, and #connected UTs in case of RNs */
            virtual int
            getTotalNumberOfUsers(const wns::scheduler::UserID user) = 0;

            /** @brief retrieve the the number of priorities,
                mapped from the QoS-Classes */
            virtual int
            getNumberOfPriorities() = 0;

            /** @brief registerCID */
            virtual void
            registerCID(wns::scheduler::ConnectionID cid, wns::scheduler::UserID userID/*nextHop!*/) = 0;

            /** @brief deregisterCID (important e.g. for Handover) */
            virtual void
            deregisterCID(wns::scheduler::ConnectionID cid, const wns::scheduler::UserID userID) = 0;

            /** @brief deregisterUser (important e.g. for Handover) */
            virtual void
            deregisterUser(const wns::scheduler::UserID userID) = 0;

            /** @brief retrieve a list of Connections for the same priority */
            virtual ConnectionList&
            getCIDListForPriority(int priority) = 0;

            /** @brief retrieve a set of Connections for the same priority */
            virtual wns::scheduler::ConnectionSet
            getConnectionsForPriority(int priority) = 0;

            /** @brief tells priority of a connection */
            virtual int
            getPriorityForConnection(wns::scheduler::ConnectionID cid) = 0;

            /** @brief true for downlink master scheduler */
            virtual bool
            getDL() const = 0;

            /** @brief tells if CQI system is available.
                Doesn't mean that at this moment there is a valid CQI information. */
            virtual bool
            getCQIAvailable() const = 0;
        };

        typedef wns::ldk::FUNConfigCreator<RegistryProxyInterface> RegistryCreator;
        typedef wns::StaticFactory<RegistryCreator> RegistryFactory;

    }} // namespace wns::scheduler
#endif // WNS_SCHEDULER_REGISTRYPROXYINTERFACE_HPP

