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
// based on implementation in openwns-rise
/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
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

// based on these references:
// [1]: Rep. ITU-R M.2135-1, "Guidelines for evaluation of radio interface technologies for IMT-Advanced", 2009
// [2]: Finland, "Guidelines for Using IMT-Advanced Channel Models", 2009

#include <IMTAPHY/pathloss/M2135Pathloss.hpp>
#include <IMTAPHY/ChannelModuleCreator.hpp>
#include <WNS/Types.hpp>
#include <IMTAPHY/detail/HashRNG.hpp>
#include <WNS/distribution/Uniform.hpp>
#include <IMTAPHY/Channel.hpp>
#include <IMTAPHY/linkManagement/LinkManager.hpp>

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    imtaphy::pathloss::M2135Pathloss,
    imtaphy::pathloss::PathlossModelInterface,
    "imtaphy.pathloss.M2135",
    imtaphy::ChannelModuleCreator);

using namespace imtaphy::pathloss;

M2135Pathloss::M2135Pathloss(Channel* _channel, wns::pyconfig::View config) :
    PathlossModelInterface(_channel, config),
    channel(_channel),
    SMaBuildingHeight(config.get<double>("SMaBuildingHeight")),
    SMaStreetWidth(config.get<double>("SMaStreetWidth")),
    RMaBuildingHeight(config.get<double>("RMaBuildingHeight")),
    RMaStreetWidth(config.get<double>("RMaStreetWidth")),
    UMaBuildingHeight(config.get<double>("UMaBuildingHeight")),
    UMaStreetWidth(config.get<double>("UMaStreetWidth")),
    feederLoss(config.get<double>("feederLoss"))
{
}

void
M2135Pathloss::onWorldCreated()
{
    ituClassifier = dynamic_cast<imtaphy::linkclassify::ITUClassifier*>(channel->getLinkManager()->getClassifier());
    staticClassifer = dynamic_cast<imtaphy::linkclassify::StaticClassifier*>(channel->getLinkManager()->getClassifier());
    assure(ituClassifier || staticClassifer, "M2135 pathloss needs ITU or Static classifier instance");
}

wns::Ratio 
M2135Pathloss::getPathloss(imtaphy::Link* link, double frequencyMHz)
{
    // 2 dB feeder loss is added for wideband calibration, 
	// cf. Link Budget Templates in M.2133 and 3GPP TR 36.814 Table A-2.2-1
	// According to the M.2133 link budget table no feeder loss is assumed 
	// for InH but Winner IMT-A Calibration simulations seem to apply 2dB also 
	// in that case.
    wns::Ratio feederLoss_ratio = wns::Ratio::from_dB(feederLoss);
    
    switch(link->getScenario())
    {
    case imtaphy::Link::UMa:
        return feederLoss_ratio + getUMaPathloss(link, frequencyMHz);
        break;
    case imtaphy::Link::UMi:
        return feederLoss_ratio + getUMiPathloss(link, frequencyMHz);
        break;
    case imtaphy::Link::RMa:
        return feederLoss_ratio + getRMaPathloss(link, frequencyMHz);
        break;
    case imtaphy::Link::SMa:
        return feederLoss_ratio + getSMaPathloss(link, frequencyMHz);
        break;
    case imtaphy::Link::InH:
        return feederLoss_ratio + getInHPathloss(link, frequencyMHz);
        break;
    default:
        assure(0, "Invalid pathloss");
        return wns::Ratio::from_dB(0.0);
    }
    assure(0, "Invalid pathloss");
    return wns::Ratio::from_dB(0.0);
}




