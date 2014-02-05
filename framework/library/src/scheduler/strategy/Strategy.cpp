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

#include <WNS/scheduler/CallBackInterface.hpp>
#include <WNS/scheduler/RegistryProxyInterface.hpp>
#include <WNS/scheduler/strategy/Strategy.hpp>
#include <WNS/scheduler/strategy/StrategyInterface.hpp>
#include <WNS/PowerRatio.hpp>

using namespace wns::scheduler;
using namespace wns::scheduler::strategy;

Strategy::Strategy(const wns::pyconfig::View& config)
    : colleagues(),
      friends(),
      pyConfig(config),
      logger(config.get("logger"))
{
    simTimeType symbolDuration = config.get<double>("symbolDuration");
    bool txMode = config.get<bool>("txMode"); // Python parameter
    bool excludeTooLowSINR = config.get<bool>("excludeTooLowSINR");
    bool powerControlSlave = config.get<bool>("powerControlSlave");
    PowerControlType powerControlType;

    MESSAGE_SINGLE(NORMAL, logger,"Strategy=" 
        << config.get<std::string>("nameInStrategyFactory"));

    // there are three positions for the scheduler...
    if (txMode && !powerControlSlave) 
    {
        // 1.) BS.Tx (DL) -> power from Python (myself)
        powerControlType = PowerControlDLMaster;
    } 
    else if(!txMode && !powerControlSlave) 
    {
        // 2.) BS.Rx (UL) -> power from Python (user)
        powerControlType = PowerControlULMaster;
    } 
    else if(txMode && powerControlSlave) 
    {
        // 3.) UT.Tx (UL) -> power from masterBurst
        powerControlType = PowerControlULSlave;
    } 
    else 
    {
        assure(0,"invalid case for powerControlType: txMode="
            << txMode <<", powerControlSlave=" << powerControlSlave);
        throw wns::Exception("invalid case for powerControlType");
    }
    getNewSchedulerState();
    schedulerState->symbolDuration = symbolDuration;
    schedulerState->isTx = txMode;
    schedulerState->powerControlType = powerControlType;
    schedulerState->excludeTooLowSINR = excludeTooLowSINR; 
}

Strategy::~Strategy()
{
    if (colleagues.apcstrategy) 
        delete colleagues.apcstrategy;
    if (colleagues.dsastrategy) 
        delete colleagues.dsastrategy;
    if (colleagues.dsafbstrategy) 
        delete colleagues.dsafbstrategy;

    // delete SchedulerState:
    if (schedulerState) 
    {
        if (schedulerState->currentState) 
        {
            schedulerState->clearMap();
            if (schedulerState->currentState->strategyInput) 
            {
                schedulerState->currentState->strategyInput = NULL;
            }
            if (schedulerState->currentState->channelQualitiesOfAllUsers) 
            {
                schedulerState->currentState->channelQualitiesOfAllUsers = ChannelQualitiesOfAllUsersPtr();
            }
            if (schedulerState->currentState->schedulingMap) 
            {
                schedulerState->currentState->schedulingMap = SchedulingMapPtr(); 
            }
            if (schedulerState->currentState->bursts) 
            {
                schedulerState->currentState->bursts = MapInfoCollectionPtr();
            }
            schedulerState->currentState->strategyInput = NULL;
            schedulerState->currentState = RevolvingStatePtr(); 
        }
        schedulerState = SchedulerStatePtr(); 
    }
}

