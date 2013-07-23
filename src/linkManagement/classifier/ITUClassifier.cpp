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


// based on these references:
// [1]: Rep. ITU-R M.2135-1, "Guidelines for evaluation of radio interface technologies for IMT-Advanced", 2009
// [2]: Finland, "Guidelines for Using IMT-Advanced Channel Models", 2009


#include <IMTAPHY/linkManagement/classifier/ITUClassifier.hpp>
#include <IMTAPHY/ChannelModuleCreator.hpp>
#include <IMTAPHY/StationPhy.hpp>
#include <IMTAPHY/detail/HashRNG.hpp>
#include <WNS/distribution/Uniform.hpp>
#include <WNS/Types.hpp>
#include <algorithm>
#include <string>

using namespace imtaphy::linkclassify;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    imtaphy::linkclassify::ITUClassifier,                                     
    imtaphy::linkclassify::LinkClassifierInterface,
    "imtaphy.linkclassify.ITU",
    imtaphy::ChannelModuleCreator);

ITUClassifier::ITUClassifier(imtaphy::Channel* channel, wns::pyconfig::View config) :
    LinkClassifierInterface(channel, config),
    staticScenario(imtaphy::Link::UMa)
{
    std::string scenarioName = config.get<std::string>("scenarioType");
    distance2D = config.get<bool>("use2Ddistance");
    outdoorOnlyUMi = config.get<bool>("onlyOutdoorDistanceUMi");
    
      std::transform(scenarioName.begin(), scenarioName.end(), scenarioName.begin(), 
                   (int (*)(int)) std::toupper);
    
    if (scenarioName == "INH")
        staticScenario = imtaphy::Link::InH;
    else if (scenarioName == "UMI")
        staticScenario = imtaphy::Link::UMi;
    else if (scenarioName == "SMA")
        staticScenario = imtaphy::Link::SMa;
    else if (scenarioName == "UMA")
        staticScenario = imtaphy::Link::UMa;
    else if (scenarioName == "RMA")
        staticScenario = imtaphy::Link::RMa;
    else 
        assure(0, "No valid scenario type chosen (choose between InH, UMi, SMa, UMa, RMa)");
}



void 
ITUClassifier::onWorldCreated()
{
    // do nothing
}

