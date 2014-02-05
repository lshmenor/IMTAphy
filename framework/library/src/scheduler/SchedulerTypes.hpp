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

#ifndef WNS_SCHEDULER_SCHEDULERTYPES_HPP
#define WNS_SCHEDULER_SCHEDULERTYPES_HPP


#include <WNS/service/dll/StationTypes.hpp>
#include <WNS/service/phy/ofdma/Pattern.hpp>
#include <WNS/service/phy/phymode/PhyModeInterface.hpp>
#include <WNS/ldk/Command.hpp>
#include <WNS/ldk/Compound.hpp>
#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/ldk/Classifier.hpp>
#include <WNS/node/Node.hpp>
#include <WNS/PowerRatio.hpp>
#include <WNS/CandI.hpp>
#include <WNS/Enum.hpp>
#include <WNS/SmartPtr.hpp>
#include <WNS/Birthmark.hpp>

#include <map>
#include <vector>
#include <set>
#include <list>
#include <string>
#include <sstream>

namespace wns { namespace scheduler {
        class MapInfoEntry; // forward declaration
        namespace strategy {
            class StrategyInput;
            class StrategyInterface;
        }

#define WIDEBAND std::numeric_limits<int>::max()
#define ANYTIME std::numeric_limits<int>::max()

    class UserID:
        public wns::IOutputStreamable,
        public wns::HasBirthmark
    {
    public:
        explicit UserID() { node_ = NULL; isBroadcast_ = false; };

        UserID(const UserID& other) { node_ = other.node_; isBroadcast_ = other.isBroadcast_; }

        explicit UserID(wns::node::Interface* node) { node_ = node; isBroadcast_ = false; };

        virtual ~UserID() {};

        bool
        isValid() const
        {
            if (!isBroadcast() && node_ != NULL)
            {
                return true;
            }
            if (isBroadcast() && node_ == NULL)
            {
                return true;
            }
            return false;
        }

        void
        setBroadcast()
        {
            isBroadcast_ = true;
        }

        bool
        isBroadcast() const
        {
            return isBroadcast_;
        }

        std::string
        getName() const
        {
            if (node_ != NULL)
            {
                return node_->getName();
            }
            else
            {
                if (isBroadcast())
                {
                    return "Broadcast";
                }
            }
            return "Unknown";
        }

        wns::node::Interface*
        getNode() const
        {
            assure(!isBroadcast(), "You asked for a Node of a broadcast user. This is invalid. Use isBroadcast() to check prior to your code");
            assure(isValid(), "UserID is invalid. You cannot ask for the node here. Use isValid() to check prior to your code");
            return node_;
        }

        int
        getNodeID() const
        {
            assure(!isBroadcast(), "You asked for a NodeID of a broadcast user. This is invalid. Use isBroadcast() to check prior to your code");
            assure(isValid(), "UserID is invalid. You cannot ask for the NodeID here. Use isValid() to check prior to your code");
            return node_->getNodeID();
        }

        bool
        operator== (const UserID& other) const
        {
            return ( (node_ == other.node_) && (isBroadcast() == other.isBroadcast()));
        }

        bool
        operator!= (const UserID& other) const
        {
            return !((*this) == other);
        }

        bool
        operator< (const UserID& other) const
        {
            if (isBroadcast() && !other.isBroadcast())
            {
                return false;
            }
            else if(!isBroadcast() && other.isBroadcast())
            {
                return true;
            }
            else if(isBroadcast() && other.isBroadcast())
            {
                return getBirthmark() < other.getBirthmark();
            }
            else
            {
                return getNodeID() < other.getNodeID();
            }
        }

        void
        operator= (const UserID& other)
        {
            node_ = other.node_;
            isBroadcast_ = other.isBroadcast_;
        }

        wns::probe::bus::ContextProviderCollection&
        getContextProviderCollection() const
        {
            assure(!isBroadcast(), "You asked for a ContextProviderCollection of a broadcast user. This is invalid. Use isBroadcast() to check prior to your code");
            assure(isValid(), "UserID is invalid. You cannot ask for the ContextProviderCollection here. Use isValid() to check prior to your code");
            return node_->getContextProviderCollection();
        }

    private:

        virtual std::string
        doToString() const
        {
            return getName();
        }

        wns::node::Interface* node_;

        bool isBroadcast_;
    };

        const simTimeType slotLengthRoundingTolerance = 1e-12;
        const int subChannelNotFound = -1;

        typedef int Symbols;
        typedef int Bits;