void
Strategy::setColleagues(queue::QueueInterface* _queue,
                        grouper::GroupingProviderInterface* _grouper,
                        RegistryProxyInterface* _registry,
                        wns::scheduler::harq::HARQInterface* _harq
    )
{
    MESSAGE_SINGLE(NORMAL, logger,"Strategy::setColleagues(): creating DSA/APC strategies...");
    colleagues.queue = _queue;
    colleagues.grouper = _grouper;
    colleagues.registry = _registry;
    colleagues.harq = _harq; 

    assure(dynamic_cast<queue::QueueInterface*>(colleagues.queue), "Need access to the queue");
    assure(dynamic_cast<grouper::GroupingProviderInterface*>(colleagues.grouper), "Need access to the grouper");
    assure(dynamic_cast<RegistryProxyInterface*>(colleagues.registry), "Need access to the registry");

    std::string apcstrategyName   = pyConfig.get<std::string>("apcstrategy.nameInAPCStrategyFactory");
    std::string dsastrategyName   = pyConfig.get<std::string>("dsastrategy.nameInDSAStrategyFactory");
    std::string dsafbstrategyName = pyConfig.get<std::string>("dsafbstrategy.nameInDSAStrategyFactory");

    MESSAGE_SINGLE(NORMAL, logger,"dsastrategy   = "<<dsastrategyName);
    MESSAGE_SINGLE(NORMAL, logger,"dsafbstrategy = "<<dsafbstrategyName);
    MESSAGE_SINGLE(NORMAL, logger,"apcstrategy   = "<<apcstrategyName);

    // create the scheduling dsastrategy
    wns::scheduler::strategy::dsastrategy::DSAStrategyCreator* dsastrategyCreator;
    dsastrategyCreator = wns::scheduler::strategy::dsastrategy::DSAStrategyFactory::creator(dsastrategyName);
    colleagues.dsastrategy = dsastrategyCreator->create(pyConfig.get<wns::pyconfig::View>("dsastrategy"));
    assure(colleagues.dsastrategy, "DSAStrategy module creation failed");

    // create the scheduling dsafallbackstrategy
    wns::scheduler::strategy::dsastrategy::DSAStrategyCreator* dsafbstrategyCreator;
    dsafbstrategyCreator = wns::scheduler::strategy::dsastrategy::DSAStrategyFactory::creator(dsafbstrategyName);
    colleagues.dsafbstrategy = dsafbstrategyCreator->create(pyConfig.get<wns::pyconfig::View>("dsafbstrategy"));
    assure(colleagues.dsafbstrategy, "DSAfbStrategy module creation failed");

    // create the scheduling apcstrategy
    wns::scheduler::strategy::apcstrategy::APCStrategyCreator* apcstrategyCreator;
    apcstrategyCreator = wns::scheduler::strategy::apcstrategy::APCStrategyFactory::creator(apcstrategyName);
    colleagues.apcstrategy = apcstrategyCreator->create(pyConfig.get<wns::pyconfig::View>("apcstrategy"));
    assure(colleagues.apcstrategy, "APCStrategy module creation failed");

    assure(colleagues.dsastrategy != NULL, "dsastrategy = NULL");
    assure(colleagues.dsafbstrategy != NULL, "dsafbstrategy = NULL");
    assure(colleagues.apcstrategy != NULL, "apcstrategy = NULL");

    colleagues.dsastrategy->setColleagues(colleagues.registry);
    colleagues.dsafbstrategy->setColleagues(colleagues.registry);
    colleagues.apcstrategy->setColleagues(colleagues.registry);

    assure(schedulerState!=SchedulerStatePtr(),"schedulerState must be valid");
    bool useCQI = colleagues.registry->getCQIAvailable();
    schedulerState->useCQI = useCQI;
    schedulerState->myUserID = colleagues.registry->getMyUserID();

    assure(!colleagues.dsafbstrategy->requiresCQI(),"dsafbstrategy must never require CQI");
    assure(useCQI || !colleagues.dsastrategy->requiresCQI(),"dsastrategy requires CQI");
    assure(useCQI || !colleagues.apcstrategy->requiresCQI(),"apcstrategy requires CQI");

    // calls method of derived class for initialization
    this->onColleaguesKnown(); 
} // setColleagues

void
Strategy::onColleaguesKnown()
{
    MESSAGE_SINGLE(NORMAL, logger,"Strategy::onColleaguesKnown()");
}

void
Strategy::setFriends(wns::service::phy::ofdma::BFInterface* _ofdmaProvider)
{
    friends.ofdmaProvider = _ofdmaProvider;
    assure(friends.ofdmaProvider != NULL, "ofdmaProvider==NULL => eirpLimited undefined");
    assure(schedulerState!=SchedulerStatePtr(),"schedulerState must be valid");

    bool eirpLimited = friends.ofdmaProvider->isEIRPLimited();
    schedulerState->eirpLimited = eirpLimited;

    assure(colleagues.registry!=NULL,"colleagues.registry==NULL");

    bool isDL = colleagues.registry->getDL(); 
    if ((isDL == true) && (schedulerState->isTx == false)) 
    {
        MESSAGE_SINGLE(NORMAL, logger,"Strategy::setFriends(): misconfiguration: isDL="
            << isDL << ", isTx=" << schedulerState->isTx);

        // workaround for WiMAC where registry->getDL() is not implemented
        isDL = false; 
    }
    schedulerState->isDL = isDL;
    assure((isDL && (schedulerState->powerControlType == PowerControlDLMaster)) || !isDL,
            logger.getLoggerName() << ": invalid: powerControlType="
            << schedulerState->powerControlType 
            << ",isDL=" << isDL << ",isTx="
            << schedulerState->isTx);

    // there are three positions for the scheduler...
    wns::scheduler::SchedulerSpotType schedulerSpot;
    if (isDL && schedulerState->isTx ) 
    {
        schedulerSpot = wns::scheduler::SchedulerSpot::DLMaster();
        assure(canHandleDL(), "DLMaster");
    } 
    else if(!isDL && !schedulerState->isTx ) 
    {
        schedulerSpot = wns::scheduler::SchedulerSpot::ULMaster();
        assure(canHandleUL(), "ULMaster");
    } 
    else if(!isDL && schedulerState->isTx ) 
    {
        schedulerSpot = wns::scheduler::SchedulerSpot::ULSlave();
        assure(canBeSlave(), "ULSlave scheduler needs SlaveAspect of strategy");
        assure(!schedulerState->useCQI,"cannot useCQI in slave scheduler");
    } 
    else 
    {
        assure(0,"invalid case for schedulerSpot: isDL="<<isDL<<", isTx="<<schedulerState->isTx);
        throw wns::Exception("invalid case for schedulerSpot");
    }
    schedulerState->schedulerSpot = schedulerSpot;

    MESSAGE_SINGLE(NORMAL, logger,"Strategy::setFriends(): isDL="
        << isDL <<", isTx=" << schedulerState->isTx
        << ", schedulerSpot=" << wns::scheduler::SchedulerSpot::toString(schedulerSpot)
        << ", useCQI=" << schedulerState->useCQI);
} // setFriends

