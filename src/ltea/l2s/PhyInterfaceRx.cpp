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

#include <IMTAPHY/ltea/l2s/PhyInterfaceRx.hpp>
#include <boost/bind.hpp>
#include <WNS/probe/bus/ContextProvider.hpp>
#include <WNS/probe/bus/ContextProviderCollection.hpp>
#include <WNS/simulator/Simulator.hpp>
#include <cmath>

#include <IMTAPHY/Channel.hpp>
#include <IMTAPHY/StationPhy.hpp>
#include <IMTAPHY/linkManagement/LinkManager.hpp>

#include <boost/tuple/tuple.hpp>

#include <WNS/probe/bus/json/probebus.hpp>
#include <WNS/events/scheduler/Interface.hpp>
#include <WNS/scheduler/queue/ISegmentationCommand.hpp>

using namespace ltea::l2s;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
     PhyInterfaceRx,
     wns::ldk::FunctionalUnit,
     "ltea.l2s.PhyInterfaceRx",
     wns::ldk::FUNConfigCreator);


PhyInterfaceRx::PhyInterfaceRx(wns::ldk::fun::FUN* fuNet, const wns::pyconfig::View& config) :
    wns::ldk::CommandTypeSpecifier<>(fuNet),
    wns::ldk::HasReceptor<>(),
    wns::ldk::HasConnector<>(),
    wns::ldk::HasDeliverer<>(),
    wns::Cloneable<PhyInterfaceRx>(),
    logger(config.get<wns::pyconfig::View>("logger")),
    harq(new ltea::mac::harq::HARQ(config.get("harq"))),
    dumpChannel(config.get<bool>("dumpChannel")),
    numRxAntennas(0),
    numServingTxAntennas(0),
    numPRBs(0),
    layer(NULL),
    rlcReader(NULL),
    eNBULScheduler(NULL),
    uplinkChannelStatusManager(
        imtaphy::receivers::feedback::UplinkChannelStatusManagerInterface::getCSM()),
    processingDelay(config.get<double>("processingDelay")),
    mmsefde()
    
{
    MESSAGE_SINGLE(NORMAL, logger, "Constructor called");

    // how can we read from all DCI providing FUs?
    dciReader = fuNet->getCommandReader("Scheduler");
    assure(dciReader, "Could not get DCI reader");
    
    harq->setDCIReader(dciReader);

    /* Command name must be provided in PyConfig if different RLC modes are used*/
    if(fuNet->knowsFunctionalUnit("rlcUnacknowledgedMode"))
        rlcReader = fuNet->getCommandReader("rlcUnacknowledgedMode");

    ltea::Layer2* layer = dynamic_cast<ltea::Layer2*>(getFUN()->getLayer());
    assure(layer, "Could not get ltea::Layer2");
}

void
PhyInterfaceRx::onShutdown()
{
    if (station->getStationType() == imtaphy::MOBILESTATION)
    {
        imtaphy::Link* servingLink = imtaphy::TheIMTAChannel::Instance().getLinkManager()->getServingLinkForMobileStation(station);
        servingBSContextCollector->put(servingLink->getBS()->getNode()->getNodeID());
        
        // no further context here
        shadowingContextCollector->put(servingLink->getShadowing().get_dB());
    }
    else // base station
    {
        imtaphy::LinkMap allLinks = imtaphy::TheIMTAChannel::Instance().getLinkManager()->getAllLinksForStation(station);
        imtaphy::LinkMap servedLinks = imtaphy::TheIMTAChannel::Instance().getLinkManager()->getServedLinksForBaseStation(station);
        
        for (imtaphy::LinkMap::const_iterator iter = allLinks.begin(); iter != allLinks.end(); iter++)
        {
            unsigned int serving = 0;
            
            if (servedLinks.find(iter->first) != servedLinks.end())
                serving = 1;
            
            propgationContextCollector->put(iter->second->getOutdoorPropagation(),
                                            boost::make_tuple("Serving", serving));
                                            
            shadowingContextCollector->put(iter->second->getShadowing().get_dB(),
                                           boost::make_tuple("Serving", serving,
                                                             "propagation", iter->second->getPropagation(),
                                                             "outdoorPropagation", iter->second->getOutdoorPropagation()));
        }
        for (NodeMomentsMap::iterator iter = perUserLinSINR.begin(); iter != perUserLinSINR.end(); iter++)
        {
            if (iter->second.trials() > 0)
            {
                int msID = iter->first->getNodeID();
                assure(iter->second.mean() != 0, "Mean SINR should never be 0 in linear scale");
                
                postFDEAvgSINRContextCollector->put(wns::Ratio::from_factor(iter->second.mean()).get_dB(), 
                                                    boost::make_tuple("MSID", msID));
            }
        }
    }
}