        /** @brief there are three positions for the scheduler... */
        typedef enum {
            PowerControlDLMaster,
            PowerControlULMaster,
            PowerControlULSlave
        } PowerControlType;
        // wns::scheduler::PowerControlType

        /** @brief there are three positions for the scheduler... */
        // wns::scheduler::SchedulerSpot
        ENUM_BEGIN(SchedulerSpot);
        ENUM(DLMaster,  1);
        ENUM(ULMaster,  2);
        ENUM(ULSlave,   3);
        ENUM_END();
        // wns::scheduler::SchedulerSpot::DLMaster()
        // wns::scheduler::SchedulerSpot::toString()
        /** @brief wns::scheduler::SchedulerSpotType */
        typedef int SchedulerSpotType;
        // allow switch-case statements:
        static const SchedulerSpotType SchedulerSpotDLMaster=wns::scheduler::SchedulerSpot::DLMaster();
        static const SchedulerSpotType SchedulerSpotULMaster=wns::scheduler::SchedulerSpot::ULMaster();
        static const SchedulerSpotType SchedulerSpotULSlave=wns::scheduler::SchedulerSpot::ULSlave();

        /** @brief for RelayNodes: in which task are we */
        ENUM_BEGIN(TaskBSorUT);
        ENUM(TaskBaseStation,  1);
        ENUM(TaskUserTerminal, 2);
        ENUM_END();
        // wns::scheduler::TaskBSorUT::TaskBaseStation()
        // wns::scheduler::TaskBSorUT::toString()
        /** @brief wns::scheduler::TaskBSorUTType */
        typedef int TaskBSorUTType;

        typedef std::list<wns::ldk::CompoundPtr> CompoundList;
        typedef wns::ldk::ClassificationID ConnectionID;
        typedef std::vector<ConnectionID> ConnectionVector;
        typedef std::list  <ConnectionID> ConnectionList;
        typedef std::set   <ConnectionID> ConnectionSet;
        typedef std::set   <UserID>       UserSet;
        /** @brief Group is quite an antiquated datastructure. Only used for grouping in WiMAC */
        typedef std::map   <UserID, wns::CandI> Group;
        typedef std::vector<bool>         UsableSubChannelVector;

        inline std::string
        printBoolVector(const std::vector<bool>& boolVector) {
            std::stringstream s;
            for (std::vector<bool>::const_iterator iter = boolVector.begin(); iter != boolVector.end(); ++iter)
            {
                s << (*iter) ? 1:0;
            }
            return s.str();
        }

        inline std::string
        printGroup(const wns::scheduler::Group& group) {
            std::stringstream s;
            s << "Group(";
            for (Group::const_iterator iter = group.begin(); iter != group.end(); )
            {
                s << iter->first.getName();
                if (++iter != group.end()) s << ",";
            }
            s << ")";
            return s.str();
        }

        inline std::string
        printUserSet(const wns::scheduler::UserSet& userSet) {
            std::stringstream s;
            s << "UserSet(";
            for (UserSet::const_iterator iter = userSet.begin(); iter != userSet.end(); )
            {
                s << iter->getName();
                if (++iter != userSet.end()) s << ",";
            }
            s << ")";
            return s.str();
        }

        inline std::string
        printConnectionVector(const wns::scheduler::ConnectionVector& connectionVector) {
            std::stringstream s;
            s << "ConnectionVector(";
            for (ConnectionVector::const_iterator iter = connectionVector.begin(); iter != connectionVector.end(); )
            {
                s << (*iter);
                if (++iter != connectionVector.end()) s << ",";
            }
            s << ")";
            return s.str();
        }

        inline std::string
        printConnectionList(const wns::scheduler::ConnectionList& connectionList) {
            std::stringstream s;
            s << "ConnectionList(";
            for (ConnectionList::const_iterator iter = connectionList.begin(); iter != connectionList.end(); )
            {
                s << (*iter);
                if (++iter != connectionList.end()) s << ",";
            }
            s << ")";
            return s.str();
        }

        inline std::string
        printConnectionSet(const wns::scheduler::ConnectionSet& connectionSet) {
            std::stringstream s;
            s << "ConnectionSet(";
            for (ConnectionSet::const_iterator iter = connectionSet.begin(); iter != connectionSet.end(); )
            {
                s << (*iter);
                if (++iter != connectionSet.end()) s << ",";
            }
            s << ")";
            return s.str();
        }

        /** @brief stream operator for class Group */
        inline std::ostream&
        operator<< (std::ostream& s, const wns::scheduler::Group& group) {
            s << "Group(";
            for (Group::const_iterator iter = group.begin(); iter != group.end(); )
            {
                s << iter->first.getName();
                if (++iter != group.end()) s << ",";
            }
            s << ")";
            return s;
        }

