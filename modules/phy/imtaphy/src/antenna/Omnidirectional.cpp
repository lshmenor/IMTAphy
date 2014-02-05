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

#include <IMTAPHY/antenna/Omnidirectional.hpp>
#include <WNS/PyConfigViewCreator.hpp>
#include <itpp/base/math/misc.h>


STATIC_FACTORY_REGISTER_WITH_CREATOR(
    imtaphy::antenna::Omnidirectional,
    imtaphy::antenna::AntennaInterface,
    "imtaphy.antenna.Omnidirectional",
    wns::PyConfigViewCreator);

using namespace imtaphy::antenna;

Omnidirectional::Omnidirectional(const wns::pyconfig::View& pyConfigView) :
    LinearAntennaArray(pyConfigView),
    dBiGain(pyConfigView.get<wns::Ratio>("antennaGain"))
{    
    if (numElements > 1)
    {
        MESSAGE_SINGLE(NORMAL, logger, "Created a linear array of " 
                       << numElements << " omnidirectional antenna elements, boresight azimuth=" 
                       << broadsideAzimuth * 180.0 / itpp::pi  << " degrees and " 
                       << dBiGain << "i gain");
    }
    else
        MESSAGE_SINGLE(NORMAL, logger, "Created omnidirectional antenna with " << dBiGain << "i gain");   
}


wns::Ratio 
Omnidirectional::getAzimuthGain(double azimuth) const
{
    return dBiGain;
}

wns::Ratio 
Omnidirectional::getElevationGain(double elevation) const
{
    return wns::Ratio::from_factor(1.0);
}

wns::Ratio
Omnidirectional::getGain(double azimuth, double elevation) const
{
    // If this would be a perfect isotropic antenna, no dBi gain would be realized
    // however, allowing a dBi gain > 0 might make sense to capture situations where,
    // e.g., in the plane being considered the pattern is omnidirectional with different
    // gains in directions not on the plane.
    
    return dBiGain;
}


