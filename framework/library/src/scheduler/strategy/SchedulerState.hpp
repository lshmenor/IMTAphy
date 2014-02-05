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

#ifndef WNS_SCHEDULER_STRATEGY_SCHEDULERSTATE_HPP
#define WNS_SCHEDULER_STRATEGY_SCHEDULERSTATE_HPP

#include <WNS/scheduler/SchedulerTypes.hpp>
#include <WNS/scheduler/MapInfoEntry.hpp>
#include <WNS/scheduler/SchedulingMap.hpp>
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

            /** @brief This object carries all info for the next PDU to be scheduled.
                It is given as a request to doAdaptiveResourceScheduling()
            */
            class RequestForResource
                    : virtual public wns::RefCountable
            {
            public:
                RequestForResource(ConnectionID _cid, UserID _user, Bits _bits, Bits _queuedBits, bool useHARQ);
                ~RequestForResource();
                std::string toString() const;
            public:
                /** @brief the duration for the request once the phyMode is known */
                simTimeType getDuration() const;

                /** @brief the connectionID of the PDU to be scheduled */
                ConnectionID cid;
                /** @brief the userID of the PDU to be scheduled */
                UserID user;
                /** @brief size of the PDU to be scheduled */
                Bits bits;
                /** @brief Queued bits in total */
                Bits queuedBits;

                /** @brief if phyModePtr is specified, this is used for further calculations
                    (e.g. the duration on the subChannel can be calculated).
                    Otherwise, undefined means: freely selectable during the following process. */
                wns::service::phy::phymode::PhyModeInterfacePtr phyModePtr;
                /** @brief proposed subChannel.
                    Never set in the original request, but written after DSA strategy. */
                int subChannel;
                /** @brief index of time slot (TDMA component) */
                int timeSlot;
                /** @brief spatial channel (either beamforming or MIMO) */
                int spatialLayer; // also called "stream" or better "spacialIndex"
                /** @brief channel state (CQI) on the proposed subChannel.
                    Contains values even if CQI is not available (uses CandI estimateTxSINRAt in this case)
                    The values are set after DSA is finished.
                    This is a datastructure containing pathloss and interference predictions. */
                ChannelQualityOnOneSubChannel cqiOnSubChannel; // memory here
                bool useHARQ;
            };

            /** @brief This collection of parameters is local for a subStrategy.
                Each substrategy class should derive from this and keep track itself.
                This state is NOT persistent within each scheduler object. */
            class RevolvingState
                    : virtual public wns::RefCountable
            {
            public:
                RevolvingState(const strategy::StrategyInput* _strategyInput);
                virtual ~RevolvingState();
                virtual std::string toString() const;
                virtual GroupingPtr getNewGrouping();
                virtual bool groupingIsValid() const;
                virtual bool sdmaGroupingIsValid() const;
                virtual GroupingPtr getGrouping() const;
                virtual void setGrouping(GroupingPtr groupingPtr);
                virtual void setCurrentPriority(int priority);
                virtual int  getCurrentPriority() const;
                virtual void clearMap();
            public:
                /** @brief contains the constant parameters given by the caller */
                const strategy::StrategyInput* strategyInput;
                /** @brief MapInfoCollection is a list of MapInfoEntryPtr;
                    this data structure collects all the results of one frame.
                    It is later asked for by getMapInfo() from outside (RRHandler) */
                MapInfoCollectionPtr bursts;
                /** @brief SchedulingMap is a hierarchical structure
                    reflecting the OFDMA subChannels and spatialLayers;
                    this data structure collects all the results of one frame.
                    It can be asked for by getSchedulingMap() from outside */
                wns::scheduler::SchedulingMapPtr schedulingMap;
                /** @brief set of all valid cids (for all priorities) */
                //ConnectionSet allActiveConnections;
                /** @brief set of all valid cids for current priority */
                ConnectionSet activeConnections;
                /** @brief full CQI information (SmartPtr) */
                ChannelQualitiesOfAllUsersPtr channelQualitiesOfAllUsers;
            protected:
                /** @brief if groupingRequired() this contains the result of the grouper */
                GroupingPtr grouping;

                /** @brief within staticPriority strategy this is the current priority */
                int currentPriority;
            }; // class RevolvingState
            /** @brief created at the beginning of startScheduling(); no need for memory tracking later */
            typedef SmartPtr<RevolvingState> RevolvingStatePtr;
            /** @brief one RevolvingStatePtr entry per timeFrame if keepStateHistory==true */
            //typedef std::vector< RevolvingStatePtr > RevolvingStateVector;


            /** @brief This collection of parameters is given to subStrategies and DSA/APC strategies
                This state is persistent within each scheduler object. */
            class SchedulerState
                    : virtual public wns::RefCountable
            {
            public:
                SchedulerState(strategy::StrategyInterface* _strategy):
                    strategy(_strategy),
                    defaultPhyModePtr(),
                    defaultTxPower(),
                    powerCapabilities(),
                    isTx(true),
                    isDL(true),
                    useCQI(false),
                    powerControlType(PowerControlDLMaster),
                    //schedulerSpot(wns::scheduler::SchedulerSpot::DLMaster()),
                    schedulerSpot(-1), // initialized to good value in setFriends
                    myUserID(NULL),
                    excludeTooLowSINR(true),
                    keepResultHistory(false),
                    symbolDuration(0.0),
                    eirpLimited(false),
                    currentState() // empty RevolvingState(), not persistent
                {
                    assure(strategy!=NULL,"strategy==NULL");
                }
                ~SchedulerState() { strategy=NULL; };

                /** @brief set (optional!) parameter defaultPhyMode
                    If this is set, no other PhyMode will be used.
                    No AMC for the simple (old) strategies */
                virtual void setDefaultPhyMode(wns::service::phy::phymode::PhyModeInterfacePtr _phyModePtr)
                { defaultPhyModePtr = _phyModePtr; /*_phyModePtr->clone();*/}
                virtual wns::service::phy::phymode::PhyModeInterfacePtr
                getDefaultPhyMode()
                { assure(defaultPhyModePtr!=wns::service::phy::phymode::PhyModeInterfacePtr(),"defaultPhyModePtr=NULL"); return defaultPhyModePtr; }
                /** @brief set (optional!) parameter defaultTxPower
                    If this is set, no other TxPower will be used.
                    No APC for the simple (old) strategies */
                virtual void setDefaultTxPower(wns::Power _txPower) {
                    defaultTxPower = _txPower; }
                /** @brief clear MapInfoEntryCollection */
                virtual void clearMap() {
                    assure(currentState!=RevolvingStatePtr(),"uninitialized currentState");
                    if (currentState!=RevolvingStatePtr()) currentState->clearMap(); }
                /** @brief get (volatile) current state */
                virtual RevolvingStatePtr
                getCurrentState() {
                    assure (currentState!=RevolvingStatePtr(),"uninitialized currentState");
                    return currentState; }
            public:
                strategy::StrategyInterface* strategy; // allow strategy to be asked
                /** @brief constant PhyMode if not set adaptively */
                wns::service::phy::phymode::PhyModeInterfacePtr defaultPhyModePtr;
                /** @brief constant txPower if not set adaptively */
                wns::Power defaultTxPower;
                /** @brief adaptive power limits (Tx: OK; Rx: depend on user).
                    Here we assume that every user has the same PowerCapabilities.
                    Otherwise we must always ask schedulerState->strategy->getPowerCapabilities(request.user); */
                PowerCapabilities powerCapabilities;
                /** @brief isTx==true for RS-TX (DL,UL) in (BS,UT,RN) */
                bool isTx;
                /** @brief isDL==true for RS-TX (DL) in (BS,RN-BS) */
                bool isDL;
                /** @brief useCQI==true if CQI should be used for SINR estimation per subChannel.
                    Otherwise the old method for a flat channel
                    with grouping and estimatedCandI is used */
                bool useCQI;
                /** @brief there are three positions for the scheduler... */
                PowerControlType powerControlType;
                /** @brief there are three positions for the scheduler... */
                SchedulerSpotType schedulerSpot;
                /** @brief userID of my own station/terminal myself */
                UserID myUserID;
                /** @brief PyConfig Attribute:
                    flag to determine whether doAdaptiveResourceScheduling cares for too low SINR */
                bool excludeTooLowSINR;
                /** @brief keepResultHistory=true means we can recall
                    the StrategyResult of elapsed timeFrames */
                bool keepResultHistory;
                /** @brief time duration of one OFDM symbol */
                simTimeType symbolDuration;
                /** @brief attribute from ofdmaProvider->isEIRPLimited() */
                bool eirpLimited;

                /** @brief this is the part of the state which changes
                    from timeFrame to timeFrame and from call to call of doStartScheduling */
                RevolvingStatePtr currentState;

            }; // class SchedulerState

            /** @brief created in the strategies; no need for memory tracking later */
            typedef SmartPtr<SchedulerState> SchedulerStatePtr;

            /** @brief function class which should be used in sort() to decide which channel quality is better */
            class betterChannelQuality
            {
            public:
                bool operator()(wns::scheduler::ChannelQualityOnOneSubChannel a, wns::scheduler::ChannelQualityOnOneSubChannel b)
                {
                    return a.pathloss.get_factor() * a.interference.get_mW()
                        < b.pathloss.get_factor() * b.interference.get_mW();
                }
            };

        }}} // namespace wns::scheduler::strategy
#endif // WNS_SCHEDULER_STRATEGY_SCHEDULERSTATE_HPP


