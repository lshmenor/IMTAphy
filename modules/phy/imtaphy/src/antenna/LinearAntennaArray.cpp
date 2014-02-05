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

#include <IMTAPHY/antenna/LinearAntennaArray.hpp>
#include <WNS/pyconfig/View.hpp>
#include <itpp/base/math/misc.h>

using namespace imtaphy::antenna;

LinearAntennaArray::LinearAntennaArray(const wns::pyconfig::View& pyConfigView) :
    AntennaInterface(pyConfigView),
    broadsideAzimuth(pyConfigView.get<double>("azimuth")),
    numElements(pyConfigView.get<unsigned int>("numElements")),
    logger(pyConfigView.get("logger"))
{
    assure(0 < numElements, "Number of elements must be positive");
    assure((-3.1416 <= broadsideAzimuth) && (broadsideAzimuth <= 3.1416), "Azimuth should be between -PI and PI in radians");

    wns::pyconfig::Sequence distanceSequence = pyConfigView.getSequence("distancesToFirstElement");
    wns::pyconfig::Sequence slantSequence = pyConfigView.getSequence("slants");
    
    distancesToFirstElement.clear();
    for (wns::pyconfig::Sequence::iterator<double> iter = distanceSequence.begin<double>(); iter != distanceSequence.end<double>(); iter++)
    {
        distancesToFirstElement.push_back(*iter);
    }
    
    assure(distancesToFirstElement.size() == numElements, "Need to provide as many distances as elements");
    assure(distancesToFirstElement[0] == 0.0, "First element is reference element -> distance must be zero");
    
    slantAngle.clear();
    for (wns::pyconfig::Sequence::iterator<double> iter = slantSequence.begin<double>(); iter != slantSequence.end<double>(); iter++)
    {
        assure((*iter > -3.1416 / 2.0) && (*iter < +3.1416 / 2.0), "Slant angle should be between -pi/2 .. pi/2");
        slantAngle.push_back(*iter);
    }
    
    MESSAGE_BEGIN(NORMAL, logger, m, "LinearAntennaArray");
        m << "Linear array with the following layout (slant angle): ";
        
        m << "(" << 57.2957795 * slantAngle[0] << "deg)";
        
        for (unsigned int i = 1; i < numElements; i++)
        {
            m << ".." << distancesToFirstElement[i] - distancesToFirstElement[i-1]<< "m..";
            m << "(" << 57.2957795 * slantAngle[i] << "deg)";
        }
    MESSAGE_END();
}


std::complex<double> 
LinearAntennaArray::getVerticalFieldPattern(double azimuth, double elevation, unsigned int elementNumber) const
{
    // there have been situations where aoa/aod have been almost exactly pi so that the assure failed
    // even though it was probably just a precision issue
    assure((-3.1416 <= azimuth) && (azimuth <= 3.1416), "Azimuth should be between -PI and PI in radians");
    assure((-3.1416 / 2.0 <= elevation) && (elevation <= 3.1416 / 2.0), 
           "Elevation should be between -Pi/2 and Pi/2");
    assure(elementNumber < numElements, "Invalid element number, have " << numElements << " elements");

    // Polarized antenna modeling is done according to 3GPP TR 36.814 A.2.1.6.1 (angle-independent in both azimuth and elevation)
    // Note: someone might want to have a look at A.2.1.6.2 for angle-dependet modeling...
    
    // simply take the square root of the corresponding power gain and consider the slant angle 
    // phi is the azimuth angle

    return std::complex<double>(sqrt(getGain(azimuth, elevation).get_factor()) * sin(slantAngle[elementNumber]), 0.0);
}

std::complex<double> 
LinearAntennaArray::getHorizontalFieldPattern(double azimuth, double elevation, unsigned int elementNumber) const
{
    assure((-3.1416 <= azimuth) && (azimuth <= 3.1416), "Azimuth should be between -PI and PI in radians");
    assure((-3.1416 / 2.0 <= elevation) && (elevation <= 3.1416 / 2.0), 
           "Elevation should be between -Pi/2 and Pi/2");

    assure(elementNumber < numElements, "Invalid element number, have " << numElements << " elements");

    // Polarized antenna modeling is done according to 3GPP TR 36.814 A.2.1.6.1 (angle-independent in both azimuth and elevation)
    // Note: someone might want to have a look at A.2.1.6.2 for angle-dependet modeling...
    
    // simply take the square root of the corresponding power gain and consider the slant angle 
    // phi is the azimuth angle

    return std::complex<double>(sqrt(getGain(azimuth, elevation).get_factor()) * cos(slantAngle[elementNumber]), 0.0);
}

unsigned int 
LinearAntennaArray::getNumberOfElements() const
{
    return numElements;
}

double 
LinearAntennaArray::getAzimuthRelativeToBroadside(double absoluteAzimuth) const
{
    double relativeAngle = absoluteAzimuth - broadsideAzimuth;
    
    // make sure that relative angle is within -Pi..Pi
    if (relativeAngle < -itpp::pi)
        relativeAngle += 2.0 * itpp::pi;
    if (relativeAngle > itpp::pi)
        relativeAngle -= 2.0 * itpp::pi;
    
    return relativeAngle;
}

double 
LinearAntennaArray::getPathDifferenceMeters(unsigned int elementNumber,
                                    double phi_n_m) const
{
    // the antenna is supposed to always have a ULA layout
    // here we compute the path difference a wavefront arriving/departing at the
    // array at an absoulte azimuth angle phi_n_m experiences between element "elementNumber" and
    // the reference element 0
    // this takes the element spacings as well as the antenna array broadsize azimuth orientation into account
    // @Jan: see Evernote July 27, 2011
    
    double relativeAngle = getAzimuthRelativeToBroadside(phi_n_m);
    
    assure((0 <= elementNumber) && (elementNumber < numElements), "Wrong element number");

    return distancesToFirstElement[elementNumber] * sin(relativeAngle);
    
}

#ifndef WNS_NDEBUG                                                   
void 
LinearAntennaArray::computeAntennaPattern(imtaphy::detail::ComplexFloatMatrix& beamformingVector, std::vector< wns::Ratio >& pattern360Degrees, double wavelengthMeters)
{
    assure(pattern360Degrees.size() == 360, "You need to pass a wns::Ratio vector with 360 entries");
    double pi = 3.14159265;
    
    // compute steering vectors
    // of course it would be a great idea to compute these only once but then the antenna class would need to know the wavelength 
    std::vector<imtaphy::detail::ComplexFloatMatrix> steeringVectors;
    
    for (unsigned int i = 0; i < 360; i++)
    {
        steeringVectors.push_back(imtaphy::detail::ComplexFloatMatrix(numElements, 1));
        
        double azimuth = static_cast<double>(i) / 180.0 * pi;

        steeringVectors[i][0][0] = std::complex<float>(1, 0);
        for (unsigned int l = 1; l < numElements; l++)
        {
            steeringVectors[i][l][0] = exp(std::complex<float>(0, 2.0 * pi * getPathDifferenceMeters(l, azimuth) / wavelengthMeters));
        }
    }
    
    // compute beamfomring vector's power into steering vector direction by taking the dot product:
    for (unsigned int i = 0; i < 360; i++)
    {
        pattern360Degrees[i] = wns::Ratio::from_factor(std::norm(imtaphy::detail::dotProductOfAColumnAndConjugateBColumn(steeringVectors[i], 0, beamformingVector, 0)));
    }

}
#endif