SchedulerStatePtr
Strategy::getNewSchedulerState()
{
    MESSAGE_SINGLE(NORMAL, logger,"Strategy::getNewSchedulerState()");

    assure(schedulerState==SchedulerStatePtr(),
        "schedulerState must be empty before making a new one");

    schedulerState = SchedulerStatePtr(new SchedulerState(this));

    assure(schedulerState!=SchedulerStatePtr(),"schedulerState must be valid");

    return schedulerState;
}

SchedulerStatePtr
Strategy::revolveSchedulerState(const StrategyInput& strategyInput)
{
    assure(schedulerState != SchedulerStatePtr(),"schedulerState must be valid");
    assure(colleagues.registry != NULL,"registry == NULL");
    assure(&strategyInput != NULL,"strategyInput == NULL");

    schedulerState->currentState = RevolvingStatePtr(new RevolvingState(&strategyInput));
    return schedulerState;
}

SchedulerStatePtr
Strategy::getSchedulerState()
{
    assure(schedulerState != SchedulerStatePtr(),"schedulerState must be valid");
    return schedulerState;
}

MapInfoCollectionPtr
Strategy::getMapInfo() const 
{
    // if startScheduling has never been called but mapHandler calls getMapInfo(): return empty
    if (schedulerState->currentState==wns::scheduler::strategy::RevolvingStatePtr())
        return MapInfoCollectionPtr(new MapInfoCollection);

    MapInfoCollectionPtr bursts = schedulerState->currentState->bursts; 

    assure(bursts!=MapInfoCollectionPtr(),"bursts must not be NULL here");

    return bursts;
}

wns::scheduler::PowerCapabilities
Strategy::getPowerCapabilities(const UserID user) const
{
    /* This method changes the state "schedulerState->powerCapabilities".
       This is good and ok if all users are the same (default of all systems),
       but if they are different (future), APC must call this function anytime.
    */
    switch (schedulerState->powerControlType)
    {
        case PowerControlDLMaster:
        {
            schedulerState->powerCapabilities = 
                colleagues.registry->getPowerCapabilities(); 
            break;
        }
        case PowerControlULMaster:
        {
            // peer unknown. Assume peer=UT.
            if (!user.isValid()) 
            { 
                MESSAGE_SINGLE(NORMAL, logger, "getPowerCapabilities(NULL): asking registry...");
                schedulerState->powerCapabilities = colleagues.registry->getPowerCapabilities(user); 

                MESSAGE_SINGLE(NORMAL, logger, "getPowerCapabilities(NULL): nominal="
                    << schedulerState->powerCapabilities.nominalPerSubband);

                return schedulerState->powerCapabilities;
            }
            schedulerState->powerCapabilities = colleagues.registry->getPowerCapabilities(user); 
            break;
        }
        case PowerControlULSlave:
        {
            if (schedulerState->defaultTxPower!=wns::Power()) 
            {
                // don't use powerCapabilities but masterBurst instead
                assure(schedulerState->defaultTxPower!=wns::Power(),"undefined defaultTxPower");
                schedulerState->powerCapabilities.nominalPerSubband = schedulerState->defaultTxPower;
                schedulerState->powerCapabilities.maxPerSubband = schedulerState->defaultTxPower;
                // limit not used for slave
                schedulerState->powerCapabilities.maxOverall = schedulerState->defaultTxPower * 1000.0; 
            } 
            // not given (by masterBurst) because there is an InputSchedulingMap (new method)
            else 
            { 
                assure(schedulerState->currentState->strategyInput->inputSchedulingMap != wns::scheduler::SchedulingMapPtr(), 
                    "need inputSchedulingMap with txPower information");

            // the power settings in inputSchedulingMap can be overwritten
            // if APC strategy is changed to use nominal power. Therefore we need these values:
            schedulerState->powerCapabilities = colleagues.registry->getPowerCapabilities(); 
            }
            break;
        }
        default:
        {
            throw wns::Exception("invalid powerControlType");
        }
    } // switch(powerControlType)
    if (user.isValid()) 
    {
        MESSAGE_SINGLE(NORMAL, logger, "getPowerCapabilities("
            << user.getName() << "): nominal="
            << schedulerState->powerCapabilities.nominalPerSubband);

    } 
    else 
    {
        MESSAGE_SINGLE(NORMAL, logger, "getPowerCapabilities(NULL): nominal="
            << schedulerState->powerCapabilities.nominalPerSubband);
    }
    assure(schedulerState->powerCapabilities.nominalPerSubband != wns::Power(),
        "undefined power nominalPerSubband="<<schedulerState->powerCapabilities.nominalPerSubband);

    return schedulerState->powerCapabilities;
}

