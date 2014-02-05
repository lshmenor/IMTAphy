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

#include <WNS/scheduler/strategy/staticpriority/ProportionalFair.hpp>
#include <WNS/scheduler/SchedulerTypes.hpp>

#include <vector>
#include <map>
#include <queue>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <math.h>

using namespace std;
using namespace wns::scheduler;
using namespace wns::scheduler::strategy;
using namespace wns::scheduler::strategy::staticpriority;

// begin example "wimac.tutorial.experiment2.staticFactory.substrategy.ProportionalFair.cpp"
STATIC_FACTORY_REGISTER_WITH_CREATOR(ProportionalFair,
                                     SubStrategyInterface,
                                     "ProportionalFair",
                                     wns::PyConfigViewCreator);
// end example
                                     
ProportionalFair::ProportionalFair(const wns::pyconfig::View& config)
    : SubStrategy(config),
      blockSize(config.get<int>("blockSize")),
      historyWeight(config.get<float>("historyWeight")),
      scalingBetweenMaxTPandPFair(config.get<float>("scalingBetweenMaxTPandPFair")),
      rateFairness(config.get<bool>("rateFairness")),
      maxRateOfSubchannel(0.0),
      allUsers(),
      preferenceVariationDistribution(NULL)
{
    assure(blockSize>0,"invalid blockSize="<<blockSize);
    MESSAGE_SINGLE(NORMAL, logger, "ProportionalFair(): constructed with blockSize="<<blockSize);
    pastDataRates.clear();
    allUsers.clear();
    preferenceVariationDistribution = new wns::distribution::Uniform(-1.0, 1.0);
    // historyWeight:
    // if 0 no history is taken into account -> maxThroughput Scheduler
    assure(scalingBetweenMaxTPandPFair>=0.0, "scalingBetweenMaxTPandPFair="<<scalingBetweenMaxTPandPFair<<" is out of bounds");
    assure(scalingBetweenMaxTPandPFair<=1.0, "scalingBetweenMaxTPandPFair="<<scalingBetweenMaxTPandPFair<<" is out of bounds");
    assure((historyWeight>=0.0)&&(historyWeight<1.0), "historyWeight="<<historyWeight<<" is out of bounds");
}

ProportionalFair::~ProportionalFair()
{
    delete preferenceVariationDistribution;
}

void
ProportionalFair::initialize()
{
    MESSAGE_SINGLE(NORMAL, logger, "ProportionalFair::initialize()");
    wns::service::phy::phymode::PhyModeInterfacePtr phyMode = colleagues.registry->getPhyModeMapper()->getHighestPhyMode();
    maxRateOfSubchannel = phyMode->getDataRate();
    MESSAGE_SINGLE(NORMAL, logger, "ProportionalFair: maxRateOfSubchannel="<<maxRateOfSubchannel<<" bit/s");
    assure(maxRateOfSubchannel>0.0, "unknown maxRateOfSubchannel");
}

