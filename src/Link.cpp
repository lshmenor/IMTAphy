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

#include <IMTAPHY/Link.hpp>
#include <IMTAPHY/Channel.hpp>

using namespace imtaphy;

Link::Link(StationPhy* bs_, StationPhy* ms_, 
           Scenario scenario_, Propagation propagation_, Propagation outdoorPropagation_,
           UserLocation userLocation_):
    bs(bs_),
    ms(ms_),
    scmLinkId(-1),
    scenario(scenario_),
    propagation(propagation_),
    userLocation(userLocation_),
    outdoorPropagation(outdoorPropagation_),
    channelMatrices(2) // uplink and downlink
{
}

// Currently we only want to provide this functionality for float precision
// Fully template the class if needed

namespace imtaphy {

    template <> void Link::initComplexFloatChannelMatrices<float>(imtaphy::Spectrum* spectrum,
                                                                  imtaphy::scm::SpatialChannelModelInterface<float>* scm)
    {
        for (unsigned int d = 0; d <= 1; d++)
        {
            imtaphy::Direction direction = static_cast<imtaphy::Direction>(d);

            channelMatrices[direction].resize(spectrum->getNumberOfPRBs(static_cast<imtaphy::Direction>(direction)));

            for (unsigned int prb = 0; prb < spectrum->getNumberOfPRBs(static_cast<imtaphy::Direction>(direction)); prb++)
            {
                channelMatrices[direction][prb] = scm->getChannelMatrix(this, static_cast<imtaphy::Direction>(direction), prb);
            }
        }
    }

    template <> void Link::initComplexFloatChannelMatrices<double>(imtaphy::Spectrum* spectrum,
                                                                   imtaphy::scm::SpatialChannelModelInterface<double>* scm)
    {
        // do nothing
        // we better don't assert here because double precision channel models are sometimes instantiated
    }

}