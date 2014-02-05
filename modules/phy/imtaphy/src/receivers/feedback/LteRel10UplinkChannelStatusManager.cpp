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

#include <IMTAPHY/receivers/feedback/LteRel10UplinkChannelStatusManager.hpp>
#include <IMTAPHY/Channel.hpp>
#include <IMTAPHY/linkManagement/LinkManager.hpp>

#include <WNS/StaticFactoryBroker.hpp>
#include <WNS/PyConfigViewCreator.hpp>

#include <algorithm>
#include <iostream>

using namespace imtaphy::receivers::feedback;

UplinkChannelStatusManagerInterface*
UplinkChannelStatusManagerInterface::getCSM()
{
    typedef wns::PyConfigViewCreator<UplinkChannelStatusManagerInterface> 
        UplinkChannelStatusManagerCreator;

    typedef wns::StaticFactoryBroker<UplinkChannelStatusManagerInterface, 
        UplinkChannelStatusManagerCreator>  BrokerType;

    std::string name = wns::simulator::getInstance()
                ->getConfiguration().get<std::string>("modules.imtaphy.uplinkStatusManager.name");

    return wns::SingletonHolder<BrokerType>::getInstance()->procure(name,
                wns::simulator::getInstance()
                ->getConfiguration().getView("modules.imtaphy.uplinkStatusManager"));
}

STATIC_FACTORY_BROKER_REGISTER(LteRel10UplinkChannelStatusManager,
                                UplinkChannelStatusManagerInterface,
                                "DefaultLteRel10UplinkChannelStatusManager");

LteRel10UplinkChannelStatusManager::LteRel10UplinkChannelStatusManager(const wns::pyconfig::View& _config) :
    config(_config),
    numPRBs(0),
    precodingModeString(config.get<std::string>("precodingMode")),
    srsPeriod(config.get<unsigned int>("srsUpdateFrequency")),
    statusDelay(config.get<unsigned int>("statusDelay")),
    initialized(false),
    codebook(&imtaphy::receivers::TheLteRel8CodebookFloat::Instance()),
    effSINRModel(imtaphy::l2s::TheMMIBEffectiveSINRModel::getInstance()),
    enabled(config.get<bool>("enabled")),
    logger(config.getView("logger"))    
{
    assure(statusDelay >= 1, "0ms delay is not supported");
    assure(srsPeriod > 0, "SRS update period must be greater than 0");
    
    std::transform(precodingModeString.begin(), precodingModeString.end(), precodingModeString.begin(), toupper);

    if (precodingModeString == "CLOSEDLOOPCODEBOOKBASED")
    {
        precodingMode = ClosedLoopCodebookBased;
    }
    else if (precodingModeString == "NOPRECODING")
    {
        precodingMode = NoPrecoding;
    }
    else
    {
        precodingMode = NoPrecoding;
        assure(0, "Unknown precoding mode");
    }
    
    // TODO: these settings (SRS start offset and periodicity) could be set individually on a per-user basis

    bufferLength = srsPeriod + statusDelay + 1;
};

void 
LteRel10UplinkChannelStatusManager::beforeTTIover(unsigned int ttiNumber)
{
    int numReceivers = baseStations.size();
    int receiverIndex; // signed int for OpenMP parallel for loop

    // these are the variables that should be private to each thread
    imtaphy::StationPhy* baseStation;
    
    
#pragma omp parallel for private(baseStation), shared(numReceivers, ttiNumber) //, default(none)
    for (receiverIndex = 0; receiverIndex < numReceivers; receiverIndex++)
    {
        baseStation = baseStations[receiverIndex];      // this is a read-only access into a vector and should be thread-safe
        assure(associatedUsers.find(baseStation) != associatedUsers.end(), "no users"); // this is a read-only access and should be thread-safe

        doUpdate(baseStation, ttiNumber);
    } // end of parallel code region
}

unsigned int
LteRel10UplinkChannelStatusManager::performRankUpdate(imtaphy::StationPhy* receivingStation)
{
    assure(0, "Not yet implemented");
    return 0;
} // end of full rank update
    