void
PhyInterfaceRx::doSendData(const wns::ldk::CompoundPtr& compound)
{
    assure(0, "We don't offer this service");
}


void
PhyInterfaceRx::doOnData(const wns::ldk::CompoundPtr& compound)
{
    assure(0, "we should only get incoming stuff via the onData method");
}

bool
PhyInterfaceRx::doIsAccepting(const wns::ldk::CompoundPtr& compound) const
{
    assure(0, "We don't offer this service");
    return false;
}

void
PhyInterfaceRx::doWakeup()
{
    assure(0, "not with us");
    getReceptor()->wakeup();
}

void 
PhyInterfaceRx::channelInitialized()
{
    imtaphy::LinkMap allLinks;
   
    wns::probe::bus::ContextProviderCollection localcpc(layer->getNode()->getContextProviderCollection());

    if (station->getStationType() == imtaphy::MOBILESTATION)
    {
        receivingInDirection = imtaphy::Downlink;
        
        imtaphy::Link* servingLink = imtaphy::TheIMTAChannel::Instance().getLinkManager()->getServingLinkForMobileStation(station);
        allLinks[servingLink->getBS()] = servingLink;
        numRxAntennas = station->getAntenna()->getNumberOfElements();
        numPRBs = imtaphy::TheIMTAChannel::Instance().getSpectrum()->getNumberOfPRBs(imtaphy::Downlink);

        numServingTxAntennas = servingLink->getBS()->getAntenna()->getNumberOfElements();

        
        servingBSContextCollector = wns::probe::bus::ContextCollectorPtr(
                                    new wns::probe::bus::ContextCollector(localcpc, "servingBS"));
        servingBSContextCollector->put(servingLink->getBS()->getNode()->getNodeID());
        
        if (dumpChannel)
            {
                this->startObserving(&(imtaphy::TheIMTAChannel::Instance()));
            }
    }
    else // we are in a Base Station
    {
        receivingInDirection = imtaphy::Uplink;
        allLinks = imtaphy::TheIMTAChannel::Instance().getLinkManager()->getServedLinksForBaseStation(station);
    }

    propgationContextCollector = wns::probe::bus::ContextCollectorPtr(
                                new wns::probe::bus::ContextCollector(localcpc, "outdoorPropagation"));

    shadowingContextCollector = wns::probe::bus::ContextCollectorPtr(
                                new wns::probe::bus::ContextCollector(localcpc, "shadowing"));


        
    channelGainContextCollector = wns::probe::bus::ContextCollectorPtr(
        new wns::probe::bus::ContextCollector(localcpc, "channelGain"));

    blockErrorContextCollector = wns::probe::bus::ContextCollectorPtr(
        new wns::probe::bus::ContextCollector(localcpc, "blockError"));

    instantaneousSINRContextCollector = wns::probe::bus::ContextCollectorPtr(
        new wns::probe::bus::ContextCollector(localcpc, "instantaneousSINR"));

    instantaneousIoTContextCollector = wns::probe::bus::ContextCollectorPtr(
        new wns::probe::bus::ContextCollector(localcpc, "instantaneousIoT"));

    postFDEAvgSINRContextCollector = wns::probe::bus::ContextCollectorPtr(
        new wns::probe::bus::ContextCollector(localcpc, "avgUplinkPostFDESINR"));
    
    jsonTracing = wns::probe::bus::ContextCollectorPtr(
        new wns::probe::bus::ContextCollector(localcpc, "phyRxTracing"));

    laProbingActualSINR = wns::probe::bus::ContextCollectorPtr(
        new wns::probe::bus::ContextCollector(localcpc, "laProbingActualSINR"));

    laProbingEstimatedSINR = wns::probe::bus::ContextCollectorPtr(
        new wns::probe::bus::ContextCollector(localcpc, "laProbingEstimatedSINR"));

    
    linearAvgEstimatedSINRContextCollector = wns::probe::bus::ContextCollectorPtr(
        new wns::probe::bus::ContextCollector(localcpc, "SINRestimationMismatch"));

    
    node2LinkMap.clear();
    for (imtaphy::LinkMap::const_iterator iter = allLinks.begin(); iter != allLinks.end(); iter++)
    {
        node2LinkMap[iter->first->getNode()] = iter->second;
    }
    
    

}

