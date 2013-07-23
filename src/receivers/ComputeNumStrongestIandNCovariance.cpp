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

#include <IMTAPHY/receivers/ComputeNumStrongestIandNCovariance.hpp>
#include <IMTAPHY/receivers/LinearReceiver.hpp>

using namespace imtaphy::receivers;


imtaphy::detail::ComplexFloatMatrixPtr
ComputeNumStrongestIandNCovariance::computeNoiseAndInterferenceCovariance(InterferencePowerMap &interferers, const imtaphy::detail::ComplexFloatMatrixPtr noiseOnlyMatrix, int excludeInterferer)
{
    // maybe we could add a MKLmatrix copy constructor or something to do this more elegantly
    // copies the pre-computed NoiseOnlyCovariance (diagonal matrix) to what will become the result
    memcpy(IandNoiseCovariance->getLocation(), noiseOnlyMatrix->getLocation(), sizeof(std::complex<float>) * numRxAntennas * numRxAntennas);

    // depending on type of receiver, this should loop over all interferers and add their covariance matrices or
    // their total receive power to the noise covariance matrix


    wns::Power totalRemainingInterference(wns::Power::from_mW(0.0));

    unsigned int numConsidered = 0;
    int index = 0;
    for (InterferencePowerMap::const_iterator iter = interferers.begin(); iter != interferers.end(); iter++)
    {
        // used for probing/analysis purposes where individual interferers (based on their strength-ranking) should be excluded
        if (index < excludeInterferer)
        {
            index++;
            continue;
        }
        index++;
            
        if (numConsidered < numStrongestToConsider)
        {
            std::complex<float>* Hi = iter->second.interferingChannelMatrix;
            assure(Hi != NULL, "Invalid interfering channel matrix");

            assure(iter->second.interferersPrecodingMatrix != imtaphy::detail::ComplexFloatMatrixPtr(), "Invalid precoding matrix");
            assure(iter->second.interferersPrecodingMatrix->getRows() == iter->second.interferingTransmission->getNumTxAntennas(), "Inconsistent info about Tx antennas");

            // sum = (alpha*A) * (alpha*A)^H + sum
            imtaphy::detail::matrixMultiplyCequalsAlphaSquareTimesAAhermitianPlusC(*IandNoiseCovariance, // increment this
                                                                                   *(iter->second.interferersEffectiveChannel), // add the alpha^2 * AA^H
                                                                                   static_cast<float>(iter->second.interferingTransmission->getTxPower().get_mW()) // alpha^2
                                                                                  );

            numConsidered++;
        }
        else
        {   // too many interferers already considered
            // just count their total power
            // note that the power in the map is the sum over all our Rx antennas
            totalRemainingInterference += iter->first;
        }
    }
    
    // as we were summing total received power (sum over our antennas),
    // we have to divide by numAntennas before adding this to each antenna's
    // entry on the diagonal
    totalRemainingInterference /= static_cast<double>(numRxAntennas);

    // add to diagonal
    for (unsigned int i = 0; i < numRxAntennas; i++)
        (*IandNoiseCovariance)[i][i] += std::complex<float>(totalRemainingInterference.get_mW(), 0.0);


    return IandNoiseCovariance;
}