// perform master scheduling
StrategyResult 
Strategy::startScheduling(const StrategyInput& strategyInput)
{
    if (strategyInput.fChannels  < 1)          
        throw wns::Exception("Need at least one subBand");

    if (strategyInput.slotLength < 0.0)        
        throw wns::Exception("Invalid slotLength");

    if (strategyInput.slotLength < schedulerState->symbolDuration)    
        throw wns::Exception("Can't schedule on slot shorter than OFDM symbol");

    if (strategyInput.numberOfTimeSlots < 1)   
        throw wns::Exception("Need at least one TimeSlot");

    // strategyInput.maxSpatialLayers > 1 means SDMA or MIMO
    if (strategyInput.maxSpatialLayers   < 1)          
        throw wns::Exception("Need at least one spatialLayer");

    assure(isNewStrategy() || strategyInput.beamforming || strategyInput.maxSpatialLayers == 1,
           "beamforming=" << strategyInput.beamforming
            << " && maxSpatialLayers="
            << strategyInput.maxSpatialLayers
            << ": MIMO not supported in old strategies");

    // prepare a new state for this timeFrame:
    schedulerState = revolveSchedulerState(strategyInput);

    // master scheduling on freshly created empty resources
    // the fresh Scheduling Map (empty and free) is prepared in the RevolvingStatePtr() constructor
    if (strategyInput.inputSchedulingMap == wns::scheduler::SchedulingMapPtr()) 
    { 
        MESSAGE_SINGLE(NORMAL, logger, "inputSchedulingMap is empty");
        
    } 
    // slave scheduling or preallocated SchedulingMap 
    else 
    { 
        MESSAGE_SINGLE(NORMAL, logger, "inputSchedulingMap exists: "
            << strategyInput.inputSchedulingMap->toString());

        assure(schedulerState->currentState->schedulingMap != wns::scheduler::SchedulingMapPtr(),
            "schedulingMap initialization failed");
        assure(schedulerState->currentState->schedulingMap == strategyInput.inputSchedulingMap,
            "schedulingMap must be set in revolveSchedulerState()");
    }
    wns::scheduler::SchedulingMapPtr schedulingMap = schedulerState->currentState->schedulingMap; // alias

    // master scheduling
    if (schedulerState->schedulerSpot != wns::scheduler::SchedulerSpot::ULSlave())
    { 
        assure(strategyInput.mapInfoEntryFromMaster == MapInfoEntryPtr(),"mapInfoEntryFromMaster must be NULL");

        MESSAGE_BEGIN(NORMAL, logger, m, "startScheduling(master): ");
        if (strategyInput.frameNrIsValid())
            m << "frameNr=" << strategyInput.frameNr << ", ";
            m << "fChannels="   << strategyInput.fChannels
            << ", maxSpatialLayers="   << strategyInput.maxSpatialLayers
            << ", slotLength=" << strategyInput.slotLength;
        MESSAGE_END();

        // not necessary but these defaults may be optionally provided by the caller:
        // may be undefined (NULL) in most cases
        schedulerState->setDefaultPhyMode(strategyInput.defaultPhyModePtr); 
        // may be undefined (0.0) in most cases
        schedulerState->setDefaultTxPower(strategyInput.defaultTxPower); 

        // only if master and beamforming
        // grouping needed for beamforming & its antenna pattern
        if (groupingRequired() && !colleagues.queue->isEmpty())
        {   
            GroupingPtr sdmaGrouping = GroupingPtr(new Grouping());
            UserSet allUsers;
            allUsers = colleagues.queue->getQueuedUsers();
            UserSet activeUsers = colleagues.registry->filterReachable(allUsers, strategyInput.getFrameNr());
            if (schedulerState->isTx) // transmitter grouping
            {
                MESSAGE_SINGLE(NORMAL, logger, "StartScheduling(): get TxGrouping");
                sdmaGrouping = colleagues.grouper->getTxGroupingPtr(activeUsers, strategyInput.maxSpatialLayers);
            } 
            // receiver grouping
            else 
            {
               MESSAGE_SINGLE(NORMAL, logger, "StartScheduling(): get RxGrouping");
               sdmaGrouping = colleagues.grouper->getRxGroupingPtr(activeUsers, strategyInput.maxSpatialLayers);
            }
            schedulerState->currentState->setGrouping(sdmaGrouping);

            assure(schedulerState->currentState->getGrouping() == sdmaGrouping,"invalid grouping");

            MESSAGE_SINGLE(NORMAL, logger, "StartScheduling(): Number of Groups = " << sdmaGrouping->groups.size());
            MESSAGE_SINGLE(NORMAL, logger, "StartScheduling(): grouping.getDebugOutput = " << sdmaGrouping->getDebugOutput());
        } 
        else 
        {
            MESSAGE_SINGLE(VERBOSE, logger, "StartScheduling(): no grouping required.");
        }
    
    } 
    // slave scheduling
    else 
    { 
        // two ways of master input:
        // 1. old way: mapInfoEntryFromMaster
        // 2. new way: inputSchedulingMap
        if (strategyInput.mapInfoEntryFromMaster != MapInfoEntryPtr()) 
        {
            assure(strategyInput.mapInfoEntryFromMaster != MapInfoEntryPtr(),"mapInfoEntryFromMaster must be non-NULL");

            MESSAGE_SINGLE(NORMAL, logger,"startScheduling(slave): fChannels="
                << strategyInput.fChannels
                << ", subBand=" 
                << strategyInput.mapInfoEntryFromMaster->subBand);

            MESSAGE_SINGLE(NORMAL, logger,"PhyMode="
                << *(strategyInput.mapInfoEntryFromMaster->phyModePtr)
                << ", txPower=" << strategyInput.mapInfoEntryFromMaster->txPower);

            schedulerState->setDefaultPhyMode(strategyInput.mapInfoEntryFromMaster->phyModePtr);
            schedulerState->setDefaultTxPower(strategyInput.mapInfoEntryFromMaster->txPower);
        } 
        else 
        {
            assure(strategyInput.inputSchedulingMap != wns::scheduler::SchedulingMapPtr(), 
                "slave scheduling requires inputSchedulingMap");

            MESSAGE_SINGLE(NORMAL, logger, "SlaveScheduling with given inputSchedulingMap...");

            // all PhyModes and TxPower are written in the inputSchedulingMap
            // I may only use those resources (subchannels,slots) where my userID is written into (UTx)
            // Before going to UT's PhyUser, take care to use the right userID!
            // it must be reverted (to BSx) in the PhyCommand to send to the right peer.

            assure(schedulerState->currentState->schedulingMap == strategyInput.inputSchedulingMap,
                "schedulingMap must be set in revolveSchedulerState()");

            // set empty all resources but keep userID, PhyMode, TxPower
            MESSAGE_SINGLE(NORMAL, logger, "inputSchedulingMap->processMasterMap()");
            schedulerState->currentState->schedulingMap->processMasterMap();

            MESSAGE_SINGLE(NORMAL, logger, "prepared master schedulingMap="
                << schedulerState->currentState->schedulingMap->toString());
        }
    }
    // empty bursts result datastructure MapInfoCollection (not schedulingMap)
    schedulerState->clearMap(); 

    // prepare CQI
    if (schedulerState->useCQI)
    {
        schedulerState->currentState->channelQualitiesOfAllUsers =
            ChannelQualitiesOfAllUsersPtr(new ChannelQualitiesOfAllUsers());
    }

    // Initialize the "working" datastructures
    // new burst result structure (part of the state):
    assure(schedulerState->currentState->bursts==MapInfoCollectionPtr(),"bursts must be NULL here");

    MapInfoCollectionPtr bursts = MapInfoCollectionPtr(new wns::scheduler::MapInfoCollection);
    schedulerState->currentState->bursts = bursts;

    // initialize helper strategies for DSA,APC:
    if (colleagues.dsastrategy != NULL)
        colleagues.dsastrategy->initialize(schedulerState, schedulingMap);

    if (colleagues.dsafbstrategy != NULL)
        colleagues.dsafbstrategy->initialize(schedulerState, schedulingMap);

    if (colleagues.apcstrategy != NULL)
        colleagues.apcstrategy->initialize(schedulerState,schedulingMap);

    // this calls the derived strategies (various algorithms):
    StrategyResult strategyResult =
        this->doStartScheduling(schedulerState, schedulingMap);

    assure(strategyResult.schedulingMap == schedulingMap,"schedulingMap mismatch");

    if (colleagues.apcstrategy != NULL)
        colleagues.apcstrategy->postProcess(schedulerState,schedulingMap);

    return strategyResult;
} // startScheduling


