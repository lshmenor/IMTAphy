/******************************************************************************
 * WNS (Wireless Network Simulator)                                           *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2004-2006                                                    *
 * Chair of Communication Networks (ComNets)                                  *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnets.rwth-aachen.de                                          *
 * www: http://wns.comnets.rwth-aachen.de                                     *
 ******************************************************************************/

#ifndef WNS_SCHEDULER_STRATEGY_STATICPRIORITY_PROPORTIONALFAIR_HPP
#define WNS_SCHEDULER_STRATEGY_STATICPRIORITY_PROPORTIONALFAIR_HPP

#include <WNS/scheduler/strategy/Strategy.hpp>
#include <WNS/scheduler/strategy/staticpriority/SubStrategy.hpp>
#include <WNS/scheduler/SchedulingMap.hpp>
#include <WNS/scheduler/queue/QueueInterface.hpp>
#include <WNS/scheduler/RegistryProxyInterface.hpp>

#include <WNS/distribution/Uniform.hpp>
#include <WNS/StaticFactory.hpp>

#include <queue>

namespace wns { namespace scheduler { namespace strategy { namespace staticpriority {

                /**
                 * @brief Proportional Fair subscheduler.
                 *
                 */

                class ProportionalFair
                    : public SubStrategy
                {
                public:
                    ProportionalFair(const wns::pyconfig::View& config);

                    ~ProportionalFair();

                    /** @brief holds the userID and its corresponding preference weight,
                        which is a float value */
                    typedef std::pair<float, UserID> UserPreference;

                    virtual void
                    initialize();

                    /** @brief provides the preference parameter for every user
                        in function of the past data rates they have reached */
                    std::priority_queue<UserPreference>
                    calculateUserPreferences(UserSet activeUsers, bool txStrategy) const;

                    /** @brief calculates the number of bits scheduled per frame for all connections */
                    std::map<UserID, float>
                    calculateBitsForConnections(const ConnectionSet& currentConnections);

                    /** @brief gives the next cid to schedule according to the users preference values */
                    virtual wns::scheduler::ConnectionID
                    getNextConnection(SchedulerStatePtr schedulerState, std::priority_queue<UserPreference> preferences);

                    /** @brief updates pastDataRates=90%*pastDataRates + 10%*currentRate=(bitsThisFrame/phaseLength) */
                    void
                    updatePastDataRates(std::map<UserID, float> bitsBeforeThisFrame,
                                        std::map<UserID, float> bitsAfterThisFrame,
                                        simTimeType phaseLength);

                    virtual wns::scheduler::MapInfoCollectionPtr
                    doStartSubScheduling(SchedulerStatePtr schedulerState,
                                         wns::scheduler::SchedulingMapPtr schedulingMap);

                protected:
                    /** @brief Returns connections belonnging to current priority of this user*/
                    ConnectionVector
                    getConnectionsForPrio(int currentPrio, const UserID user);
                    /** @brief Return a random connection out of the connection vector*/
                    wns::scheduler::ConnectionID
                    getRandomConnection(ConnectionVector currentPrioConns);
                    /** @brief Number of packets to schedule of the same cid before proceeding to the next one */
                    int blockSize;
                    /** @brief exponential window sliding average; p_i=(1-h)*c_i + h*i_{i-1} */
                    float historyWeight;
                    /** @brief used for the user preference calculation; 0.0=MaxThroughput; 1.0=ProportionalFair */
                    float scalingBetweenMaxTPandPFair;
                    /** @brief indicates whether goal is rate (true) or resource (false) fairness */
                    bool rateFairness;
                    float maxRateOfSubchannel;
                    UserSet allUsers;
                    std::map<UserID, float> bitsForUsers;
                    std::map<UserID, float> bitsThisFrame;
                    std::map<UserID, float> pastDataRates;
                    std::map<UserID, wns::CandI> sinrs;
                    /** @brief distribution for random numbers used to variate the preference a little bit */
                    wns::distribution::Uniform* preferenceVariationDistribution;
                };
            }}}}
namespace std {
    template <>
    struct less<wns::scheduler::strategy::staticpriority::ProportionalFair::UserPreference>:
        public binary_function<wns::scheduler::strategy::staticpriority::ProportionalFair::UserPreference,
                               wns::scheduler::strategy::staticpriority::ProportionalFair::UserPreference,
                               bool>{
        /** @brief these pointers are used for sorting users preferences: in case they got the same
            preference value the first UserID is taken */
        bool operator()(const wns::scheduler::strategy::staticpriority::ProportionalFair::UserPreference & lhs,
                        const wns::scheduler::strategy::staticpriority::ProportionalFair::UserPreference & rhs) const
        {
            if(lhs.first == rhs.first) {
                return lhs.second.getName() < rhs.second.getName();
            }
            return lhs.first < rhs.first;
        }
    };
}
#endif
