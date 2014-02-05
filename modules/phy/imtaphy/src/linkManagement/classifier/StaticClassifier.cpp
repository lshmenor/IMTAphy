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

#include <IMTAPHY/ChannelModuleCreator.hpp>
#include <IMTAPHY/linkManagement/classifier/StaticClassifier.hpp>
#include <IMTAPHY/detail/HashRNG.hpp>
#include <WNS/distribution/Uniform.hpp>
#include <algorithm>
#include <string>
#include <cctype>

using namespace imtaphy::linkclassify;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    imtaphy::linkclassify::StaticClassifier,                                     
    imtaphy::linkclassify::LinkClassifierInterface,
    "imtaphy.linkclassify.Static",
    imtaphy::ChannelModuleCreator);

StaticClassifier::StaticClassifier(imtaphy::Channel* channel, wns::pyconfig::View config) :
    LinkClassifierInterface(channel, config),
    distance2D(true)
{
    std::string scenarioName = config.get<std::string>("scenario");
    std::transform(scenarioName.begin(), scenarioName.end(), scenarioName.begin(), 
                   (int (*)(int))std::toupper);

    if (scenarioName == "INH")
        staticType.scenario = imtaphy::Link::InH;
    else if (scenarioName == "UMI")
        staticType.scenario = imtaphy::Link::UMi;
    else if (scenarioName == "SMA")
        staticType.scenario = imtaphy::Link::SMa;
    else if (scenarioName == "UMA")
        staticType.scenario = imtaphy::Link::UMa;
    else if (scenarioName == "RMA")
        staticType.scenario = imtaphy::Link::RMa;
    else 
        assure(0, "No valid scenario type chosen (choose between InH, UMi, SMa, UMa, RMa)");

    
    std::string propagation = config.get<std::string>("propagation");
    std::string outdoorPropagation = config.get<std::string>("outdoorPropagation");
    std::transform(propagation.begin(), propagation.end(), propagation.begin(), 
                   (int (*)(int)) std::toupper);
    std::transform(outdoorPropagation.begin(), outdoorPropagation.end(), outdoorPropagation.begin(), 
                   (int (*)(int)) std::toupper);

    if (outdoorPropagation == "LOS")
        staticType.outdoorPropagation = imtaphy::Link::LoS;
    else if (outdoorPropagation == "NLOS")
        staticType.outdoorPropagation = imtaphy::Link::LoS;
    else
    {
        staticType.outdoorPropagation = imtaphy::Link::INVALID;
        assure(0, "No valid outdoorPropagation set (choose between LOS, NLOS, etc.");
    }
    
    if (propagation == "LOS")
        staticType.propagation = imtaphy::Link::LoS;
    else if (propagation == "NLOS")
        staticType.propagation = imtaphy::Link::NLoS;
    else if (propagation == "OTOI")
    {
        assure(staticType.scenario == imtaphy::Link::UMi, "OtoI only valid for UMi scenario");
        staticType.propagation = imtaphy::Link::UMiO2I;
    }
    else 
    {
        staticType.propagation = imtaphy::Link::INVALID;
        assure(0, "No valid linkType set (choose between LOS, NLOS, etc.");
    }
    
    std::string userLocation = config.get<std::string>("userLocation");
    std::transform(userLocation.begin(), userLocation.end(), userLocation.begin(), 
                   (int (*)(int)) std::toupper);

    if (userLocation == "OUTDOOR")
        staticType.userLocation = imtaphy::Link::Outdoor;

    else if (userLocation == "INDOOR")
        staticType.userLocation = imtaphy::Link::Indoor;

    else if (userLocation == "INVEHICLE")
    {
        staticType.userLocation = imtaphy::Link::InVehicle;
    }
    else 
    {
        staticType.userLocation = imtaphy::Link::NotApplicable;
    }
}



void 
StaticClassifier::onWorldCreated()
{
    // do nothing
}

imtaphy::linktype::LinkTypes 
StaticClassifier::classifyLink(const imtaphy::StationPhy& station1,
                               const imtaphy::StationPhy& station2,
                               const wns::Position& wrappedMSposition)
{
    return staticType;
}

double
StaticClassifier::getDistance(wns::Position bsPosition, wns::Position wrappedMSposition)
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
StaticClassifier::getUMiIndoorPartOfDistance(wns::Position bsPosition, wns::Position originalMSposition, wns::Position wrappedMSposition)
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
