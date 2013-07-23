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

#include <IMTAPHY/Channel.hpp>

#include <limits>
#include <WNS/distribution/Uniform.hpp>
#include <WNS/simulator/ISimulator.hpp>
#include <IMTAPHY/StationPhy.hpp>
#include <IMTAPHY/ChannelModuleCreator.hpp>
#include <WNS/pyconfig/Parser.hpp>
#include <WNS/distribution/Uniform.hpp>

#include <itpp/itbase.h>
#include <itpp/base/math/misc.h>
#include <IMTAPHY/Link.hpp>
#include <IMTAPHY/linkManagement/LinkManager.hpp>
#include <IMTAPHY/receivers/ReceiverInterface.hpp>
#include <iostream>

using namespace imtaphy;

Channel::Channel() :
    pathlossModel(NULL),
    spatialChannelModel(NULL),
    lsCorrelation(NULL),
    largeScaleParams(NULL),
    linkManager(NULL),
    transmissionsPerPRB(2), // for uplink and downlink
    config(wns::simulator::getInstance()->getConfiguration().getView("modules").getView("imtaphy").getView("channelConfig")),
    logger(config.get("logger")),
    tti(0),
    initialized(false),
    transmissionIdCounter(0)
{
    // Init the it++ Random Number Generator with a Random Number from the 
    // openWNS generator. If its seed is fixed, it will also be fixed for it++
    wns::distribution::StandardUniform rnd = wns::distribution::StandardUniform();
    unsigned int itppSeed = rnd() * UINT_MAX;
    itpp::RNG_reset(itppSeed);
    
        
    spectrum = new Spectrum(config.get("spectrum"));    
    
    MESSAGE_SINGLE(NORMAL, logger, "Created the IMTAPHY Channel with " << spectrum->getNumberOfPRBs(imtaphy::Downlink) << " downlink PRBs and " 
                                                                       << spectrum->getNumberOfPRBs(imtaphy::Uplink) << " uplink PRBs"); 
      
    transmissionsPerPRB[Uplink].resize(spectrum->getNumberOfPRBs(imtaphy::Uplink));
    transmissionsPerPRB[Downlink].resize(spectrum->getNumberOfPRBs(imtaphy::Downlink));
        
    baseStations.clear();
    mobileStations.clear();
  
    this->startPeriodicTimeout(0.001, 0.0); // make the timeout for the next TTI every millisecond
}

Channel::Channel(int dummy) : // this is just for unit testing to avoid regular constructor
    config(wns::pyconfig::Parser()) // create empty config
{

}

LinkManager* Channel::getLinkManager() const
{
    assure(linkManager, "Not yet initialized");
        
    return linkManager;
}

scm::SpatialChannelModelInterface<SCMPRECISION>* 
Channel::getSpatialChannelModel() const
{
    return spatialChannelModel;
}

Spectrum* 
Channel::getSpectrum() const
{
    return spectrum;
}


void
Channel::onWorldCreated() // TODO: it would be nice if the simulator could call this directly; but we could also work-around by telling the stationPhys to call it
{
    if (initialized)
        return;
    else
        initialized = true;
    
        
    linkManager = new LinkManager(this, baseStations, mobileStations,
                                  config.get("linkManager"));
    
    wns::pyconfig::View pathlossModelConfig = config.get("pathlossModel");
    std::string plugin = pathlossModelConfig.get<std::string>("nameInChannelFactory"); // name under which the C++ implementation of the model is registered
    PathlossModelCreator* pcc = PathlossModelFactory::creator(plugin);
    pathlossModel = pcc->create(this, pathlossModelConfig);
    pathlossModel->onWorldCreated();
    
    // Generate large scale parameters according to M2135/Winner
    itpp::Real_Timer timer;
    timer.reset();
    timer.tic();
    LinkVector allLinks = linkManager->getAllLinks();
    imtaphy::lsparams::RandomMatrix* rnGen = new imtaphy::lsparams::RandomMatrix();
    lsCorrelation = new lsparams::LSCorrelation(allLinks, linkManager, rnGen);
    largeScaleParams = lsCorrelation->generateLSCorrelation();

    MESSAGE_SINGLE(VERBOSE, logger,"Took "<<timer.get_time()/60.0 << " minutes for generating LS parameters for " << allLinks.size() <<" links");

    
    // the link manager should only try to access pathloss/shadowing after LS params have been setup
    linkManager->onPathlossAndShadowingReady();
    linkManager->doBeforeSCMinit();

    wns::pyconfig::View spatialChannelModelConfig = config.get("spatialChannelModel");
    plugin = spatialChannelModelConfig.get<std::string>("nameInChannelFactory"); // name under which the C++ implementation of the model is registered
    SpatialChannelModelCreator* scmc = SpatialChannelModelFactory::creator(plugin);
    spatialChannelModel = scmc->create(this, spatialChannelModelConfig);

    MESSAGE_SINGLE(NORMAL, logger, "Initializing the spatial Channel Model for " << linkManager->getSCMLinks().size() << " links"); 
    spatialChannelModel->onWorldCreated(linkManager, largeScaleParams, false);

    linkManager->doAfterSCMinit(spatialChannelModel, getSpectrum());
    
    
    // Tell all stations that the channel is now setup and ready
    for (imtaphy::StationList::const_iterator bsIter = baseStations.begin(); bsIter != baseStations.end(); bsIter++)
        (*bsIter)->channelInitialized();
       
    for (imtaphy::StationList::const_iterator msIter=mobileStations.begin(); msIter!=mobileStations.end() ; msIter++) 
        (*msIter)->channelInitialized();
    
    // start the channel model at t=0
    spatialChannelModel->evolve(0.0);
}