wns::Ratio 
M2135Pathloss::getSMaPathloss(imtaphy::Link* link, double frequencyMHz)
{
    assure((link->getOutdoorPropagation() == imtaphy::Link::NLoS) ||
           (link->getOutdoorPropagation() == imtaphy::Link::LoS),
           "Only NLOS and LOS links are supported");
           
    double bsHeight = link->getBS()->getPosition().getZ();
    double utHeight = link->getWrappedMSposition().getZ();

    wns::Distance distance = (ituClassifier)?
                            (ituClassifier->getDistance(link->getBS()->getPosition(), link->getWrappedMSposition())):
                            (staticClassifer->getDistance(link->getBS()->getPosition(), link->getWrappedMSposition()));

    // For IMT-A calibrations:
    assure(bsHeight == 35, "BS height assumed to be 35m in SMa for IMT-A calibration");
    assure(utHeight == 1.5, "UT height assumed to be 1.5m in SMa for IMT-A calibration");
    assure(SMaBuildingHeight == 10, "SMa building height should be 10m for IMT-A calibration ");
    assure(SMaStreetWidth == 20, "SMa street width should be 20m for IMT-A calibration ");
    assure(distance >= 35, "Distance too small for IMT-A, see Table 8-2");
    
    assure(bsHeight >= 10.0, "BS Height must be at least 10 m");
    assure(bsHeight < 150.0, "BS Height cannot be larger than 150 m");
    assure(utHeight > 1.0, "UT Height must be at least 1 m");
    assure(utHeight <= 10.0, "UT Height cannot be larger than 10 m");    
    
    assure((0.0 < frequencyMHz) && (10e03 > frequencyMHz), "No useful range for frequency in MHz");
  
    double pl = 0.0;
    if (link->getOutdoorPropagation() == imtaphy::Link::LoS)
    {
        double dBP = 2.0 * 3.14 * bsHeight * utHeight * frequencyMHz / 3.0e02;
        if (distance < dBP)
        {
            pl = 20.0 * log10(distance * 40.0 * 3.14 * frequencyMHz/3000.0);
            pl += std::min(0.03 * pow(SMaBuildingHeight, 1.72), 10.0) * log10(distance);
            pl -= std::min(0.044 * pow(SMaBuildingHeight, 1.72), 14.77);
            pl += 0.002*log10(SMaBuildingHeight)*distance;
        }
        else // distance >= dBP
        {
             // pathloss in dBP distance
            pl = 20.0 * log10(dBP * 40 * 3.14 * frequencyMHz/3000.0);
            pl += std::min(0.03 * pow(SMaBuildingHeight, 1.72), 10.0) * log10(dBP);
            pl -= std::min(0.044 * pow(SMaBuildingHeight, 1.72), 14.77);
            pl += 0.002*log10(SMaBuildingHeight)*dBP;

            // plus additional offset
            pl += 40.0 * log10(distance / dBP);
        }
    } // end SMa LoS
    else // NLOS
    {
        // this is the NLoS SMa pathloss model
        pl = 161.04 - 7.1 * log10(SMaStreetWidth) + 7.5 * log10(SMaBuildingHeight);
        pl -= (24.37 - 3.7 * pow(SMaBuildingHeight / bsHeight, 2.0)) * log10(bsHeight);
        pl += (43.42 - 3.1 * log10(bsHeight) ) * (log10(distance) - 3.0);
        pl += 20.0 * log10(frequencyMHz/1000.0);
        pl -= 3.2 * pow(log10(11.75 * utHeight), 2.0) - 4.97;
    } // end SMa NLoS
    
    // Outdoor-to-Indoor and Outdoor-to-Vehicle pathloss
    if (link->getUserLocation() == imtaphy::Link::Indoor)
        pl += 20.0;
    else if (link->getUserLocation() == imtaphy::Link::InVehicle)
    {
        // for O2V shadowing take the real MS position
        pl += getO2Vshadowing(9.0, 5.0, link->getBS()->getPosition(), link->getMS()->getPosition());
    }
    return wns::Ratio::from_dB(pl);
}
           

