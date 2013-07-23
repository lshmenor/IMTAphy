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

#include <IMTAPHY/pathloss/SingleSlope.hpp>
#include <IMTAPHY/ChannelModuleCreator.hpp>
#include <IMTAPHY/Channel.hpp>
#include <WNS/Assure.hpp>
#include <math.h>

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    imtaphy::pathloss::SingleSlope,
    imtaphy::pathloss::PathlossModelInterface,
    "imtaphy.pathloss.SingleSlope",
    imtaphy::ChannelModuleCreator);

using namespace imtaphy::pathloss;

SingleSlope::SingleSlope(Channel* _channel, wns::pyconfig::View config) :
    PathlossModelInterface(_channel, config),
    frequencyFactorLOS(config.get<double>("frequencyFactorLOS")),
    frequencyFactorNLOS(config.get<double>("frequencyFactorNLOS")),
    distanceFactorLOS(config.get<double>("distanceFactorLOS")),
    distanceFactorNLOS(config.get<double>("distanceFactorNLOS")),
    offsetLOS(config.get<double>("offsetLOS")),
    offsetNLOS(config.get<double>("offsetNLOS")),
    minPathloss(config.get<double>("minPathloss"))
{
    assure(frequencyFactorLOS > 0.0, "No negative parameters for single slope pathloss model");
    assure(frequencyFactorNLOS > 0.0, "No negative parameters for single slope pathloss model");
    assure(distanceFactorLOS > 0.0, "No negative parameters for single slope pathloss model");
    assure(distanceFactorNLOS > 0.0, "No negative parameters for single slope pathloss model");
    // assure(offsetLOS > 0.0, "No negative parameters for single slope pathloss model");
    // assure(offsetNLOS > 0.0, "No negative parameters for single slope pathloss model");
    assure(minPathloss > 0.0, "No negative parameters for single slope pathloss model");
}

wns::Ratio
SingleSlope::getPathloss(imtaphy::Link* link, double frequencyMHz)
{
    assure((link->getPropagation() == imtaphy::Link::NLoS) || 
           (link->getPropagation() == imtaphy::Link::LoS), 
           "Only NLOS and LOS links are supported");    
    
    double distance = (link->getBS()->getPosition() - link->getWrappedMSposition()).abs();
    double pathloss;
    
    if (link->getPropagation() == imtaphy::Link::LoS)
        pathloss = offsetLOS + distanceFactorLOS * log10(distance) + frequencyFactorLOS * log10(frequencyMHz);
    else // NLOS case
        pathloss = offsetNLOS + distanceFactorNLOS * log10(distance) + frequencyFactorNLOS * log10(frequencyMHz);

    assure(pathloss >= 0.0, "Pathloss computation cannot yield negative dB value");
    
    if (pathloss < minPathloss)
        pathloss = minPathloss;
    
    return wns::Ratio::from_dB(pathloss);
}