MapInfoEntryPtr
Strategy::doAdaptiveResourceScheduling(RequestForResource& request,
                                       SchedulingMapPtr schedulingMap)
{
    assure(colleagues.dsastrategy != NULL, "dsastrategy==NULL");
    assure(colleagues.dsafbstrategy != NULL, "dsafbstrategy==NULL");
    assure(colleagues.apcstrategy != NULL, "apcstrategy==NULL");
    assure(schedulerState->currentState != RevolvingStatePtr(),
        "currentState must be valid");
    assure(schedulerState->currentState->strategyInput != NULL, 
        "strategyInput must be valid");

    int frameNr = schedulerState->currentState->strategyInput->getFrameNr();
    MESSAGE_SINGLE(NORMAL, logger,"doAdaptiveResourceScheduling("
        << request.user.getName()
        << ",cid="
        << request.cid
        << ",bits="
        << request.bits
        << "): useCQI="
        << schedulerState->useCQI);

    // empty means no result. Filled later
    MapInfoEntryPtr resultMapInfoEntry; 

    // The slave RS-TX in the UT does not need to and cannot do AdaptiveResourceScheduling:
    if (schedulerState->powerControlType == PowerControlULSlave)
    {
        assure(schedulerState->schedulerSpot==wns::scheduler::SchedulerSpot::ULSlave(),
               "PowerControlULSlave requires SchedulerSpot::ULSlave");
        assure(schedulerState->currentState->strategyInput!=NULL,"need strategyInput");

        // don't do anything. Just return the known=given masterBurst
        if (schedulerState->currentState->strategyInput->mapInfoEntryFromMaster != MapInfoEntryPtr())
        { 
            assure(schedulerState->currentState->strategyInput->mapInfoEntryFromMaster != MapInfoEntryPtr(),
                "need masterBurst");

            // copy and new SmartPtr to carry the result
            MapInfoEntryPtr mapInfoEntry = MapInfoEntryPtr(
                new MapInfoEntry(*(schedulerState->currentState->strategyInput->mapInfoEntryFromMaster))); 

            assure(mapInfoEntry != MapInfoEntryPtr(),"need masterBurst");
            assure(mapInfoEntry->user.isValid(),"need user in masterBurst");

            MESSAGE_SINGLE(NORMAL, logger,"doAdaptiveResourceScheduling(): using masterBurst in slave mode: user="
                << UserID(mapInfoEntry->user).getName()
                << " -> request.user="
                << request.user.getName()
                << " (destination peer)");

            mapInfoEntry->user = request.user;

            assure(mapInfoEntry->phyModePtr!=wns::service::phy::phymode::PhyModeInterfacePtr(),
                "phyMode must be defined in masterBurst" << mapInfoEntry->toString());

            // needed later
            request.phyModePtr=mapInfoEntry->phyModePtr; 

            // no space
            if (!schedulingMap->pduFitsInto(request,mapInfoEntry)) 
                return resultMapInfoEntry; 

            return mapInfoEntry;
        } 
        else 
        {
            MESSAGE_SINGLE(NORMAL, logger,"doAdaptiveResourceScheduling(): using inputSchedulingMap. request.user="
                << request.user.getName()
                << " (destination peer)");

            assure(schedulerState->currentState->strategyInput->inputSchedulingMap != wns::scheduler::SchedulingMapPtr(), +
                "slave scheduling requires inputSchedulingMap");
            assure(!colleagues.dsastrategy->requiresCQI(),"SlaveScheduler DSAStrategy must not require CQI");
        }
    }

    // SmartPtr (allocated in CQI)
    ChannelQualitiesOnAllSubBandsPtr cqiForUser; 

    assure(schedulerState->defaultPhyModePtr == wns::service::phy::phymode::PhyModeInterfacePtr(),
           "defaultPhyModePtr must not be set at this point (either master scheduler or inputSchedulingMap method)");

    if (schedulerState->useCQI)
    {
        assure (schedulerState->powerControlType!=PowerControlULSlave,"slave must not use CQI");

        // is the CQI state already available in our state?
        // NO -> get current CQI state
        if (!schedulerState->currentState->channelQualitiesOfAllUsers->knowsUser(request.user))
        { 
            // RS-TX (master DL)
            if (schedulerState->isTx)
            { 
                // TODO: change CQIHandler so that it produces SmartPtr
                cqiForUser = colleagues.registry->getChannelQualities4UserOnDownlink(request.user, frameNr);
            } 
            // RS-RX (master UL)
           else 
            { 
                cqiForUser = colleagues.registry->getChannelQualities4UserOnUplink(request.user, frameNr);
            }
            (*(schedulerState->currentState->channelQualitiesOfAllUsers)).insert(
                std::map<UserID,ChannelQualitiesOnAllSubBandsPtr>::value_type(request.user,cqiForUser));

        } 
        else 
        {
            cqiForUser = schedulerState->currentState->channelQualitiesOfAllUsers->find(request.user)->second;
        }
    } 

    dsastrategy::DSAResult dsaResult;
    int subChannel = dsastrategy::DSAsubChannelNotFound;
    int timeSlot = 0; 
    int spatialLayer = 0; 
    bool CQIrequired = colleagues.dsastrategy->requiresCQI();
    bool CQIavailable = (cqiForUser!=ChannelQualitiesOnAllSubBandsPtr())
        && (schedulerState->currentState->channelQualitiesOfAllUsers != ChannelQualitiesOfAllUsersPtr())
        && ((*(schedulerState->currentState->channelQualitiesOfAllUsers))[request.user]->size() > 0);

    MESSAGE_SINGLE(NORMAL, logger,"doAdaptiveResourceScheduling("
        << request.user.getName()
        << ",cid=" << request.cid
        << ",bits=" << request.bits
        << "): useCQI=" << schedulerState->useCQI
        << ",CQIrequired=" << CQIrequired 
        << ",CQIavailable="<<CQIavailable);

    // memory of request structure
    ChannelQualityOnOneSubChannel& cqiOnSubChannel
        = request.cqiOnSubChannel; 

    // start dynamic subchannel assignment (DSA):
    // with or without CQI information?
    if (schedulerState->useCQI && CQIrequired && CQIavailable)
    {
        assure (schedulerState->powerControlType!=PowerControlULSlave,"slave must not use CQI");
        MESSAGE_SINGLE(NORMAL, logger,"doAdaptiveResourceScheduling("
            << request.user.getName()
            << ",cid="
            << request.cid
            << ",bits="
            << request.bits
            << "): calling dsastrategy");

        // start dynamic subchannel assignment
        dsaResult = colleagues.dsastrategy->getSubChannelWithDSA(request, schedulerState, schedulingMap);
        subChannel = dsaResult.subChannel;
        timeSlot = dsaResult.timeSlot;
        spatialLayer = dsaResult.spatialLayer;

        if (subChannel==dsastrategy::DSAsubChannelNotFound)
            return resultMapInfoEntry; 

        assure(cqiForUser!=ChannelQualitiesOnAllSubBandsPtr(), "cqiForUser["
            << request.user.getName()
            << "]==NULL");

        // this writes into request structure:
        // copy
        cqiOnSubChannel = (*cqiForUser)[subChannel]; 

        assure(&cqiOnSubChannel == &(request.cqiOnSubChannel),"copy failed: addresses don't match");

        MESSAGE_SINGLE(NORMAL, logger,"doAdaptiveResourceScheduling("
            << request.user.getName()
            << ",cid=" << request.cid
            << ",bits=" << request.bits
            << "): subChannel="<<subChannel);
    } 
    // no CQI required or available
    else 
    { 
        if (!CQIrequired) 
        {
            MESSAGE_SINGLE(NORMAL, logger,"doAdaptiveResourceScheduling(): calling dsastrategy");
            // start dynamic subchannel assignment
            dsaResult = colleagues.dsastrategy->getSubChannelWithDSA(request, schedulerState, schedulingMap);
        } 
        // no CQI available (principially or temporary)
        else 
        { 
            MESSAGE_SINGLE(NORMAL, logger,"doAdaptiveResourceScheduling(): calling dsafbstrategy (fallback)");
            dsaResult = colleagues.dsafbstrategy->getSubChannelWithDSA(request, schedulerState, schedulingMap);
        }
        subChannel = dsaResult.subChannel;
        timeSlot = dsaResult.timeSlot;
        spatialLayer = dsaResult.spatialLayer;

        if (subChannel==dsastrategy::DSAsubChannelNotFound)
            return resultMapInfoEntry; 

        // assume flat channel and nominal TxPower for this case
        wns::Power nominalPowerPerSubChannel = getPowerCapabilities(request.user).nominalPerSubband;
        cqiOnSubChannel = (schedulerState->isTx)?
            (colleagues.registry->estimateTxSINRAt(request.user)) // Tx
            :
            (colleagues.registry->estimateRxSINROf(request.user)); // Rx
    } 

    // Tell result of DSA: subChannel
    MESSAGE_SINGLE(NORMAL, logger,"doAdaptiveResourceScheduling("
        << request.user.getName()
        << ",cid="<<request.cid
        << ",bits="<<request.bits<<"):"
        << " subChannel.tSlot.spatialLayer="
        << subChannel<<"."<<timeSlot<<"."<<spatialLayer);

    if (subChannel == dsastrategy::DSAsubChannelNotFound)
        return resultMapInfoEntry; 

    // must be replaced by code which copes with that case:
    assure(subChannel != dsastrategy::DSAsubChannelNotFound, "DSAsubChannelNotFound");

    // fix the proposed subChannel value:
    request.subChannel = subChannel;
    request.timeSlot = timeSlot;
    request.spatialLayer = spatialLayer;

    wns::Power txPower;
    apcstrategy::APCResult apcResult;
    /** @todo: instead of this if-clause we could also use the "APCSlave" strategy */
    // Master
    if (schedulerState->powerControlType!=PowerControlULSlave)
    { 
        // do adaptive power allocation
        apcResult = colleagues.apcstrategy->doStartAPC(request, schedulerState, schedulingMap);
        // not (yet) possible: 
        if (apcResult.txPower == wns::Power())
            return resultMapInfoEntry; 

        // fix the proposed phyMode value:
        request.phyModePtr = apcResult.phyModePtr;
        txPower = apcResult.txPower;

        MESSAGE_SINGLE(NORMAL, logger,"doAdaptiveResourceScheduling(): txP="
            << apcResult.txPower
            << ", pl=" << cqiOnSubChannel.pathloss
            << ", sinr=" << apcResult.sinr
            << ", PhyMode=" << *(apcResult.phyModePtr));

        double minSINRforPhyMode = colleagues.registry->getPhyModeMapper()->getMinimumSINR();

        // this could mean "APC failed"
        if ((apcResult.sinr.get_dB() < minSINRforPhyMode))
        { 
            MESSAGE_SINGLE(NORMAL, logger,"doAdaptiveResourceScheduling(): too low SINR! sinr="
                << apcResult.sinr
                << ", PhyMode="
                << *(apcResult.phyModePtr)
                << " requires " 
                << minSINRforPhyMode << "dB");

            // If the value is extremely low, it is likely that something bad has happened
            assure(apcResult.estimatedCandI.carrier.get_dBm() > -190,
                "sinr=" << apcResult.sinr.get_dB() 
                << " but minSINRforPhyMode="
                << minSINRforPhyMode << " not reached. Estimation was: C="
                << apcResult.estimatedCandI.carrier 
                << ", I="<<apcResult.estimatedCandI.interference<<" (blind)");

            if (schedulerState->excludeTooLowSINR)
            {
                return resultMapInfoEntry; 
            }
        }
    } 
    // slave scheduling (PowerControlULSlave)
    else 
    { 
        request.phyModePtr = schedulingMap->getPhyModeUsedInResource(subChannel,timeSlot,spatialLayer);
        txPower = schedulingMap->getTxPowerUsedInResource(subChannel,timeSlot,spatialLayer);
    }

    // This really fixes the proposed values of DSA and APC.
    resultMapInfoEntry = MapInfoEntryPtr(new MapInfoEntry());
    resultMapInfoEntry->frameNr = frameNr;
    resultMapInfoEntry->subBand = subChannel;
    resultMapInfoEntry->timeSlot = timeSlot;
    resultMapInfoEntry->spatialLayer = spatialLayer;
    resultMapInfoEntry->user = request.user;
    resultMapInfoEntry->sourceUser = schedulerState->myUserID;
    resultMapInfoEntry->txPower = txPower; // apcResult.txPower;
    resultMapInfoEntry->phyModePtr = request.phyModePtr; // = apcResult.phyModePtr
    if(schedulerState->powerControlType != PowerControlULSlave)
        resultMapInfoEntry->estimatedCQI = cqiOnSubChannel; 
    else
    {   
        resultMapInfoEntry->estimatedCQI = 
            schedulingMap->subChannels[subChannel].temporalResources[timeSlot]
                ->physicalResources[spatialLayer].getEstimatedCQI();
    }
   
    // Set antennaPattern etc. according to grouping result
    if (groupingRequired()) 
    {
        assure(schedulerState->currentState->getGrouping() != GroupingPtr(),"invalid grouping");

        wns::service::phy::ofdma::PatternPtr antennaPattern;
        antennaPattern = schedulerState->currentState->getGrouping()->patterns[request.user];
        int groupIndex = schedulerState->currentState->getGrouping()->userGroupNumber[request.user] - 1;

        assure(groupIndex <= schedulerState->currentState->getGrouping()->groups.size(), "invalid group index");

        Group currentGroup = schedulerState->currentState->getGrouping()->groups[groupIndex];
        wns::CandI estimatedCandI = currentGroup[request.user];
        resultMapInfoEntry->estimatedCQI.carrier = estimatedCandI.C;
        resultMapInfoEntry->estimatedCQI.interference = estimatedCandI.I;
        resultMapInfoEntry->estimatedCQI.pathloss = estimatedCandI.PL;
        resultMapInfoEntry->estimatedCQI.sdma.iIntra = estimatedCandI.sdma.iIntra;
        resultMapInfoEntry->txPower = txPower / (1.0 * currentGroup.size());
        resultMapInfoEntry->phyModePtr = colleagues.registry->getPhyModeMapper()->getBestPhyMode(estimatedCandI.C / estimatedCandI.I);

        assure(antennaPattern != wns::service::phy::ofdma::PatternPtr(),"invalid pattern");

        resultMapInfoEntry->pattern = antennaPattern;
    }
    resultMapInfoEntry->start = wns::scheduler::undefinedTime;
    resultMapInfoEntry->end   = wns::scheduler::undefinedTime;

    return resultMapInfoEntry;
} // doAdaptiveResourceScheduling