wns::Ratio 
M2135Pathloss::getRMaPathloss(imtaphy::Link* link, double frequencyMHz)
{
    assure((link->getOutdoorPropagation() == imtaphy::Link::NLoS) ||
           (link->getOutdoorPropagation() == imtaphy::Link::LoS),
           "Only NLOS and LOS links are supported");
    
    double bsHeight = link->getBS()->getPosition().getZ();
    double utHeight = link->getWrappedMSposition().getZ();

    wns::Distance distance = (ituClassifier)?
                            (ituClassifier->getDistance(link->getBS()->getPosition(), link->getWrappedMSposition())):
                            (staticClassifer->getDistance(link->getBS()->getPosition(), link->getWrappedMSposition()));

    // For IMT-A calibrations:
    assure(bsHeight == 35, "BS height assumed to be 35m in RMa for IMT-A calibration");
    assure(utHeight == 1.5, "UT height assumed to be 1.5m in RMa for IMT-A calibration");
    assure(RMaBuildingHeight == 5, "RMa building height should be 5m for IMT-A calibration ");
    assure(RMaStreetWidth == 20, "RMa street width should be 20m for IMT-A calibration ");
    assure(distance >= 35, "Distance too small for IMT-A, see Table 8-2");
    

    assure(bsHeight >= 10.0, "BS Height must be at least 10 m");
    assure(bsHeight < 150.0, "BS Height cannot be larger than 150 m");
    assure(utHeight > 1.0, "UT Height must be at least 1 m");
    assure(utHeight <= 10.0, "UT Height cannot be larger than 10 m");    
    
    assure((0.0 < frequencyMHz) && (10e03 > frequencyMHz), "No useful range for frequency in MHz");
    
    double pl;
    if (link->getOutdoorPropagation() == imtaphy::Link::LoS)
    {
        double dBP = 2.0 * 3.1415 * bsHeight * utHeight * frequencyMHz / 3.0e02;
        if (distance < dBP)
        {
            pl = 20.0 * log10(distance * 40.0 * 3.1415 * frequencyMHz/3000.0);
            pl += std::min(0.03 * pow(RMaBuildingHeight, 1.72), 10.0) * log10(distance);
            pl -= std::min(0.044 * pow(RMaBuildingHeight, 1.72), 14.77);
            pl += 0.002*log10(RMaBuildingHeight)*distance;
        } 
        else // distance >= dBP
        {
            // pathloss in dBP distance
            pl = 20.0 * log10(dBP * 40.0 * 3.1415 * frequencyMHz/3000.0);
            pl += std::min(0.03 * pow(RMaBuildingHeight, 1.72), 10.0) * log10(dBP);
            pl -= std::min(0.044 * pow(RMaBuildingHeight, 1.72), 14.77);
            pl += 0.002*log10(RMaBuildingHeight)*dBP;

            // plus additional offset
            pl += 40.0 * log10(distance / dBP);
        }
    } // end RMa LoS
    else  // NLOS
    {
        pl = 161.04 - 7.1 * log10(RMaStreetWidth) + 7.5 * log10(RMaBuildingHeight);
        pl -= (24.37 - 3.7 * pow(RMaBuildingHeight / bsHeight, 2.0)) * log10(bsHeight);
        pl += (43.42 - 3.1 * log10(bsHeight) ) * (log10(distance) - 3.0);
        pl += 20.0 * log10(frequencyMHz/1000.0);
        pl -= 3.2 * pow(log10(11.75 * utHeight), 2.0) - 4.97;
    } // end RMa NLoS

    // All RMa users are in cars, so we have to add outdoor-to-vehicle shadowing.
    // According to "Guidelines for Using IMT-Advanced Channel Models", this has a log-normal
    // distribution with a mean value of 9 dB and std. dev. of 5 dB which. Links to the same
    // user but different BSs have to have identical random values!
    // See [2], Table 1
    
    // real MS position for O2V shadowing
    double o2vShadowing = getO2Vshadowing(9.0, 5.0, link->getBS()->getPosition(), link->getMS()->getPosition()); 
    
    pl += o2vShadowing;

    return wns::Ratio::from_dB(pl);
}
           

