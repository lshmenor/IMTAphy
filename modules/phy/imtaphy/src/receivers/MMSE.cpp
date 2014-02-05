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

#include <IMTAPHY/receivers/MMSE.hpp>
#include <IMTAPHY/receivers/ComputeNumStrongestIandNCovariance.hpp>

using namespace imtaphy::receivers;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    imtaphy::receivers::Type1MMSE,
    imtaphy::receivers::ReceiverInterface,
    "imtaphy.receiver.Type1MMSE",
    imtaphy::receivers::StationModuleCreator);


Type1MMSE::Type1MMSE(imtaphy::StationPhy* _station, const wns::pyconfig::View& pyConfigView) :
    MMSEBase(_station, pyConfigView)
{
    // Pyconfig can be useful for getting the number of interferers. But it does not 
    // need to be passed to the computeNumStrongestIandNCovariance. Hence it is being removed.
    // that's for the filter computation
    noiseAndIcovariance = new ComputeNumStrongestIandNCovariance(numRxAntennas, 0);
    
    // that's for computing the SINR; -1 should mean consider all
    interferenceTreatment = new ComputeNumStrongestIandNCovariance(numRxAntennas, -1);
}