std::priority_queue<ProportionalFair::UserPreference>
ProportionalFair::calculateUserPreferences(UserSet activeUsers, bool txStrategy) const
{
    std::map<UserID, ChannelQualityOnOneSubChannel> sinrs;

    // preference for every user in a priority queue which automatically sorts
    std::priority_queue<UserPreference> preferences;
    assure(preferences.size()==0, "preferences.size() must be 0");

    for ( UserSet::const_iterator iter = activeUsers.begin();
          iter != activeUsers.end(); ++iter)
    {
        UserID user = *iter;
        assure(user.isValid(), "No valid user");

        //determines the users SINRs depending on the tx/Rx mode of the strategy
        if (txStrategy)
        {
            sinrs[user] = colleagues.registry->estimateTxSINRAt(user);
        }
        else
        {
            sinrs[user] = colleagues.registry->estimateRxSINROf(user);
        }

        // calculate PhyModeRate for available users
        wns::Ratio sinr(sinrs[user].carrier / sinrs[user].interference);
        wns::service::phy::phymode::PhyModeInterfacePtr phyMode = colleagues.registry->getPhyModeMapper()->getBestPhyMode(sinr);
        assure(phyMode->isValid(),"invalid PhyMode");

        // calculate userRate, which is the maximum possible data rate
        // for one subChannel for the best PhyMode available here
        float phyModeRate = phyMode->getDataRate(); // rate [b/s] of one subChannel
        float referenceRate;
        float pastDataRate = 1.0;

        // get the past data rate for this user:
        // iterate over the global past data rates map of all users and
        // find the past data rate for this user
        // there is exactly one pastDataRate value per userID
        int weight = colleagues.registry->getTotalNumberOfUsers(user);
        for (std::map<UserID, float>::const_iterator iter = pastDataRates.begin();
             iter != pastDataRates.end(); ++iter)
        {
            if (iter->first/*userID*/ == user)
            {
                float dataRate = iter->second;
                // a RN must get a better share of the bandwidth
                // here: proportional to its number of users:
                assure(weight>0, "numberOfUsers(" <<user.getName()<<")=" << weight);
                dataRate /= static_cast<float>(weight);
                // dataRate now has the meaning of a weight.
                pastDataRate = dataRate;
            }
        } // for all userIDs in pastDataRates

        if (pastDataRate < 0.01)
            pastDataRate = 0.01;

        // preference is achievable current user rate divided by a history
        // factor that takes the past throughput of this user into account

        assure(maxRateOfSubchannel>0.0, "unknown maxRateOfSubchannel");

        // goal is either rate (true) or resource (false) fairness
        if (rateFairness == true)
        {
            referenceRate = maxRateOfSubchannel;
        }
        else
        {
            referenceRate = phyModeRate;
        }

        // maxRateOfSubchannel is constant, with range [0..1][bit/s]
        float resultMaxThroughput = referenceRate / maxRateOfSubchannel;
        float resultPropFair      = referenceRate / pastDataRate; // can be any range
        if (scalingBetweenMaxTPandPFair <= 0.5) {
            // variate the preference for each user, so that they differ a little bit (1%)
            // and the automatic sorting does not always give the same order
            // (would be a problem for identical preference weights).
            resultMaxThroughput *=  (1 + 0.01*(*preferenceVariationDistribution)());
        }
        float UserPref =
            (1.0-scalingBetweenMaxTPandPFair) * resultMaxThroughput
            +scalingBetweenMaxTPandPFair  * resultPropFair;

        MESSAGE_SINGLE(NORMAL, logger, "getPreference("<<user.getName()<<"): weight=" << weight << ", pastDataRate= "<<pastDataRate<<" bit/s, UserPreference= "<<UserPref<<" (resultMaxThroughput="<<resultMaxThroughput<<",resultProportionalFair="<<resultPropFair<<")");

        // calculate preferences for users and order them
        preferences.push(UserPreference(UserPref, user));
    }
    return preferences;
}

//std::map<UserID, Bit(int)>
std::map<UserID, float>
ProportionalFair::calculateBitsForConnections(const ConnectionSet& currentConnections)
{
    std::map<UserID, float> bitsForUsers;

    for ( wns::scheduler::ConnectionSet::const_iterator iter = currentConnections.begin();
          iter != currentConnections.end();
          ++iter )
    {
        ConnectionID currentConnection = *iter;
        wns::scheduler::UserID user = colleagues.registry->getUserForCID(currentConnection);

        Bit queueLength = 0;
        if (colleagues.queue->queueHasPDUs(currentConnection))
        {
            queueLength = colleagues.queue->numBitsForCid(currentConnection);
        }

        if (bitsForUsers.find(user) == bitsForUsers.end())
        {
            bitsForUsers[user] = queueLength;
        }
        else
        {
            bitsForUsers[user] += queueLength;
        }
    }
    return bitsForUsers;
}