wns::Ratio
M2135Pathloss::getInHPathloss(imtaphy::Link* link, double frequencyMHz)
{
    assure((link->getOutdoorPropagation() == imtaphy::Link::NLoS) ||
           (link->getOutdoorPropagation() == imtaphy::Link::LoS),
           "Only NLOS and LOS links are supported");
           
    double bsHeight = link->getBS()->getPosition().getZ();
    double utHeight = link->getMS()->getPosition().getZ();
  
    wns::Distance distance = (ituClassifier)?
                            (ituClassifier->getDistance(link->getBS()->getPosition(), link->getWrappedMSposition())):
                            (staticClassifer->getDistance(link->getBS()->getPosition(), link->getWrappedMSposition()));    
    
    // For IMT-A calibrations:
    assure(bsHeight == 6, "BS height assumed to be 6m in InH for IMT-A calibration"); // [1] Table 8-2
    assure(utHeight == 1.5, "UT height assumed to be 1.5m in InH for IMT-A calibration"); // reference?
    assure(distance >= 3.0, "Distance too for InH");
        
    
    assure((0.0 < frequencyMHz) && (10e03 > frequencyMHz), "No useful range for frequency in MHz");

    assure(bsHeight >= 3.0, "BS Height must be at least 3 m");
    assure(bsHeight <= 6.0, "BS Height cannot be larger than 6 m");
    assure(utHeight >= 1.0, "BS Height must be at least 1 m");
    assure(utHeight <= 2.5, "BS Height cannot be larger than 2.5 m");

    if (link->getOutdoorPropagation() == imtaphy::Link::LoS)
    {
        assure(distance < 100.0, "This model (ITU-InH LoS) is only valid for a maximum distance of 100m");

        double pl = 32.8 + 16.9 * log10(distance);
      
        // Frequency is given in MHz (model uses GHz)
        pl += 20.0 * log10(frequencyMHz/1000.0);

        return wns::Ratio::from_dB(pl);
    } // end InH-LoS
    else //NLOS case
    {
        assure(distance < 150.0, "This model (ITU-InH NLoS) is only valid for a maximum distance of 150m");

        double pl = 11.5 + 43.3 * log10(distance);

        // Frequency is given in MHz (model uses GHz)
        pl += 20.0 * log10(frequencyMHz/1000.0);

        return wns::Ratio::from_dB(pl);
    } // end InH-NLoS
}

