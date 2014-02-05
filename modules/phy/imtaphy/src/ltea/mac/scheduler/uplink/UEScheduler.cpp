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

#include <IMTAPHY/ltea/mac/scheduler/uplink/UEScheduler.hpp>
#include <algorithm>
#include <DLL/UpperConvergence.hpp>
//#include <IMTAPHY/ltea/pdcp/PDCPCommand.hpp>

#include <DLL/StationManager.hpp>

#include <IMTAPHY/Channel.hpp>

STATIC_FACTORY_REGISTER_WITH_CREATOR(
     ltea::mac::scheduler::uplink::UEScheduler,
     wns::ldk::FunctionalUnit,
     "ltea.mac.scheduler.uplink.UE",
     wns::ldk::FUNConfigCreator);



using namespace ltea::mac::scheduler::uplink;

UEScheduler::UEScheduler(wns::ldk::fun::FUN* fuNet, const wns::pyconfig::View& config) :
    wns::ldk::CommandTypeSpecifier<ltea::mac::DownlinkControlInformation>(fuNet),
    wns::ldk::HasReceptor<>(),
    wns::ldk::HasConnector<>(),
    wns::ldk::HasDeliverer<>(),
    fun(fuNet),
    layer(dynamic_cast<ltea::Layer2*>(fun->getLayer())),
    logger(config.get<wns::pyconfig::View>("logger")),
    txService(NULL),
    mcsLookup(&(ltea::mac::TheMCSLookup::Instance())),
    harq(new ltea::mac::harq::HARQ(config.get("harq"))),
    channel(imtaphy::TheIMTAChannel::getInstance()),
    spectrum(channel->getSpectrum()),
    myULScheduler(NULL),
    totalTxPower(wns::Power::from_dBm(config.get<double>("totalTxPowerdBm")))
{
    assure(fun, "No valid FUN pointer");
    assure(layer, "Could not get Layer2 pointer");
    
    wns::pyconfig::View queueConfig = config.getView("queue");
    queue = wns::StaticFactory<wns::PyConfigViewCreator<
        ltea::rlc::IQueue> >::creator(queueConfig.get<std::string>("nameInQueueFactory"))->create(queueConfig);
    queue->setFUN(fuNet);

    queue->setFUN(fuNet);
    
    
}

void 
UEScheduler::onFUNCreated()
{
    wns::ldk::FunctionalUnit::onFUNCreated();
    
    friends.pdcpCommandReader = fun->getCommandReader("pdcp");
    
    ltea::Layer2* layer = dynamic_cast<ltea::Layer2*>(getFUN()->getLayer());
    assure(layer, "Expecting an ltea::Layer2*");
    
    txService = layer->getTxService();
       
    this->startObserving(&(imtaphy::TheIMTAChannel::Instance()));

    dciReader = fun->getCommandReader(this->getName());
    assure(dciReader, "Could not get my command reader!?");
    harq->setDCIReader(dciReader);
    
    myStation = dynamic_cast<imtaphy::StationPhy*>(txService);
    numTxAntennas = myStation->getAntenna()->getNumberOfElements();
    
    MESSAGE_SINGLE(NORMAL, logger, "Uplink UE Scheduler created");
    
    wns::probe::bus::ContextProviderCollection localcpc(layer->getNode()->getContextProviderCollection());
    tbSizeContextCollector = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(localcpc, "tbSize"));
}


void
UEScheduler::doSendData(const wns::ldk::CompoundPtr& compound)
{
    assure(doIsAccepting(compound), "Not accepting compound");
    
    queue->put(compound);
    
    MESSAGE_SINGLE(NORMAL, logger, "Accepting compound in doSendData");
    

}

void
UEScheduler::doOnData(const wns::ldk::CompoundPtr& compound)
{
    getDeliverer()->getAcceptor(compound)->onData(compound);
}

bool
UEScheduler::doIsAccepting(const wns::ldk::CompoundPtr& compound) const
{
    return queue->isAccepting(compound);
}
void
UEScheduler::doWakeup()
{
    getReceptor()->wakeup();
}