void 
LteRel10UplinkChannelStatusManager::determineNoPrecodingSINRs(imtaphy::StationPhy* baseStation, 
                                                              imtaphy::StationPhy* userStation, 
                                                              LteRel10UplinkChannelStatusContainer* channelStatus, 
                                                              unsigned int rank,
                                                              unsigned int tti)
{   

    assure(receivers.find(baseStation) != receivers.end(), "Don't have the eNB's receiver");
    imtaphy::receivers::LinearReceiver* receiver = receivers[baseStation];
    
    // TODO: switch to UL precoding matrices
    std::vector<imtaphy::detail::ComplexFloatMatrixPtr> precoding;
    precoding.push_back(codebook->getPrecodingMatrix(userStation->getAntenna()->getNumberOfElements(),
                                                    rank,
                                                    imtaphy::receivers::feedback::EqualPower));

        
    imtaphy::interface::PRBVector prbs = channelStatus->getPRBsToUpdate(tti);

    
    for (unsigned int f = 0; f < prbs.size(); f++)
    {
        imtaphy::receivers::ReceiverFeedback best = receiver->computeFeedback(prbs[f], rank, 
                                                                              userStation, 
                                                                              referencePowerMap[userStation], 
                                                                              precoding);

        channelStatus->updateChannelStatus(prbs[f], wns::Ratio::from_factor(best.bestSINRs[0]), wns::Ratio::from_factor(best.bestSINRs[0]), best.pmiRanking[0], tti);
    }
    
    // everything updated for this TTI
    channelStatus->ttiOver(tti);
}

void
LteRel10UplinkChannelStatusManager::doUpdate(imtaphy::StationPhy* baseStation, unsigned int ttiNumber)
{
    // if there are no associated users to that BS, we can skip the whole thing
    if (associatedUsers.find(baseStation) == associatedUsers.end())
        return;
    
    unsigned int numAssociated = associatedUsers[baseStation].size();
    
    for (unsigned int u = 0; u < numAssociated; u++) 
    {
        imtaphy::StationPhy* userStation = associatedUsers[baseStation][u];    

        determineNoPrecodingSINRs(baseStation, userStation, usersChannelStates[userStation], 1, ttiNumber);
    }
}

void 
LteRel10UplinkChannelStatusManager::setReferencePerPRBTxPowerForUser(wns::node::Interface* node, wns::Power referencePower)
{
    assure(node2StationLookup.find(node) != node2StationLookup.end(), "Got reference power for unknown node");

    referencePowerMap[node2StationLookup[node]] = referencePower;
}


void 
LteRel10UplinkChannelStatusManager::registerBS(imtaphy::StationPhy* baseStation, 
                                               std::vector< imtaphy::StationPhy* >& associatedMobileStations, 
                                               imtaphy::Channel* channel)
{
    imtaphy::receivers::LinearReceiver* receiver = dynamic_cast<imtaphy::receivers::LinearReceiver*>(baseStation->getReceiver());
    assure(receiver, "Expected a LinearReceiver instance");
   
    receivers[baseStation] = receiver;
    
    if (!initialized)
    {
        if (enabled)
        {
            this->startObserving(channel);
        }
        
        numPRBs = channel->getSpectrum()->getNumberOfPRBs(imtaphy::Uplink);
        srsOffsets = std::vector<unsigned int>(numPRBs, 1);

        initialized = true;
    }

    if (associatedMobileStations.size() > 0)
    {
        MESSAGE_SINGLE(NORMAL, logger, "Adding eNB with " << associatedMobileStations.size() << " associated UEs");
        
        associatedUsers[baseStation] = associatedMobileStations;
        baseStations.push_back(baseStation); // we need this vector in the #pragma omp parallel for loop
    }
    
    for (unsigned int u = 0; u < associatedMobileStations.size(); u++)
    {
        imtaphy::StationPhy* station = associatedMobileStations[u];
        wns::node::Interface* node = station->getNode();
        node2StationLookup[node]= station;
        
        usersChannelStates[station] = new LteRel10UplinkChannelStatusContainer(node, numPRBs, srsOffsets, srsPeriod, bufferLength);
        
        // some initial transmit power value
        referencePowerMap[station] = wns::Power::from_mW(1);
    }
    
    node2StationLookup[baseStation->getNode()] = baseStation;
}

LteRel10UplinkChannelStatusPtr 
LteRel10UplinkChannelStatusManager::getChannelState(wns::node::Interface* node, unsigned int ttiNumber)
{
    assure(node2StationLookup.find(node) != node2StationLookup.end(), "No station registered for this node");
    
    MESSAGE_SINGLE(NORMAL, logger, "For current TTI=" << ttiNumber << " returning channel state for user " << node->getName() << " with delay of " << statusDelay);
    
    // get a delayed entry from the ringbuffer
    return usersChannelStates[node2StationLookup[node]]->getChannelState(ttiNumber - statusDelay);
}

void 
LteRel10UplinkChannelStatusManager::updatePRBsForUserNow(wns::node::Interface* user, imtaphy::interface::PRBVector& prbs)
{
     assure(node2StationLookup.find(user) != node2StationLookup.end(), "No station registered for this node");
     assure(usersChannelStates.find(node2StationLookup[user]) != usersChannelStates.end(), "status container not found");

     usersChannelStates[node2StationLookup[user]]->updatePRBsNow(prbs);
}


