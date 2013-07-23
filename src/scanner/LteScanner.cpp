/*******************************************************************************
 * This file is part of IMTAphy
 * _____________________________________________________________________________
 *
 * Copyright (C) 2010
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

#include <IMTAPHY/scanner/LteScanner.hpp>
#include <WNS/osi/PDU.hpp>
#include <WNS/probe/bus/ContextProvider.hpp>
#include <WNS/probe/bus/ContextProviderCollection.hpp>

#include <IMTAPHY/linkManagement/LinkManager.hpp>
#include <IMTAPHY/StationPhy.hpp>
#include <boost/bind.hpp>
#include <limits>

using namespace imtaphy::scanner;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    LteScanner,
    wns::node::component::Interface,
    "imtaphy.LteScanner",
    wns::node::component::ConfigCreator);

LteScanner::LteScanner(wns::node::Interface* _node, const wns::pyconfig::View& _pyco) : 
    Component(_node, _pyco),
    channel(&TheIMTAChannel::Instance()),
    numTTIs(UINT_MAX),
    txServiceName(_pyco.get<std::string>("phyDataTransmission")),
    rxServiceName(_pyco.get<std::string>("phyDataReception")),
    tti(0),
    config(_pyco),
    currentSourceId(0),
    sinrPDF(-50.0, 100.0, 15000, wns::evaluation::statistics::PDF::linear,
            wns::evaluation::statistics::PDF::scientific, "SINR PDF", "SINR PDF")

{
    getNode()->addService(rxServiceName, this);

    moments.reset();
    sinrPDF.reset();
}

void
LteScanner::onWorldCreated()
{
    txService = getService<imtaphy::interface::DataTransmission*>(txServiceName);
    station = dynamic_cast<StationPhy*>(txService);
    assure(station, "Need to access station object");
    
    this->startObserving(channel);

    // init the probe bus
    initProbes();
}
void 
LteScanner::onData(std::vector<wns::ldk::CompoundPtr> transportBlocks, 
                                wns::node::Interface* source, 
                                imtaphy::interface::TransmissionStatusPtr status) 
{
    // currently only looking at layer 0
    
    wns::ldk::CompoundPtr pdu = transportBlocks[0];
    
    if (pdu->getLengthInBits() > 0)
    {
#ifndef WNS_NDEBUG
        unsigned int layers = status->getNumberOfLayers();
        imtaphy::interface::PRBVector prbs = status->getPRBs();
        std::cout << " from sender " << source->getName() << " with " << layers << " layers and " << prbs.size() << " PRBs:";
        
        std::cout << "     with the follwing SINRs: \n";
        std::cout << "PRB#\t\t";
        for (unsigned int i = 0; i < layers; i++)
            std::cout << "Layer " << i << "\t\t";
        std::cout << "\n";
        for (unsigned int i = 0; i < prbs.size(); i++)
        {
            std::cout << status->getPRBid(i) << "\t\t";
            for (unsigned int j = 0; j < layers; j++)
                std::cout << status->getSINR(prbs[i], j) << "\t";
            std::cout << "\n";
        }
      
#endif    
        
        
        StationPhy* remoteStation = dynamic_cast<StationPhy*>(source->getService<imtaphy::interface::DataTransmission*>(txServiceName));
        assure(remoteStation, "could not get remote station");
        
        imtaphy::Link* link = channel->getLinkManager()->getLink(remoteStation, station);
        
        currentSourceId = source->getNodeID();
        wblContextCollector->put(-1.0 * link->getWidebandLoss().get_dB());

        shadowingContextCollector->put(link->getShadowing().get_dB());
        pathlossContextCollector->put(-1.0 * link->getPathloss().get_dB());

        imtaphy::interface::SINRVector sinrs = status->getSINRsForLayer(0);
        
        // SINR could be different on different prbs:
        for (unsigned int prb = 0; prb < sinrs.size(); prb++)
        {
            sinrContextCollector->put(sinrs[prb].get_dB());
            
            //            std::cout << "probing info MSID=" << getNode()->getNodeID() << " prb=" << prb << " fastFading=" << link->getCurrentFastFading(prb).get_factor() << "\n";
            
            // save current sinr values to moments container for linear
            // averaging over frequency and time
            moments.put(sinrs[prb].get_factor());
            sinrPDF.put(sinrs[prb].get_factor());
        }
    }
    else
        ; // ignore this, it was meant to create interference 
}

unsigned int
LteScanner::getSourceId()
{
    return currentSourceId;
}

void 
LteScanner::beforeTTIover(unsigned int _tti)
{
    tti = _tti;
    
    if (tti == numTTIs)
    {
        if (moments.trials() > 0)
        {
            avgSINRContextCollector->put(10.0 * log10(moments.mean()));
            
            // when both the serving BS's link and the interfering links are 
            // fading, the SINR will be the ratio of two random variables 
            // (S and sum over I) which follow, e.g., a Cauchy distribution if
            // S and I would be Gaussian random variables. Because instantaneous 
            // values could be arbitrarily large, taking an arithmetic mean might 
            // (as in the avgSINRContextCollector above) not be a good idea.
            // We therefore try to estimate a "mean" SINR by computing a truncated
            // mean, taking the arithmetic mean of the 19% midle percentiles:
            
//             double mean = 0.0;
//             for (int i = 41; i < 60; i++)
//                 mean += sinrPDF.getPercentile(i);
//             
//             medianSINRContextCollector->put(10.0 * log10(mean / 19.0));

            // double mean = 0.0;
            // for (int i = 50; i < 51; i++)
            //     mean += sinrPDF.getPercentile(i);
            
            // medianSINRContextCollector->put(10.0 * log10(mean / 1.0));

        }
    }
}

void
LteScanner::initProbes()
{
    wns::probe::bus::ContextProviderCollection localcpc(getNode() ->getContextProviderCollection());
        
    localcpc.addProvider(wns::probe::bus::contextprovider::Constant("MSID", getNode()->getNodeID()));
    localcpc.addProvider(wns::probe::bus::contextprovider::Callback("BSID", boost::bind(&LteScanner::getSourceId, this)));
    localcpc.addProvider(wns::probe::bus::contextprovider::Constant("scenario.x", station->getPosition().getX()));
    localcpc.addProvider(wns::probe::bus::contextprovider::Constant("scenario.y", station->getPosition().getY()));    
    
    wblContextCollector = wns::probe::bus::ContextCollectorPtr(
        new wns::probe::bus::ContextCollector(localcpc,
                                              config.get<std::string>("wblProbeName")));

    sinrContextCollector = wns::probe::bus::ContextCollectorPtr(
        new wns::probe::bus::ContextCollector(localcpc,
                                              config.get<std::string>("sinrProbeName")));

    shadowingContextCollector = wns::probe::bus::ContextCollectorPtr(
        new wns::probe::bus::ContextCollector(localcpc,
                                              config.get<std::string>("shadowingProbeName")));

    pathlossContextCollector = wns::probe::bus::ContextCollectorPtr(
        new wns::probe::bus::ContextCollector(localcpc,
                                              config.get<std::string>("pathlossProbeName")));

    avgSINRContextCollector = wns::probe::bus::ContextCollectorPtr(
        new wns::probe::bus::ContextCollector(localcpc,
                                              config.get<std::string>("avgSINRProbeName")));
                    
    medianSINRContextCollector = wns::probe::bus::ContextCollectorPtr(
        new wns::probe::bus::ContextCollector(localcpc,
                                              config.get<std::string>("medianSINRProbeName")));
                                 
}

void 
LteScanner::setSimulationDuration(unsigned int numTTIs_)
{
    numTTIs = numTTIs_;
}