void 
UEScheduler::onNewTTI(unsigned int ttiNumber)
{
    // without PRBs, we don't need to do anything here (used to disable uplink scheduling)
    if (spectrum->getNumberOfPRBs(imtaphy::Uplink) == 0)
        return;
    
    // maybe move this to a better place but be careful to assure that all mobiles are already
    // associated to the base station
    if (ttiNumber == 1)
    {
        std::vector<wns::node::Interface*> associatedStations =  txService->getAssociatedNodes();

        assure(associatedStations.size() == 1, "We should only be associated to our serving base station");
        myServingBaseStation = associatedStations[0];
        
        myULScheduler = dynamic_cast<ltea::mac::scheduler::uplink::enb::SchedulerBase*>(myServingBaseStation->getService<ltea::mac::scheduler::uplink::enb::UplinkGrants*>("UplinkGrants"));
        assure(myULScheduler, "Could not access the UL scheduler in the eNB");
        
        ltea::mac::scheduler::uplink::SchedulingRequest schedulingRequest;
        schedulingRequest.requestingUser = layer->getNode();
        schedulingRequest.totalAvailableTxPower = totalTxPower;
        schedulingRequest.ueScheduler = this;
        
        // request an uplink grant, currently once and for all
        myULScheduler->schedulingRequest(schedulingRequest);
    }
    
    MESSAGE_SINGLE(NORMAL, logger, "Following grants are available at tti number= " << ttiNumber << ":");
    for (std::map<unsigned int, ltea::mac::scheduler::uplink::SchedulingGrant>::const_iterator iter = receivedGrants.begin();
         iter != receivedGrants.end(); iter++)
     {
         MESSAGE_SINGLE(NORMAL, logger, "Grant for TTI " << iter->first << " by " << myServingBaseStation->getName() << " with " << iter->second.prbPowerPrecoders.size() << " PRBs");
     }

    unsigned int harqProcessId = ttiNumber % 8;

    MESSAGE_SINGLE(NORMAL, logger, "For current TTI " << ttiNumber << " I'm acccessing harqProcessId " << harqProcessId);
    
    if (receivedGrants.size() > 0)
    {
        assure(receivedGrants.begin()->first >= ttiNumber, "Somehow I still have an outdated grant");
    }
    
    // TODO: adapt for UL spatial multiplexing
    if (harq->knowsUser(myServingBaseStation) &&  harq->hasRetransmission(myServingBaseStation, harqProcessId, 0))
    {
        wns::ldk::CompoundPtr retransmission = harq->getRetransmission(myServingBaseStation, harqProcessId, 0);
            
        std::vector<wns::ldk::CompoundPtr> tbs(1);
        tbs[0] = retransmission;

        assure(dciReader->commandIsActivated(retransmission->getCommandPool()), "Retransmissions are expected to already have their DCI information stored from the first transmission attempt");
        ltea::mac::DownlinkControlInformation* dci = dciReader->readCommand<ltea::mac::DownlinkControlInformation>(retransmission->getCommandPool());
        assure(dci, "Could not get DCI");

        MESSAGE_SINGLE(NORMAL, logger, "Starting retransmission of TB with ID= " << dci->magic.id << " previously transmitted in in TTI " << dci->magic.transmittedInTTI << " now retransmitting in TTI " << ttiNumber);
        
        // update this to allow correct timing of synchronous HARQ retransmission
        dci->magic.transmittedInTTI = ttiNumber;
        
        txService->registerTransmission(myServingBaseStation,
                                        tbs,
                                        1, // rank-1 forced
                                        dci->local.prbPowerPrecoders
                                        );
        harq->retransmissionStarted(myServingBaseStation, harqProcessId, 0);
    }
    else
    {    
        if (receivedGrants.find(ttiNumber) != receivedGrants.end())
        {
            if (queue->queueHasPDUs(myServingBaseStation))
            {
                MESSAGE_SINGLE(NORMAL, logger, "Got a grant, have traffic for my BS, will start transmitting");
                                
                ltea::mac::scheduler::uplink::SchedulingGrant& grant = receivedGrants[ttiNumber];
                
                assure(grant.scheduledForTTI == ttiNumber, "Wrong grant");
                
                unsigned int layers = 1;
                unsigned int blockSize = mcsLookup->getSize(grant.mcsIndex,
                                                            grant.prbPowerPrecoders.size(),
                                                            layers);
                                                            
                MESSAGE_SINGLE(NORMAL, logger, "Starting transmission on grant with " << grant.prbPowerPrecoders.size() << " PRBs, estimated SINR=" << grant.estimatedSINR 
                                                << " MCSindex=" << grant.mcsIndex << " CR=" << grant.codeRate
                                                << " blocksize=" << blockSize << " from queue with currently " << queue->numBitsForUser(myServingBaseStation) << " bits available");
                
                wns::ldk::CompoundPtr compound = queue->getHeadOfLinePDUSegment(myServingBaseStation, blockSize);
                
                wns::Power totalPower;
                for (imtaphy::interface::PrbPowerPrecodingMap::const_iterator iter = grant.prbPowerPrecoders.begin(); iter != grant.prbPowerPrecoders.end(); iter++)
                {
                    totalPower += iter->second.power;
                }
                
                double epsilon = 0.01;
                assure(totalTxPower.get_mW() <= totalTxPower.get_mW() + epsilon, "Scheduling grant by BS should not assume more Tx power (" 
                                                                                                                       << totalTxPower.get_mW() << " mW) than actually available ("
                                                                                                                       << totalPower.get_mW() << " mW)");
                
                ltea::mac::DownlinkControlInformation* dci0 = activateCommand( compound->getCommandPool() );
                dci0->peer.assignedToLayers = std::vector<unsigned int>(1,1);
                dci0->peer.blockSize = blockSize;
                dci0->peer.codeRate = grant.codeRate;
                dci0->peer.modulation = mcsLookup->getModulation(grant.mcsIndex);
                dci0->local.prbPowerPrecoders = grant.prbPowerPrecoders;
                dci0->magic.mcsIndex = grant.mcsIndex;
                dci0->magic.lastHARQAttempt = false;
                dci0->magic.transmittedInTTI = ttiNumber;
                dci0->magic.id = layer->getNode()->getNodeID() * 100000 + ttiNumber;
                dci0->magic.estimatedLinkAdaptationSINR = grant.estimatedSINR; 
                dci0->magic.direction = imtaphy::Uplink;
                dci0->magic.estimatedLinearAvgSINR = grant.estimatedLinearAvgSINR;
                
                tbSizeContextCollector->put(blockSize);
                
                for (imtaphy::interface::PrbPowerPrecodingMap::const_iterator iter = grant.prbPowerPrecoders.begin(); iter != grant.prbPowerPrecoders.end(); iter++)
                {
                    dci0->magic.prbTracingDict[iter->first]["TxPwrPRBmW"] = iter->second.power.get_mW();
                    dci0->magic.prbTracingDict[iter->first]["NumPRBs"] = dci0->local.prbPowerPrecoders.size();
                    dci0->magic.prbTracingDict[iter->first]["TotalPwrmW"] = totalTxPower.get_mW();
                    
                    // merge any tracing info coming from eNB
                    dci0->magic.prbTracingDict[iter->first].insert(grant.prbTracingDict[iter->first].begin(), grant.prbTracingDict[iter->first].end());
                }
                
                std::vector<wns::ldk::CompoundPtr> transportBlocks;
                transportBlocks.push_back(compound);

                harq->storeScheduledTransportBlock(myServingBaseStation, compound, harqProcessId, 0);  // store spatial 0 TB for HARQ

                txService->registerTransmission(myServingBaseStation,
                                                transportBlocks,
                                                grant.rank, // number of layers
                                                dci0->local.prbPowerPrecoders
                                                );
                MESSAGE_SINGLE(NORMAL, logger, "Started initial transmission of TB with id " << dci0->magic.id);

            }
            else
            {
                MESSAGE_SINGLE(NORMAL, logger, "Scheduling grant, but no traffic available... Go, fix buffer status reports");
            }
            
        }
    }
    
    // regardless of whether we used it, delete this grant to make room for future grants 
    receivedGrants.erase(ttiNumber);

    
    // unblock the queues
    getReceptor()->wakeup();
}

void 
UEScheduler::deliverSchedulingGrant(unsigned int scheduleForTTI, ltea::mac::scheduler::uplink::SchedulingGrant& grant)
{
    receivedGrants[scheduleForTTI] = grant;
}




