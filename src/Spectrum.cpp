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

#include <IMTAPHY/Spectrum.hpp>

using namespace imtaphy;


Spectrum::Spectrum(const wns::pyconfig::View& pyConfig) :
    systemCenterFrequency(2),
    prbBandwidth(pyConfig.get<double>("prbBandwidthHz")),
    centerFrequencyPerPRB(2),
    numberOfPRBs(2)
{
    systemCenterFrequency[imtaphy::Downlink] = pyConfig.get<double>("centerFrequencyDownlinkHz");
    systemCenterFrequency[imtaphy::Uplink] = pyConfig.get<double>("centerFrequencyUplinkHz");
    
    numberOfPRBs[Uplink] = pyConfig.get<int>("numberOfULPRBs");
    numberOfPRBs[Downlink] = pyConfig.get<int>("numberOfDLPRBs");

    assure((numberOfPRBs[Uplink] > 0) || (numberOfPRBs[Downlink] > 0), "Either Number of DL or UL PRBs must be positive");
    assure(systemCenterFrequency[Downlink] > 0, "Downlink Center Frequency must be positive");
    assure(systemCenterFrequency[Uplink] > 0, "Uplink Center Frequency must be positive");

    if (std::fabs(systemCenterFrequency[Downlink] - systemCenterFrequency[Uplink]) 
        < static_cast<double>(numberOfPRBs[Downlink] + numberOfPRBs[Uplink]) * prbBandwidth / 2.0)
    {
        std::cout << "WARNING: Uplink (center frequency=" 
                  << systemCenterFrequency[Uplink] << ") and Downlink (" 
                  << systemCenterFrequency[Downlink] << ") spectrum (partly) overlapping! The resulting UL/DL interference will *not* be considered!\n";
    }
    
    setPRBfrequencies();
}
    
Spectrum::Spectrum(double centerFreq, double prbBW, unsigned int numDLPRBs, unsigned int numULPRBs) :
    systemCenterFrequency(2),
    prbBandwidth(prbBW),
    centerFrequencyPerPRB(2),
    numberOfPRBs(2)
{
    systemCenterFrequency[imtaphy::Downlink] = centerFreq;
    systemCenterFrequency[imtaphy::Uplink] = centerFreq;
    
    numberOfPRBs[Uplink] = numULPRBs;
    numberOfPRBs[Downlink] = numDLPRBs;
        
    assure((numberOfPRBs[Uplink] > 0) || (numberOfPRBs[Downlink] > 0), "Either Number of DL or UL PRBs must be positive");
    assure(systemCenterFrequency[Uplink] > 0, "Uplink Center Frequency must be positive");
    assure(systemCenterFrequency[Downlink] > 0, "Uplink Center Frequency must be positive");
    assure((0.0 <= prbBandwidth) && (prbBandwidth / 2.0 <= systemCenterFrequency[Uplink]) && (prbBandwidth / 2.0 <= systemCenterFrequency[Downlink]), "Invalid PRB bandwidth");

    setPRBfrequencies();
}
    
void
Spectrum::setPRBfrequencies()  
{
    centerFrequencyPerPRB[Uplink].resize(numberOfPRBs[Uplink]);
    centerFrequencyPerPRB[Downlink].resize(numberOfPRBs[Downlink]);

    // Determine if we have an even or odd number of PRBs
    // If it is odd, the "middle" PRBs has the systemCenterFrequency as its center frequency
    // If the number is even, the the two "middle" PRBs have center frequencies of system center +/- prbBW/2

    // for uplink and downlink
    for (unsigned int d = 0; d <= 1; d++)
    {

        double lowestPRBfrequency;

        if (numberOfPRBs[d] % 2)
        { // odd number of PRBs
            lowestPRBfrequency = systemCenterFrequency[d] - float((numberOfPRBs[d] - 1) / 2) * prbBandwidth;
        }
        else // even number of PRBs
        {
            lowestPRBfrequency = systemCenterFrequency[d] - float(numberOfPRBs[d] / 2) * prbBandwidth - prbBandwidth / 2.0;
        }

        for (unsigned int f = 0; f < numberOfPRBs[d]; f++)
        {
            centerFrequencyPerPRB[d][f] = lowestPRBfrequency + float(f)*prbBandwidth;
        }
    }
}
imtaphy::interface::PRB 
Spectrum::getNumberOfPRBs(Direction direction) const
{
    return numberOfPRBs[direction];
}


double 
Spectrum::getPRBbandWidthHz() const
{
    return prbBandwidth;
}


double 
Spectrum::getPRBcenterFrequencyHz(Direction direction, imtaphy::interface::PRB prb) const
{
    assure(( 0 <= prb) && (prb < numberOfPRBs[direction]), "Invalid PRB index");

    return centerFrequencyPerPRB[direction][prb];
}


double Spectrum::getSystemCenterFrequencyHz(Direction direction) const
{
    return systemCenterFrequency[direction];
}

double 
imtaphy::Spectrum::getSystemCenterFrequencyWavelenghtMeters(Direction direction) const
{
    return  3E08 / getSystemCenterFrequencyHz(direction);
}