// preferences should be a member
wns::scheduler::ConnectionID
ProportionalFair::getNextConnection(SchedulerStatePtr schedulerState,
                                    std::priority_queue<UserPreference> preferences)
{
    wns::scheduler::ConnectionID next = -1;

    while (!preferences.empty())
    {
        int priority = schedulerState->currentState->getCurrentPriority();
        const float preference = preferences.top().first;
        const UserID user = preferences.top().second;
        MESSAGE_SINGLE(NORMAL, logger, "Selected user="<<user.getName());

        ConnectionVector currentPrioConns = getConnectionsForPrio(priority, user);

        if(!currentPrioConns.empty())
        {
            next = getRandomConnection(currentPrioConns);
            MESSAGE_SINGLE(NORMAL, logger, "Selected connection with CID="<<next);
            return next;
        }
        preferences.pop();
    }
    return next;
}

// return connections for user belonging to current priority
ConnectionVector
ProportionalFair::getConnectionsForPrio(int currentPrio, const UserID user)
{
    ConnectionVector currentPrioConns;
    ConnectionVector allRegisteredConns = colleagues.registry->getConnectionsForUser(user);
    for (ConnectionVector::const_iterator iter = allRegisteredConns.begin();
         iter != allRegisteredConns.end();
         ++iter)
    {
        wns::scheduler::ConnectionID currentConnection = *iter;
        // check if the connection has the current priority
        if (colleagues.registry->getPriorityForConnection(currentConnection) == currentPrio)
        {
            if (colleagues.queue->queueHasPDUs(currentConnection))
            {
                currentPrioConns.push_back(currentConnection);
            }
            // else: this conection is empty, go to the next connection of this user
        }
        // else: this connection belongs to another, i.e. lower priority, go to the next connection of this user
    }
    return currentPrioConns;
}

wns::scheduler::ConnectionID
ProportionalFair::getRandomConnection(ConnectionVector currentPrioConns)
{
    int numberOfConns = currentPrioConns.size();
    wns::distribution::Uniform randomPositionDistribution(0.0, numberOfConns);
    float randomNumber = (randomPositionDistribution)();
    int randomPosition = static_cast<int>(randomNumber);
    MESSAGE_SINGLE(NORMAL, logger, "Drew random number="<<randomNumber<< ", position="<< randomPosition << " out of "<< numberOfConns << " total connections.");
    assure(randomPosition<numberOfConns, "Random position of connections="<< randomPosition << " out of range of current connections size="<<numberOfConns);
    return currentPrioConns[randomPosition];
}

// maybe we could get rid of the phase length
void
ProportionalFair::updatePastDataRates(std::map<UserID, float> bitsBeforeThisFrame,
                                      std::map<UserID, float> bitsAfterThisFrame,
                                      simTimeType phaseLength)
{
    UserID user;
    float bitsThisFrame = 0.0;
    float pastDataRate = 0.0;
    float currentRate = 0.0;

    for (std::map<UserID, float>::const_iterator iter = bitsBeforeThisFrame.begin();
         iter != bitsBeforeThisFrame.end(); ++iter)
    {
        user = iter->first;
        bitsThisFrame = bitsBeforeThisFrame[user] - bitsAfterThisFrame[user];
        currentRate = bitsThisFrame / phaseLength;
        pastDataRate = pastDataRates[user];

        if (pastDataRates.find(user) != pastDataRates.end()) {
            pastDataRates[user] = (1.0-historyWeight) * currentRate + historyWeight * pastDataRates[user];
        } else {
            // new user
            pastDataRates[user] = currentRate;
        }
        MESSAGE_SINGLE(NORMAL, logger, "updatePastDataRates("<<user.getName()<<","<<phaseLength<<"s): pastDataRate: new= "<< pastDataRates[user]<<" bit/s, old= "<<pastDataRate<<" bit/s, currentRate= "<<currentRate<<" bit/s");
    }
}