wns::Ratio 
M2135Pathloss::getUMaPathloss(imtaphy::Link* link, double frequencyMHz)
{
    assure((link->getOutdoorPropagation() == imtaphy::Link::NLoS) ||
           (link->getOutdoorPropagation() == imtaphy::Link::LoS),
           "Only NLOS and LOS links are supported");
    
    double bsHeight = link->getBS()->getPosition().getZ();
    double utHeight = link->getWrappedMSposition().getZ();

    wns::Distance distance = (ituClassifier)?
                            (ituClassifier->getDistance(link->getBS()->getPosition(), link->getWrappedMSposition())):
                            (staticClassifer->getDistance(link->getBS()->getPosition(), link->getWrappedMSposition()));
    // For IMT-A calibrations:
    assure(bsHeight == 25, "BS height assumed to be 25m in UMa for IMT-A calibration"); // [1] Table 8-2
    assure(utHeight == 1.5, "UT height assumed to be 1.5m in UMa for IMT-A calibration"); // implicit from PL model
    assure(UMaBuildingHeight == 20, "UMa building height should be 20m for IMT-A calibration ");
    assure(UMaStreetWidth == 20, "UMa street width should be 20m for IMT-A calibration ");
    assure(distance >= 25, "Distance too small for IMT-A, see Table 8-2");

    
    assure(bsHeight >= 10.0, "BS Height must be at least 10 m");
    assure(bsHeight < 150.0, "BS Height cannot be larger than 150 m");
    assure(utHeight > 1.0, "UT Height must be at least 1 m");
    assure(utHeight <= 10.0, "UT Height cannot be larger than 10 m");    
    
    assure((0.0 < frequencyMHz) && (10e03 > frequencyMHz), "No useful range for frequency in MHz");
    
    double pl;
    if (link->getOutdoorPropagation() == imtaphy::Link::LoS)
    {
        // this is the LOS UMa pathloss model
        // For UMaLOS the effective heights are used heff = h - 1.0
        bsHeight -= 1.0;
        utHeight -= 1.0;

        double dBP = 4.0 * bsHeight * utHeight * frequencyMHz / 3.0e02;

        if (distance < dBP)
        {
            pl = 22.0 * log10(distance) + 28.0 + 20.0 * log10(frequencyMHz/1000.0);
        }
        else
        { // distance >= dBP
            pl = 40.0 * log10(distance) + 7.8;  // according to M2135 this should be d1, but that does not make sense here
            pl -= 18.0 * log10(bsHeight) + 18.0 * log10(utHeight);
            pl += 2.0 * log10(frequencyMHz / 1000.0);
        }
    }
    else 
    {
        // this is the NLOS UMa pathloss model
        pl = 161.04 - 7.1 * log10(UMaStreetWidth) + 7.5 * log10(UMaBuildingHeight);
        pl -= (24.37 - 3.7 * pow(UMaBuildingHeight / bsHeight, 2.0)) * log10(bsHeight);
        pl += (43.42 - 3.1 * log10(bsHeight) ) * (log10(distance) - 3.0);
        pl += 20.0 * log10(frequencyMHz/1000.0);
        pl -= 3.2 * pow(log10(11.75 * utHeight), 2.0) - 4.97;
    }
    
    // All UMa users are in cars, so we have to add outdoor-to-vehicle shadowing.
    // According to "Guidelines for Using IMT-Advanced Channel Models", this has a log-normal
    // distribution with a mean value of 9 dB and std. dev. of 5 dB. Links to the same
    // user but different BSs have to have identical random values!
    // See [2], Table 1
    double o2vShadowing = getO2Vshadowing(9.0, 5.0, link->getBS()->getPosition(), link->getMS()->getPosition());
  
    pl += o2vShadowing;

    return wns::Ratio::from_dB(pl);
}