void
Channel::registerStationPhy(StationPhy* station)
{
    if (station->getStationType() == BASESTATION)
        baseStations.push_back(station);
    else
        mobileStations.push_back(station);
}


void
Channel::registerTransmission(TransmissionPtr transmission) 
{
    imtaphy::Direction direction = transmission->getDirection();
    const imtaphy::interface::PrbPowerPrecodingMap& prbPowerPrecodingMap = transmission->getPrbPowerPrecodingMap();
    
    assure((direction == imtaphy::Uplink) || (direction == imtaphy::Downlink), "Direction can either be UL or DL");
    
    transmission->setId(transmissionIdCounter++);
    
    for (imtaphy::interface::PrbPowerPrecodingMap::const_iterator iter = prbPowerPrecodingMap.begin();
         iter != prbPowerPrecodingMap.end(); iter++)
    {
        imtaphy::interface::PRB prb = iter->first;
        
        assure((prb < transmissionsPerPRB[direction].size()) && (prb >= 0), "Invalid PRB.");
      
        transmissionsPerPRB[direction][prb].push_back(transmission);
      
    }
    allCurrentTransmissions.push_back(transmission);
}

const TransmissionVector& 
Channel::getTransmissionsOnPRB(Direction direction, imtaphy::interface::PRB prb) const
{
    assure((prb < transmissionsPerPRB[direction].size()) && (prb >= 0), "Invalid PRB.");
    
    return transmissionsPerPRB[direction][prb];
}



void
Channel::periodically() // TTI over
{
    // notify all subscribed receivers that the TTI is almost over
    // tti old, channel old
    wns::Subject<imtaphy::interface::IMTAphyObserver>::forEachObserver(BeforeTTIOver(tti));

    itpp::Real_Timer timer;
    timer.tic();
    
    // and notify the receivers about the transmissions
    // tti old, channel old
    evaluateAllTransmissions(); 
    timer.toc();
    MESSAGE_SINGLE(VERBOSE, logger, "Evaluating all transmission took " << timer.get_time() << " seconds");

    // now, clear current transmissions:
    allCurrentTransmissions.clear();
    
    for (unsigned int prb = 0; prb < transmissionsPerPRB[imtaphy::Downlink].size(); prb++)
    {
        transmissionsPerPRB[imtaphy::Downlink][prb].clear();
    }
    for (unsigned int prb = 0; prb < transmissionsPerPRB[imtaphy::Uplink].size(); prb++)
    {
        transmissionsPerPRB[imtaphy::Uplink][prb].clear();
    }

    // now move on to next TTI
    tti++;
    
    // and evolve the channel
    timer.reset();
    MESSAGE_SINGLE(NORMAL, logger, "Now evolving the channel for TTI from t= " << (tti-1) * 0.001 << " until " << (tti) * 0.001); 
    timer.tic();
    spatialChannelModel->evolve((tti) * 0.001); // for the next TTI
    MESSAGE_SINGLE(VERBOSE, logger,"SCM took "<<timer.get_time()<<" seconds for evolving " << linkManager->getSCMLinks().size() << " links");
    timer.toc();

    
    // trigger the new TTI beginning in all stations, use the OnNewTTI functor:
    wns::Subject<imtaphy::interface::IMTAphyObserver>::forEachObserver(OnNewTTI(tti));
    
}