imtaphy::linktype::LinkTypes 
ITUClassifier::classifyLink(const imtaphy::StationPhy& baseStation,
                            const imtaphy::StationPhy& mobileStation,
                            const wns::Position& wrappedMSposition)
{
    // the propagation type and user location are determined here 
    // has to be static in order to avoid initializing it each time
    static wns::distribution::Uniform dis(0.0, 1.0, wns::simulator::getRNG());
    static unsigned int initialSeed = dis() * UINT_MAX;
    imtaphy::linktype::LinkTypes lt;

    detail::HashRNG hrngUserLocation(initialSeed+1999,
                                 baseStation.getPosition(),
                                 mobileStation.getPosition(), // Attention: this is the real position because the mobile's location
                                 // (e.g. outdoor, indoor, inVehicle) is always the same regardless
                                 // of wrap-around
                                 false, true); // correlate ms positions, i.e. identical shadowing from same mobile

    // Determine user locations based on specs. in [1] table 8.2
    switch (staticScenario)
    {
    case imtaphy::Link::InH:
        lt.userLocation = imtaphy::Link::Indoor; // obviously, all indoors
        break;
    case imtaphy::Link::UMi: // 50% users outdoor, 50% Indoor
        if (hrngUserLocation() < 0.5)
            lt.userLocation = imtaphy::Link::Outdoor;
        else
        {
            lt.propagation = imtaphy::Link::UMiO2I;
            // this is the O2I situation. Note that the propagation in this case is only
            // relevant for the spatial channel model and outdoor propagation will be
            // determined below for pathloss based on LoS/NLoS to the wall
            lt.userLocation = imtaphy::Link::Indoor;
        }
        break;
    case imtaphy::Link::SMa: // 50% users Indoor, 50% in Vehicle
        if (hrngUserLocation() < 0.5)
            lt.userLocation = imtaphy::Link::InVehicle;
        else
            lt.userLocation = imtaphy::Link::Indoor;
        break;
    case imtaphy::Link::UMa: // all users in vehicle
    case imtaphy::Link::RMa: // all users in vehicle
        lt.userLocation = imtaphy::Link::InVehicle;
        break;
    default:
        assure (0, "Scenario type not specified correctly for linkClassifier");
    }
    
    // the scenario is fixed and given by the config
    lt.scenario = staticScenario;

    // It is important for correct large scale results (SINR) that 
    // the propagation from a certain mobile to all base stations located
    // at the same site is identical (but random). To achieve this, we use
    // the Hash Random Number Generator from openwns-rise that 
    detail::HashRNG hrng(initialSeed,
                         baseStation.getPosition(),
                         mobileStation.getPosition(), // here we use the real position (but wrapped Pos would also work)
                         true, true); // correlate both bs and ms positions, i.e., 
    // identical shadowing from same mobile to same *site*
    
    // here we have to use the wrapped position because it is the one that gives the relevant distance
    wns::Distance distance = getDistance(baseStation.getPosition(), wrappedMSposition);
    
    double P_LoS = 0.0; //LoS probability

    // See [1] Table A1-3 for LoS probability definition
    switch (staticScenario)
    {
    case imtaphy::Link::InH:
        if (distance <= 18.0)
            P_LoS = 1.0;
        else if (distance < 37.0)
            P_LoS = exp(-((distance-18.0)/27.0));
        else
            P_LoS = 0.5;
        break;
    case imtaphy::Link::UMi:
         // [2] states that for UMi the LoS probability shall be computed based on the outdoor portion of the link 
         // for users that are indoors (see 2.2.4 in [2]), i.e., dout = (distance - indoorDistance)
         // but calibrating against 3GPP results (from R1-092019) suggests that the full distance is used which leads 
         // to a lower percentage of LoS users and hence a wideband loss / path gain CDF that is shifted to the left
        if (outdoorOnlyUMi && (lt.userLocation == imtaphy::Link::Indoor))
         {   // for UMi indoor users, subtract the indoor part from the distance
	   distance -= getUMiIndoorPartOfDistance(baseStation.getPosition(), mobileStation.getPosition(), wrappedMSposition);
         }
            
        P_LoS = std::min(18.0/distance, 1.0)*(1.0 - exp(-distance/36.0)) + exp(-distance/36.0);
        break;
    case imtaphy::Link::SMa:
        if (distance <= 10.0)
            P_LoS = 1.0;
        else
            P_LoS = exp(-((distance-10.0)/200.0));
        break;
    case imtaphy::Link::UMa:
        P_LoS = std::min(18.0/distance, 1.0)*(1.0 - exp(-distance/63.0)) + exp(-distance/63.0);
        break;
    case imtaphy::Link::RMa:
        if (distance <= 10.0)
            P_LoS = 1.0;
        else
            P_LoS = exp( -((distance-10.0)/1000.0) );
        break;
    default:
        assure (0, "Scenario type not specified correctly for linkClassifier");
    }
    
    if (hrng() < P_LoS)
    {
        lt.outdoorPropagation = imtaphy::Link::LoS;
        // don't overwrite UMiO2I propagation
        if (!((lt.scenario == imtaphy::Link::UMi) && (lt.userLocation == imtaphy::Link::Indoor)))
        {
            lt.propagation = imtaphy::Link::LoS;
        }
    }
    else
    {
        // don't overwrite UMiO2I propagation
        if (!((lt.scenario == imtaphy::Link::UMi) && (lt.userLocation == imtaphy::Link::Indoor)))
        {
            lt.propagation = imtaphy::Link::NLoS;
        }
        lt.outdoorPropagation = imtaphy::Link::NLoS;
    }
    
    return lt;
}

double
ITUClassifier::getDistance(wns::Position bsPosition, wns::Position wrappedMSposition)
{
    if (distance2D)
    {
        return sqrt(pow(bsPosition.getX() - wrappedMSposition.getX(), 2.0) +
                    pow(bsPosition.getY() - wrappedMSposition.getY(), 2.0));
    }
    else
    {
        return (bsPosition - wrappedMSposition).abs();
    }    
}


double
ITUClassifier::getUMiIndoorPartOfDistance(wns::Position bsPosition, wns::Position originalMSposition, wns::Position wrappedMSposition)
{
    double distance = getDistance(bsPosition, wrappedMSposition);

    // has to be static in order to avoid initializing it each time
    static wns::distribution::Uniform dis(0.0, 1.0, wns::simulator::getRNG());
    static unsigned int initialSeed = dis() * UINT_MAX;

    // Only take into account the UT for indoor penetration loss
    // Use the HashRNG as an engine for the boost normal_distribution RNG
    detail::HashRNG hash(initialSeed + 2637,
                        bsPosition,
                        originalMSposition, // here, take wrapped position because it is distance-dependent
                        true, true); // correlate on both positions: other sites might lead to different indoor distances
    // See [2] Section 2.2.2
    double distanceIndoor = 0.0;
    if (distance < 25.0)
        distanceIndoor = distance * hash(); //random distance between 0 and "distance"
    else
        distanceIndoor = 25.0*hash(); // random distance between 0 and 25

    return distanceIndoor;
}