wns::Ratio 
M2135Pathloss::getUMiPathloss(imtaphy::Link* link, double frequencyMHz)
{
    // NOTE: This pathloss model implementation is only valid for
    // hexagonal cell layout. For Manhattan layout, the pathloss model
    // is completely different and should be applied accordingly if required
    assure((link->getOutdoorPropagation() == imtaphy::Link::NLoS) ||
           (link->getOutdoorPropagation() == imtaphy::Link::LoS),
           "Only NLOS, and LOS outdoor propagations are supported");
           
    double bsHeight = link->getBS()->getPosition().getZ();
    double utHeight = link->getWrappedMSposition().getZ();

    wns::Distance distance = (ituClassifier)?
                            (ituClassifier->getDistance(link->getBS()->getPosition(), link->getWrappedMSposition())):
                            (staticClassifer->getDistance(link->getBS()->getPosition(), link->getWrappedMSposition()));
    wns::Distance indoorDistance = (ituClassifier)?
                            (ituClassifier->getUMiIndoorPartOfDistance(link->getBS()->getPosition(), link->getMS()->getPosition(), link->getWrappedMSposition())):
                            (staticClassifer->getUMiIndoorPartOfDistance(link->getBS()->getPosition(), link->getMS()->getPosition(), link->getWrappedMSposition()));

    // For IMT-A calibrations:
    assure(bsHeight == 10, "BS height assumed to be 10m in UMi for IMT-A calibration");
    assure(utHeight == 1.5, "UT height assumed to be 1.5m in UMi for IMT-A calibration");
    assure(distance >= 10, "Distance too small for IMT-A, see Table 8-2");
    
    
    assure(bsHeight == 10, "BaseStation height not correct for applying UMi Pathloss model (should be 10m)");
    assure(bsHeight >= 10.0, "BS Height must be at least 10 m");
    assure(bsHeight < 150.0, "BS Height cannot be larger than 150 m");

    assure(utHeight > 1.0, "UT Height must be at least 1 m");
    assure(utHeight <= 2.5, "UT Height cannot be larger than 2.5 m");
        
    // this is according to M.2135 Table A1-2, should not happen if deployment is correct
    assure(distance >= 10.0, "distance too close for UMi pathloss model");
    assure(distance <= 2000, "distance too far for UMi pathloss model");
   
    assure((0.0 < frequencyMHz) && (10e03 > frequencyMHz), "No useful range for frequency in MHz");
    double pl = 0.0;

    // outdoor part of pathloss. According to [2], the pathloss of the outdoor part is
    // computed based on the full distance (outdoor+indoor) (yes, really!)
    if (link->getOutdoorPropagation() == imtaphy::Link::LoS)
    {
        // For UMiLOS the effictive heights are used heff = h - 1.0
        // to account for the effective environment height (see M2135 footnote 1 to table A1-2)
        bsHeight -= 1.0;
        utHeight -= 1.0;
        
        // The decision on LoS1 and LoS2 (below/above breakpoint) is done on the outdoor distance only but the
        // pathloss is computed on the full distance...
        // [2] "Below or above break point formula has to be used depending on the distance from the BS to the wall of the building"
        double dBP = 4.0 * bsHeight * utHeight * frequencyMHz / 3.0e02;
        if (distance - indoorDistance < dBP)
        {
            pl = 22.0 * log10(distance) + 28.0 + 20.0 * log10(frequencyMHz/1000.0);
        }
        else
        {
            pl = 40.0 * log10(distance) + 7.8;
            pl -= 18.0 * log10(bsHeight) + 18.0 * log10(utHeight);
            pl += 2.0 * log10(frequencyMHz/1000.0);
        }
    } // end UMiLoS
    else  // NLOS
    {
        pl =  36.7 * log10(distance) + 22.7 + 26.0 * log10(frequencyMHz/1000.0);
    } // end UMiNLoS
    
    // Now add the Pathloss_indoor and Pathloss_Through_Wall(20dB)
    if (link->getUserLocation() == imtaphy::Link::Indoor)
    {
        assure(link->getPropagation() == imtaphy::Link::UMiO2I, "Inconsistent classification");

        // through-wall pathloss is 20 dB, see [2] Section 2.2.2     
        pl += 20.0 + 0.5 * indoorDistance;
    }
    else
    {
        // else if outdoor condition: do not add anything, just return the pathloss value
        
        assure(link->getUserLocation() != imtaphy::Link::InVehicle, "UMi cannot be in Car, Invalid userLocation specified");
        assure(link->getUserLocation() == imtaphy::Link::Outdoor, "UMis must be outdoors if they are not indoors");
        assure(link->getPropagation() != imtaphy::Link::UMiO2I, "UMI O2I only applicable to indoor users");
    }
    
    return wns::Ratio::from_dB(pl);
}
           
double 
M2135Pathloss::getO2Vshadowing(double mean, double stdDev, wns::Position bsPosition, wns::Position msPosition)
{
    // has to be static in order to avoid initializing it each time
    static wns::distribution::Uniform dis(0.0, 1.0, wns::simulator::getRNG());
    static unsigned int initialSeed = dis() * UINT_MAX;

    // Only take into account the UT for in-car penetration loss
    // Use the HashRNG as an engine for the boost normal_distribution RNG
    detail::HashRNG onlyUT(initialSeed + 2637,
                           bsPosition,
                           msPosition,
                           false, true);

    boost::normal_distribution<double> shadow(mean, stdDev);

    double penetrationLoss = shadow(onlyUT);
    return penetrationLoss;
}