wns::service::phy::phymode::PhyModeInterfacePtr
Strategy::getBestPhyMode(const wns::Ratio& sinr) const
{
    if (schedulerState->currentState->strategyInput->defaultPhyModePtr != 
        wns::service::phy::phymode::PhyModeInterfacePtr()) 
    {
        assure(schedulerState->currentState->strategyInput->defaultPhyModePtr->isValid(),
            "defaultPhyModePtr is invalid!");

        return schedulerState->currentState->strategyInput->defaultPhyModePtr;
    } 
    // do link adaptation
    else 
    {
        return colleagues.registry->getBestPhyMode(sinr);
    }
}

wns::scheduler::SchedulerSpotType
Strategy::getSchedulerSpotType() const 
{
    assure(schedulerState!=SchedulerStatePtr(),"schedulerState must be valid");
    assure(schedulerState->schedulerSpot>0,"uninitialized schedulerSpot="
        << schedulerState->schedulerSpot);

    return schedulerState->schedulerSpot;
}

bool
Strategy::isTx() const
{
    // comes from Python. isTx=true for RS-TX (DL,UL) in (BS,UT,RN)
    assure(schedulerState!=SchedulerStatePtr(),"schedulerState must be valid");

    return schedulerState->isTx; 
}

// method only to support the old strategies. Not intended for new schedulers.
wns::Power
Strategy::getTxPower() const
{
    wns::Power power;
    MESSAGE_SINGLE(NORMAL, logger, "getTxPower()");

    assure(schedulerState != SchedulerStatePtr(),
        "schedulerState must be valid");
    assure(schedulerState->currentState != RevolvingStatePtr(),
        "currentState must be valid");
    assure(schedulerState->currentState->strategyInput != NULL,
        "need strategyInput");
    assure(!isNewStrategy(),
        "getTxPower() only allowed for the old obsolete strategies");

    // not yet prepared. Do it now.
    if (schedulerState->powerCapabilities.nominalPerSubband==wns::Power())
    { 
        MESSAGE_SINGLE(NORMAL, logger, "getTxPower: asking getPowerCapabilities(NULL)...");
        // NULL because peer unknown
        getPowerCapabilities(UserID()); 
    }
    power = schedulerState->powerCapabilities.nominalPerSubband;

    assure(power != wns::Power(),"undefined power=" << power);

    return power;
}

