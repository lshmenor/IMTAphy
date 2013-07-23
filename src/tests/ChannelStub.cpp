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

#include <IMTAPHY/tests/ChannelStub.hpp>
#include <IMTAPHY/tests/StationPhyStub.hpp>

using namespace imtaphy::tests;

ChannelStub::ChannelStub():
    imtaphy::Channel(0) // avoid the standard constructor
{
    
}

void imtaphy::tests::ChannelStub::setSpectrum(imtaphy::Spectrum* _spectrum)
{
    spectrum = _spectrum;
}


void
ChannelStub::registerStationPhy(StationPhyStub* station)
{ 
    if (station->getStationType() == BASESTATION)
        baseStations.push_back(station);
    else
        mobileStations.push_back(station);
}

void 
ChannelStub::setPathlossModel(imtaphy::pathloss::PathlossModelInterface* _pathlossModel)
{
    pathlossModel = _pathlossModel;
}