        /** @brief Data struct used to exchange info about user's power
         * capabilities */
        struct PowerCapabilities
        {
            PowerCapabilities() :
                maxPerSubband(),
                nominalPerSubband(),
                maxOverall()
            {}

            explicit
            PowerCapabilities(const wns::pyconfig::View& config) :
                maxOverall(config.get<wns::Power>("maxOverall")),
                maxPerSubband(config.get<wns::Power>("maxPerSubband")),
                nominalPerSubband(config.get<wns::Power>("nominalPerSubband"))
            {}

            /** @brief total transmit power limit over the whole bandwidth */
            wns::Power maxOverall;
            /** @brief transmit power limit over one subChannel */
            wns::Power maxPerSubband;
            /** @brief default transmit power over one subChannel */
            wns::Power nominalPerSubband;
        }; // PowerCapabilities

        /** @brief Info about joint power and resource allocation, currently
         * used by PCRR scheduling strategy */
        struct PowerAllocation {
            wns::Power txPowerPerSubband;
            UserID user;
            unsigned long int maxNumSubbands;
            double resourceShare;

            PowerAllocation() :
                txPowerPerSubband(),
                user(),
                maxNumSubbands(),
                resourceShare()
            {}

            std::string
            toString() const
            {
                std::stringstream ss;
                ss << user.getName() << ": " << maxNumSubbands << " simultaneously with " << txPowerPerSubband << " each." << std::endl;
                return ss.str();
            }
        }; // PowerAllocation

        typedef std::map<UserID, PowerAllocation> PowerMap;
        typedef std::map<UserID, wns::service::phy::ofdma::PatternPtr> AntennaPatternsPerUser;

        class Grouping
                : virtual public wns::RefCountable
        {
        public:
            Grouping() {};

            ~Grouping() {};

            std::string getDebugOutput() {
                std::stringstream ss;
                for (unsigned int i = 0; i < groups.size(); ++i) {
                    ss << "Group" << i << "(";
                    for (Group::iterator iter = groups[i].begin();
                         iter != groups[i].end();
                         ++iter)
                        ss << (*iter).first.getName() << " @ "
                           << (*iter).second.C / (*iter).second.I << ", )" << std::endl;
                }
                return ss.str();
            }

            wns::Ratio shareOfPowerPerStreams(unsigned int groupNumber){

                return shareOfPowerPerStreams( groups.at(groupNumber) );

            }

            wns::Ratio shareOfPowerPerStreams(Group group){
                // transmitting multiple streams the available Tx power
                // is shared among all streams
                assure( !group.empty() , "group does not exists");

                unsigned int concurrentStreams = group.size();
                return wns::Ratio().from_factor(1.0 / concurrentStreams);
            }

            wns::Ratio eirpReductionOfPower(unsigned int groupNumber){

                return eirpReductionOfPower( groups.at(groupNumber) );

            }

            wns::Ratio eirpReductionOfPower(Group group){
                // in EIRP limited spectrum, the maximum adaptive
                // antenna gain of the sum of all patterns have to be
                // normalised to one
                assure( !group.empty() , "group does not exists");

                wns::service::phy::ofdma::SumPattern sumPattern;
                assure(sumPattern.getSize() == 0, "new sum pattern is not empty");

                for (Group::iterator itr = group.begin();
                     itr != group.end();
                     itr++) {
                    wns::service::phy::ofdma::PatternPtr patternOfUser = patterns[itr->first];
                    // In case of no beamforming, adding the invalid
                    // pattern would fail, therefore we avoid it
                    /** @todo pab, 20070327: for mixed operation of
                     *  users with and without beams we would have
                     *  to add a real omni-pattern here.
                     */
                    if (patternOfUser != wns::service::phy::ofdma::PatternPtr())
                        sumPattern.add(patternOfUser);
                }
                // again, for no beamforming we return the standard factor
                if (sumPattern.getSize() == 0)
                    return wns::Ratio().from_factor( 1 );

                return wns::Ratio().from_factor( 1 / sumPattern.getMaxGain().get_factor() );
            }

            AntennaPatternsPerUser patterns;
            std::vector<Group> groups;
            std::map<UserID, int> userGroupNumber; // what group is user in
        }; // Grouping
        // a copy of the original structure is bad for memory/time
        typedef SmartPtr<Grouping> GroupingPtr;