float
Strategy::getResourceUsage() const
{
    assure(schedulerState!=SchedulerStatePtr(),"schedulerState must be valid");
    assure(schedulerState->currentState!=RevolvingStatePtr(),"currentState must be valid");
    assure(schedulerState->currentState->schedulingMap!=wns::scheduler::SchedulingMapPtr(),"schedulingMap must be valid");

    // This is the default implementation. Overload in your desired strategy
    // if you want information about the resource usage.
    // In general it is better to call schedulingMap->getResourceUsage() in the system specific methods after scheduling
    if (schedulerState->currentState->schedulingMap!=SchedulingMapPtr()) 
    {
        return schedulerState->currentState->schedulingMap->getResourceUsage();
    } 
    else 
    {
        return 0.0;
    }
}

bool
Strategy::groupingRequired() const
{
    assure(schedulerState != SchedulerStatePtr(),
        "schedulerState must be valid");
    assure(schedulerState->currentState != RevolvingStatePtr(),
        "currentState must be valid");
    assure(schedulerState->currentState->strategyInput != NULL,
        "schedulerState->currentState->strategyInput must be valid");

    bool isMaster = (schedulerState->schedulerSpot == wns::scheduler::SchedulerSpot::DLMaster() 
        || schedulerState->schedulerSpot == wns::scheduler::SchedulerSpot::ULMaster());

    return (schedulerState->currentState->strategyInput->beamforming && isMaster);
}
