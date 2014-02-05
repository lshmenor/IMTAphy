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

#include <IMTAPHY/antenna/AntennaITU.hpp>
#include <WNS/PyConfigViewCreator.hpp>
#include <WNS/Assure.hpp>
#include <itpp/base/math/misc.h>

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    imtaphy::antenna::AntennaITU,
    imtaphy::antenna::AntennaInterface,
    "imtaphy.antenna.ITU",
    wns::PyConfigViewCreator);

using namespace imtaphy::antenna;

AntennaITU::AntennaITU(const wns::pyconfig::View& pyConfigView) :
    LinearAntennaArray(pyConfigView),
    dBiGain(pyConfigView.get<wns::Ratio>("antennaGain")),
    downtilt(pyConfigView.get<double>("downtilt"))
{
    assure((-3.1416/2.0 <= downtilt) && (downtilt <= 3.1416/2.0), 
           "Downtilt should be between -PI/2 and PI/2 in radians. 0 degrees (0 rad) downtilt is horizontal, -90 degrees (-PI/2 rad) is looking straight up, and +90 degrees (PI/2 rad) is looking straight into the ground.");
    
    if (numElements == 1)
    {

        MESSAGE_SINGLE(NORMAL, logger, "Created single ITU antenna with downtilt of " 
                       << downtilt << " and boresight azimuth of " 
                       << broadsideAzimuth << " and gain of " 
                       << dBiGain << "i");
    }
    else
    {
        MESSAGE_SINGLE(NORMAL, logger, "Created ITU antenna array consisting of "
                       << numElements << " elements. The array has a downtilt of " 
                       << downtilt << " and array broadside azimuth of " 
                       << broadsideAzimuth << " with per-element gain of " 
                       << dBiGain << "i");
    }
    
}

wns::Ratio 
AntennaITU::getAzimuthGain(double absoluteAzimuth) const
{
    // See M.2135 Section 8.5.1.1 equation (1)

    assure((-3.1416 <= absoluteAzimuth) && (absoluteAzimuth <= 3.1416), "Azimuth should be between -PI and PI in radians");

    
    // theta_3dB = 70 degree = 1.221730475 rad
    double theta_3dB = 70.0 / 180.0 * itpp::pi;

    double relativeAngle = getAzimuthRelativeToBroadside(absoluteAzimuth);
    
    double a = relativeAngle / theta_3dB;
    
    wns::Ratio attenuation = std::max(wns::Ratio::from_dB(-12.0 * a * a),
                                      wns::Ratio::from_dB(-20.0));
    
    return attenuation;
}



wns::Ratio 
AntennaITU::getElevationGain(double elevation) const
{
    // See M.2135 Section 8.5.1.1 equation (2)
    
    assure((-3.1416 / 2.0 <= elevation) && (elevation <= 3.1416 / 2.0), 
           "Elevation should be between -Pi/2 and Pi/2");
    
    // elevation 3dB value 15 degrees
    double phi_3dB = 15.0 / 180.0 * itpp::pi;
    
    double a = (elevation - downtilt) / phi_3dB;
    
    wns::Ratio attenuation = std::max(wns::Ratio::from_dB(-12.0 * a * a),
                                      wns::Ratio::from_dB (-20.0));
    
    return attenuation;
}

wns::Ratio
AntennaITU::getGain(double azimuth, double elevation) const
{
    // See M.2135 Section 8.5.1.1 equation following (2)
    
    wns::Ratio attenuation = std::max(this->getElevationGain(elevation) + this->getAzimuthGain(azimuth),
                                      wns::Ratio::from_dB(-20.0));
    
    return (dBiGain + attenuation);
}