wns::scheduler::MapInfoCollectionPtr
ProportionalFair::doStartSubScheduling(SchedulerStatePtr schedulerState,
                                       wns::scheduler::SchedulingMapPtr schedulingMap)

{
    MapInfoCollectionPtr mapInfoCollection = MapInfoCollectionPtr(new wns::scheduler::MapInfoCollection); // result datastructure
    UserSet allUsersInQueue = colleagues.queue->getQueuedUsers();
    UserSet activeUsers     = colleagues.registry->filterReachable(allUsersInQueue);
    ConnectionSet &currentConnections = schedulerState->currentState->activeConnections;

    MESSAGE_SINGLE(NORMAL, logger, "activeUsers= "<< activeUsers.size()<<" , currentConnections= "<<printConnectionSet(currentConnections)<<" ");

    if (activeUsers.empty() || currentConnections.empty()) return mapInfoCollection; // nothing to do

    simTimeType slotLength = schedulingMap->getSlotLength();
    bool txStrategy = schedulerState->isTx;
    std::map<UserID, float> bitsBeforeThisFrame = calculateBitsForConnections(currentConnections);

    MESSAGE_BEGIN(NORMAL, logger, m, "ProportionalFair");
    for (std::map<UserID, float>::const_iterator iter = bitsBeforeThisFrame.begin();
         iter != bitsBeforeThisFrame.end(); ++iter)
    {
        m << "\n User " << iter->first.getName() << " has " << iter->second;
        m << " queued bits.";
    }
    MESSAGE_END();

    std::map<UserID, float> pastDataRates;
    // make preferences a member, then no return value needed
    std::priority_queue<UserPreference> preferences = calculateUserPreferences(activeUsers, txStrategy);

    // returns the connection of the user with the highest preference, i.e. lowest past data rate
    wns::scheduler::ConnectionID currentConnection = getNextConnection(schedulerState, preferences);
    //wns::scheduler::UserID user = colleagues.registry->getUserForCid(currentConnection);

    bool spaceLeft= true;
    int pduCounter = 0;
    // static const noCID and then check for (currentConnection != noCID)
    while(spaceLeft && (currentConnection >= 0))
    {
        // schedule blockSize PDUs for this CID
        while( colleagues.queue->queueHasPDUs(currentConnection) && spaceLeft)
        {
            spaceLeft = scheduleCid(schedulerState,schedulingMap,currentConnection,pduCounter,blockSize,mapInfoCollection);
        } // while PDUs in queue
        /*if (!colleagues.queue->queueHasPDUs(currentConnection))
          { // exit because of queue empty (most probable case for low traffic)
          currentConnections.erase(currentConnection);
          if (currentConnections.empty()) break; // all queues empty
          }*/
        currentConnection = getNextConnection(schedulerState, preferences);
        // user = colleagues.registry->getUserForCid(currentConnection);
        MESSAGE_SINGLE(NORMAL, logger, "doStartSubScheduling(): next connection="<<currentConnection);
    } // while(spaceLeft)
    MESSAGE_SINGLE(NORMAL, logger, "doStartSubScheduling(): ready: mapInfoCollection of size="<<mapInfoCollection->size());

    std::map<UserID, float> bitsAfterThisFrame = calculateBitsForConnections(currentConnections);
    
    MESSAGE_BEGIN(NORMAL, logger, m, "ProportionalFair");
    for (std::map<UserID, float>::const_iterator iter = bitsAfterThisFrame.begin();
         iter != bitsAfterThisFrame.end(); ++iter)
    {
        m << "\n User " << iter->first.getName() << " has " << iter->second;
        m << " queued bits left after this frame.";
    }
    MESSAGE_END();

    assure(bitsBeforeThisFrame.size() == bitsAfterThisFrame.size(), "bitsBeforeThisFrame and bitsAfterThisFrame do not have the same number of users!");
    updatePastDataRates(bitsBeforeThisFrame, bitsAfterThisFrame, slotLength);
    return mapInfoCollection;
}