void
PhyInterfaceRx::beforeTTIover(unsigned int ttiNumber)
{
    // This will only be called in a mobile station
    
    imtaphy::LinkMap allLinks = imtaphy::TheIMTAChannel::Instance().getLinkManager()->getAllLinksForStation(station);
    
    for (imtaphy::LinkMap::const_iterator iter = allLinks.begin(); iter != allLinks.end(); iter++)
    {
        imtaphy::Link* currentLink = iter->second;

        unsigned int numTxAntennas = currentLink->getBS()->getAntenna()->getNumberOfElements();
        double wbl =  1.0 / currentLink->getRSRP().get_mW();
        
        for (unsigned int prb = 0; prb < numPRBs; prb++)
        {
            imtaphy::detail::ComplexFloatMatrixPtr channelMatrix = currentLink->getChannelMatrix(receivingInDirection, prb);
            for (unsigned int antennaPair = 0; antennaPair < numRxAntennas * numTxAntennas; antennaPair++)
            {
                float gain = norm(channelMatrix->getLocation()[antennaPair]) * wbl;
                channelGainContextCollector->put(10.0*log10(gain), boost::make_tuple("TTI", ttiNumber,
                                                                                    "PRB", prb,
                                                                                    "scmLinkId", currentLink->getSCMlinkId(),
                                                                                    "antennaPair", antennaPair));
            }
        }
    }
}

