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

#include <IMTAPHY/link2System/MMIBeffectiveSINR.hpp>
#include <math.h>

using namespace imtaphy::l2s;

MMIBeffectiveSINR::MMIBeffectiveSINR() :
    EffectiveSINRModelBase(),
    lookForKey(1),
    result(imtaphy::detail::getResultShape<MiSinrLutType::HyperCubeType, 1>()),
    interpolation(),
    minSINR(pow10(-15.0 / 10.0)),
    maxSINR(pow10(25.0 / 10.0))
{
    int resolution = 1000;
    
    std::vector<unsigned int> shape(1, resolution);
    
    std::vector<double> qam16Keys(resolution);
    std::vector<double> qam64Keys(resolution);
    std::vector<double> sinrs(resolution);
    std::vector<unsigned int> position(1);
    
    for (int index = 0; index < resolution; index++)
    {
        sinrs[index] = (maxSINR - minSINR) * double(index)/double(resolution) + minSINR;
        qam16Keys[index] = forwardMapping(wns::Ratio::from_factor(sinrs[index]), QAM16());
        qam64Keys[index] = forwardMapping(wns::Ratio::from_factor(sinrs[index]), QAM64());
    }

    minMIqam16 = *std::min_element(qam16Keys.begin(), qam16Keys.end());
    maxMIqam16 = *std::max_element(qam16Keys.begin(), qam16Keys.end());
    minMIqam64 = *std::min_element(qam64Keys.begin(), qam64Keys.end());
    maxMIqam64 = *std::max_element(qam64Keys.begin(), qam64Keys.end());
    
    qam16Lut = new MiSinrLutType(shape, std::vector<std::vector<double> >(1,qam16Keys), false);
    qam64Lut = new MiSinrLutType(shape, std::vector<std::vector<double> >(1,qam64Keys), false);

    for (int index = 0; index < resolution; index++)
    {
        position[0] = index;
        qam16Lut->fill(position, sinrs[index]);
        qam64Lut->fill(position, sinrs[index]);
    }
}

double 
MMIBeffectiveSINR::forwardMapping(wns::Ratio _sinr, ModulationScheme modulation)
{
    double mi = 0.0;
    double sinr = _sinr.get_factor();

    // also the J functions have limited ranges in which  they operate
    if (sinr > maxSINR)
        sinr = maxSINR;
    if (sinr < minSINR)
        sinr = minSINR;
    
    // according to Table 29 in IEEE 802.16m-08/004r5    
    switch(modulation.getBitsPerSymbol())
    {  
        case 2:
            mi  = J(2.0 * sqrt(sinr));
            break;
        case 4:
            mi = 0.5 * J(0.8 * sqrt(sinr)) + 
                 0.25 * J(2.17 * sqrt(sinr)) + 
                 0.25 * J(0.965 * sqrt(sinr));
            break;
        case 6:
            mi = 1.0/3.0 * J(1.47 * sqrt(sinr)) + 
                 1.0/3.0 * J(0.529 * sqrt(sinr)) + 
                 1.0/3.0 * J(0.366 * sqrt(sinr));
            break;
        default:
            assure(0, "unsupported modulation");
    }
    return mi;
}

wns::Ratio 
MMIBeffectiveSINR::reverseMapping(double average, ModulationScheme modulation) 
{
    double sinr = -1.0;
    
    switch(modulation.getBitsPerSymbol())
    {  
        case 2:
            // revert mi  = J(2.0 * sqrt(sinr))
            sinr = Jinv(average) / 2.0;
            sinr *= sinr;

            if (sinr < minSINR)
                sinr = minSINR;
            if (sinr > maxSINR)
                sinr = maxSINR;
            
            break;
        case 4:
            if (average <= minMIqam16)
                sinr = minSINR;
            else if (average >= maxMIqam16)
                sinr = maxSINR;
            else
            {
                lookForKey[0] = average;
                result = qam16Lut->getHyperCube(lookForKey);
                sinr = interpolation.linear(result, lookForKey);
            }
            break;
        case 6:
            if (average <= minMIqam64)
                sinr = minSINR;
            else if (average >= maxMIqam64)
                sinr = maxSINR;
            else
            {
                lookForKey[0] = average;
                result = qam64Lut->getHyperCube(lookForKey);
                sinr = interpolation.linear(result, lookForKey);
            }
            break;
        default:
            assure(0, "unsupported modulation");
    }
    
    return wns::Ratio::from_factor(sinr);
}

inline
double MMIBeffectiveSINR::J(double x) const
{
    double x2 = x * x;
    double x3 = x2 * x;
    
    if (x < 1.6363)
        return -0.04210661 * x3 + 0.209252 * x2 - 0.00640081 * x;
    else
        return 1.0 - exp(0.00181492 * x3 - 0.142675 * x2 - 0.0822054 * x + 0.0549608);
}

inline
double MMIBeffectiveSINR::Jinv(double y) const
{
    assure((y >= 0) && (y <= 1), "y is supposed to be a mean mutual information per bit, must be between 0 and 1");

    if (y < 0.3646)
        return 1.09542 * y*y + 0.214217 * y + 2.33727 * sqrt(y);
    else
        return -0.706692 * log(-0.386013 * (y - 1.0)) + 1.75017 * y;
}