void
Channel::evaluateAllTransmissions() const
{

    MESSAGE_SINGLE(NORMAL, logger, "Evaluating " << allCurrentTransmissions.size() << " current transmissions ...\n");
    
    // We have to do a little re-organization first. We want to parallelize the reception for the individual receivers but avoid
    // reentrant calls to the same receiver. This can happen in uplink when a single eNB receives from different UEs.
    
    typedef std::set<TransmissionPtr> TransmissionSet;
    typedef std::map<imtaphy::receivers::ReceiverInterface*, TransmissionSet> ReceiverTransmissionMap;
    ReceiverTransmissionMap receiversAndTransmissionsMap;
    
    for (unsigned int n = 0; n < allCurrentTransmissions.size(); n++)
    {
        receiversAndTransmissionsMap[allCurrentTransmissions[n]->getDestination()->getReceiver()].insert(allCurrentTransmissions[n]);
    }
    
    unsigned int n = 0;
    std::vector<TransmissionSet> transmissionSetVector(receiversAndTransmissionsMap.size());
    
    for (ReceiverTransmissionMap::const_iterator iter = receiversAndTransmissionsMap.begin(); iter != receiversAndTransmissionsMap.end(); n++, iter++)
    {
        transmissionSetVector[n] = iter->second;
    }
    
    int num = transmissionSetVector.size();
    int i; // signed int for OpenMP parallel for loop
    TransmissionPtr transmission;
    TransmissionSet::const_iterator iter;
    TransmissionSet transmissionsThisReceiver;
    
    // do the computation intensive part of the receiver operation in parallel
    #pragma omp parallel for private(iter, transmission, transmissionsThisReceiver), shared(num, transmissionSetVector), default(none)
    for (i = 0; i < num; i++)
    {
        transmissionsThisReceiver = transmissionSetVector[i];
        
        for (iter = transmissionsThisReceiver.begin(); iter != transmissionsThisReceiver.end(); iter++)
        {
            transmission = *iter;
            transmission->getDestination()->getReceiver()->receive(transmission);
        }
    }

    // deliver the results of the previous step sequentially (single-threaded)
    // to avoid any race conditions (e.g. in the event scheduler)
    for (unsigned int i = 0; i < allCurrentTransmissions.size(); i++)
    {
        transmission = allCurrentTransmissions[i];
        transmission->getDestination()->getReceiver()->deliverReception(transmission);
    }
}




wns::Ratio
Channel::computePathloss(imtaphy::Link* link)
{
    // pathloss models take frequencies in MHz; we always use the downlink center frequency and apply that pathloss also to the uplink
    wns::Ratio pathloss = pathlossModel->getPathloss(link, spectrum->getSystemCenterFrequencyHz(imtaphy::Downlink) / 1E06);

    MESSAGE_SINGLE(NORMAL, logger, "Pathloss model returned a pathloss of " << pathloss << " between " << link->getBS()->getName() << " and " << link->getMS()->getName());
        
    return pathloss;
}


wns::Ratio
Channel::computeShadowing(Link* link)
{
    wns::Ratio shadowing = wns::Ratio::from_factor(1.0);
    
    assure(lsCorrelation, "No valid LS correlation pointer");

    if (largeScaleParams->find(link) != largeScaleParams->end())
            shadowing = wns::Ratio::from_factor((*largeScaleParams)[link].getShadowFading());
    return shadowing;
}

StationList
Channel::getAllBaseStations() const
{
    return baseStations;        
}

StationList
Channel::getAllMobileStations() const
{
    return mobileStations;
}


double
Channel::getAzimuth(wns::Position me, wns::Position other) const
{
    // Assumptions:
    // The y-axis points straight north, the x-axis east
    // For azimuth angles 0 degrees is north and positive angles
    // are clockwise from the northern direction. Azimuth goes from
    // -Pi to Pi and is given in radians.
    
    double deltaX = other.getX() - me.getX();
    double deltaY = other.getY() - me.getY();

    double angle = (itpp::pi / 2.0) - atan2(deltaY, deltaX);
    
    // due to the Pi/2 offset, the angle could be bigger than Pi
    if (angle > itpp::pi)
        angle -= 2.0 * itpp::pi;
    
    assure((-itpp::pi <= angle) && (angle <= itpp::pi), "Computed azimuth angle out of bounds");
    
    return angle;
    
}

double
Channel::getElevation(wns::Position me, wns::Position other) const
{
    double deltaX = other.getX() - me.getX();
    double deltaY = other.getY() - me.getY();
    double deltaZ = other.getZ() - me.getZ();
    
    double distance = sqrt(deltaX * deltaX + deltaY * deltaY);
    
    return -1.0 * atan(deltaZ / distance);
}