void
PhyInterfaceRx::processingDelayOver(std::vector<wns::ldk::CompoundPtr> transportBlocks,
                                    wns::node::Interface* source,
                                    imtaphy::interface::TransmissionStatusPtr status)
{
    assure(transportBlocks.size() <= 2, "Not more than 2 parallel transport blocks allowed");

    for (unsigned int i = 0; i < transportBlocks.size(); i++)
    {
        wns::ldk::CompoundPtr compound = transportBlocks[i];

        ltea::mac::DownlinkControlInformation* dci =
            dciReader->readCommand<ltea::mac::DownlinkControlInformation>(compound->getCommandPool());
       
        bool success;
        if (harq->decodeReceivedTransportBlock(source, compound, status, dci->magic.spatialID))
        {
            success = true;
            
            // the HARQ process successfully decoded the transport block
            blockErrorContextCollector->put(0.0,
                                            boost::make_tuple("attempt", dci->magic.transmissionAttempts,
                                                              "mcsIndex", dci->magic.mcsIndex,
                                                              "stream", i)
                                           );
            MESSAGE_SINGLE(NORMAL, logger, "Successfully received TB from " << source->getName() << " with id=" << dci->magic.id << " which was transmitted in tti=" << dci->magic.transmittedInTTI );

            // deliver to higher sublayers
            getDeliverer()->getAcceptor(compound)->onData(compound);
        }
        else
        {
            success = false;
            // the HARQ process could not decode the transport block
            blockErrorContextCollector->put(1.0,
                                            boost::make_tuple("attempt", dci->magic.transmissionAttempts,
                                                              "mcsIndex", dci->magic.mcsIndex,
                                                              "stream", i)
                                            );
                                            
            if (receivingInDirection == imtaphy::Uplink)
            {
                if (!dci->magic.lastHARQAttempt)
                {
                    // notify the UL scheduler about the failed transmission so that it knows a 
                    // HARQ retransmission will follow in (transmittedInTTI + 8)
                    assure(eNBULScheduler, "eNB UL scheduler not set");
                    
                    imtaphy::interface::PRBVector prbs(dci->local.prbPowerPrecoders.size());
                    unsigned int i = 0;
                    for (imtaphy::interface::PrbPowerPrecodingMap::const_iterator iter = dci->local.prbPowerPrecoders.begin();
                         iter != dci->local.prbPowerPrecoders.end(); iter++, i++)
                     {
                         prbs[i] = iter->first;
                     }
                    MESSAGE_SINGLE(NORMAL, logger, "Triggering register Retransmission for TB from " << source->getName() << " with id=" << dci->magic.id << " which was transmitted in tti=" << dci->magic.transmittedInTTI );
                    eNBULScheduler->registerHARQRetransmission(source, prbs, dci->magic.transmittedInTTI);
                }
            }                            
            // so, nothing to deliver to higher sublayers
        }
        
        ///////////////
        double linearSINRavg = 0.0;
        int linearSINRcounter = 0;
        for (unsigned int l = 0; l < dci->peer.assignedToLayers.size(); l++)
        {
            imtaphy::interface::SINRVector sinrs = status->getSINRsForLayer(dci->peer.assignedToLayers[l]);
            for (unsigned int p = 0; p < sinrs.size(); p++)
            {
                linearSINRavg += sinrs[p].get_factor();
                linearSINRcounter++;
            }
        }
        if (dci->magic.estimatedLinearAvgSINR.get_factor() > 0)
        {
            linearAvgEstimatedSINRContextCollector->put(wns::Ratio::from_factor((linearSINRavg / static_cast<double>(linearSINRcounter)) / dci->magic.estimatedLinearAvgSINR.get_factor()).get_dB());
        }
        //std::cout << "Linear average of SINR estimate: " << dci->magic.estimatedLinearAvgSINR 
        //            << " actual average = " << wns::Ratio::from_factor(linearSINRavg / static_cast<double>(linearSINRcounter)) << "\n"; 
        ///////////////
                    
                    
        // probe to json tracing but only if it will be recorded
        if (jsonTracing->hasObservers())
        {
            wns::simulator::Time stop = wns::simulator::getEventScheduler()->getTime() - processingDelay;
            wns::simulator::Time start = stop - 0.001;
            wns::probe::bus::json::Object objdoc;
            objdoc["Transmission"]["Receiver"] = wns::probe::bus::json::String(station->getNode()->getName());
            objdoc["Transmission"]["Sender"] = wns::probe::bus::json::String(source->getName());
            objdoc["Transmission"]["Start"] =  wns::probe::bus::json::Number(start);
            objdoc["Transmission"]["Stop"] =  wns::probe::bus::json::Number(stop);
            objdoc["Transmission"]["TBid"] =  wns::probe::bus::json::Number(dci->magic.id);
            objdoc["Transmission"]["SpatialTB"] =  wns::probe::bus::json::Number(dci->magic.spatialID);
            objdoc["Transmission"]["Decodable"] =  wns::probe::bus::json::Boolean(success);
            objdoc["Transmission"]["Eff.SINR"] = wns::probe::bus::json::Number(dci->magic.effSINR.get_dB());
            objdoc["Transmission"]["Est.LA_SINR"] = wns::probe::bus::json::Number(dci->magic.estimatedLinkAdaptationSINR.get_dB());
            objdoc["Transmission"]["MCS"] = wns::probe::bus::json::Number(dci->magic.mcsIndex);
            objdoc["Transmission"]["CR"] = wns::probe::bus::json::Number(dci->peer.codeRate);
            objdoc["Transmission"]["BLER"] = wns::probe::bus::json::Number(dci->magic.bler);
            objdoc["Transmission"]["HARQ.PID"] = wns::probe::bus::json::Number(dci->peer.processID);
            objdoc["Transmission"]["HARQ.NDI"] = wns::probe::bus::json::Boolean(dci->peer.NDI);
            objdoc["Transmission"]["HARQ.attempt"] = wns::probe::bus::json::Number(dci->magic.transmissionAttempts);
            

            if(rlcReader != NULL)
            {
                wns::scheduler::queue::ISegmentationCommand* segCmd =
                    rlcReader->readCommand<wns::scheduler::queue::ISegmentationCommand>(compound->getCommandPool());
                objdoc["Transmission"]["RLC.segments"] = wns::probe::bus::json::Number(segCmd->getNumSDUs());
                objdoc["Transmission"]["RLC.headerSize"] = wns::probe::bus::json::Number(segCmd->headerSize());
                objdoc["Transmission"]["RLC.payloadSize"] = wns::probe::bus::json::Number(segCmd->dataSize());
                objdoc["Transmission"]["RLC.pduSize"] = wns::probe::bus::json::Number(segCmd->totalSize());
            }    
                    
            std::vector<unsigned int> layers = dci->peer.assignedToLayers;
            
            objdoc["Transmission"]["LayersPerTB"] = wns::probe::bus::json::Number(layers.size());

            for (unsigned int l = 0; l < layers.size(); l++)
            {
                objdoc["Transmission"]["Layer"] = wns::probe::bus::json::Number(layers[l]);
                
                imtaphy::interface::PRBVector prbs = status->getPRBs();
                imtaphy::interface::SINRVector sinrs = status->getSINRsForLayer(layers[l]);
                assure(prbs.size() == sinrs.size(), "status must have exactly one SINR per PRB");
                
                for (unsigned int p = 0; p < prbs.size(); p++)
                {
                    // add all dict elements to json tracing
                    for (ltea::mac::SchedulingTracingDict::const_iterator iter = dci->magic.prbTracingDict[prbs[p]].begin(); 
                         iter != dci->magic.prbTracingDict[prbs[p]].end(); iter++)
                    {
                        objdoc["Transmission"][iter->first] = wns::probe::bus::json::Number(iter->second);
                    }
                    
                    objdoc["Transmission"]["PRB"] = wns::probe::bus::json::Number(prbs[p]);
                    objdoc["Transmission"]["SINR"] = wns::probe::bus::json::Number(sinrs[p].get_dB());
                    wns::probe::bus::json::probeJSON(jsonTracing, objdoc);
                    
                    // linkadaptation probing
                    if (laProbingActualSINR->hasObservers() || laProbingEstimatedSINR->hasObservers())
                    {
                        unsigned int msId;
                        if (receivingInDirection == imtaphy::Uplink)
                            msId = source->getNodeID();
                        else
                            msId = station->getNode()->getNodeID();
                        laProbingActualSINR->put(sinrs[p].get_dB(), boost::make_tuple("PRB", prbs[p],
                                                                                      "MSID", msId));
                        laProbingEstimatedSINR->put(dci->magic.estimatedLinkAdaptationSINR.get_dB(), boost::make_tuple("PRB", prbs[p],
                                                                                                                       "MSID", msId));
                    }

                }
            } // over layers 
        } // end of json probing
    }
}
        
  
void 
PhyInterfaceRx::onData( std::vector<wns::ldk::CompoundPtr> transportBlocks, 
                        wns::node::Interface* source, 
                        imtaphy::interface::TransmissionStatusPtr status)
{

    
    
    MESSAGE_BEGIN(NORMAL, logger, m, "onData: ");
        unsigned int layers = status->getNumberOfLayers();
        imtaphy::interface::PRBVector prbs = status->getPRBs();
        m << " from sender " << source->getName() << " with " << layers << " layers and " << prbs.size() << " PRBs:";
        
        m << "     with the follwing SINRs: \n";
        m << "PRB#\t\t";
        for (unsigned int i = 1; i <= layers; i++)
            m << "Layer " << i << "\t\t";
        m << "\n";
        for (unsigned int i = 0; i < prbs.size(); i++)
        {
            m << status->getPRBid(i) << "\t\t";
            for (unsigned int j = 1; j <= layers; j++)
                m << status->getSINR(prbs[i], j) << "\t";
            m << "\n";
        }
       
    MESSAGE_END();
    
    if (instantaneousSINRContextCollector->hasObservers() ||
        ((receivingInDirection == imtaphy::Uplink) && postFDEAvgSINRContextCollector->hasObservers()))
    {
        for (unsigned int j = 1; j <= status->getNumberOfLayers(); j++)
        {
            if (receivingInDirection == imtaphy::Uplink)
            {
                wns::Ratio postEqualization = mmsefde.getEffectiveSINR(status->getSINRsForLayer(j), imtaphy::l2s::Generic()); // modulation not used
                
                instantaneousSINRContextCollector->put(postEqualization.get_dB());
                perUserLinSINR[source].put(postEqualization.get_factor());
            }
            else
            {
                imtaphy::interface::SINRVector sinrs = status->getSINRsForLayer(j);
                for (unsigned int i = 0; i < sinrs.size(); i++)
                {
                    instantaneousSINRContextCollector->put(sinrs[i].get_dB(),
                                                           boost::make_tuple("Layer", j)
                    );
                }
            }
        }
    }
    if (instantaneousIoTContextCollector->hasObservers())
    {
        for (unsigned int j = 1; j <= status->getNumberOfLayers(); j++)
        {
            imtaphy::interface::IoTVector iots = status->getIoTsForLayer(j);
            for (unsigned int i = 0; i < iots.size(); i++)
            {
                instantaneousIoTContextCollector->put(iots[i].get_dB(),
                                                      boost::make_tuple("Layer", j)
                );
            }
        }
    }

    // If we are receiving in the uplink, we can use the reference symbols of the transmission to estimate
    // the channel to have channel estimates when scheduling upcoming uplink transmissions
    if (receivingInDirection == imtaphy::Uplink)
    {
        imtaphy::interface::PRBVector prbs = status->getPRBs();
        uplinkChannelStatusManager->updatePRBsForUserNow(source, prbs);
    }

    // after approx 3ms processing delay
    wns::simulator::getEventScheduler()->scheduleDelay(
        boost::bind(&PhyInterfaceRx::processingDelayOver, this, transportBlocks,
                                                                source,
                                                                status),
                                                                processingDelay); // processing delay, see, e.g., Dahlman 4G, p. 256
}

void 
PhyInterfaceRx::onFUNCreated()
{
    wns::ldk::FunctionalUnit::onFUNCreated();
    
    layer = dynamic_cast<ltea::Layer2*>(getFUN()->getLayer());
    assure(layer, "Could not get layer");

    station = dynamic_cast<imtaphy::StationPhy*>(layer->getTxService());
    assure(station, "Could not get StationPhy");

    
    if (station->getStationType() == imtaphy::BASESTATION)
    {
        eNBULScheduler = getFUN()->findFriend<ltea::mac::scheduler::uplink::enb::SchedulerBase*>("ULScheduler");
        assure(eNBULScheduler, "I'm in an eNB (Uplink) but can't get the eNB's uplink scheduler");
    }
}