        /** @brief This is used e.g. for building ResourceRequests */
        struct QueueStatus {
            QueueStatus()
            {
                numOfBits = 0;
                numOfCompounds = 0;
            }

            unsigned int numOfBits;
            unsigned int numOfCompounds;

            bool
            operator==(const QueueStatus& other) const
            {
                return this->numOfBits == other.numOfBits &&
                        this->numOfCompounds == other.numOfCompounds;                    
            };

            bool
            operator!=(const QueueStatus& other) const
            {
                return !((*this) == other);
            };

        };
        /** @brief Holds QueueStatus for all cids. ResourceRequests contain such a container. */
        typedef wns::container::Registry<ConnectionID, QueueStatus> QueueStatusContainer;

        /** @brief data structure which is used to store channel quality */
        class ChannelQualityOnOneSubChannel
        {
        public:
            ChannelQualityOnOneSubChannel():
                pathloss(),
                interference(),
                carrier()
            {
                 sdma.iIntra = wns::Power();
            }

            ChannelQualityOnOneSubChannel(wns::Ratio _pathloss, 
                                        wns::Power _interference, 
                                        wns::Power _carrier,
                                        wns::Power _iIntra = wns::Power::from_mW(0.0),
                                        wns::Ratio _effSINR = wns::Ratio::from_factor(1.0),
                                        int _timeSlot = ANYTIME):
                pathloss(_pathloss),
                interference(_interference),
                carrier(_carrier),
                effectiveSINR(_effSINR),
                timeSlot(_timeSlot)
                
            {
                sdma.iIntra = _iIntra;
            }

            /** @brief measured RxPower */
            wns::Power carrier;
            /** @brief measured pathloss (counted positive, i.e. x dB with x>0) */
            wns::Ratio pathloss;
            /** @brief measured (I + N) */
            wns::Power interference;

            /** @brief Effective SINR*/
            wns::Ratio effectiveSINR;

            /** @biref For time (frame) dependent measurements */
            int timeSlot;
            struct {
                // estimated intra-cell interference
                wns::Power iIntra;
            } sdma;
        }; // ChannelQualityOnOneSubChannel

        //typedef std::vector<ChannelQualityOnOneSubChannel> ChannelsQualitiesOnAllSubBand; // index is real(OFDMA) subchannel number
        class ChannelQualitiesOnAllSubBands
                : virtual public wns::RefCountable,
                  public std::vector<ChannelQualityOnOneSubChannel>
        {
        public:
            ChannelQualitiesOnAllSubBands() {};
            ~ChannelQualitiesOnAllSubBands() {};
        };
        /** @brief SmartPtr created in the CQI; no need for memory tracking later */
        typedef SmartPtr<ChannelQualitiesOnAllSubBands> ChannelQualitiesOnAllSubBandsPtr;

        //typedef	std::map<UserID, ChannelsQualitiesOnAllSubBand*> ChannelQualitiesOfAllUsers;
        class ChannelQualitiesOfAllUsers
                : virtual public wns::RefCountable,
                  public std::map<UserID, ChannelQualitiesOnAllSubBandsPtr>
        {
        public:
            ChannelQualitiesOfAllUsers() {};
            ~ChannelQualitiesOfAllUsers() {};
            virtual bool knowsUser(UserID user) {
                return (find(user) != end()); }
        };
        typedef SmartPtr<ChannelQualitiesOfAllUsers> ChannelQualitiesOfAllUsersPtr;

        /** @brief function class which should be used in sort() to decide which channel quality is better */
        class BetterChannelQuality
        {
        public:
            /** @brief true if a is better than b */
            bool operator()(wns::scheduler::ChannelQualityOnOneSubChannel a, wns::scheduler::ChannelQualityOnOneSubChannel b)
            {
                if (a.pathloss == wns::Ratio()) {
                    return false;
                }
                if (b.pathloss == wns::Ratio()) { // undefined
                    return true; // a is better if b is undefined
                }
                return a.pathloss.get_factor() * a.interference.get_mW()
                    < b.pathloss.get_factor() * b.interference.get_mW();
            }
        };

        /** @brief function class which should compute the capacity of channel to decide which channel capacity is better */
        class BetterChannelCapacity
        {
        public:
            /** @brief true if a is better than b */
            bool operator()(double a, double b)
            {
                if (a == 0.0) {
                    return false;
                }
                if (b == 0.0) { // undefined
                    return true; // a is better if b is undefined
                }

                return a>b;
            }
        };

    }} // namespace wns::scheduler
#endif // WNS_SCHEDULER_SCHEDULERTYPES_HPP


